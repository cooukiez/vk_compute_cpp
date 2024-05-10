//
// Created by Ludw on 4/25/2024.
//

#include "../app.h"

void App::create_cmd_pool() {
    VkCommandPoolCreateInfo cmd_pool_info{};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmd_pool_info.queueFamilyIndex = qf_indices.qf_comp.value();

    if (vkCreateCommandPool(dev, &cmd_pool_info, nullptr, &cmd_pool) != VK_SUCCESS)
        throw std::runtime_error("failed to create graphics command pool.");
}

VkCommandBuffer App::begin_single_time_cmd() {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = cmd_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer cmd_buf;
    vkAllocateCommandBuffers(dev, &alloc_info, &cmd_buf);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd_buf, &beginInfo);

    return cmd_buf;
}

void App::end_single_time_cmd(VkCommandBuffer cmd_buf) {
    vkEndCommandBuffer(cmd_buf);

    VkSubmitInfo submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd_buf;

    vkQueueSubmit(q_comp, 1, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(q_comp);

    vkFreeCommandBuffers(dev, cmd_pool, 1, &cmd_buf);
}
