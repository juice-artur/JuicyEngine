#include "jepch.h"
#include "ImGuiLayer.h"

#include "Core/Application.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>

namespace JuicyEngine
{
ImGuiLayer* ImGuiLayer::s_Instance = nullptr;

ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
{
    s_Instance = this;
}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::OnAttach()
{
    auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    vkCreateDescriptorPool(Context->GetDevice()->GetLogicalDevice(), &pool_info, nullptr, &ImGuiDescriptorPool);
    JE_CORE_ASSERT(ImGuiDescriptorPool != VK_NULL_HANDLE, "Failed to create ImGuiDescriptorPool!")

    SDL_Window* window = Application::Get().GetWindow().GetSDLWindow();
    JE_CORE_ASSERT(window, "No SDL window available for ImGui!")

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = Context->GetInstance();
    init_info.PhysicalDevice = Context->GetDevice()->GetPhysicalDevice();
    init_info.Device = Context->GetDevice()->GetLogicalDevice();
    init_info.QueueFamily = Context->GetDevice()->GetQueueFamilyIndices().GraphicsFamily.value();
    init_info.Queue = Context->GetDevice()->GetGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = ImGuiDescriptorPool;
    init_info.MinImageCount = 2;
    init_info.ImageCount = Context->GetSwapchain().GetImageCount();
    init_info.Allocator = nullptr;
    init_info.PipelineInfoMain.RenderPass = Context->GetImGuiRenderPass();
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    JE_CORE_ASSERT(ImGuiDescriptorPool != VK_NULL_HANDLE, "ImGuiDescriptorPool is null!")
    JE_CORE_ASSERT(init_info.PipelineInfoMain.RenderPass != VK_NULL_HANDLE, "ImGuiRenderPass is null!")

    ImGui_ImplSDL3_InitForVulkan(window);
    ImGui_ImplVulkan_Init(&init_info);
}

void ImGuiLayer::OnDetach()
{
    auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(Context->GetDevice()->GetLogicalDevice(), ImGuiDescriptorPool, nullptr);
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnUpdate() {}

void ImGuiLayer::OnEvent(Event& event) {}

void ImGuiLayer::Begin()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize = ImVec2((float) app.GetWindow().GetWidth(), (float) app.GetWindow().GetHeight());

    ImGui::NewFrame();
}

void ImGuiLayer::Render()
{
    ImGui::Render();
}
}
