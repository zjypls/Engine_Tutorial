//
// Created by z on 24-1-16.
//
#pragma once
#define VK_INFO(TYPE,ACT) VK_STRUCTURE_TYPE_##TYPE##_##ACT##_INFO
#define VK_CHECK(value,str) Z_CORE_ASSERT(value==VK_SUCCESS,str)
#include <optional>
#include "vulkan/vulkan.h"

#include "Include/glfw/include/GLFW/glfw3.h"

#include "Z/Core/Core.h"
#include "Z/Core/Log.h"
#include "Platform/Vulkan/VulkanRenderInterface.h"


namespace Z{
    namespace VulkanUtils{

        VkResult CreateDebugUtils(VkInstance                                instance,
                                  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                  const VkAllocationCallbacks*              pAllocator,
                                  VkDebugUtilsMessengerEXT*                 pDebugMessenger){
            auto func=(PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT");
            if(func){
                return func(instance,pCreateInfo,pAllocator,pDebugMessenger);
            }
            return VK_ERROR_UNKNOWN;
        }
        void DestroyDebugUtils(VkInstance instance,VkDebugUtilsMessengerEXT utils,const VkAllocationCallbacks* pAllocator){
            auto func=(PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
            if(func)
                func(instance,utils,pAllocator);
        }

        QueueFamilyIndices findQueueFamily(VkPhysicalDevice device,VkSurfaceKHR surface){
            uint32 count=0;
            vkGetPhysicalDeviceQueueFamilyProperties(device,&count, nullptr);
            std::vector<VkQueueFamilyProperties> properties(count);
            vkGetPhysicalDeviceQueueFamilyProperties(device,&count,properties.data());
            QueueFamilyIndices indices{};
            for(int i=0;i<properties.size();++i){
                if(properties[i].queueFlags&VK_QUEUE_GRAPHICS_BIT)
                    indices.graphics=i;

                if(properties[i].queueFlags&VK_QUEUE_COMPUTE_BIT)
                    indices.compute=i;

                VkBool32 support=VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface,&support);
                if(support)
                    indices.present=i;

                if(indices.isComplete())
                    break;
            }
            return indices;
        }

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                                     VkImageTiling                tiling,
                                     VkFormatFeatureFlags         features,
                                     VkPhysicalDevice device)
        {
            for (VkFormat format : candidates)
            {
                VkFormatProperties props;
                vkGetPhysicalDeviceFormatProperties(device, format, &props);

                if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
                {
                    return format;
                }
                else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
                {
                    return format;
                }
            }

            Z_CORE_ERROR("findSupportedFormat failed");
            return VkFormat();
        }

        VkFormat findDepthFormat(VkPhysicalDevice device)
        {
            return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                       VK_IMAGE_TILING_OPTIMAL,
                                       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,device);
        }


        SwapChainSupportDetails GetSwapChainDetails(VkPhysicalDevice device,VkSurfaceKHR surface){
            SwapChainSupportDetails details{};
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&details.capabilities);

            uint32 count=0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&count, nullptr);
            Z_CORE_ASSERT(count>0,"error: no surface format support !");
            details.formats.resize(count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&count,details.formats.data());
            count=0;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&count, nullptr);
            Z_CORE_ASSERT(count>0,"error: no present mode support !");
            details.presentModes.resize(count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&count,details.presentModes.data());
            return details;
        }

        VkSurfaceFormatKHR chooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>&formats){
            for(auto format:formats){
                if(format.format==VK_FORMAT_R8G8B8A8_SRGB&&format.colorSpace==VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    return format;
            }
            return formats[0];
        }

        VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>&modes){
            for(auto mode:modes){
                if(mode==VK_PRESENT_MODE_MAILBOX_KHR)return mode;
            }
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D chooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities,GLFWwindow*window)
        {
            if (capabilities.currentExtent.width != UINT32_MAX)
            {
                return capabilities.currentExtent;
            }
            else
            {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);

                VkExtent2D actualExtent = {static_cast<uint32>(width), static_cast<uint32>(height)};

                actualExtent.width =
                        std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height =
                        std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }

        VkBool32 VKAPI_PTR DefaultDebugCall(
                VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
                const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
                void*                                            pUserData){
            switch(messageSeverity){
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    Z_CORE_ERROR("Vulkan Error({0}):{1}",pCallbackData->pMessageIdName,pCallbackData->pMessage);
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    Z_CORE_WARN("Vulkan Warning({0}):{1}",pCallbackData->pMessageIdName,pCallbackData->pMessage);
                    break;
                default:
                    Z_CORE_INFO("Vulkan Info({0}):{1}",pCallbackData->pMessageIdName,pCallbackData->pMessage);
                    break;
            }
            return VK_FALSE;
        }

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo,bool UsingDefaultDebugCall=true)
        {
            createInfo       = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity =
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType =
                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            if(UsingDefaultDebugCall)
                createInfo.pfnUserCallback = DefaultDebugCall;
        }

        auto GetRequireExtensions(bool debug){
            uint32 count=0;
            const char** extensions= glfwGetRequiredInstanceExtensions(&count);
            std::vector<const char*> results(count);
            for(int i=0;i<count;++i)
                results[i]=extensions[i];
            if(debug)
                results.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            return results;
        }

        uint32 findMemoryType(uint32 typeFilter,VkPhysicalDevice device, VkMemoryPropertyFlags properties) {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

            for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }

        void CreateVulkanImage(VkPhysicalDevice      physical_device,
                               VkDevice              device,
                               uint32              image_width,
                               uint32              image_height,
                               VkFormat              format,
                               VkImageTiling         image_tiling,
                               VkImageUsageFlags     image_usage_flags,
                               VkMemoryPropertyFlags memory_property_flags,
                               VkImage&              image,
                               VkDeviceMemory&       memory,
                               VkImageCreateFlags    image_create_flags,
                               uint32              array_layers,
                               uint32              miplevels){
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
                             uint32           layer_count,
                             uint32           miplevels){
            auto viewInfo=VkImageViewCreateInfo{};
            viewInfo.sType=VK_INFO(IMAGE_VIEW,CREATE);
            viewInfo.image=image;
            viewInfo.format=format;
            viewInfo.viewType=view_type;
            viewInfo.subresourceRange.aspectMask=image_aspect_flags;
            viewInfo.subresourceRange.layerCount=layer_count;
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