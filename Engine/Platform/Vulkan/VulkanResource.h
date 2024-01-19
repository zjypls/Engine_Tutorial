//
// Created by z on 24-1-16.
//

#ifndef ENGINEALL_VULKANRESOURCE_H
#define ENGINEALL_VULKANRESOURCE_H

#include <vulkan/vulkan.h>

#include "Z/Core/Core.h"
#include "Z/Renderer/RenderResource.h"
#define VULKAN_IMPL_RESOURCE(type) class Z_API Vulkan##type final : public type{ \
public:                                                                          \
    void Set(Vk##type s_##type){m_##type=s_##type;}                              \
    [[nodiscard]] auto Get() const {return m_##type;}                            \
private:                                                                         \
    Vk##type m_##type;                                                           \
}                                                                                \


namespace Z {
    VULKAN_IMPL_RESOURCE(Buffer);
    VULKAN_IMPL_RESOURCE(Image);
    VULKAN_IMPL_RESOURCE(ShaderModule);
    VULKAN_IMPL_RESOURCE(DeviceMemory);
    VULKAN_IMPL_RESOURCE(DescriptorPool);
    VULKAN_IMPL_RESOURCE(DescriptorSet);
    VULKAN_IMPL_RESOURCE(Framebuffer);
    VULKAN_IMPL_RESOURCE(RenderPass);
    VULKAN_IMPL_RESOURCE(Pipeline);
    VULKAN_IMPL_RESOURCE(CommandBuffer);
    VULKAN_IMPL_RESOURCE(CommandPool);
    VULKAN_IMPL_RESOURCE(Device);

}

#endif //ENGINEALL_VULKANRESOURCE_H
