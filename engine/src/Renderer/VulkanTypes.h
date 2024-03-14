#pragma once
#include"Core\Log.h"
#include "vulkan\vulkan.h"
#include "Platform\Common\Types.h"
#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			LOG_CRITICAL("Detected Vulkan error: {0}" , err  ); \
			abort();                                                \
		}                                                           \
	} while (0)
