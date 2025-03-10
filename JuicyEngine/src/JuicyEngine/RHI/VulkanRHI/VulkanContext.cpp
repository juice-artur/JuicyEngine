#include "jepch.h"
#include "VulkanContext.h"
#include "Platform/VulkanPlatform.h"
#include "VulkanShaderModule.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glm/gtx/euler_angles.hpp"

namespace JuicyEngine
{
VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
{
    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: JE_ERROR(callbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: JE_WARN(callbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: JE_INFO(callbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: JE_TRACE(callbackData->pMessage); break;

    }
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

VulkanContext::VulkanContext(Window* windowHandle) : m_Window(windowHandle)
{
    JE_CORE_ASSERT(windowHandle, "Window handle is null!");
}

VulkanContext::~VulkanContext()
{
    vkDeviceWaitIdle(m_Device.GetLogicalDevice());
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(m_Device.GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_Device.GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_Device.GetLogicalDevice(), inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(m_Device.GetLogicalDevice(), commandPool, nullptr);

    for (auto framebuffer : swapChainFramebuffers)
    {
        vkDestroyFramebuffer(m_Device.GetLogicalDevice(), framebuffer, nullptr);
    }

    vkDestroyPipeline(m_Device.GetLogicalDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_Device.GetLogicalDevice(), pipelineLayout, nullptr);
    vkDestroyRenderPass(m_Device.GetLogicalDevice(), renderPass, nullptr);
    m_Swapchain->Destroy();

    vkDestroyBuffer(m_Device.GetLogicalDevice(), m_VertexBuffer.Handle, nullptr);
    vkFreeMemory(m_Device.GetLogicalDevice(), m_VertexBuffer.Memory, nullptr);

    vkDestroyBuffer(m_Device.GetLogicalDevice(), m_IndexBuffer.Handle, nullptr);
    vkFreeMemory(m_Device.GetLogicalDevice(), m_IndexBuffer.Memory, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    m_Device.Cleanup();
    DestroyDebugUtilsMessengerEXT(m_Instance, debugMessenger, nullptr);
    vkDestroyInstance(m_Instance, nullptr);
}

void VulkanContext::Init()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    // TODO: Change this to the actual name of the application
    appInfo.pApplicationName = "JuicyEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "JuicyEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> extensions;
    GetVulkanPlatformRequiredExtensionNames(extensions);
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    // TODO: Add validation layers
#ifdef JE_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    unsigned int availableExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(0, &availableExtensionCount, 0);
    std::vector<VkExtensionProperties> availableExtensions;
    availableExtensions.resize(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(0, &availableExtensionCount, availableExtensions.data());

    // Verify required extensions are available.
    for (unsigned int i = 0; i < extensions.size(); ++i)
    {
        bool found = false;
        for (unsigned int j = 0; j < availableExtensionCount; ++j)
        {
            if (std::strcmp(extensions[i], availableExtensions[j].extensionName) == 0)
            {
                found = true;
                JE_INFO("Required exension found: {0}...", extensions[i]);
                break;
            }
        }

        if (!found)
        {
            JE_CORE_FATAL("Required extension is missing: {0}", extensions[i]);
        }
    }

    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    unsigned int requiredValidationLayerCount = requiredValidationLayers.size();

    // Obtain a list of available validation layers
    unsigned int availableLayerCount = 0;

    vkEnumerateInstanceLayerProperties(&availableLayerCount, 0);
    std::vector<VkLayerProperties> availableLayers;
    availableLayers.resize(availableLayerCount);
    vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

    // Verify all required layers are available.
    for (unsigned int i = 0; i < requiredValidationLayerCount; ++i)
    {
        bool found = false;
        for (unsigned int j = 0; j < availableLayerCount; ++j)
        {
            if (std::strcmp(requiredValidationLayers[i], availableLayers[j].layerName) == 0)
            {
                found = true;
                JE_CORE_INFO("Found validation layer: {0}...", requiredValidationLayers[i]);
                break;
            }
        }

        if (!found)
        {
            JE_CORE_FATAL("Required validation layer is missing: {0}", requiredValidationLayers[i]);
        }
    }

    createInfo.enabledLayerCount = requiredValidationLayers.size();
    createInfo.ppEnabledLayerNames = requiredValidationLayers.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    PopulateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = &debugCreateInfo;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
    JE_CORE_ASSERT("Failed to create Instance", result != VK_SUCCESS)

    SetupDebugMessenger();

    JE_CORE_TRACE("Creating Vulkan surface for window {0}...", m_Window->GetNativeWindow());
    if (!VulkanPlatformCreateSurface(m_Instance, &m_Surface, *m_Window))
    {
        JE_CORE_FATAL("Failed to create platform surface for window {0}...", m_Window->GetNativeWindow());
    }

    m_Device = VulkanDevice(&m_Instance, m_Surface);
    m_Device.Init();

    m_Swapchain = new VulkanSwapchain(m_Device, *m_Window, m_Surface);
    m_Swapchain->Create();

    CreateGraphicsPipelines();
    CreateFramebuffers();
    InitBuffers();

    CreateCommandBuffers();
}

void VulkanContext::SwapBuffers()
{
    JE_CORE_ASSERT(true, "Implement this!");
}

void VulkanContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = vkDebugCallback;
}

void VulkanContext::SetupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);
    if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {
        JE_CORE_FATAL("Failed to set up debug messenger!");
    }
}

void VulkanContext::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_Swapchain->swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    CreateCommandPool();

    VkResult result = vkCreateRenderPass(m_Device.GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass);
    JE_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass!");

    CreateSyncObjects();
}

void VulkanContext::CreateFramebuffers()
{
    swapChainFramebuffers.resize(m_Swapchain->swapChainImageViews.size());
    for (size_t i = 0; i < m_Swapchain->swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = {m_Swapchain->swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_Swapchain->swapChainExtent.width;
        framebufferInfo.height = m_Swapchain->swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_Device.GetLogicalDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VulkanContext::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = m_Device.FindQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_Device.GetLogicalDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        JE_CORE_ASSERT(false, "failed to create command pool!");
    }
}

void VulkanContext::CreateBuffer(Buffer& buffer, uint64_t newSize)
{

    VkBufferCreateInfo bufferCI = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferCI.size = newSize;
    bufferCI.usage = buffer.Usage;
    bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    JE_CORE_ASSERT(
        vkCreateBuffer(m_Device.GetLogicalDevice(), &bufferCI, nullptr, &buffer.Handle) == VK_SUCCESS, "failed to create buffer!");

    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(m_Device.GetLogicalDevice(), buffer.Handle, &req);
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = req.size;
    alloc_info.memoryTypeIndex = FindMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    JE_CORE_ASSERT(vkAllocateMemory(m_Device.GetLogicalDevice(), &alloc_info, nullptr, &buffer.Memory) == VK_SUCCESS,
        "failed to create Allocate memmory!");

    JE_CORE_ASSERT(
        vkBindBufferMemory(m_Device.GetLogicalDevice(), buffer.Handle, buffer.Memory, 0) == VK_SUCCESS, "failed to bind vertex buffer!");
    buffer.Size = req.size;
}

void VulkanContext::CreateCommandBuffers()
{
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    VkResult result = vkAllocateCommandBuffers(m_Device.GetLogicalDevice(), &allocInfo, commandBuffers.data());
    JE_CORE_ASSERT(result == VK_SUCCESS, "failed to create command pool!");
}

void VulkanContext::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        JE_CORE_ASSERT(false, "failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_Swapchain->swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    glm::vec3 position = glm::vec3(-1.0f, -2.0f, -3.0f);
    DrawSquare(commandBuffer, position);

    position = glm::vec3(0.0f, 0.0f, 0.0f);
    DrawSquare(commandBuffer, position);

    vkCmdEndRenderPass(commandBuffer);

    JE_CORE_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "failed to record command buffer!");
}

void VulkanContext::DrawSquare(VkCommandBuffer commandBuffer, glm::vec3 position)
{
    glm::vec3 translation = position * m_CubeScale;

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    glm::vec3 camPos = {0.f, 0.f, -2.f};

    glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);

    glm::mat4 projection = glm::perspective(
        glm::radians(70.f), (float)m_Swapchain->swapChainExtent.width / (float)m_Swapchain->swapChainExtent.height, 0.1f, 200.0f);
    projection[1][1] *= -1;

    m_PushConstants.ViewProjection = projection * view;

    m_PushConstants.Transform =
        glm::translate(glm::mat4(1.0f), translation) *
        glm::eulerAngleXYZ(glm::radians(m_CubeRotation.x), glm::radians(m_CubeRotation.y), glm::radians(m_CubeRotation.z));

    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &m_PushConstants);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_Swapchain->swapChainExtent.width;
    viewport.height = (float)m_Swapchain->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_Swapchain->swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_VertexBuffer.Handle, offsets);

    vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.Handle, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}

void VulkanContext::CreateSyncObjects()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(m_Device.GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_Device.GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_Device.GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {
            JE_CORE_ASSERT(false, "failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanContext::CreateGraphicsPipelines()
{

    VulkanShaderModule vertShaderModule(m_Device, "assets/shaders/vert.spv");
    VulkanShaderModule fragShaderModule(m_Device, "assets/shaders/frag.spv");

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = vertShaderModule.CreateShaderStage(VK_SHADER_STAGE_VERTEX_BIT);

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = fragShaderModule.CreateShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT);

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    std::vector dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_Swapchain->swapChainExtent.width);
    viewport.height = static_cast<float>(m_Swapchain->swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_Swapchain->swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;  // Optional
    rasterizer.depthBiasClamp = 0.0f;           // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;     // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;           // Optional
    multisampling.pSampleMask = nullptr;             // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
    multisampling.alphaToOneEnable = VK_FALSE;       // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;  // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;  // Optional
    colorBlending.blendConstants[1] = 0.0f;  // Optional
    colorBlending.blendConstants[2] = 0.0f;  // Optional
    colorBlending.blendConstants[3] = 0.0f;  // Optional

    std::array<VkPushConstantRange, 1> pushConstantRanges;
    pushConstantRanges[0].offset = 0;
    pushConstantRanges[0].size = sizeof(glm::mat4) * 2;
    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();
    pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)pushConstantRanges.size();

    VkResult resultPipeline = vkCreatePipelineLayout(m_Device.GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout);
    JE_CORE_ASSERT(resultPipeline == VK_SUCCESS, "failed to create pipeline layout!");

    CreateRenderPass();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.layout = pipelineLayout;

    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VkResult graphicsResult =
        vkCreateGraphicsPipelines(m_Device.GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
    JE_CORE_ASSERT(graphicsResult == VK_SUCCESS, "failed to create Graphics Pipelines!");
}

uint32_t VulkanContext::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_Device.GetPhysicalDevice(), &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanContext::InitBuffers()
{
    m_VertexBuffer.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    CreateBuffer(m_VertexBuffer, sizeof(vertexData[0]) * vertexData.size());

    m_IndexBuffer.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    CreateBuffer(m_IndexBuffer, sizeof(indices));

    glm::vec3* vbMemory = nullptr;
    JE_CORE_ASSERT(vkMapMemory(m_Device.GetLogicalDevice(), m_VertexBuffer.Memory, 0, sizeof(vertexData[0]) * vertexData.size(), 0,
                       (void**)&vbMemory) == VK_SUCCESS,
        "Failed to map vertex buffer memory")

    if (vbMemory) memcpy(vbMemory, vertexData.data(), sizeof(vertexData[0]) * vertexData.size());

    uint32_t* ibMemory = nullptr;
    JE_CORE_ASSERT(vkMapMemory(m_Device.GetLogicalDevice(), m_IndexBuffer.Memory, 0, sizeof(indices), 0, (void**)&ibMemory) == VK_SUCCESS,
        "Failed to map index buffer memory")

    if (ibMemory) memcpy(ibMemory, indices, sizeof(indices));

    VkMappedMemoryRange range[2] = {};
    range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range[0].memory = m_VertexBuffer.Memory;
    range[0].size = VK_WHOLE_SIZE;

    range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range[1].memory = m_IndexBuffer.Memory;
    range[1].size = VK_WHOLE_SIZE;  // ��� indices.size() * sizeof(uint32_t)

    JE_CORE_ASSERT(vkFlushMappedMemoryRanges(m_Device.GetLogicalDevice(), 2, range) == VK_SUCCESS, "Failed to flush memory");

    vkUnmapMemory(m_Device.GetLogicalDevice(), m_VertexBuffer.Memory);
    vkUnmapMemory(m_Device.GetLogicalDevice(), m_IndexBuffer.Memory);
}

void VulkanContext::DrawFrame()
{
    vkWaitForFences(m_Device.GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(m_Device.GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_Device.GetLogicalDevice(), m_Swapchain->m_SwapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    RecordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_Device.m_GraphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        JE_CORE_ASSERT(false, "failed to submit draw command buffer!")
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_Swapchain->m_SwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(m_Device.m_PresentQueue, &presentInfo);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanContext::RecreateSwapchain()
{
    vkDeviceWaitIdle(m_Device.GetLogicalDevice());
    for (auto framebuffer : swapChainFramebuffers)
    {
        vkDestroyFramebuffer(m_Device.GetLogicalDevice(), framebuffer, nullptr);
    }
    m_Swapchain->RecreateSwapchain();
    CreateFramebuffers();
}
}  // namespace JuicyEngine