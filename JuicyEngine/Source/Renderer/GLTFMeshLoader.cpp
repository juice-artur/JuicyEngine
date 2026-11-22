#include "GLTFMeshLoader.h"

#include <tiny_gltf_v3.h>
#include <stb_image.h>
#include "Core/Log.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <numeric>

namespace JuicyEngine
{
namespace
{
size_t ComponentSize(int ComponentType)
{
    switch (ComponentType)
    {
    case TG3_COMPONENT_TYPE_BYTE:
    case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
        return 1;
    case TG3_COMPONENT_TYPE_SHORT:
    case TG3_COMPONENT_TYPE_UNSIGNED_SHORT:
        return 2;
    default:
        return 4;
    }
}

const tg3_accessor* FindAccessor(const tg3_model& Model, int Index)
{
    if (Index < 0 || static_cast<uint32_t>(Index) >= Model.accessors_count)
        return nullptr;
    return &Model.accessors[Index];
}

bool GetViewData(const tg3_model& Model,
                 const tg3_accessor& Accessor,
                 const uint8_t*& OutData,
                 size_t& OutStride)
{
    if (Accessor.buffer_view < 0 || static_cast<uint32_t>(Accessor.buffer_view) >= Model.buffer_views_count)
        return false;

    const auto& View = Model.buffer_views[Accessor.buffer_view];
    if (View.buffer < 0 || static_cast<uint32_t>(View.buffer) >= Model.buffers_count)
        return false;

    const auto& Buffer = Model.buffers[View.buffer];
    const size_t Components = Accessor.type == TG3_TYPE_VEC3 ? 3 :
                              Accessor.type == TG3_TYPE_VEC2 ? 2 :
                                                               1;
    const size_t ElementSize = ComponentSize(Accessor.component_type) * Components;
    const size_t Offset = static_cast<size_t>(View.byte_offset) + static_cast<size_t>(Accessor.byte_offset);

    OutStride = View.byte_stride > 0 ? static_cast<size_t>(View.byte_stride) : ElementSize;

    if (Offset > Buffer.data.count)
        return false;
    if (Accessor.count > 0 &&
        Offset + (static_cast<size_t>(Accessor.count) - 1) * OutStride + ElementSize > Buffer.data.count)
        return false;

    OutData = Buffer.data.data + Offset;
    return true;
}

bool ReadVec3(const tg3_model& Model, int AccessorIndex, std::vector<glm::vec3>& Out)
{
    const auto* Accessor = FindAccessor(Model, AccessorIndex);
    if (!Accessor || Accessor->type != TG3_TYPE_VEC3 ||
        Accessor->component_type != TG3_COMPONENT_TYPE_FLOAT)
        return false;

    const uint8_t* Data = nullptr;
    size_t Stride = 0;
    if (!GetViewData(Model, *Accessor, Data, Stride))
        return false;

    Out.clear();
    Out.reserve(static_cast<size_t>(Accessor->count));
    for (uint64_t i = 0; i < Accessor->count; ++i)
    {
        float Values[3] {};
        std::memcpy(Values, Data + i * Stride, sizeof(Values));
        Out.emplace_back(Values[0], Values[1], Values[2]);
    }
    return true;
}

bool ReadVec2(const tg3_model& Model, int AccessorIndex, std::vector<glm::vec2>& Out)
{
    const auto* Accessor = FindAccessor(Model, AccessorIndex);
    if (!Accessor || Accessor->type != TG3_TYPE_VEC2 ||
        Accessor->component_type != TG3_COMPONENT_TYPE_FLOAT)
        return false;

    const uint8_t* Data = nullptr;
    size_t Stride = 0;
    if (!GetViewData(Model, *Accessor, Data, Stride))
        return false;

    Out.clear();
    Out.reserve(static_cast<size_t>(Accessor->count));
    for (uint64_t i = 0; i < Accessor->count; ++i)
    {
        float Values[2] {};
        std::memcpy(Values, Data + i * Stride, sizeof(Values));
        Out.emplace_back(Values[0], Values[1]);
    }
    return true;
}

bool ReadIndices(const tg3_model& Model, int AccessorIndex, std::vector<uint32_t>& Out)
{
    const auto* Accessor = FindAccessor(Model, AccessorIndex);
    if (!Accessor || Accessor->type != TG3_TYPE_SCALAR)
        return false;

    const uint8_t* Data = nullptr;
    size_t Stride = 0;
    if (!GetViewData(Model, *Accessor, Data, Stride))
        return false;

    Out.clear();
    Out.reserve(static_cast<size_t>(Accessor->count));
    for (uint64_t i = 0; i < Accessor->count; ++i)
    {
        const uint8_t* P = Data + i * Stride;
        switch (Accessor->component_type)
        {
        case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
            Out.push_back(*P);
            break;
        case TG3_COMPONENT_TYPE_BYTE:
            Out.push_back(static_cast<uint32_t>(static_cast<int8_t>(*P)));
            break;
        case TG3_COMPONENT_TYPE_UNSIGNED_SHORT:
        {
            uint16_t Value = 0;
            std::memcpy(&Value, P, sizeof(Value));
            Out.push_back(Value);
            break;
        }
        case TG3_COMPONENT_TYPE_SHORT:
        {
            int16_t Value = 0;
            std::memcpy(&Value, P, sizeof(Value));
            Out.push_back(static_cast<uint32_t>(Value));
            break;
        }
        case TG3_COMPONENT_TYPE_UNSIGNED_INT:
        {
            uint32_t Value = 0;
            std::memcpy(&Value, P, sizeof(Value));
            Out.push_back(Value);
            break;
        }
        case TG3_COMPONENT_TYPE_INT:
        {
            int32_t Value = 0;
            std::memcpy(&Value, P, sizeof(Value));
            Out.push_back(static_cast<uint32_t>(Value));
            break;
        }
        default:
            return false;
        }
    }
    return true;
}

glm::mat4 ToMat4(const double Matrix[16])
{
    glm::mat4 Result(1.0f);
    for (int Column = 0; Column < 4; ++Column)
        for (int Row = 0; Row < 4; ++Row)
            Result[Column][Row] = static_cast<float>(Matrix[Column * 4 + Row]);
    return Result;
}

void AccumulateTransform(const tg3_model& Model,
                         int NodeIndex,
                         const glm::mat4& Parent,
                         glm::mat4& Out,
                         int Depth)
{
    if (Depth > 64 || NodeIndex < 0 || static_cast<uint32_t>(NodeIndex) >= Model.nodes_count)
        return;

    const auto& Node = Model.nodes[NodeIndex];
    glm::mat4 Local = Parent;

    if (Node.has_matrix)
    {
        Local *= ToMat4(Node.matrix);
    }
    else
    {
        const glm::vec3 Translation(static_cast<float>(Node.translation[0]),
                                    static_cast<float>(Node.translation[1]),
                                    static_cast<float>(Node.translation[2]));
        const glm::quat Rotation(static_cast<float>(Node.rotation[3]),
                                 static_cast<float>(Node.rotation[0]),
                                 static_cast<float>(Node.rotation[1]),
                                 static_cast<float>(Node.rotation[2]));
        const glm::vec3 Scale(static_cast<float>(Node.scale[0]),
                              static_cast<float>(Node.scale[1]),
                              static_cast<float>(Node.scale[2]));

        Local *= glm::translate(glm::mat4(1.0f), Translation) * glm::mat4_cast(Rotation) * glm::scale(glm::mat4(1.0f), Scale);
    }

    if (Node.mesh >= 0 && Out == glm::mat4(1.0f))
        Out = Local;

    for (uint32_t i = 0; i < Node.children_count; ++i)
        AccumulateTransform(Model, Node.children[i], Local, Out, Depth + 1);
}

int Base64Value(char C)
{
    if (C >= 'A' && C <= 'Z') return C - 'A';
    if (C >= 'a' && C <= 'z') return C - 'a' + 26;
    if (C >= '0' && C <= '9') return C - '0' + 52;
    if (C == '+' || C == '-') return 62;
    if (C == '/' || C == '_') return 63;
    return -1;
}

std::optional<std::vector<uint8_t>> DecodeBase64DataUri(std::string_view Uri)
{
    const size_t Comma = Uri.find(',');
    if (Uri.rfind("data:", 0) != 0 || Comma == std::string_view::npos)
        return std::nullopt;

    std::vector<uint8_t> Result;
    int Accumulator = 0;
    int Bits = 0;

    for (char C : Uri.substr(Comma + 1))
    {
        if (C == '=') break;
        if (std::isspace(static_cast<unsigned char>(C))) continue;

        const int Value = Base64Value(C);
        if (Value < 0) return std::nullopt;

        Accumulator = (Accumulator << 6) | Value;
        Bits += 6;
        if (Bits >= 8)
        {
            Bits -= 8;
            Result.push_back(static_cast<uint8_t>((Accumulator >> Bits) & 0xFF));
        }
    }
    return Result;
}

std::optional<TextureData> DecodeImage(const tg3_model& Model,
                                       const tg3_image& Image,
                                       const std::filesystem::path& BaseDir)
{
    int Width = 0;
    int Height = 0;
    int Channels = 0;
    stbi_uc* Pixels = nullptr;

    // Avocado.glb: текстура вбудована у GLB через bufferView
    if (Image.buffer_view >= 0 && static_cast<uint32_t>(Image.buffer_view) < Model.buffer_views_count)
    {
        const auto& View = Model.buffer_views[Image.buffer_view];
        if (View.buffer >= 0 && static_cast<uint32_t>(View.buffer) < Model.buffers_count)
        {
            const auto& Buffer = Model.buffers[View.buffer];
            if (View.byte_offset + View.byte_length <= Buffer.data.count)
            {
                Pixels = stbi_load_from_memory(Buffer.data.data + View.byte_offset,
                                               static_cast<int>(View.byte_length),
                                               &Width, &Height, &Channels, 4);
            }
        }
    }
    else if (Image.uri.data && Image.uri.len > 0)
    {
        const std::string_view Uri(Image.uri.data, Image.uri.len);
        if (Uri.rfind("data:", 0) == 0)
        {
            auto Bytes = DecodeBase64DataUri(Uri);
            if (Bytes)
                Pixels = stbi_load_from_memory(Bytes->data(),
                                               static_cast<int>(Bytes->size()),
                                               &Width, &Height, &Channels, 4);
        }
        else
        {
            std::filesystem::path Path(Uri);
            if (Path.is_relative())
                Path = BaseDir / Path;
            Pixels = stbi_load(Path.string().c_str(), &Width, &Height, &Channels, 4);
        }
    }

    if (!Pixels)
    {
        JE_CORE_WARN("Failed to decode glTF baseColor image (bufferView={0})", Image.buffer_view);
        return std::nullopt;
    }

    TextureData Result;
    Result.Width = Width;
    Result.Height = Height;
    Result.Pixels.assign(Pixels, Pixels + static_cast<size_t>(Width) * static_cast<size_t>(Height) * 4);
    stbi_image_free(Pixels);
    return Result;
}

std::optional<TextureData> ReadBaseColorTexture(const tg3_model& Model,
                                                int MaterialIndex,
                                                const std::filesystem::path& BaseDir)
{
    if (MaterialIndex < 0 || static_cast<uint32_t>(MaterialIndex) >= Model.materials_count)
        return std::nullopt;

    const int TextureIndex = Model.materials[MaterialIndex].pbr_metallic_roughness.base_color_texture.index;
    if (TextureIndex < 0 || static_cast<uint32_t>(TextureIndex) >= Model.textures_count)
        return std::nullopt;

    const int ImageIndex = Model.textures[TextureIndex].source;
    if (ImageIndex < 0 || static_cast<uint32_t>(ImageIndex) >= Model.images_count)
        return std::nullopt;

    return DecodeImage(Model, Model.images[ImageIndex], BaseDir);
}

int FindAttribute(const tg3_primitive& Primitive, std::string_view Name)
{
    for (uint32_t i = 0; i < Primitive.attributes_count; ++i)
    {
        const tg3_str& Key = Primitive.attributes[i].key;
        if (Key.data && std::string_view(Key.data, Key.len) == Name)
            return Primitive.attributes[i].value;
    }
    return -1;
}

bool AppendMesh(const tg3_model& Model, int MeshIndex, Mesh& Out, const std::filesystem::path& BaseDir)
{
    if (MeshIndex < 0 || static_cast<uint32_t>(MeshIndex) >= Model.meshes_count)
        return false;

    bool Added = false;
    const auto& MeshData = Model.meshes[MeshIndex];
    for (uint32_t p = 0; p < MeshData.primitives_count; ++p)
    {
        const auto& Primitive = MeshData.primitives[p];
        if (Primitive.mode != TG3_MODE_TRIANGLES)
            continue;

        const int PositionIndex = FindAttribute(Primitive, "POSITION");
        if (PositionIndex < 0)
            continue;

        std::vector<glm::vec3> Positions;
        if (!ReadVec3(Model, PositionIndex, Positions) || Positions.empty())
            continue;

        std::vector<glm::vec2> UVs;
        const int UVIndex = FindAttribute(Primitive, "TEXCOORD_0");
        if (UVIndex >= 0)
            ReadVec2(Model, UVIndex, UVs);

        std::vector<uint32_t> Indices;
        if (Primitive.indices >= 0)
        {
            if (!ReadIndices(Model, Primitive.indices, Indices))
                continue;
        }
        else
        {
            Indices.resize(Positions.size());
            std::iota(Indices.begin(), Indices.end(), 0u);
        }

        glm::vec3 Color(1.0f);
        if (Primitive.material >= 0 && static_cast<uint32_t>(Primitive.material) < Model.materials_count)
        {
            const auto& Factor = Model.materials[Primitive.material].pbr_metallic_roughness.base_color_factor;
            Color = glm::vec3(static_cast<float>(Factor[0]),
                              static_cast<float>(Factor[1]),
                              static_cast<float>(Factor[2]));
        }

        const uint32_t Base = static_cast<uint32_t>(Out.Vertices.size());
        for (size_t i = 0; i < Positions.size(); ++i)
            Out.Vertices.push_back({Positions[i], Color, i < UVs.size() ? UVs[i] : glm::vec2(0.0f)});

        for (uint32_t Index : Indices)
            Out.Indices.push_back(Index + Base);

        if (!Out.BaseColorTexture.Valid())
        {
            auto Texture = ReadBaseColorTexture(Model, Primitive.material, BaseDir);
            if (Texture)
                Out.BaseColorTexture = std::move(*Texture);
        }

        Added = true;
    }
    return Added;
}
} // namespace

std::optional<Mesh> GLTFMeshLoader::Load(const std::filesystem::path& Path)
{
    tg3_model Model {};
    tg3_error_stack Errors {};

    const std::string PathStr = Path.string();
	const tg3_error_code Err = tg3_parse_file(&Model, &Errors, PathStr.c_str(), static_cast<uint32_t>(PathStr.size()), nullptr);

    for (uint32_t i = 0; i < Errors.count; ++i)
    {
        if (Errors.entries[i].severity == TG3_SEVERITY_WARNING)
            JE_CORE_WARN("glTF warning: {0}", Errors.entries[i].message ? Errors.entries[i].message : "");
        else if (Errors.entries[i].severity == TG3_SEVERITY_ERROR)
            JE_CORE_ERROR("glTF error: {0}", Errors.entries[i].message ? Errors.entries[i].message : "");
    }

    if (Err != TG3_OK)
    {
        JE_CORE_ERROR("Failed to load mesh '{0}' (code={1})", PathStr, static_cast<int>(Err));
        tg3_error_stack_free(&Errors);
        tg3_model_free(&Model);
        return std::nullopt;
    }

    tg3_error_stack_free(&Errors);

    Mesh Result;
    Result.Name = Path.filename().string();
    const std::filesystem::path BaseDir = Path.parent_path();
    
    if (Model.scenes_count > 0)
    {
        const int SceneIndex = Model.default_scene >= 0 ? Model.default_scene : 0;
        if (static_cast<uint32_t>(SceneIndex) < Model.scenes_count)
        {
            const auto& Scene = Model.scenes[SceneIndex];
            for (uint32_t i = 0; i < Scene.nodes_count; ++i)
                AccumulateTransform(Model, Scene.nodes[i], glm::mat4(1.0f), Result.Transform, 0);
        }
    }

    for (uint32_t i = 0; i < Model.nodes_count; ++i)
    {
        if (Model.nodes[i].mesh >= 0 && AppendMesh(Model, Model.nodes[i].mesh, Result, BaseDir))
            break;
    }

    if (Result.Vertices.empty())
    {
        for (uint32_t i = 0; i < Model.meshes_count; ++i)
            if (AppendMesh(Model, static_cast<int>(i), Result, BaseDir))
                break;
    }

    tg3_model_free(&Model);

    if (Result.Vertices.empty() || Result.Indices.empty())
    {
        JE_CORE_ERROR("Mesh '{0}' contains no triangle geometry", PathStr);
        return std::nullopt;
    }

    JE_CORE_INFO("Loaded mesh '{0}': {1} vertices, {2} indices, texture={3}",
                 Result.Name,
                 Result.Vertices.size(),
                 Result.Indices.size(),
                 Result.BaseColorTexture.Valid());
    return Result;
}
} // namespace JuicyEngine