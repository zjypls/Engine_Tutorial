//
// Created by z on 24-1-16.
//
#pragma once
#define VK_INFO(TYPE,ACT) VK_STRUCTURE_TYPE_##TYPE##_##ACT##_INFO
#define VK_CHECK(value,str) Z_CORE_ASSERT(value==VK_SUCCESS,str)
#include <optional>
#include "vulkan/vulkan.h"
#include "spirv_cross/spirv_glsl.hpp"
#include "shaderc/shaderc.hpp"

#include "Include/glfw/include/GLFW/glfw3.h"

#include "Z/Core/Core.h"
#include "Z/Core/Log.h"
#include "Platform/Vulkan/VulkanRenderInterface.h"


namespace Z{
    namespace VulkanUtils{

        namespace Tools{
            shaderc_shader_kind ShaderStageToShaderc(ShaderStageFlag stage){
                switch(stage){
                    case ShaderStageFlag::VERTEX:
                        return shaderc_glsl_vertex_shader;
                    case ShaderStageFlag::FRAGMENT:
                        return shaderc_glsl_fragment_shader;
                    case ShaderStageFlag::COMPUTE:
                        return shaderc_glsl_compute_shader;
                    case ShaderStageFlag::GEOMETRY:
                        return shaderc_glsl_geometry_shader;
                    case ShaderStageFlag::TESSELLATION_CONTROL:
                        return shaderc_glsl_tess_control_shader;
                    case ShaderStageFlag::TESSELLATION_EVALUATION:
                        return shaderc_glsl_tess_evaluation_shader;
                    default:
                        Z_CORE_ERROR("error: unknown shader stage !");
                        return shaderc_glsl_vertex_shader;
                }
            }

            std::string GetShaderStageMacroName(ShaderStageFlag stage){
                switch(stage){
                #define tCase(stage) case Z::ShaderStageFlag::stage: return "Z_"#stage
                    tCase(VERTEX);
                    tCase(FRAGMENT);
                    tCase(COMPUTE);
                    tCase(GEOMETRY);
                    tCase(TESSELLATION_CONTROL);
                    tCase(TESSELLATION_EVALUATION);
                #undef tCase
                }
            }

            std::vector<std::vector<uint32>> CompileShader(const std::string& sources,const std::vector<ShaderStageFlag> &stages){
                Z_CORE_ASSERT(stages.size()>0,"error: no shader stage !");
                std::vector<std::vector<uint32>> results;
                for(int i=0;i<stages.size();++i){
                    shaderc::Compiler compiler;
                    shaderc::CompileOptions options;
                    options.SetTargetEnvironment(shaderc_target_env_vulkan,shaderc_env_version_vulkan_1_0);
                    options.AddMacroDefinition(GetShaderStageMacroName(stages[i]));
                    options.SetOptimizationLevel(shaderc_optimization_level_performance);
                    auto stage= ShaderStageToShaderc(stages[i]);
                    auto result=compiler.CompileGlslToSpv(sources,stage,"shader",options);
                    if(result.GetCompilationStatus()!=shaderc_compilation_status_success){
                        Z_CORE_ERROR("error: failed to compile shader !");
                        Z_CORE_ERROR(result.GetErrorMessage());
                        continue;
                    }
                    results.push_back({result.begin(),result.end()});
                }
                return results;
            }

            std::vector<uint32> ReadFile(const std::string& path){
                std::ifstream file(path,std::ios::ate|std::ios::binary);
                Z_CORE_ASSERT(file.is_open(),"error: failed to open file !");
                size_t size=static_cast<size_t>(file.tellg());
                std::vector<uint32> buffer(size/sizeof(uint32));
                file.seekg(0);
                file.read(reinterpret_cast<char*>(buffer.data()),size);
                file.close();
                return buffer;
            }
            
        }

        struct DescriptorInfo{
            std::vector<VkDescriptorSetLayoutBinding> bindings;
        };
        struct ShaderSourceCompileInfo{
            std::vector<std::vector<uint32>> irCode;
            std::vector<VkShaderStageFlagBits> stageFlags;
            DescriptorInfo descriptorInfos;
        };

        uint32 FindMemoryType(VkPhysicalDevice device,uint32 typeFilter,VkMemoryPropertyFlags properties){
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(device,&memProperties);
            for(uint32 i=0;i<memProperties.memoryTypeCount;++i){
                if((typeFilter&(1<<i))&&(memProperties.memoryTypes[i].propertyFlags&properties)==properties)
                    return i;
            }
            Z_CORE_ERROR("failed to find suitable memory type !");
            return 0;
        }

        uint32 GetPixelSizeFromFormat(VkFormat format){
            switch(format){
                case VK_FORMAT_R8G8B8A8_SRGB:
                    return 4;
                case VK_FORMAT_R8G8B8A8_UNORM:
                    return 4;
                case VK_FORMAT_R8G8B8A8_SNORM:
                    return 4;
                case VK_FORMAT_R8G8B8A8_UINT:
                    return 4;
                case VK_FORMAT_R8G8B8A8_SINT:
                    return 4;
                case VK_FORMAT_R32G32B32A32_SFLOAT:
                    return 16;
                case VK_FORMAT_R32G32B32A32_UINT:
                    return 16;
                case VK_FORMAT_R32G32B32A32_SINT:
                    return 16;
                case VK_FORMAT_R32G32B32_SFLOAT:
                    return 12;
                case VK_FORMAT_R32G32B32_UINT:
                    return 12;
                case VK_FORMAT_R32G32B32_SINT:
                    return 12;
                case VK_FORMAT_R32G32_SFLOAT:
                    return 8;
                case VK_FORMAT_R32G32_UINT:
                    return 8;
                case VK_FORMAT_R32G32_SINT:
                    return 8;
                case VK_FORMAT_R32_SFLOAT:
                    return 4;
                case VK_FORMAT_R32_UINT:
                    return 4;
                case VK_FORMAT_R32_SINT:
                    return 4;
                case VK_FORMAT_R8G8B8_SRGB:
                    return 3;
                case VK_FORMAT_R8G8B8_UNORM:
                    return 3;
                case VK_FORMAT_R8G8B8_SNORM:
                    return 3;
                case VK_FORMAT_R8G8B8_UINT:
                    return 3;
                case VK_FORMAT_R8G8B8_SINT:
                    return 3;
                default:
                    Z_CORE_ERROR("error: unknown format !");
                    return 0;
            }
        }

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

        ShaderSourceCompileInfo ReflectShader(const std::string&sources,const std::vector<ShaderStageFlag> &stages){
            ShaderSourceCompileInfo info;
            info.irCode=Tools::CompileShader(sources,stages);
            Z_CORE_ASSERT(info.irCode.size()==stages.size(),"error: shader stage count not match source count !");
            auto &descriptorSets=info.descriptorInfos;
            for(int i=0;i<info.irCode.size();++i){
                spirv_cross::CompilerGLSL compiler(info.irCode[i]);
                spirv_cross::CompilerGLSL::Options options;
                options.version=450;
                options.es=false;
                options.vulkan_semantics=true;
                auto stage=stages[i];
                compiler.set_common_options(options);
                auto irCode=compiler.compile();
                info.stageFlags.push_back(static_cast<VkShaderStageFlagBits>(stage));
                auto resources=compiler.get_shader_resources();
                for(auto &resource:resources.uniform_buffers){
                    VkDescriptorSetLayoutBinding binding{};
                    binding.binding=compiler.get_decoration(resource.id,spv::DecorationBinding);
                    binding.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    binding.stageFlags=static_cast<VkShaderStageFlagBits>(stage);
                    binding.descriptorCount=1;
                    descriptorSets.bindings.push_back(binding);
                }
                for(auto &resource:resources.storage_buffers){
                    VkDescriptorSetLayoutBinding binding{};
                    binding.binding=compiler.get_decoration(resource.id,spv::DecorationBinding);
                    binding.descriptorType=VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    binding.stageFlags=static_cast<VkShaderStageFlagBits>(stage);
                    binding.descriptorCount=1;
                    descriptorSets.bindings.push_back(binding);
                }
                for(auto &resource:resources.sampled_images){
                    VkDescriptorSetLayoutBinding binding{};
                    binding.binding=compiler.get_decoration(resource.id,spv::DecorationBinding);
                    binding.descriptorType=VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    binding.stageFlags=static_cast<VkShaderStageFlagBits>(stage);
                    binding.descriptorCount=1;
                    descriptorSets.bindings.push_back(binding);
                }
            }
            return info;
        }

        VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device,const DescriptorInfo& info){
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount=info.bindings.size();
            createInfo.pBindings=info.bindings.data();
            createInfo.flags=0;
            VkDescriptorSetLayout layout;
            auto res= vkCreateDescriptorSetLayout(device,&createInfo, nullptr,&layout);
            VK_CHECK(res,"failed to create descriptor set layout !");
            return layout;
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
                if(format.format==VK_FORMAT_B8G8R8A8_UNORM&&format.colorSpace==VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
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

        std::vector<const char*> checkLayerEnable(std::vector<const char*> requiredLayers){
            uint32 Count=0;
            vkEnumerateInstanceLayerProperties(&Count, nullptr);
            Z_CORE_ASSERT(Count>0,"illegal layer count for instance !");
            std::vector<VkLayerProperties> layers(Count);
            vkEnumerateInstanceLayerProperties(&Count,layers.data());
            for(auto &layer:layers){
                for(auto reqLayer=requiredLayers.begin();reqLayer!=requiredLayers.end();++reqLayer){
                    if(strcmp(layer.layerName,*reqLayer)==0){
                        requiredLayers.erase(reqLayer);
                        break;
                    }
                }
            }
            return requiredLayers;
        }
        std::vector<const char*> checkExtensionEnable(std::vector<const char*> requiredExtensions){
            uint32 Count=0;
            vkEnumerateInstanceExtensionProperties(nullptr,&Count, nullptr);
            Z_CORE_ASSERT(Count>0,"illegal extension count for instance !");
            std::vector<VkExtensionProperties> extensions(Count);
            vkEnumerateInstanceExtensionProperties(nullptr,&Count,extensions.data());
            for(auto extension:extensions){
                for(auto ext=requiredExtensions.begin();ext!=requiredExtensions.end();++ext){
                    if(strcmp(extension.extensionName,*ext)==0){
                        requiredExtensions.erase(ext);
                        break;
                    }
                }
            }
            return requiredExtensions;
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
                               uint32              miplevels,
                               VkSampleCountFlagBits sample_count=VK_SAMPLE_COUNT_1_BIT){
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
            createInfo.samples=sample_count;

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