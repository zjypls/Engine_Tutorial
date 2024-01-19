//
// Created by z on 24-1-18.
//


#include "Platform/Vulkan/VulkanGraphicInterface.h"
#include "Platform/Vulkan/VulkanUtils.h"

namespace Z {
    void VulkanGraphicInterface::CreateImage(const ImageInfo &info,Image*& image,DeviceMemory*& memory) {
        image=new VulkanImage{};
        memory=new VulkanDeviceMemory{};
        VkImage resImage;
        VkDeviceMemory resMemory;
        auto [width,height,depth]=info.extent;
        VulkanUtils::CreateVulkanImage(physicalDevice,device,width,height,
                                                  (VkFormat)info.format,(VkImageTiling)info.tilling,(VkImageLayout)info.initialLayout,
                                                  (VkMemoryPropertyFlagBits)info.memoryPropertyFlag,resImage,resMemory,
                                                  (VkImageCreateFlags)info.createFlag, info.arrayLayers,info.mipMapLevels);
        ((VulkanImage*)image)->Set(resImage);
        ((VulkanDeviceMemory*)memory)->Set(resMemory);
    }

    void VulkanGraphicInterface::CreateBuffer(const BufferInfo &info,Buffer*&buffer,DeviceMemory*&memory) {
        buffer=new VulkanBuffer{};
        memory=new VulkanDeviceMemory{};
        VkBuffer resBuffer;
        VkDeviceMemory resMemory;
        VulkanUtils::CreateBuffer(physicalDevice,device,info.size,
                                  (VkBufferUsageFlags)info.usage,(VkMemoryPropertyFlags)info.properties,
                                  resBuffer,resMemory);
        ((VulkanBuffer*)buffer)->Set(resBuffer);
        ((VulkanDeviceMemory*)memory)->Set(resMemory);
    }
} // Z