//
// Created by Ludw on 4/25/2024.
//

#ifndef VCW_PROP_H
#define VCW_PROP_H

#define APP_NAME "Vulkan App"
#define ENGINE_NAME "No Engine"

// #define VERBOSE
#define VALIDATION

const std::vector<const char *> val_layers = {
        "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char *> dev_exts = {
        // VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME // not required anymore
};

const VkComponentMapping DEFAULT_COMPONENT_MAPPING = {.r = VK_COMPONENT_SWIZZLE_IDENTITY, .g = VK_COMPONENT_SWIZZLE_IDENTITY, .b = VK_COMPONENT_SWIZZLE_IDENTITY, .a = VK_COMPONENT_SWIZZLE_IDENTITY};
const VkImageSubresourceRange DEFAULT_SUBRESOURCE_RANGE = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1};
const VkImageSubresourceLayers DEFAULT_SUBRESOURCE_LAYERS = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};

const VkBorderColor DEFAULT_SAMPLER_BORDER_COLOR = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

#endif //VCW_PROP_H