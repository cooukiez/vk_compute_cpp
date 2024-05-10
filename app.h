//
// Created by Ludw on 4/25/2024.
//

#include "inc.h"
#include "prop.h"
#include "util.h"

#ifndef VCW_APP_H
#define VCW_APP_H

//
// vulkan debug
//
VkResult create_debug_callback(VkInstance inst, const VkDebugUtilsMessengerCreateInfoEXT *p_create_inf,
                               const VkAllocationCallbacks *p_alloc, VkDebugUtilsMessengerEXT *p_debug_msg);

void destroy_debug_callback(VkInstance inst, VkDebugUtilsMessengerEXT debug_msg, const VkAllocationCallbacks *p_alloc);

void get_debug_msg_info(VkDebugUtilsMessengerCreateInfoEXT &debug_info);

VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT msg_type,
               const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, void *p_user_data);

bool check_validation_support();

struct VCW_QueueFamilyIndices {
    std::optional<uint32_t> qf_comp;

    bool is_complete() {
        return qf_comp.has_value();
    }
};

struct VCW_Buffer {
    VkDeviceSize size;
    VkBuffer buf;
    VkDeviceMemory mem;
    void *p_mapped_mem = nullptr;
};

struct VCW_Image {
    VkImage img;
    VkDeviceMemory mem;

    VkImageView view;

    VkSampler sampler;
    bool has_sampler = false;

    VkExtent3D extent;
    VkFormat format;
    VkImageLayout cur_layout;
};

class App {
public:
    void run() {
        init_app();
        compute();
        get_results();
        clean_up();
    }

    VkInstance inst;
    VkDebugUtilsMessengerEXT debug_msg;

    VkPhysicalDevice phy_dev = VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties phy_dev_mem_props;
    VkPhysicalDeviceProperties phy_dev_props;

    std::vector<VkQueueFamilyProperties> qf_props;
    VCW_QueueFamilyIndices qf_indices;
    VkDevice dev;
    VkQueue q_comp;

    VkPipelineLayout pipe_layout;
    VkPipeline pipe;

    std::vector<VkDescriptorSetLayout> desc_set_layouts;
    std::vector<VkDescriptorPoolSize> desc_pool_sizes;
    VkDescriptorPool desc_pool;
    std::vector<VkDescriptorSet> desc_sets;

    VCW_Buffer data_buf;
    VCW_Buffer result_buf;

    VkCommandPool cmd_pool;

    //
    //
    //
    // function definitions
    //
    //

    void init_app();

    void compute();

    void get_results();

    void clean_up();

    //
    // vulkan instance
    //
    static std::vector<const char *> get_required_exts();

    void create_inst();

    void setup_debug_msg();

    //
    //
    // vulkan primitives
    //
    // physical device
    //
    static std::vector<VkQueueFamilyProperties> get_qf_props(VkPhysicalDevice loc_phy_dev);

    VCW_QueueFamilyIndices find_qf(VkPhysicalDevice loc_phy_dev);

    static bool check_phy_dev_ext_support(VkPhysicalDevice loc_phy_dev);

    bool is_phy_dev_suitable(VkPhysicalDevice loc_phy_dev);

    void pick_phy_dev();

    //
    // logical device
    //
    void create_dev();

    //
    // buffers
    //
    uint32_t find_mem_type(uint32_t type_filter, VkMemoryPropertyFlags mem_flags);

    VCW_Buffer create_buf(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags mem_props);

    void map_buf(VCW_Buffer *p_buf);

    void unmap_buf(VCW_Buffer *p_buf);

    void cp_data_to_buf(VCW_Buffer *p_buf, void *p_data);

    void cp_buf(VCW_Buffer src_buf, VCW_Buffer dst_buf);

    void clean_up_buf(VCW_Buffer buf);

    //
    // images
    //
    VkFormat find_supported_format(const std::vector<VkFormat> &possible_formats, VkImageTiling tiling,
                                   VkFormatFeatureFlags features);

    VkFormat find_depth_format();

    bool has_stencil_component(VkFormat format);

    VCW_Image create_img(VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags mem_props);

    void create_img_view(VCW_Image *p_img, VkImageAspectFlags aspect_flags);

    void create_sampler(VCW_Image *p_img, VkFilter filter, VkSamplerAddressMode address_mode);

    static VkAccessFlags get_access_mask(VkImageLayout layout);

    static void transition_img_layout(VkCommandBuffer cmd_buf, VCW_Image *p_img, VkImageLayout layout,
                                      VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage);

    static void cp_buf_to_img(VkCommandBuffer cmd_buf, VCW_Buffer buf, VCW_Image img, VkExtent2D extent);

    static void
    blit_img(VkCommandBuffer cmd_buf, VCW_Image src, VkExtent3D src_extent, VCW_Image dst, VkExtent3D dst_extent,
             VkFilter filter);

    static void blit_img(VkCommandBuffer cmd_buf, VCW_Image src, VCW_Image dst, VkFilter filter);

    static void copy_img(VkCommandBuffer cmd_buf, VCW_Image src, VCW_Image dst);

    void clean_up_img(VCW_Image img);

    //
    // descriptor pool
    //
    void add_desc_set_layout(uint32_t binding_count, VkDescriptorSetLayoutBinding *p_bindings);

    void add_pool_size(uint32_t desc_count, VkDescriptorType desc_type);

    void create_desc_pool(uint32_t max_sets);

    void write_buf_desc_binding(VCW_Buffer buf, uint32_t dst_set, uint32_t dst_binding, VkDescriptorType desc_type);

    void write_img_desc_binding(VCW_Image img, uint32_t dst_set, uint32_t dst_binding, VkDescriptorType desc_type);

    void clean_up_desc();

    //
    // pipeline prerequisites
    //
    VkShaderModule create_shader_mod(const std::vector<char> &code);

    //
    // render prerequisites
    //
    void create_cmd_pool();

    VkCommandBuffer begin_single_time_cmd();

    void end_single_time_cmd(VkCommandBuffer cmd_buf);

    //
    //
    // personalized vulkan initialization
    //
    void create_desc_pool_layout();

    void create_result_buf();

    void create_data_buf();

    void create_pipe();

    void write_desc_pool();
};

#endif //VCW_APP_H
