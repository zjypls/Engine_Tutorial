//
// Created by z on 24-1-18.
//

#ifndef ENGINEALL_VULKANGRACPHICINTERFACE_H
#define ENGINEALL_VULKANGRACPHICINTERFACE_H
#include <vulkan/vulkan.h>

#include "Z/Renderer/GraphicInterface.h"

namespace Z {

    class VulkanGraphicInterface final : public GraphicInterface{
    public:
        void CreateImage(const ImageInfo& info,Image*& image,DeviceMemory*& memory)override;
        void CreateBuffer(const BufferInfo& info,Buffer*&buffer,DeviceMemory*& memory)override;
    private:
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
    };

} // Z

#endif //ENGINEALL_VULKANGRACPHICINTERFACE_H
