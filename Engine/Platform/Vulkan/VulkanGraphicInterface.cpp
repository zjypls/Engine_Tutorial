//
// Created by z on 24-1-18.
//

#include "Z/Core/Application.h"

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

    void VulkanGraphicInterface::CreateInstance(){
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pEngineName="Z";
        applicationInfo.pApplicationName="Z_Renderer";
        applicationInfo.applicationVersion= VK_MAKE_VERSION(1,0,0);
        applicationInfo.engineVersion= VK_MAKE_VERSION(1,0,0);
        applicationInfo.apiVersion=VK_API_VERSION_1_3;

        auto info= VkInstanceCreateInfo{};
        info.sType= VK_INFO(INSTANCE,CREATE);
        auto extensions=VulkanUtils::GetRequireExtensions(enableDebugUtils);
        info.ppEnabledExtensionNames=extensions.data();
        info.enabledExtensionCount= extensions.size();

        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerInfo{};
        if(enableValidationLayer){
            info.enabledLayerCount=validationLayers.size();
            info.ppEnabledLayerNames=validationLayers.data();
            VulkanUtils::populateDebugMessengerCreateInfo(debugUtilsMessengerInfo);
            info.pNext=&debugUtilsMessengerInfo;
        }else{
            info.enabledLayerCount=0;
            info.pNext= nullptr;
        }
        auto res= vkCreateInstance(&info, nullptr,&instance);
        VK_CHECK(res,"failed to create instance !");

    }

    void VulkanGraphicInterface::initializeDebugMessenger() {
        if(enableDebugUtils){
            VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
            VulkanUtils::populateDebugMessengerCreateInfo(debugInfo);
            auto res=VulkanUtils::CreateDebugUtils(instance,&debugInfo, nullptr,&debugUtilsMessenger);
            VK_CHECK(res,"failed to create DebugUtilsMessenger !");
        }
    }

    void VulkanGraphicInterface::CreateWindowSurface() {
        auto res=glfwCreateWindowSurface(instance,(GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(),
                                nullptr,&surface);
        VK_CHECK(res,"failed to create window surface !");
    }

    void VulkanGraphicInterface::initializePhysicalDevice() {
        physicalDevice=VK_NULL_HANDLE;
        uint32 count=0;
        vkEnumeratePhysicalDevices(instance,&count, nullptr);
        Z_CORE_ASSERT(count,"No suitable GPU for vulkan,may need to check your vulkan driver !");
        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(instance,&count,devices.data());
        for(const auto&phyDevice:devices){
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(phyDevice,&properties);
            if(properties.deviceType==VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
                physicalDevice=phyDevice;
            }
        }
        if(physicalDevice==VK_NULL_HANDLE)
            physicalDevice=devices[0];
    }

    void VulkanGraphicInterface::CreateLogicalDevice() {
        familyIndices=VulkanUtils::findQueueFamily(physicalDevice,surface);

        std::vector indices{familyIndices.graphics.value(),familyIndices.compute.value(),familyIndices.present.value()};

        std::vector<VkDeviceQueueCreateInfo> deviceQueueInfos{};
        float priority=1.f;
        for(const auto i:indices){
            VkDeviceQueueCreateInfo info{};
            info.sType=VK_INFO(DEVICE_QUEUE,CREATE);
            info.queueCount=1;
            info.pQueuePriorities=&priority;
            info.queueFamilyIndex=i;
            deviceQueueInfos.push_back(info);
        }

        auto deviceInfo=VkDeviceCreateInfo{};
        deviceInfo.sType=VK_INFO(DEVICE,CREATE);
        deviceInfo.pQueueCreateInfos=deviceQueueInfos.data();
        deviceInfo.queueCreateInfoCount=deviceQueueInfos.size();


        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
        deviceFeatures.independentBlend = VK_TRUE;
        deviceFeatures.geometryShader = VK_TRUE;

        std::vector extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        deviceInfo.pEnabledFeatures=&deviceFeatures;
        deviceInfo.ppEnabledExtensionNames=extensions.data();
        deviceInfo.enabledExtensionCount=extensions.size();

        auto res=vkCreateDevice(physicalDevice,&deviceInfo, nullptr,&device);
        VK_CHECK(res,"failed to create device !");
        depthFormat=VulkanUtils::findDepthFormat(physicalDevice);

        VkQueue graphicqueue;
        vkGetDeviceQueue(device,familyIndices.graphics.value(),0,&graphicqueue);
        graphicsQueue=new VulkanQueue{};
        ((VulkanQueue*)graphicsQueue)->Set(graphicqueue);

        VkQueue computqueue;
        vkGetDeviceQueue(device,familyIndices.compute.value(),0,&computqueue);
        ((VulkanQueue*)computeQueue)->Set(computqueue);
    }

    void VulkanGraphicInterface::CreateCommandPool() {
        
    }

    void VulkanGraphicInterface::CreateCommandBuffers() {

    }

    void VulkanGraphicInterface::CreateDescriptorPool() {

    }

    void VulkanGraphicInterface::CreateSyncPrimitives() {

    }

    void VulkanGraphicInterface::CreateSwapchain() {

    }

    void VulkanGraphicInterface::CreateSwapchainImageViews() {

    }

    void VulkanGraphicInterface::CreateFramebufferImageAndView() {

    }

    void VulkanGraphicInterface::CreateAssetAllocator() {

    }
} // Z