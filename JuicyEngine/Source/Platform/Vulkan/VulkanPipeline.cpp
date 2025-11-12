#include "VulkanPipeline.h"

#include "VulkanContext.h"
#include "Core/Core.h"
#include "Renderer/Mesh.h"
#include "Renderer/Pipeline.h"
#include <array>


namespace JuicyEngine
{
	void VulkanPipeline::Create(const PipelineCreateInfo& Info)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

		VkVertexInputBindingDescription BindingDescriptions = {
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
		};

		std::array<VkVertexInputAttributeDescription, 3> AttributeDescriptions {};

		AttributeDescriptions[0].binding = 0;
		AttributeDescriptions[0].location = 0;
		AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[0].offset = offsetof(Vertex, Position);

		AttributeDescriptions[1].binding = 0;
		AttributeDescriptions[1].location = 1;
		AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[1].offset = offsetof(Vertex, Color);

		AttributeDescriptions[2].binding = 0;
		AttributeDescriptions[2].location = 2;
		AttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		AttributeDescriptions[2].offset = offsetof(Vertex, TexCoord);

		VkPipelineVertexInputStateCreateInfo VertexInputInfo {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &BindingDescriptions,
			.vertexAttributeDescriptionCount = AttributeDescriptions.size(),
			.pVertexAttributeDescriptions = AttributeDescriptions.data(),
		};

		VkPipelineInputAssemblyStateCreateInfo InputAssembly {};
		InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		InputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo ViewportState {};
		ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportState.viewportCount = 1;
		ViewportState.scissorCount = 1;
		VkPipelineRasterizationStateCreateInfo Rasterizer {};
		Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		Rasterizer.depthClampEnable = VK_FALSE;
		Rasterizer.rasterizerDiscardEnable = VK_FALSE;
		Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		Rasterizer.lineWidth = 1.0f;
		Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		Rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo Multisampling {};
		Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		Multisampling.sampleShadingEnable = VK_FALSE;
		Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		
		VkPipelineDepthStencilStateCreateInfo DepthStencil{};
		DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		DepthStencil.depthTestEnable = VK_TRUE;
		DepthStencil.depthWriteEnable = VK_TRUE;
		DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		DepthStencil.depthBoundsTestEnable = VK_FALSE;
		DepthStencil.stencilTestEnable = VK_FALSE;
		

		VkPipelineColorBlendAttachmentState ColorBlendAttachment {};
		ColorBlendAttachment.colorWriteMask
		    = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo ColorBlending {};
		ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		ColorBlending.logicOpEnable = VK_FALSE;
		ColorBlending.logicOp = VK_LOGIC_OP_COPY;
		ColorBlending.attachmentCount = 1;
		ColorBlending.pAttachments = &ColorBlendAttachment;
		ColorBlending.blendConstants[0] = 0.0f;
		ColorBlending.blendConstants[1] = 0.0f;
		ColorBlending.blendConstants[2] = 0.0f;
		ColorBlending.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> DynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo DynamicState {};
		DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
		DynamicState.pDynamicStates = DynamicStates.data();

		VkDescriptorSetLayoutBinding UboLayoutBinding{};
		UboLayoutBinding.binding = 0;
		UboLayoutBinding.descriptorCount = 1;
		UboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		UboLayoutBinding.pImmutableSamplers = nullptr;
		UboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		
		VkDescriptorSetLayoutBinding SamplerLayoutBinding{};
		SamplerLayoutBinding.binding = 1;
		SamplerLayoutBinding.descriptorCount = 1;
		SamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		SamplerLayoutBinding.pImmutableSamplers = nullptr;
		SamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		
		std::array<VkDescriptorSetLayoutBinding, 2> Bindings = {UboLayoutBinding, SamplerLayoutBinding};
		
		VkDescriptorSetLayoutCreateInfo LayoutInfo{};
		LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
		LayoutInfo.pBindings = Bindings.data();

		auto CreateDescriptorResult = vkCreateDescriptorSetLayout( Context->GetDevice()->GetLogicalDevice(), &LayoutInfo, nullptr, &DescriptorSetLayout);
		JE_CORE_ASSERT(CreateDescriptorResult == VK_SUCCESS, "Failed to create descriptor set layout!")
		
		VkPipelineLayoutCreateInfo PipelineLayoutInfo {};
		PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutInfo.setLayoutCount = 1;
		PipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
		PipelineLayoutInfo.pushConstantRangeCount = 0;

		auto PipelineLayoutResult = vkCreatePipelineLayout(
		    Context->GetDevice()->GetLogicalDevice(), &PipelineLayoutInfo, nullptr, &PipelineLayout);

		JE_CORE_ASSERT(PipelineLayoutResult == VK_SUCCESS, "Failed to create pipeline layout!")

		VkGraphicsPipelineCreateInfo PipelineInfo {};
		PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		PipelineInfo.stageCount = Info.VulkanShaderModules.size();
		PipelineInfo.pStages = Info.VulkanShaderModules.data();
		PipelineInfo.pVertexInputState = &VertexInputInfo;
		PipelineInfo.pInputAssemblyState = &InputAssembly;
		PipelineInfo.pViewportState = &ViewportState;
		PipelineInfo.pRasterizationState = &Rasterizer;
		PipelineInfo.pMultisampleState = &Multisampling;
		PipelineInfo.pColorBlendState = &ColorBlending;
		PipelineInfo.pDynamicState = &DynamicState;
		PipelineInfo.layout = PipelineLayout;
		PipelineInfo.renderPass = Info.RenderPass->GetVulkanRenderPass();
		PipelineInfo.subpass = 0;
		PipelineInfo.pDepthStencilState = &DepthStencil;
		PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		
		auto Result = vkCreateGraphicsPipelines(
		    Context->GetDevice()->GetLogicalDevice(), VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &Pipeline);

		JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create graphics pipeline!")
	}

	void VulkanPipeline::Shutdown()
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		vkDestroyPipeline(Context->GetDevice()->GetLogicalDevice(), Pipeline, nullptr);
		vkDestroyPipelineLayout(Context->GetDevice()->GetLogicalDevice(), PipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(Context->GetDevice()->GetLogicalDevice(), DescriptorSetLayout, nullptr);
	}

	void VulkanPipeline::Bind(VkCommandBuffer CommandBuffer)
	{
		vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);
	}
	VkDescriptorSetLayout& VulkanPipeline::GetDescriptorSetLayout()
	{
		return DescriptorSetLayout;
	}
	VkPipelineLayout& VulkanPipeline::GetPipelineLayout()
	{
		return PipelineLayout;
	}
} // namespace JuicyEngine