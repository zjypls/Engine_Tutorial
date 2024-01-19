//
// Created by z on 24-1-16.
//
#define VK_INFO(TYPE,ACT) VK_STRUCTURE_TYPE_##TYPE##_##ACT##_INFO
#define VK_CHECK(value,str) Z_CORE_ASSERT(value==VK_SUCCESS,str)
#include <vulkan/vulkan.h>

#include "Z/Core/Core.h"
#include "Z/Core/Log.h"
#include "Platform/Vulkan/VulkanResource.h"


namespace Z{
    namespace VulkanUtils{

        uint findMemoryType(uint32_t typeFilter,VkPhysicalDevice device, VkMemoryPropertyFlags properties) {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }

        void CreateVulkanImage(VkPhysicalDevice      physical_device,
                               VkDevice              device,
                               uint32_t              image_width,
                               uint32_t              image_height,
                               VkFormat              format,
                               VkImageTiling         image_tiling,
                               VkImageUsageFlags     image_usage_flags,
                               VkMemoryPropertyFlags memory_property_flags,
                               VkImage&              image,
                               VkDeviceMemory&       memory,
                               VkImageCreateFlags    image_create_flags,
                               uint32_t              array_layers,
                               uint32_t              miplevels){
            auto createInfo=VkImageCreateInfo{};
            createInfo.sType=VK_INFO(IMAGE,CREATE);
            createInfo.extent={image_width,image_height,1};
            createInfo.format=format;
            createInfo.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
            createInfo.arrayLayers=array_layers;
            createInfo.mipLevels=miplevels;
            createInfo.imageType=VK_IMAGE_TYPE_2D;
            createInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
            createInfo.usage=image_usage_flags;
            createInfo.tiling=image_tiling;
            createInfo.flags=image_create_flags;

            auto res= vkCreateImage(device, &createInfo, nullptr, &image);
            VK_CHECK(res,"failed to create image!");

            VkMemoryRequirements requireMents{};
            vkGetImageMemoryRequirements(device,image,&requireMents);

            VkMemoryAllocateInfo allocateInfo{};
            allocateInfo.sType=VK_INFO(MEMORY,ALLOCATE);
            allocateInfo.allocationSize=requireMents.size;
            allocateInfo.memoryTypeIndex= findMemoryType(requireMents.memoryTypeBits,
                                                         physical_device,memory_property_flags);

            res= vkAllocateMemory(device,&allocateInfo, nullptr,&memory);
            VK_CHECK(res,"create Image false!");
            vkBindImageMemory(device,image,memory,0);
        }

        VkImageView CreateImageView(VkDevice           device,
                             VkImage&           image,
                             VkFormat           format,
                             VkImageAspectFlags image_aspect_flags,
                             VkImageViewType    view_type,
                             uint32_t           layout_count,
                             uint32_t           miplevels){
            auto viewInfo=VkImageViewCreateInfo{};
            viewInfo.sType=VK_INFO(IMAGE_VIEW,CREATE);
            viewInfo.image=image;
            viewInfo.format=format;
            viewInfo.viewType=view_type;
            viewInfo.subresourceRange.aspectMask=image_aspect_flags;
            viewInfo.subresourceRange.layerCount=layout_count;
            viewInfo.subresourceRange.levelCount=miplevels;
            viewInfo.subresourceRange.baseArrayLayer=0;
            viewInfo.subresourceRange.baseMipLevel=0;
            VkImageView view{};
            auto res= vkCreateImageView(device,&viewInfo, nullptr,&view);
            VK_CHECK(res,"failed to create image view !");
            return view;
        }

        void CreateBuffer(VkPhysicalDevice physicalDevice,VkDevice device,
                          VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags propertyFlags,
                          VkBuffer &buffer,VkDeviceMemory& memory){
            VkBufferCreateInfo info{};
            info.sType=VK_INFO(BUFFER,CREATE);
            info.size=size;
            info.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
            info.usage=usage;
            auto res=vkCreateBuffer(device,&info, nullptr,&buffer);
            VK_CHECK(res,"failed to create buffer !");


            VkMemoryRequirements requirements{};
            vkGetBufferMemoryRequirements(device,buffer,&requirements);

            auto allocateInfo=VkMemoryAllocateInfo{};
            allocateInfo.sType=VK_INFO(MEMORY,ALLOCATE);
            allocateInfo.memoryTypeIndex= findMemoryType(requirements.memoryTypeBits,
                                                         physicalDevice,propertyFlags);
            allocateInfo.allocationSize=requirements.size;

            res= vkAllocateMemory(device,&allocateInfo, nullptr,&memory);
            VK_CHECK(res,"failed to allocate memory !");
            vkBindBufferMemory(device,buffer,memory,0);
        }
    }
}