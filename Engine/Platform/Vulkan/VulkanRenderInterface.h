//
// Created by z on 24-1-16.
//

#ifndef ENGINEALL_VULKANRENDERINTERFACE_H
#define ENGINEALL_VULKANRENDERINTERFACE_H

#include "vulkan/vulkan.h"

#include "Z/Core/Core.h"
#include "Z/Renderer/RenderInterface.h"
#define VULKAN_IMPL_RESOURCE(type) class Z_API Vulkan##type final : public type{ \
public:                                                                          \
    using ResType=Vk##type;                                                      \
    void Set(ResType s_##type){m_##type=s_##type;}                               \
    [[nodiscard]] ResType Get() const {return m_##type;}                         \
    const ResType* GetPtr()const{return &m_##type;}                              \
private:                                                                         \
    ResType m_##type;                                                            \
}                                                                                \


namespace Z {
    VULKAN_IMPL_RESOURCE(Buffer);
    VULKAN_IMPL_RESOURCE(Image);
    VULKAN_IMPL_RESOURCE(ImageView);
    VULKAN_IMPL_RESOURCE(BufferView);
    VULKAN_IMPL_RESOURCE(ShaderModule);
    VULKAN_IMPL_RESOURCE(DeviceMemory);
    VULKAN_IMPL_RESOURCE(DescriptorPool);
    VULKAN_IMPL_RESOURCE(DescriptorSet);
    VULKAN_IMPL_RESOURCE(Framebuffer);
    VULKAN_IMPL_RESOURCE(CommandBuffer);
    VULKAN_IMPL_RESOURCE(CommandPool);
    VULKAN_IMPL_RESOURCE(Queue);
    VULKAN_IMPL_RESOURCE(PipelineLayout);
    VULKAN_IMPL_RESOURCE(Pipeline);
    VULKAN_IMPL_RESOURCE(DescriptorSetLayout);
    VULKAN_IMPL_RESOURCE(Sampler);
    VULKAN_IMPL_RESOURCE(Fence);
    VULKAN_IMPL_RESOURCE(Semaphore);

    class VulkanRenderPass:public RenderPassInterface {
    public:
        void Set(VkRenderPass renderpass){m_RenderPass=renderpass;}
        [[nodiscard]]auto Get(){return m_RenderPass;}
    private:
        VkRenderPass m_RenderPass;
    };



    struct QueueFamilyIndices
    {
        std::optional<uint32> graphics;
        std::optional<uint32> present;
        std::optional<uint32> compute;

        bool isComplete() { return graphics.has_value() && present.has_value() && compute.has_value();}
    };
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentModes;
    };
}

#endif //ENGINEALL_VULKANRENDERINTERFACE_H
