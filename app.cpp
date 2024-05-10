//
// Created by Ludw on 4/25/2024.
//

#include "app.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

void App::init_app() {
    //
    // vulkan core initialization
    //
    create_inst();
    setup_debug_msg();

    pick_phy_dev();
    create_dev();

    //
    // compute initialization
    //
    create_desc_pool_layout();
    create_pipe();

    create_cmd_pool();

    create_data_buf();
    create_result_buf();

    create_desc_pool(1);

    write_desc_pool();
}


void App::create_data_buf() {
    VkDeviceSize buf_size = sizeof(glm::vec4) * 10;

    VCW_Buffer staging_buf = create_buf(buf_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    map_buf(&staging_buf);

    std::memset(staging_buf.p_mapped_mem, 0, static_cast<size_t>(buf_size));
    auto *voxels = static_cast<glm::vec4 *>(staging_buf.p_mapped_mem);

    // create some sample data
    for (size_t i = 0; i < 10; i++) {
        voxels[i] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    unmap_buf(&staging_buf);

    data_buf = create_buf(buf_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    cp_buf(staging_buf, data_buf);

    clean_up_buf(staging_buf);
}

void App::create_result_buf() {
    VkDeviceSize buf_size = sizeof(glm::vec4) * 10;

    result_buf = create_buf(buf_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void App::create_desc_pool_layout() {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding data_buf_layout_binding{};
    data_buf_layout_binding.binding = 0;
    data_buf_layout_binding.descriptorCount = 1;
    data_buf_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    data_buf_layout_binding.pImmutableSamplers = nullptr;
    data_buf_layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings.push_back(data_buf_layout_binding);

    VkDescriptorSetLayoutBinding result_buf_layout_binding{};
    result_buf_layout_binding.binding = 1;
    result_buf_layout_binding.descriptorCount = 1;
    result_buf_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    result_buf_layout_binding.pImmutableSamplers = nullptr;
    result_buf_layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings.push_back(result_buf_layout_binding);

    add_desc_set_layout(static_cast<uint32_t>(bindings.size()), bindings.data());

    add_pool_size(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
}

VkShaderModule App::create_shader_mod(const std::vector<char> &code) {
    VkShaderModuleCreateInfo mod_info{};
    mod_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    mod_info.codeSize = code.size();
    mod_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule mod;
    if (vkCreateShaderModule(dev, &mod_info, nullptr, &mod) != VK_SUCCESS)
        throw std::runtime_error("failed to create shader module.");

    return mod;
}

void App::create_pipe() {
    auto comp_code = read_file("comp.spv");

    VkShaderModule comp_module = create_shader_mod(comp_code);

    VkPipelineShaderStageCreateInfo comp_stage_info{};
    comp_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    comp_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    comp_stage_info.module = comp_module;
    comp_stage_info.pName = "main";

    VkPipelineLayoutCreateInfo pipe_layout_info{};
    pipe_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipe_layout_info.setLayoutCount = static_cast<uint32_t>(desc_set_layouts.size());
    pipe_layout_info.pSetLayouts = desc_set_layouts.data();

    if (vkCreatePipelineLayout(dev, &pipe_layout_info, nullptr, &pipe_layout) != VK_SUCCESS)
        throw std::runtime_error("failed to create pipeline layout.");

    VkComputePipelineCreateInfo pipe_info{};
    pipe_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipe_info.stage = comp_stage_info;

    pipe_info.layout = pipe_layout;
    pipe_info.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &pipe_info, nullptr, &pipe) != VK_SUCCESS)
        throw std::runtime_error("failed to create graphics pipeline.");

    vkDestroyShaderModule(dev, comp_module, nullptr);
}

void App::write_desc_pool() {
    write_buf_desc_binding(data_buf, 0, 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    write_buf_desc_binding(result_buf, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
}

void App::compute() {
    VkCommandBuffer cmd_buf = begin_single_time_cmd();

    vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, pipe);
    vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, pipe_layout, 0, 1, &desc_sets[0], 0, nullptr);

    vkCmdDispatch(cmd_buf, 1, 1, 1);

    end_single_time_cmd(cmd_buf);
}

void App::get_results() {
    map_buf(&result_buf);

    glm::vec4 *results;
    std::memcpy(&results, result_buf.p_mapped_mem, result_buf.size);

    unmap_buf(&result_buf);

    for (int i = 0; i < 10; i++) {
        std::cout << "result[" << i << "] = " << results[i] << std::endl;
    }
}

void App::clean_up() {
    vkDestroyPipeline(dev, pipe, nullptr);
    vkDestroyPipelineLayout(dev, pipe_layout, nullptr);

    clean_up_desc();

    vkDestroyCommandPool(dev, cmd_pool, nullptr);
    vkDestroyDevice(dev, nullptr);

#ifdef VALIDATION
    destroy_debug_callback(inst, debug_msg, nullptr);
#endif

    vkDestroyInstance(inst, nullptr);
}