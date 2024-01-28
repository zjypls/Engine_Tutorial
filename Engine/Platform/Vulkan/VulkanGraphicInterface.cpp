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

        std::set indices{familyIndices.graphics.value(),familyIndices.compute.value(),familyIndices.present.value()};

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

        VkQueue computequeue;
        vkGetDeviceQueue(device,familyIndices.compute.value(),0,&computequeue);
        computeQueue=new VulkanQueue{};
        ((VulkanQueue*)computeQueue)->Set(computequeue);
    }

    void VulkanGraphicInterface::CreateCommandPool() {
        auto commandPoolInfo= VkCommandPoolCreateInfo{};
        commandPoolInfo.sType=VK_INFO(COMMAND_POOL,CREATE);
        commandPoolInfo.queueFamilyIndex=familyIndices.graphics.value();
        commandPoolInfo.flags=VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        auto res= vkCreateCommandPool(device,&commandPoolInfo, nullptr,&transientCommandPool);
        VK_CHECK(res,"failed to create command pool !");

        for(auto&pool:commandPools){
            auto poolInfo=VkCommandPoolCreateInfo{};
            poolInfo.sType=VK_INFO(COMMAND_POOL,CREATE);
            poolInfo.flags=VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex=familyIndices.graphics.value();
            auto res= vkCreateCommandPool(device,&poolInfo, nullptr,&pool);
            VK_CHECK(res,"failed to create command pool !");
        }
    }

    void VulkanGraphicInterface::CreateCommandBuffers() {
        int index=0;
        auto info=VkCommandBufferAllocateInfo{};
        info.sType=VK_INFO(COMMAND_BUFFER,ALLOCATE);
        info.commandBufferCount=1;
        info.level=VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        for(auto&buffer:commandBuffers){
            info.commandPool=commandPools[index++];
            auto res= vkAllocateCommandBuffers(device,&info,&buffer);
            VK_CHECK(res,"failed to allocate command buffer !");
        }
    }

    void VulkanGraphicInterface::CreateDescriptorPool() {
        VkDescriptorPoolSize poolSizes[7];
        poolSizes[0].type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        poolSizes[0].descriptorCount = 3 + 2 + 2 + 2 + 1 + 1 + 3 + 3;
        poolSizes[1].type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[1].descriptorCount = 1 + 1 + 1 * maxVertexBlendCount;
        poolSizes[2].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[2].descriptorCount = 1 * maxMaterialCount;
        poolSizes[3].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[3].descriptorCount = 3 + 5 * maxMaterialCount + 1 + 1; // ImGui_ImplVulkan_CreateDeviceObjects
        poolSizes[4].type            = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        poolSizes[4].descriptorCount = 4 + 1 + 1 + 2;
        poolSizes[5].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[5].descriptorCount = 3;
        poolSizes[6].type            = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        poolSizes[6].descriptorCount = 1;


        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType=VK_INFO(DESCRIPTOR_POOL,CREATE);
        poolInfo.pPoolSizes=poolSizes;
        poolInfo.poolSizeCount=sizeof(poolSizes)/sizeof(poolSizes[0]);
        poolInfo.maxSets=1000;//Todo:improve

        auto res= vkCreateDescriptorPool(device,&poolInfo, nullptr,&descriptorPool);
        VK_CHECK(res,"failed to create descriptor pool !");

    }

    void VulkanGraphicInterface::CreateSyncSignals() {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType=VK_INFO(FENCE,CREATE);
        fenceInfo.flags=VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType=VK_INFO(SEMAPHORE,CREATE);

        for(int i=0;i<maxFlightFrames;++i){
            auto res0= vkCreateFence(device,&fenceInfo, nullptr,&frameFences[i]);
            auto res1= vkCreateSemaphore(device,&semaphoreInfo, nullptr,&imageAvailable[i]);
            auto res2= vkCreateSemaphore(device,&semaphoreInfo, nullptr,&imageRenderFinish[i]);
            VK_CHECK(res0,"failed to create fences !");
            VK_CHECK(res1,"failed to create semaphore !");
            VK_CHECK(res2,"failed to create semaphore !");
        }
    }

    void VulkanGraphicInterface::CreateSwapchain() {
        auto swapchainDetails=VulkanUtils::GetSwapChainDetails(physicalDevice,surface);
        auto swapchainformat=VulkanUtils::chooseSwapChainFormat(swapchainDetails.formats);
        auto extent=VulkanUtils::chooseSwapchainExtentFromDetails(swapchainDetails.capabilities,
                                                                       (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
        auto presentMode=VulkanUtils::chooseSwapChainPresentMode(swapchainDetails.presentModes);


        uint32 swapchainImageCount=swapchainDetails.capabilities.minImageCount+1;
        if(swapchainDetails.capabilities.maxImageCount>0&&swapchainImageCount>swapchainDetails.capabilities.maxImageCount)
            swapchainImageCount=swapchainDetails.capabilities.maxImageCount;
        swapChainInfo.imageCount=swapchainImageCount;
        swapChainInfo.minImageCount=swapchainDetails.capabilities.minImageCount;
        swapChainInfo.maxImageCount=swapchainDetails.capabilities.maxImageCount;
        swapChainInfo.swapchainImageFormat=(Format)swapchainformat.format;

        auto swapchainInfo=VkSwapchainCreateInfoKHR{};
        swapchainInfo.sType=VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface=surface;
        swapchainInfo.imageExtent=extent;
        swapchainInfo.imageFormat=swapchainformat.format;
        swapchainInfo.imageColorSpace=swapchainformat.colorSpace;
        swapchainInfo.minImageCount=swapchainImageCount;
        swapchainInfo.presentMode=presentMode;

        swapchainInfo.compositeAlpha=VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.preTransform=swapchainDetails.capabilities.currentTransform;
        swapchainInfo.imageUsage=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32 queueFamilyIndices[] = {familyIndices.graphics.value(), familyIndices.present.value()};

        if (familyIndices.graphics != familyIndices.present)
        {
            swapchainInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            swapchainInfo.queueFamilyIndexCount = 2;
            swapchainInfo.pQueueFamilyIndices   = queueFamilyIndices;
        }
        else
        {
            swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        swapchainInfo.imageArrayLayers=1;
        swapchainInfo.clipped=VK_TRUE;
        swapchainInfo.oldSwapchain=VK_NULL_HANDLE;

        auto res= vkCreateSwapchainKHR(device,&swapchainInfo, nullptr,&swapchain);
        VK_CHECK(res,"failed to create swap chain !");

        vkGetSwapchainImagesKHR(device,swapchain,&swapchainImageCount, nullptr);
        swapchainImages.resize(swapchainImageCount);
        vkGetSwapchainImagesKHR(device,swapchain,&swapchainImageCount,swapchainImages.data());

        swapchainExtent=extent;
        swapchainFormat=swapchainformat.format;
        scissor={0,0,extent};
    }

    void VulkanGraphicInterface::CreateSwapchainImageViews() {
        swapchainImageViews.resize(swapchainImages.size());
        for(int i=0;i<swapchainImageViews.size();++i){
            VulkanUtils::CreateImageView(device,swapchainImages[i],swapchainFormat,
                                         VK_IMAGE_ASPECT_COLOR_BIT,VK_IMAGE_VIEW_TYPE_2D,1,1);
        }
    }

    void VulkanGraphicInterface::CreateFramebufferImageAndView() {

    }

    void VulkanGraphicInterface::CreateVmaAllocator() {
        VmaVulkanFunctions vulkanFunctions    = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr   = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion       = VK_API_VERSION_1_3;
        allocatorCreateInfo.physicalDevice         = physicalDevice;
        allocatorCreateInfo.device                 = device;
        allocatorCreateInfo.instance               = instance;
        allocatorCreateInfo.pVulkanFunctions       = &vulkanFunctions;

        vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator);
    }

    void VulkanGraphicInterface::Init(const GraphicSpec &spec) {
        CreateInstance();
        initializeDebugMessenger();
        CreateWindowSurface();
        initializePhysicalDevice();
        CreateLogicalDevice();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateDescriptorPool();
        CreateSyncSignals();
        CreateSwapchain();
        CreateSwapchainImageViews();
        CreateFramebufferImageAndView();
        CreateVmaAllocator();
    }
} // Z