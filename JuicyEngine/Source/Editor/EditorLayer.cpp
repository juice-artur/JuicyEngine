#include "EditorLayer.h"
#include "jepch.h"

#include "Core/Application.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>

namespace JuicyEngine
{
EditorLayer::EditorLayer() : Layer("EditorLayer") {}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnAttach() {}

void EditorLayer::OnDetach() {}

void EditorLayer::OnUpdate()
{
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    if (first_frame)
    {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, nullptr, &dockspace_id);

        ImGuiID dock_right_up, dock_right_down;
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.35f, &dock_right_up, &dock_right_down);

        ImGui::DockBuilderDockWindow("FPS", dockspace_id);
        ImGui::DockBuilderDockWindow("Scene Viewport", dock_main_id);
        ImGui::DockBuilderDockWindow("Properties", dock_right_up);
        ImGui::DockBuilderDockWindow("Console", dock_right_down);
        ImGui::DockBuilderFinish(dockspace_id);
        first_frame = false;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                Application::Get().GetWindow().SetEventCallback(nullptr);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Begin("FPS");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("Delta time: %.4f s", m_FrameTime);
    ImGui::End();

    ImGui::Begin("Scene Viewport");
    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
    if (viewport_panel_size.x > 0.0f && viewport_panel_size.y > 0.0f)
    {
        uint32_t vp_width = static_cast<uint32_t>(viewport_panel_size.x);
        uint32_t vp_height = static_cast<uint32_t>(viewport_panel_size.y);

        auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
        if (Context)
        {
            VkExtent2D current_size = Context->GetViewportSize();
            if (current_size.width != vp_width || current_size.height != vp_height)
            {
                Context->ResizeViewportFramebuffer(vp_width, vp_height);
            }

            ImTextureID texture_id = reinterpret_cast<ImTextureID>(Context->GetViewportDescriptorSet());
            ImGui::Image(texture_id, viewport_panel_size);
        }
    }
    ImGui::End();

    ImGui::Begin("Properties");
    ImGui::Text("No selection");
    ImGui::End();

    ImGui::Begin("Console");
    ImGui::TextUnformatted("Console output will appear here.");
    ImGui::End();

    ImGui::End();
}

void EditorLayer::OnEvent(Event& event) {}
}
