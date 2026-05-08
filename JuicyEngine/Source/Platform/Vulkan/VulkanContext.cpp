#include "VulkanContext.h"

#include "VulkanBuffer.h"
#include "VulkanUtils.h"
#include "jepch.h"
#include "vulkan/vulkan.h"
#include <vulkan/vk_enum_string_helper.h>
#include "VulkanShader.h"
#include "Core/Core.h"
#include "Renderer/Pipeline.h"
#define GLM_FORCE_RADIANS
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

namespace JuicyEngine
{
VulkanContext::VulkanContext() {}

void VulkanContext::Init(void* Window)
{
    WindowPtr = Window;

    m_DeviceManager = new VulkanDeviceManager();
    m_DeviceManager->Init(Window);

    VkCommandPoolCreateInfo PoolInfo {};
    PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    PoolInfo.queueFamilyIndex = GetDevice()->GetQueueFamilyIndices().GraphicsFamily.value();

    auto CreateCommandPoolResult =
        vkCreateCommandPool(GetDevice()->GetLogicalDevice(), &PoolInfo, nullptr, &CommandPool);

    JE_CORE_ASSERT(CreateCommandPoolResult == VK_SUCCESS,
                   "Failed to create command pool: {0}",
                   string_VkResult(CreateCommandPoolResult))

    SwapChain.Init(m_DeviceManager->GetSurface()->GetSurface(), Window);
    CreateDepthResources();
    RenderPass->CreateRenderPass(SwapChain.GetFormat(), SwapChain.GetSwapChainImageViews(), SwapChain.GetExtent());
    CreateImGuiRenderPass();
    CreateImGuiFramebuffers();
    CreateViewportRenderPass();
    CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (auto& CB : CommandBuffers)
        CB.Init(GetDevice()->GetLogicalDevice(), CommandPool);
    CreateGraphicsPipeline();
    CreateViewportGraphicsPipeline();
    CreateSyncObjects();

    VertexBuffer.reset(static_cast<std::unique_ptr<VulkanVertexBuffer>::pointer>(VertexBuffer::Create(Vertices)));
    IndexBuffer.reset(static_cast<std::unique_ptr<VulkanIndexBuffer>::pointer>(IndexBuffer::Create(Indices)));
    UniformBuffer.reset(static_cast<std::unique_ptr<VulkanUniformBuffer>::pointer>(UniformBuffer::Create(sizeof(Ubo))));
    Texture.reset(new VulkanTexture2D("Assets/Textures/statue.jpg"));

    Ubo.View = glm::lookAt(glm::vec3(0.0f, 2.0f, -6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Ubo.Proj = glm::perspective(
        glm::radians(60.0f), SwapChain.GetExtent().width / (float) SwapChain.GetExtent().height, 0.1f, 100.0f);
    Ubo.Proj[1][1] *= -1;
    UniformBuffer->UploadData(sizeof(Ubo), &Ubo);

    CreateDescriptorPool();
    CreateDescriptorSets();
    // Viewport framebuffer created on first use to avoid validation layer issues

    vkDeviceWaitIdle(GetDevice()->GetLogicalDevice());
}

void VulkanContext::SwapBuffers()
{
    if (Skip)
    {
        Skip = false;
        return;
    }

    uint32_t ImageIndex = CurrentSyncIndex;

    VkSubmitInfo SubmitInfo {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore WaitSemaphores[] = {ImageAvailableSemaphores[ImageIndex]};
    VkPipelineStageFlags WaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = WaitSemaphores;
    SubmitInfo.pWaitDstStageMask = WaitStages;

    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffers[ImageIndex].GetCommandBuffer();

    VkSemaphore SignalSemaphores[] = {RenderFinishedSemaphores[ImageIndex]};
    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = SignalSemaphores;

    auto QueueSubmitResult = vkQueueSubmit(GetDevice()->GetGraphicsQueue(), 1, &SubmitInfo, InFlightFences[ImageIndex]);
    JE_ASSERT(QueueSubmitResult == VK_SUCCESS, "Queue submit failed!")

    VkPresentInfoKHR PresentInfo {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores = SignalSemaphores;

    VkSwapchainKHR SwapChains[] = {SwapChain.GetSwapChain()};
    PresentInfo.swapchainCount = 1;
    PresentInfo.pSwapchains = SwapChains;

    PresentInfo.pImageIndices = &RenderPass->SwapChainImageIndex;

    auto PresentResult = vkQueuePresentKHR(GetDevice()->GetPresentQueue(), &PresentInfo);
    if (PresentResult == VK_ERROR_OUT_OF_DATE_KHR || PresentResult == VK_SUBOPTIMAL_KHR)
    {
        FramebufferResized = true;
    }
}

void VulkanContext::Shutdown()
{
    vkDeviceWaitIdle(GetDevice()->GetLogicalDevice());

    GraphicsPipeline.Shutdown();
    ViewportGraphicsPipeline.Shutdown();

    vkDestroyDescriptorPool(GetDevice()->GetLogicalDevice(), DescriptorPool, nullptr);

    VertexBuffer.reset();
    IndexBuffer.reset();
    Texture.reset();
    UniformBuffer.reset();

    vkDestroyCommandPool(GetDevice()->GetLogicalDevice(), CommandPool, nullptr);

    for (auto Semaphore : ImageAvailableSemaphores)
        vkDestroySemaphore(GetDevice()->GetLogicalDevice(), Semaphore, nullptr);
    for (auto Semaphore : RenderFinishedSemaphores)
        vkDestroySemaphore(GetDevice()->GetLogicalDevice(), Semaphore, nullptr);
    for (auto Fence : InFlightFences)
        vkDestroyFence(GetDevice()->GetLogicalDevice(), Fence, nullptr);

    RenderPass->Shutdown();
    RenderPass.reset();

    vkDestroyRenderPass(GetDevice()->GetLogicalDevice(), ImGuiRenderPass, nullptr);
    DestroyImGuiFramebuffers();

    vkDestroyRenderPass(GetDevice()->GetLogicalDevice(), ViewportRenderPass, nullptr);
    DestroyViewportFramebuffer();

    vkDestroyImageView(GetDevice()->GetLogicalDevice(), DepthImageView, nullptr);
    vkDestroyImage(GetDevice()->GetLogicalDevice(), DepthImage, nullptr);
    vkFreeMemory(GetDevice()->GetLogicalDevice(), DepthImageMemory, nullptr);

    SwapChain.Shutdown();

    m_DeviceManager->Shutdown();
    delete m_DeviceManager;
    m_DeviceManager = nullptr;
}

void VulkanContext::Draw()
{
    if (FramebufferResized)
    {
        vkDeviceWaitIdle(GetDevice()->GetLogicalDevice());
        RecreateSwapChain();
        FramebufferResized = false;
    }

    CurrentSyncIndex = CurrentFrameIndex % MAX_FRAMES_IN_FLIGHT;

    vkWaitForFences(GetDevice()->GetLogicalDevice(), 1, &InFlightFences[CurrentSyncIndex], VK_TRUE, UINT64_MAX);
    vkResetFences(GetDevice()->GetLogicalDevice(), 1, &InFlightFences[CurrentSyncIndex]);

    VkResult Result = vkAcquireNextImageKHR(GetDevice()->GetLogicalDevice(),
                                            SwapChain.GetSwapChain(),
                                            UINT64_MAX,
                                            ImageAvailableSemaphores[CurrentSyncIndex],
                                            VK_NULL_HANDLE,
                                            &RenderPass->SwapChainImageIndex);

    if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
    {
        if (CurrentFrameIndex == 0)
        {
            JE_CORE_WARN("Swapchain suboptimal on first frame, skipping...");
            vkDeviceWaitIdle(GetDevice()->GetLogicalDevice());
            RecreateSwapChain();
            Skip = true;
            CurrentFrameIndex++;
            return;
        }
        FramebufferResized = true;
        Skip = true;
        CurrentFrameIndex++;
        return;
    }

    static auto StartTime = std::chrono::high_resolution_clock::now();
    auto CurrentTime = std::chrono::high_resolution_clock::now();
    float Time = std::chrono::duration<float, std::chrono::seconds::period>(CurrentTime - StartTime).count();

    Ubo.Model = glm::rotate(glm::mat4(1.0f), Time * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Ubo.View = glm::lookAt(glm::vec3(0.0f, 2.0f, -6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Ubo.Proj = glm::perspective(
        glm::radians(60.0f), SwapChain.GetExtent().width / (float) SwapChain.GetExtent().height, 0.1f, 100.0f);
    Ubo.Proj[1][1] *= -1;

    UniformBuffer->UploadData(sizeof(Ubo), &Ubo);

    RecordCommandBuffer(CommandBuffers[CurrentSyncIndex]);
    CurrentFrameIndex++;
}
VulkanDevice* VulkanContext::GetDevice() const
{
    return m_DeviceManager->GetDevice();
}
VkInstance VulkanContext::GetInstance() const
{
    return m_DeviceManager->GetInstance();
}
VulkanSwapChain& VulkanContext::GetSwapchain()
{
    return SwapChain;
}
std::shared_ptr<VulkanRenderPass> VulkanContext::GetRenderPass() const
{
    return RenderPass;
}
VkRenderPass VulkanContext::GetImGuiRenderPass() const
{
    return ImGuiRenderPass;
}
VkCommandPool VulkanContext::GetCommandPool() const
{
    return CommandPool;
}

VulkanContext::~VulkanContext() {}

void VulkanContext::CreateGraphicsPipeline()
{
    auto VertShaderCode = VulkanShader::ReadFile("Assets/Shaders/vert.spv");
    auto FragShaderCode = VulkanShader::ReadFile("Assets/Shaders/frag.spv");

    VkShaderModule VertShaderModule = VulkanShader::CreateShaderModule(GetDevice()->GetLogicalDevice(), VertShaderCode);
    VkShaderModule FragShaderModule = VulkanShader::CreateShaderModule(GetDevice()->GetLogicalDevice(), FragShaderCode);

    VkPipelineShaderStageCreateInfo VertShaderStageInfo {};
    VertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    VertShaderStageInfo.module = VertShaderModule;
    VertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = FragShaderModule;
    fragShaderStageInfo.pName = "main";

    PipelineCreateInfo Info = {};
    Info.RenderPass = RenderPass;
    Info.VulkanShaderModules = std::vector {VertShaderStageInfo, fragShaderStageInfo};
    GraphicsPipeline.Create(Info);

    vkDestroyShaderModule(GetDevice()->GetLogicalDevice(), FragShaderModule, nullptr);
    vkDestroyShaderModule(GetDevice()->GetLogicalDevice(), VertShaderModule, nullptr);
}

void VulkanContext::CreateViewportGraphicsPipeline()
{
    if (ViewportRenderPass == VK_NULL_HANDLE)
        return;

    auto VertShaderCode = VulkanShader::ReadFile("Assets/Shaders/vert.spv");
    auto FragShaderCode = VulkanShader::ReadFile("Assets/Shaders/frag.spv");

    VkShaderModule VertShaderModule = VulkanShader::CreateShaderModule(GetDevice()->GetLogicalDevice(), VertShaderCode);
    VkShaderModule FragShaderModule = VulkanShader::CreateShaderModule(GetDevice()->GetLogicalDevice(), FragShaderCode);

    VkPipelineShaderStageCreateInfo VertShaderStageInfo {};
    VertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    VertShaderStageInfo.module = VertShaderModule;
    VertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = FragShaderModule;
    fragShaderStageInfo.pName = "main";

    std::shared_ptr<VulkanRenderPass> ViewportRP = std::make_shared<VulkanRenderPass>();
    ViewportRP->SetRenderPass(ViewportRenderPass);

    PipelineCreateInfo Info = {};
    Info.RenderPass = ViewportRP;
    Info.VulkanShaderModules = std::vector {VertShaderStageInfo, fragShaderStageInfo};
    ViewportGraphicsPipeline.Create(Info);

    vkDestroyShaderModule(GetDevice()->GetLogicalDevice(), FragShaderModule, nullptr);
    vkDestroyShaderModule(GetDevice()->GetLogicalDevice(), VertShaderModule, nullptr);
}

void VulkanContext::CreateImGuiRenderPass()
{
    VkAttachmentDescription ColorAttachment {};
    ColorAttachment.format = SwapChain.GetFormat();
    ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference ColorAttachmentRef {};
    ColorAttachmentRef.attachment = 0;
    ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription Subpass {};
    Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    Subpass.colorAttachmentCount = 1;
    Subpass.pColorAttachments = &ColorAttachmentRef;

    VkSubpassDependency Dependencies[2] {};
    Dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    Dependencies[0].dstSubpass = 0;
    Dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    Dependencies[0].srcAccessMask = 0;
    Dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    Dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    Dependencies[1].srcSubpass = 0;
    Dependencies[1].dstSubpass = 0;
    Dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    Dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    Dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    Dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    Dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo RenderPassInfo {};
    RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassInfo.attachmentCount = 1;
    RenderPassInfo.pAttachments = &ColorAttachment;
    RenderPassInfo.subpassCount = 1;
    RenderPassInfo.pSubpasses = &Subpass;
    RenderPassInfo.dependencyCount = 2;
    RenderPassInfo.pDependencies = Dependencies;

    VkResult Result = vkCreateRenderPass(GetDevice()->GetLogicalDevice(), &RenderPassInfo, nullptr, &ImGuiRenderPass);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create ImGui render pass!")
}

void VulkanContext::CreateImGuiFramebuffers()
{
    const auto& ImageViews = SwapChain.GetSwapChainImageViews();
    ImGuiFramebuffers.resize(ImageViews.size());

    for (size_t i = 0; i < ImageViews.size(); i++)
    {
        VkFramebufferCreateInfo FramebufferInfo {};
        FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FramebufferInfo.renderPass = ImGuiRenderPass;
        FramebufferInfo.attachmentCount = 1;
        FramebufferInfo.pAttachments = &ImageViews[i];
        FramebufferInfo.width = SwapChain.GetExtent().width;
        FramebufferInfo.height = SwapChain.GetExtent().height;
        FramebufferInfo.layers = 1;

        VkResult Result =
            vkCreateFramebuffer(GetDevice()->GetLogicalDevice(), &FramebufferInfo, nullptr, &ImGuiFramebuffers[i]);
        JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create ImGui framebuffer!")
    }
}

void VulkanContext::DestroyImGuiFramebuffers()
{
    for (auto Framebuffer : ImGuiFramebuffers)
    {
        vkDestroyFramebuffer(GetDevice()->GetLogicalDevice(), Framebuffer, nullptr);
    }
    ImGuiFramebuffers.clear();
}

void VulkanContext::RecordCommandBuffer(VulkanRenderCommandBuffer& CB)
{
    CB.Begin();

    if (ViewportFramebuffer != VK_NULL_HANDLE && ViewportSize.width > 0 && ViewportSize.height > 0)
    {
        VkRenderPassBeginInfo ViewportRPInfo {};
        ViewportRPInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        ViewportRPInfo.renderPass = ViewportRenderPass;
        ViewportRPInfo.framebuffer = ViewportFramebuffer;
        ViewportRPInfo.renderArea.offset = {0, 0};
        ViewportRPInfo.renderArea.extent = ViewportSize;

        std::array<VkClearValue, 2> ViewportClearValues {};
        ViewportClearValues[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
        ViewportClearValues[1].depthStencil = {1.0f, 0};
        ViewportRPInfo.clearValueCount = static_cast<uint32_t>(ViewportClearValues.size());
        ViewportRPInfo.pClearValues = ViewportClearValues.data();

        vkCmdBeginRenderPass(CB.GetCommandBuffer(), &ViewportRPInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport VP {};
        VP.x = 0.0f;
        VP.y = 0.0f;
        VP.width = static_cast<float>(ViewportSize.width);
        VP.height = static_cast<float>(ViewportSize.height);
        VP.minDepth = 0.0f;
        VP.maxDepth = 1.0f;
        vkCmdSetViewport(CB.GetCommandBuffer(), 0, 1, &VP);

        VkRect2D ScissorVP {.offset = {0, 0}, .extent = ViewportSize};
        vkCmdSetScissor(CB.GetCommandBuffer(), 0, 1, &ScissorVP);

        VkDeviceSize Offsets[] = {0};
        vkCmdBindVertexBuffers(CB.GetCommandBuffer(), 0, 1, &VertexBuffer->GetBuffer(), Offsets);
        vkCmdBindIndexBuffer(CB.GetCommandBuffer(), IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdBindDescriptorSets(CB.GetCommandBuffer(),
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                ViewportGraphicsPipeline.GetPipelineLayout(),
                                0,
                                1,
                                &DescriptorSet,
                                0,
                                nullptr);
        ViewportGraphicsPipeline.Bind(CB.GetCommandBuffer());
        vkCmdDrawIndexed(CB.GetCommandBuffer(), static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(CB.GetCommandBuffer());
    }

    RenderPass->Begin(CB.GetCommandBuffer(), SwapChain.GetExtent());
    RenderPass->End(CB.GetCommandBuffer());

    if (ImGui::GetDrawData())
    {
        VkImageMemoryBarrier Barrier {};
        Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        Barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        Barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        Barrier.image = SwapChain.GetSwapChainImages()[RenderPass->SwapChainImageIndex];
        Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Barrier.subresourceRange.baseMipLevel = 0;
        Barrier.subresourceRange.levelCount = 1;
        Barrier.subresourceRange.baseArrayLayer = 0;
        Barrier.subresourceRange.layerCount = 1;
        Barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(CB.GetCommandBuffer(),
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &Barrier);

        VkRenderPassBeginInfo RenderPassInfo {};
        RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassInfo.renderPass = ImGuiRenderPass;
        RenderPassInfo.framebuffer = ImGuiFramebuffers[RenderPass->SwapChainImageIndex];
        RenderPassInfo.renderArea.offset = {0, 0};
        RenderPassInfo.renderArea.extent = SwapChain.GetExtent();
        RenderPassInfo.clearValueCount = 0;

        vkCmdBeginRenderPass(CB.GetCommandBuffer(), &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), CB.GetCommandBuffer());
        vkCmdEndRenderPass(CB.GetCommandBuffer());
    }

    CB.End();
}

void VulkanContext::CreateSyncObjects()
{
    VkSemaphoreCreateInfo SemaphoreInfo {};
    SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo FenceInfo {};
    FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(GetDevice()->GetLogicalDevice(), &SemaphoreInfo, nullptr, &ImageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateSemaphore(GetDevice()->GetLogicalDevice(), &SemaphoreInfo, nullptr, &RenderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
            vkCreateFence(GetDevice()->GetLogicalDevice(), &FenceInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS)
        {
            JE_CORE_ASSERT(false, "Failed to create synchronization objects for a frame!")
        }
    }
}

void VulkanContext::RecreateSwapChain()
{
    for (auto Semaphore : ImageAvailableSemaphores)
        vkDestroySemaphore(GetDevice()->GetLogicalDevice(), Semaphore, nullptr);
    for (auto Semaphore : RenderFinishedSemaphores)
        vkDestroySemaphore(GetDevice()->GetLogicalDevice(), Semaphore, nullptr);
    for (auto Fence : InFlightFences)
        vkDestroyFence(GetDevice()->GetLogicalDevice(), Fence, nullptr);

    GraphicsPipeline.Shutdown();
    ViewportGraphicsPipeline.Shutdown();
    RenderPass->Shutdown();
    DestroyImGuiFramebuffers();
    SwapChain.Shutdown();

    vkDestroyImageView(GetDevice()->GetLogicalDevice(), DepthImageView, nullptr);
    vkDestroyImage(GetDevice()->GetLogicalDevice(), DepthImage, nullptr);
    vkFreeMemory(GetDevice()->GetLogicalDevice(), DepthImageMemory, nullptr);

    SwapChain.Init(m_DeviceManager->GetSurface()->GetSurface(), WindowPtr);
    CreateDepthResources();
    RenderPass->CreateRenderPass(SwapChain.GetFormat(), SwapChain.GetSwapChainImageViews(), SwapChain.GetExtent());
    CreateImGuiFramebuffers();
    CreateGraphicsPipeline();
    CreateViewportGraphicsPipeline();
    CreateSyncObjects();

    vkResetCommandPool(GetDevice()->GetLogicalDevice(), CommandPool, 0);
}

void VulkanContext::CreateDescriptorPool()
{
    VkDescriptorPoolSize PoolSizes[] = {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2},
                                        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2}};

    VkDescriptorPoolCreateInfo PoolInfo {};
    PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    PoolInfo.flags = 0;
    PoolInfo.poolSizeCount = 2;
    PoolInfo.pPoolSizes = PoolSizes;
    PoolInfo.maxSets = 2;

    auto Result = vkCreateDescriptorPool(GetDevice()->GetLogicalDevice(), &PoolInfo, nullptr, &DescriptorPool);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create descriptor pool!")
}

void VulkanContext::CreateDescriptorSets()
{
    std::array<VkDescriptorSetLayout, 1> SetLayouts = {GraphicsPipeline.GetDescriptorSetLayout()};

    VkDescriptorSetAllocateInfo AllocInfo {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocInfo.descriptorPool = DescriptorPool;
    AllocInfo.descriptorSetCount = 1;
    AllocInfo.pSetLayouts = SetLayouts.data();

    auto Result = vkAllocateDescriptorSets(GetDevice()->GetLogicalDevice(), &AllocInfo, &DescriptorSet);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create descriptor set!")
    JE_CORE_ASSERT(DescriptorSet != VK_NULL_HANDLE, "DescriptorSet is null after allocation!")
    JE_CORE_ASSERT(GraphicsPipeline.GetDescriptorSetLayout() != VK_NULL_HANDLE, "DescriptorSetLayout is null!")

    VkDescriptorBufferInfo BufferInfo = {
        .buffer = UniformBuffer->GetBuffer(), .offset = 0, .range = sizeof(UniformBufferObject)};

    VkDescriptorImageInfo ImageInfo {};
    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    ImageInfo.imageView = Texture->GetImageView();
    ImageInfo.sampler = Texture->GetSampler();

    std::array<VkWriteDescriptorSet, 2> DescriptorWrites {};

    DescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    DescriptorWrites[0].dstSet = DescriptorSet;
    DescriptorWrites[0].dstBinding = 0;
    DescriptorWrites[0].dstArrayElement = 0;
    DescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    DescriptorWrites[0].descriptorCount = 1;
    DescriptorWrites[0].pBufferInfo = &BufferInfo;

    DescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    DescriptorWrites[1].dstSet = DescriptorSet;
    DescriptorWrites[1].dstBinding = 1;
    DescriptorWrites[1].dstArrayElement = 0;
    DescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    DescriptorWrites[1].descriptorCount = 1;
    DescriptorWrites[1].pImageInfo = &ImageInfo;

    vkUpdateDescriptorSets(
        GetDevice()->GetLogicalDevice(), DescriptorWrites.size(), DescriptorWrites.data(), 0, nullptr);
}
void VulkanContext::CreateDepthResources()
{
    VkFormat DepthFormat = FindDepthFormat();
    VulkanTexture2D::CreateImage(SwapChain.GetExtent().width,
                                 SwapChain.GetExtent().height,
                                 DepthFormat,
                                 VK_IMAGE_TILING_OPTIMAL,
                                 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                 DepthImage,
                                 DepthImageMemory);
    DepthImageView = VulkanTexture2D::CreateImageView(DepthImage, DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}
VkFormat VulkanContext::FindDepthFormat()
{
    return FindSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
VkImageView& VulkanContext::GetDepthImageView()
{
    return DepthImageView;
}
VkFormat VulkanContext::FindSupportedFormat(const std::vector<VkFormat>& Candidates,
                                            VkImageTiling Tiling,
                                            VkFormatFeatureFlags Features)
{
    for (VkFormat Format : Candidates)
    {
        VkFormatProperties Props;
        vkGetPhysicalDeviceFormatProperties(GetDevice()->GetPhysicalDevice(), Format, &Props);

        if (Tiling == VK_IMAGE_TILING_LINEAR && (Props.linearTilingFeatures & Features) == Features)
        {
            return Format;
        }
        else if (Tiling == VK_IMAGE_TILING_OPTIMAL && (Props.optimalTilingFeatures & Features) == Features)
        {
            return Format;
        }
    }

    JE_CORE_ASSERT(false, "Failed to find supported format!")
    return VK_FORMAT_UNDEFINED;
}

void VulkanContext::CreateViewportRenderPass()
{
    VkFormat ColorFormat = SwapChain.GetFormat();
    VkFormat DepthFormat = FindDepthFormat();

    VkAttachmentDescription ColorAttachment {};
    ColorAttachment.format = ColorFormat;
    ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference ColorAttachmentRef {};
    ColorAttachmentRef.attachment = 0;
    ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription DepthAttachment {};
    DepthAttachment.format = DepthFormat;
    DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference DepthAttachmentRef {};
    DepthAttachmentRef.attachment = 1;
    DepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription Subpass {};
    Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    Subpass.colorAttachmentCount = 1;
    Subpass.pColorAttachments = &ColorAttachmentRef;
    Subpass.pDepthStencilAttachment = &DepthAttachmentRef;

    VkSubpassDependency Dependency {};
    Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    Dependency.dstSubpass = 0;
    Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    Dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    Dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription Attachments[] = {ColorAttachment, DepthAttachment};
    VkRenderPassCreateInfo RenderPassInfo {};
    RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassInfo.attachmentCount = 2;
    RenderPassInfo.pAttachments = Attachments;
    RenderPassInfo.subpassCount = 1;
    RenderPassInfo.pSubpasses = &Subpass;
    RenderPassInfo.dependencyCount = 1;
    RenderPassInfo.pDependencies = &Dependency;

    VkResult Result =
        vkCreateRenderPass(GetDevice()->GetLogicalDevice(), &RenderPassInfo, nullptr, &ViewportRenderPass);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create viewport render pass!")
}

void VulkanContext::CreateViewportFramebuffer(uint32_t Width, uint32_t Height)
{
    DestroyViewportFramebuffer();

    VkFormat ColorFormat = SwapChain.GetFormat();
    VkFormat DepthFormat = FindDepthFormat();

    VulkanTexture2D::CreateImage(Width,
                                 Height,
                                 ColorFormat,
                                 VK_IMAGE_TILING_OPTIMAL,
                                 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                     VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                 ViewportImage,
                                 ViewportImageMemory);

    ViewportImageView = VulkanTexture2D::CreateImageView(ViewportImage, ColorFormat, VK_IMAGE_ASPECT_COLOR_BIT);

    VulkanTexture2D::CreateImage(Width,
                                 Height,
                                 DepthFormat,
                                 VK_IMAGE_TILING_OPTIMAL,
                                 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                 ViewportDepthImage,
                                 ViewportDepthImageMemory);

    ViewportDepthImageView =
        VulkanTexture2D::CreateImageView(ViewportDepthImage, DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    VkImageView FramebufferAttachments[] = {ViewportImageView, ViewportDepthImageView};
    VkFramebufferCreateInfo FramebufferInfo {};
    FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    FramebufferInfo.renderPass = ViewportRenderPass;
    FramebufferInfo.attachmentCount = 2;
    FramebufferInfo.pAttachments = FramebufferAttachments;
    FramebufferInfo.width = Width;
    FramebufferInfo.height = Height;
    FramebufferInfo.layers = 1;

    VkResult Result =
        vkCreateFramebuffer(GetDevice()->GetLogicalDevice(), &FramebufferInfo, nullptr, &ViewportFramebuffer);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create viewport framebuffer!")

    VkSamplerCreateInfo SamplerInfo {};
    SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    SamplerInfo.magFilter = VK_FILTER_LINEAR;
    SamplerInfo.minFilter = VK_FILTER_LINEAR;
    SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    SamplerInfo.anisotropyEnable = VK_FALSE;
    SamplerInfo.maxAnisotropy = 1.0f;
    SamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    SamplerInfo.unnormalizedCoordinates = VK_FALSE;
    SamplerInfo.compareEnable = VK_FALSE;
    SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    Result = vkCreateSampler(GetDevice()->GetLogicalDevice(), &SamplerInfo, nullptr, &ViewportSampler);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create viewport sampler!")

    VkCommandBuffer Cmd = RHI::Vulkan::VulkanUtils::BeginSingleTimeCommands();
    VkImageMemoryBarrier Barrier {};
    Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    Barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    Barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    Barrier.image = ViewportImage;
    Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    Barrier.subresourceRange.baseMipLevel = 0;
    Barrier.subresourceRange.levelCount = 1;
    Barrier.subresourceRange.baseArrayLayer = 0;
    Barrier.subresourceRange.layerCount = 1;
    Barrier.srcAccessMask = 0;
    Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
    vkCmdPipelineBarrier(Cmd,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &Barrier);
    RHI::Vulkan::VulkanUtils::EndSingleTimeCommands(Cmd);

    ViewportDescriptorSet = ImGui_ImplVulkan_AddTexture(ViewportSampler, ViewportImageView, VK_IMAGE_LAYOUT_GENERAL);
    JE_CORE_ASSERT(ViewportDescriptorSet != VK_NULL_HANDLE, "Failed to create viewport descriptor set!")

    ViewportSize = {Width, Height};
}

void VulkanContext::DestroyViewportFramebuffer()
{
    if (ViewportDescriptorSet != VK_NULL_HANDLE)
    {
        ImGui_ImplVulkan_RemoveTexture(ViewportDescriptorSet);
        ViewportDescriptorSet = VK_NULL_HANDLE;
    }
    if (ViewportSampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(GetDevice()->GetLogicalDevice(), ViewportSampler, nullptr);
        ViewportSampler = VK_NULL_HANDLE;
    }
    if (ViewportFramebuffer != VK_NULL_HANDLE)
    {
        vkDestroyFramebuffer(GetDevice()->GetLogicalDevice(), ViewportFramebuffer, nullptr);
        ViewportFramebuffer = VK_NULL_HANDLE;
    }
    if (ViewportImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(GetDevice()->GetLogicalDevice(), ViewportImageView, nullptr);
        ViewportImageView = VK_NULL_HANDLE;
    }
    if (ViewportImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(GetDevice()->GetLogicalDevice(), ViewportImage, nullptr);
        ViewportImage = VK_NULL_HANDLE;
    }
    if (ViewportImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(GetDevice()->GetLogicalDevice(), ViewportImageMemory, nullptr);
        ViewportImageMemory = VK_NULL_HANDLE;
    }
    if (ViewportDepthImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(GetDevice()->GetLogicalDevice(), ViewportDepthImageView, nullptr);
        ViewportDepthImageView = VK_NULL_HANDLE;
    }
    if (ViewportDepthImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(GetDevice()->GetLogicalDevice(), ViewportDepthImage, nullptr);
        ViewportDepthImage = VK_NULL_HANDLE;
    }
    if (ViewportDepthImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(GetDevice()->GetLogicalDevice(), ViewportDepthImageMemory, nullptr);
        ViewportDepthImageMemory = VK_NULL_HANDLE;
    }
}

void VulkanContext::ResizeViewportFramebuffer(uint32_t Width, uint32_t Height)
{
    if (Width == 0 || Height == 0)
        return;
    if (ViewportSize.width == Width && ViewportSize.height == Height)
        return;

    vkDeviceWaitIdle(GetDevice()->GetLogicalDevice());

    vkDestroyRenderPass(GetDevice()->GetLogicalDevice(), ViewportRenderPass, nullptr);
    CreateViewportRenderPass();

    ViewportGraphicsPipeline.Shutdown();
    CreateViewportGraphicsPipeline();

    CreateViewportFramebuffer(Width, Height);
}

VkImageView VulkanContext::GetViewportImageView() const
{
    return ViewportImageView;
}

VkSampler VulkanContext::GetViewportSampler() const
{
    return ViewportSampler;
}

VkDescriptorSet VulkanContext::GetViewportDescriptorSet() const
{
    return ViewportDescriptorSet;
}

VkRenderPass VulkanContext::GetViewportRenderPass() const
{
    return ViewportRenderPass;
}

VkFramebuffer VulkanContext::GetViewportFramebuffer() const
{
    return ViewportFramebuffer;
}

VkExtent2D VulkanContext::GetViewportSize() const
{
    return ViewportSize;
}
} // namespace JuicyEngine