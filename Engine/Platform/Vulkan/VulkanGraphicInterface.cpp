//
// Created by z on 24-1-18.
//
#include "vulkan/vk_enum_string_helper.h"
#include "Z/Core/Application.h"
#include "Z/Renderer/RenderResourceTypes.h"

#include "Platform/Vulkan/VulkanGraphicInterface.h"
#include "Platform/Vulkan/VulkanUtils.h"

namespace Z {

    constexpr uint64 selfDefStructureOffset=sizeof(VkStructureType)+sizeof(VkFlags)+sizeof(void*);

    constexpr VkPhysicalDeviceType preferPhysicalDeviceType=VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

    void VulkanGraphicInterface::CreateImage(const ImageInfo &info,Image*& image,DeviceMemory*& memory) {
        image=new VulkanImage{};
        memory=new VulkanDeviceMemory{};
        VkImage resImage;
        VkDeviceMemory resMemory;
        auto [width,height,depth]=info.extent;
        VulkanUtils::CreateVulkanImage(physicalDevice,device,width,height,
                                                  (VkFormat)info.format,(VkImageTiling)info.tilling,(VkImageUsageFlags)info.usageFlag,
                                                  (VkMemoryPropertyFlagBits)info.memoryPropertyFlag,resImage,resMemory,
                                                  (VkImageCreateFlags)info.createFlag, info.arrayLayers,info.mipMapLevels,
                                                  (VkSampleCountFlagBits)info.sampleCount);
        ((VulkanImage*)image)->Set(resImage);
        ((VulkanDeviceMemory*)memory)->Set(resMemory);
    }

    void VulkanGraphicInterface::CreateImage(const ImageInfo &info, Image *&image, DeviceMemory *&memory, ImageView *&imageView, void *pixelData) {
        CreateImage(info,image,memory);
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image=((VulkanImage*)image)->Get();
        viewInfo.viewType=VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format=(VkFormat)info.format;
        viewInfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel=0;
        viewInfo.subresourceRange.levelCount=info.mipMapLevels;
        viewInfo.subresourceRange.baseArrayLayer=0;
        viewInfo.subresourceRange.layerCount=info.arrayLayers;
        VkImageView view;
        auto res= vkCreateImageView(device,&viewInfo,nullptr,&view);
        VK_CHECK(res,"failed to create image view !");
        imageView=new VulkanImageView{};
        ((VulkanImageView*)imageView)->Set(view);
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;
        auto size=info.extent.width*info.extent.height*VulkanUtils::GetPixelSizeFromFormat((VkFormat)info.format);
        VulkanUtils::CreateBuffer(physicalDevice,device,size,
                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  stagingBuffer,stagingMemory);
        void* data;
        vkMapMemory(device,stagingMemory,0,size,0,&data);
        memcpy(data,pixelData,size);
        vkUnmapMemory(device,stagingMemory);
        auto commandBuffer=BeginOnceSubmit();
        VkImageMemoryBarrier barrier{};
        barrier.sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout=VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout=VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;
        barrier.image=((VulkanImage*)image)->Get();
        barrier.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel=0;
        barrier.subresourceRange.levelCount=info.mipMapLevels;
        barrier.subresourceRange.baseArrayLayer=0;
        barrier.subresourceRange.layerCount=info.arrayLayers;
        barrier.srcAccessMask=0;
        barrier.dstAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(commandBuffer,VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,0,0,nullptr,0,nullptr,1,&barrier);
        VkImageSubresourceLayers subresource{};
        subresource.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
        subresource.mipLevel=0;
        subresource.baseArrayLayer=0;
        subresource.layerCount=info.arrayLayers;
        VkBufferImageCopy region{};
        region.bufferOffset=0;
        region.bufferRowLength=0;
        region.bufferImageHeight=0;
        region.imageSubresource=subresource;
        region.imageOffset={0,0,0};
        region.imageExtent={info.extent.width,info.extent.height,1};
        vkCmdCopyBufferToImage(commandBuffer,stagingBuffer,((VulkanImage*)image)->Get(),VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&region);
        barrier.oldLayout=VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask=VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandBuffer,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,0,0,nullptr,0,nullptr,1,&barrier);
        EndOnceSubmit(commandBuffer);
        vkDestroyBuffer(device,stagingBuffer,nullptr);
        vkFreeMemory(device,stagingMemory,nullptr);
    }

    void VulkanGraphicInterface::CreateFrameBuffer(const FramebufferInfo &info, Z::Framebuffer *&frameBuffer) {
        std::vector<VkImageView> attachments(info.attachmentCount);
        for(int i=0;i<info.attachmentCount;++i){
            attachments[i]=((VulkanImageView*)info.pAttachments[i])->Get();
        }
        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType=VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass=((VulkanRenderPass*)info.renderPass)->Get();
        frameBufferInfo.attachmentCount=attachments.size();
        frameBufferInfo.pAttachments=attachments.data();
        frameBufferInfo.width=info.extent.width;
        frameBufferInfo.height=info.extent.height;
        frameBufferInfo.layers=info.layers;
        VkFramebuffer buffer;
        auto res= vkCreateFramebuffer(device,&frameBufferInfo,nullptr,&buffer);
        VK_CHECK(res,"failed to create frame buffer !");
        frameBuffer=new VulkanFramebuffer{};
        ((VulkanFramebuffer*)frameBuffer)->Set(buffer);
    }

    void VulkanGraphicInterface::CreateImageView(const ImageViewInfo &info, ImageView *&imageView) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image=((VulkanImage*)info.image)->Get();
        viewInfo.viewType=(VkImageViewType)info.viewType;
        viewInfo.format=(VkFormat)info.format;
        viewInfo.subresourceRange.aspectMask=(VkImageAspectFlags)info.subresourceRange.aspectMask;
        viewInfo.subresourceRange.baseMipLevel=info.subresourceRange.baseMipLevel;
        viewInfo.subresourceRange.levelCount=info.subresourceRange.levelCount;
        viewInfo.subresourceRange.baseArrayLayer=info.subresourceRange.baseArrayLayer;
        viewInfo.subresourceRange.layerCount=info.subresourceRange.layerCount;
        VkImageView view;
        auto res= vkCreateImageView(device,&viewInfo,nullptr,&view);
        VK_CHECK(res,"failed to create image view !");
        imageView=new VulkanImageView{};
        ((VulkanImageView*)imageView)->Set(view);
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

    void VulkanGraphicInterface::CreateBuffer(const BufferInfo& info,Buffer *&buffer, DeviceMemory *&memory, void *data) {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;
        VulkanUtils::CreateBuffer(physicalDevice,device,info.size,
                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  stagingBuffer,stagingMemory);
        void* mapData;
        vkMapMemory(device,stagingMemory,0,info.size,0,&mapData);
        memcpy(mapData,data,info.size);
        vkUnmapMemory(device,stagingMemory);

        VkBuffer resBuffer;
        VkDeviceMemory resMemory;
        VulkanUtils::CreateBuffer(physicalDevice,device,info.size,
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT|(VkBufferUsageFlagBits)info.usage,
                                  (VkMemoryPropertyFlagBits)info.properties,
                                  resBuffer,resMemory);

        auto commandBuffer=BeginOnceSubmit();
        VkBufferCopy copyRegion{};
        copyRegion.size=info.size;
        vkCmdCopyBuffer(commandBuffer,stagingBuffer,resBuffer,1,&copyRegion);
        EndOnceSubmit(commandBuffer);
        vkDestroyBuffer(device,stagingBuffer,nullptr);
        vkFreeMemory(device,stagingMemory,nullptr);
        buffer=new VulkanBuffer{};
        memory=new VulkanDeviceMemory{};
        ((VulkanBuffer*)buffer)->Set(resBuffer);
        ((VulkanDeviceMemory*)memory)->Set(resMemory);
    }

    void VulkanGraphicInterface::CreateShaderModule(const ShaderModuleCreateInfo &moduleInfo, ShaderModule *&module) {
        VkShaderModuleCreateInfo info{};
        info.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize=moduleInfo.codeSize;
        info.pCode=moduleInfo.pCode;
        VkShaderModule Module{};
        auto res= vkCreateShaderModule(device,&info,nullptr,&Module);
        VK_CHECK(res,"failed to create shader module !");
        module=new VulkanShaderModule{};
        ((VulkanShaderModule*)module)->Set(Module);
    }

    VkCommandBuffer VulkanGraphicInterface::BeginOnceSubmit() {

        auto allocateInfo = VkCommandBufferAllocateInfo{};
        allocateInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level=VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool=transientCommandPool;
        allocateInfo.commandBufferCount=1;
        VkCommandBuffer buffer{};
        vkAllocateCommandBuffers(device,&allocateInfo,&buffer);
        auto beginInfo = VkCommandBufferBeginInfo{};
        beginInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags=VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(buffer,&beginInfo);
        return buffer;
    }

    void VulkanGraphicInterface::EndOnceSubmit(VkCommandBuffer buffer) {
        vkEndCommandBuffer(buffer);
        auto submitInfo = VkSubmitInfo{};
        submitInfo.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount=1;
        submitInfo.pCommandBuffers=&buffer;
        auto queue=((VulkanQueue*)graphicsQueue)->Get();
        vkQueueSubmit(queue,1,&submitInfo,VK_NULL_HANDLE);
        auto res = vkQueueWaitIdle(queue);
        VK_CHECK(res, string_VkResult(res));
        vkFreeCommandBuffers(device,transientCommandPool,1,&buffer);
    }

    void VulkanGraphicInterface::CreateInstance(){
        #ifdef Z_DEBUG
        if(!VulkanUtils::checkLayerEnable(validationLayers).empty() ||
            !VulkanUtils::checkExtensionEnable({VK_EXT_DEBUG_UTILS_EXTENSION_NAME}).empty()){
            Z_CORE_WARN("Validation layer or debug utils extension is not available !");
            enableDebugUtils= false;
            enableValidationLayer= false;
        }
        #endif

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
            if(properties.deviceType == preferPhysicalDeviceType){
                Z_CORE_INFO("Using GPU : {0}",properties.deviceName);
                physicalDevice=phyDevice;
                break;
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

        VkQueue presentqueue;
        vkGetDeviceQueue(device,familyIndices.present.value(),0,&presentqueue);
        presentQueue=new VulkanQueue();
        ((VulkanQueue*)presentQueue)->Set(presentqueue);

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
        commandPools.resize(maxFlightFrames);
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
        commandBuffers.resize(maxFlightFrames);
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
        poolSizes[3].descriptorCount = 3 + 5 * maxMaterialCount + 1 + 1; 
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
        poolInfo.flags=VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
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

        frameFences.resize(maxFlightFrames);
        imageAvailable.resize(maxFlightFrames);
        imageRenderFinish.resize(maxFlightFrames);

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
        swapChainInfo.swapchainExtent={extent.width,extent.height};

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
        maxFlightFrames=swapchainImageCount;
        swapchainExtent=extent;
        swapchainFormat=swapchainformat.format;
        scissor={0,0,extent};
    }

    void VulkanGraphicInterface::CreateSwapchainImageViews() {
        swapchainImageViews.resize(swapchainImages.size());
        for(int i=0;i<swapchainImageViews.size();++i){
            swapchainImageViews[i]=VulkanUtils::CreateImageView(device,swapchainImages[i],swapchainFormat,
                                         VK_IMAGE_ASPECT_COLOR_BIT,VK_IMAGE_VIEW_TYPE_2D,1,1);
        }
    }

    void VulkanGraphicInterface::CreateFramebufferImageAndView() {

    }

    void VulkanGraphicInterface::CreateVmaAllocator() {
        // VmaVulkanFunctions vulkanFunctions    = {};
        // vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        // vulkanFunctions.vkGetDeviceProcAddr   = &vkGetDeviceProcAddr;

        // VmaAllocatorCreateInfo allocatorCreateInfo = {};
        // allocatorCreateInfo.vulkanApiVersion       = VK_API_VERSION_1_3;
        // allocatorCreateInfo.physicalDevice         = physicalDevice;
        // allocatorCreateInfo.device                 = device;
        // allocatorCreateInfo.instance               = instance;
        // allocatorCreateInfo.pVulkanFunctions       = &vulkanFunctions;

        // vmaCreateAllocator(&allocatorCreateInfo, &vmaAllocator);
    }

    void VulkanGraphicInterface::ReCreateSwapChain() {
        DestroySwapchain();
        CreateSwapchain();
        CreateSwapchainImageViews();
    }

    void VulkanGraphicInterface::InitInnerSetLayout(){

        innerSetLayouts.resize(2);

        DescriptorSetLayoutBinding bindingInfo[2]{};
        bindingInfo[0].binding=0;
        bindingInfo[0].descriptorCount=1;
        bindingInfo[0].descriptorType=DescriptorType::UNIFORM_BUFFER;
        bindingInfo[0].stageFlags=ShaderStageFlag::VERTEX;
        bindingInfo[1].binding=1;
        bindingInfo[1].descriptorCount=1;
        bindingInfo[1].descriptorType=DescriptorType::STORAGE_BUFFER;
        bindingInfo[1].stageFlags=ShaderStageFlag::VERTEX;

        DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount=2;
        layoutInfo.pBindings=bindingInfo;
        
        CreateDescriptorSetLayout(layoutInfo, innerSetLayouts[0]);

        DescriptorSetLayoutBinding lightDataSetBindingInfo[2]{};
        lightDataSetBindingInfo[0].binding=0;
        lightDataSetBindingInfo[0].descriptorType=DescriptorType::UNIFORM_BUFFER;
        lightDataSetBindingInfo[0].descriptorCount=1;
        lightDataSetBindingInfo[0].stageFlags=ShaderStageFlag::FRAGMENT;
        lightDataSetBindingInfo[1].binding=1;
        lightDataSetBindingInfo[1].descriptorType=DescriptorType::COMBINED_IMAGE_SAMPLER;
        lightDataSetBindingInfo[1].descriptorCount=1;
        lightDataSetBindingInfo[1].stageFlags=ShaderStageFlag::FRAGMENT;

        DescriptorSetLayoutCreateInfo lightDataSetInfo{};
        lightDataSetInfo.bindingCount=2;
        lightDataSetInfo.pBindings=lightDataSetBindingInfo;

        CreateDescriptorSetLayout(lightDataSetInfo,innerSetLayouts[1]);
    }

    void VulkanGraphicInterface::Init(const GraphicSpec &spec) {
        CreateInstance();
        initializeDebugMessenger();
        CreateWindowSurface();
        initializePhysicalDevice();
        CreateLogicalDevice();
        CreateSwapchain();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateDescriptorPool();
        CreateSyncSignals();
        CreateSwapchainImageViews();
        CreateVmaAllocator();
        CreateDefaultSampler();
        InitInnerSetLayout();
    }

    void VulkanGraphicInterface::Shutdown() {

        for(auto& layout:innerSetLayouts){
            DestroyDescriptorSetLayout(layout);
        }

        for(auto view:swapchainImageViews) {
            vkDestroyImageView(device,view,nullptr);
        }

        vkDestroyCommandPool(device,transientCommandPool,nullptr);
        for(int i=0;i<maxFlightFrames;++i) {
            vkDestroyFence(device,frameFences[i],nullptr);
            vkDestroySemaphore(device,imageRenderFinish[i],nullptr);
            vkDestroySemaphore(device,imageAvailable[i],nullptr);
            vkFreeCommandBuffers(device,commandPools[i],1,&commandBuffers[i]);
            vkDestroyCommandPool(device,commandPools[i],nullptr);
        }

        vkDestroyDescriptorPool(device,descriptorPool,nullptr);

        vkDestroySwapchainKHR(device,swapchain,nullptr);

        vkDestroySampler(device,((VulkanSampler*)defaultLinearSampler)->Get(),nullptr);
        vkDestroySampler(device,((VulkanSampler*)defaultNearestSampler)->Get(),nullptr);
        delete defaultLinearSampler;
        delete defaultNearestSampler;

        vkDestroyDevice(device,nullptr);

        vkDestroySurfaceKHR(instance,surface,nullptr);
        if(enableDebugUtils) {
            VulkanUtils::DestroyDebugUtils(instance,debugUtilsMessenger,nullptr);
        }
        vkDestroyInstance(instance,nullptr);
    }

    bool VulkanGraphicInterface::prepareBeforeRender(const std::function<void()> &funcCallAfterRecreateSwapChain) {
        auto acquireRes=vkAcquireNextImageKHR(device,swapchain,UINT64_MAX,imageAvailable[currentFrameIndex],
            VK_NULL_HANDLE,&currentSwapChainImageIndex);

        if(VK_SUBOPTIMAL_KHR==acquireRes||VK_ERROR_OUT_OF_DATE_KHR==acquireRes) {
            Z_CORE_WARN("Recreate swapchain !");
            ReCreateSwapChain();
            funcCallAfterRecreateSwapChain();
            return true;
        }else if(VK_SUCCESS!=acquireRes) {
            Z_CORE_ASSERT(false,"false to acquire next image from swapchain !");
            return true;
        }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags=0;

        auto res=vkBeginCommandBuffer(commandBuffers[currentFrameIndex],&beginInfo);

        VK_CHECK(res,"failed to begin command buffer !");

        return false;
    }

    void VulkanGraphicInterface::SubmitTask(const std::function<void()>&funcCallAfterRecreateSwapChain) {
        auto res=vkEndCommandBuffer(commandBuffers[currentFrameIndex]);
        VK_CHECK(res,"failed to end command buffer !");

        static VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo         submit_info   = {};
        submit_info.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount     = 1;
        submit_info.pWaitSemaphores        = &imageAvailable[currentFrameIndex];
        submit_info.pWaitDstStageMask      = waitStages;
        submit_info.commandBufferCount     = 1;
        submit_info.pCommandBuffers        = &commandBuffers[currentFrameIndex];
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &imageRenderFinish[currentFrameIndex];

        res = vkResetFences(device, 1, &frameFences[currentFrameIndex]);

        if (VK_SUCCESS != res)
        {
            Z_CORE_ERROR("failed to reset fences !");
            return;
        }
        res =
            vkQueueSubmit(((VulkanQueue*)graphicsQueue)->Get(), 1, &submit_info, frameFences[currentFrameIndex]);

        if (VK_SUCCESS != res)
        {
            Z_CORE_ERROR("failed to submit task !");
            return;
        }

        VkPresentInfoKHR present_info   = {};
        present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores    = &imageRenderFinish[currentFrameIndex];
        present_info.swapchainCount     = 1;
        present_info.pSwapchains        = &swapchain;
        present_info.pImageIndices      = &currentSwapChainImageIndex;

        res = vkQueuePresentKHR(((VulkanQueue*)presentQueue)->Get(), &present_info);
        if (VK_ERROR_OUT_OF_DATE_KHR == res || VK_SUBOPTIMAL_KHR == res)
        {
            ReCreateSwapChain();
            funcCallAfterRecreateSwapChain();
            return;
        }
        else if (VK_SUCCESS != res)
        {
            Z_CORE_ERROR("failed to present image !");
            return;
        }

        currentFrameIndex = (currentFrameIndex + 1) % maxFlightFrames;
    }

    void VulkanGraphicInterface::WaitForFences() {
        auto res=vkWaitForFences(device,1,&frameFences[currentFrameIndex],VK_TRUE,UINT64_MAX);
        VK_CHECK(res,"failed to wait fences !");
    }

    void VulkanGraphicInterface::DeviceWaiteIdle() {
        auto res= vkDeviceWaitIdle(device);
        VK_CHECK(res,"Unknown error happened when wait idle !");
    }

    void VulkanGraphicInterface::ResetCommandPool() {
        auto res=vkResetCommandPool(device,commandPools[currentFrameIndex],0);
        VK_CHECK(res,"failed to reset command pool !");
    }

    void VulkanGraphicInterface::BeginRenderPass(VkCommandBuffer buffer, const Z::RenderPassBeginInfo &info) {
        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType=VK_INFO(RENDER_PASS,BEGIN);
        beginInfo.framebuffer=((VulkanFramebuffer*)info.framebuffer)->Get();
        beginInfo.renderArea=*(VkRect2D*)&info.renderArea;
        //beginInfo.renderArea.extent=swapchainExtent;
        beginInfo.renderPass=((VulkanRenderPass*)info.renderPass)->Get();
        //Todo:fix :
        //mark: info.pClearValues is a stack array , it work well in debug mode with no optimize
        // but in release mode with optimize , it will have a undefined value
        // this caused validation layer to report a error :
        // vkCmdBeginRenderPass(): pClearValues[1].depth must be between 0 and 1.
        beginInfo.clearValueCount=info.clearValueCount;
        beginInfo.pClearValues=(VkClearValue*)info.pClearValues;

        vkCmdBeginRenderPass(buffer,&beginInfo,VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanGraphicInterface::EndRenderPass() {
        vkCmdEndRenderPass(commandBuffers[currentFrameIndex]);
    }

    void VulkanGraphicInterface::EndRenderPass(CommandBuffer* buffer) {
        vkCmdEndRenderPass(((VulkanCommandBuffer*)buffer)->Get());
    }

    void VulkanGraphicInterface::CreateRenderPass(const RenderPassCreateInfo &info, RenderPassInterface *&renderPassInterface) {
        VkRenderPassCreateInfo Info{};
        Info.sType=VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        std::vector<VkAttachmentDescription> descriptions(info.attachmentCount);
        int index=0;
        if(info.pAttachments!=nullptr)
        for(auto&description:descriptions) {
            auto&attachment=info.pAttachments[index];
            description.format=(VkFormat)attachment.format;
            description.initialLayout=(VkImageLayout)attachment.initialLayout;
            description.finalLayout=(VkImageLayout)attachment.finalLayout;
            description.loadOp=(VkAttachmentLoadOp)attachment.loadOp;
            description.storeOp=(VkAttachmentStoreOp)attachment.storeOp;
            description.stencilLoadOp=(VkAttachmentLoadOp)attachment.stencilLoadOp;
            description.stencilStoreOp=(VkAttachmentStoreOp)attachment.stencilStoreOp;
            description.samples=(VkSampleCountFlagBits)attachment.samples;
            ++index;
        }
        index=0;
        std::vector<VkSubpassDependency> dependencies(info.dependencyCount);
        if(info.pDependencies!=nullptr)
        for(auto&dependency:dependencies) {
            const auto&dep=info.pDependencies[index];
            dependency.dstSubpass=(uint32)dep.dstSubpass;
            dependency.srcSubpass=(uint32)dep.srcSubpass;
            dependency.dstAccessMask=(VkAccessFlags)dep.dstAccessMask;
            dependency.srcAccessMask=(VkAccessFlags)dep.srcAccessMask;
            dependency.dstStageMask=(VkPipelineStageFlags)dep.dstStageMask;
            dependency.srcStageMask=(VkPipelineStageFlags)dep.srcStageMask;
            dependency.dependencyFlags=(VkDependencyFlags)dep.dependencyFlags;
            ++index;
        }
        index=0;
        std::vector<VkSubpassDescription> subpasses(info.subpassCount);
        std::vector<std::vector<VkAttachmentReference>> attachmentsReferences(info.subpassCount);
        std::vector<std::vector<VkAttachmentReference>> inputReferences(info.subpassCount);
        std::vector<VkAttachmentReference> depthStencilReferences(info.subpassCount);
        if(info.pSubpasses!=nullptr)
            for(auto&subpass:subpasses) {
                const auto& des=info.pSubpasses[index];
                auto&attachmentRef=attachmentsReferences[index];
                auto&inputRef=inputReferences[index];
                auto&depthRef=depthStencilReferences[index];
                attachmentRef.resize(des.colorAttachmentCount);
                inputRef.resize(des.inputAttachmentCount);
                for(int i=0;i<des.colorAttachmentCount;++i) {
                    attachmentRef[i].attachment=des.pColorAttachments[i].attachment;
                    attachmentRef[i].layout=(VkImageLayout)des.pColorAttachments[i].layout;
                }
                for(int i=0;i<des.inputAttachmentCount;++i) {
                    inputRef[i].attachment=des.pInputAttachments[i].attachment;
                    inputRef[i].layout=(VkImageLayout)des.pInputAttachments[i].layout;
                }
                if(des.pDepthStencilAttachment!=nullptr) {
                    depthRef.attachment=des.pDepthStencilAttachment->attachment;
                    depthRef.layout=(VkImageLayout)des.pDepthStencilAttachment->layout;
                }
                subpass.colorAttachmentCount=des.colorAttachmentCount;
                subpass.inputAttachmentCount=des.inputAttachmentCount;
                subpass.pipelineBindPoint=(VkPipelineBindPoint)des.pipelineBindPoint;
                subpass.pColorAttachments=attachmentRef.data();
                subpass.pDepthStencilAttachment=des.pDepthStencilAttachment==nullptr?nullptr:&depthRef;
                subpass.pInputAttachments=inputRef.data();
                ++index;
            }
        Info.pAttachments=descriptions.data();
        Info.attachmentCount=descriptions.size();
        Info.dependencyCount=dependencies.size();
        Info.pDependencies=dependencies.data();
        Info.pSubpasses=subpasses.data();
        Info.subpassCount=subpasses.size();

        VkRenderPass renderpass{};
        auto res=vkCreateRenderPass(device,&Info,nullptr,&renderpass);
        renderPassInterface=new VulkanRenderPass{};
        VK_CHECK(res,"failed to create RenderPass !");
        ((VulkanRenderPass*)renderPassInterface)->Set(renderpass);
    }

    void VulkanGraphicInterface::CreateGraphicPipeline(const GraphicPipelineCreateInfo &createInfo,
        Pipeline *&graphicPipeline) {
        std::vector<VkPipelineShaderStageCreateInfo> stages(createInfo.stageCount);
        std::vector<VkSpecializationInfo> stagesSpecInfo(stages.size());
        for(int i=0;i<createInfo.stageCount;++i) {
            const auto&stage=createInfo.pStages[i];
            stages[i].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stages[i].flags=(VkPipelineShaderStageCreateFlags)stage.flags;
            stages[i].module=((VulkanShaderModule*)stage.module)->Get();
            stages[i].pName=stage.pName;
            stages[i].pNext=stage.pNext;
            auto &specInfo=stagesSpecInfo[i];
            specInfo.dataSize=stage.pSpecializationInfo->dataSize;
            specInfo.pData=stage.pSpecializationInfo->pData;
            specInfo.mapEntryCount=stage.pSpecializationInfo->mapEntryCount;
            specInfo.pMapEntries=(VkSpecializationMapEntry*)stage.pSpecializationInfo->pMapEntries;
            stages[i].pSpecializationInfo=&specInfo;
        }

        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount=createInfo.pDynamicState->dynamicStateCount;
        dynamicStateInfo.pDynamicStates=(VkDynamicState*)createInfo.pDynamicState->pDynamicStates;

        VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{};
        vertexInputStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateInfo.pVertexAttributeDescriptions=(VkVertexInputAttributeDescription*)createInfo.pVertexInputState->pVertexAttributeDescriptions;
        vertexInputStateInfo.vertexAttributeDescriptionCount=createInfo.pVertexInputState->vertexAttributeDescriptionCount;
        vertexInputStateInfo.pVertexBindingDescriptions=(VkVertexInputBindingDescription*)createInfo.pVertexInputState->pVertexBindingDescriptions;
        vertexInputStateInfo.vertexBindingDescriptionCount=createInfo.pVertexInputState->vertexBindingDescriptionCount;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo{};
        inputAssemblyStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateInfo.topology=(VkPrimitiveTopology)createInfo.pInputAssemblyState->topology;
        inputAssemblyStateInfo.primitiveRestartEnable=createInfo.pInputAssemblyState->primitiveRestartEnable;
        inputAssemblyStateInfo.pNext=nullptr;

        VkPipelineColorBlendStateCreateInfo blendStateInfo{};
        blendStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendStateInfo.attachmentCount=createInfo.pColorBlendState->attachmentCount;
        blendStateInfo.logicOp=(VkLogicOp)createInfo.pColorBlendState->logicOp;
        blendStateInfo.logicOpEnable=createInfo.pColorBlendState->logicOpEnable;
        for(int i=0;i<4;++i)
            blendStateInfo.blendConstants[i]=createInfo.pColorBlendState->blendConstants[i];
        blendStateInfo.attachmentCount=createInfo.pColorBlendState->attachmentCount;
        blendStateInfo.pAttachments=(VkPipelineColorBlendAttachmentState*)(createInfo.pColorBlendState->pAttachments);
        blendStateInfo.pNext=nullptr;

        VkPipelineViewportStateCreateInfo viewportStateInfo{};
        viewportStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.pScissors=(VkRect2D*)createInfo.pViewportState->pScissors;
        viewportStateInfo.pViewports=(VkViewport*)createInfo.pViewportState->pViewports;
        viewportStateInfo.scissorCount=createInfo.pViewportState->scissorCount;
        viewportStateInfo.viewportCount=createInfo.pViewportState->viewportCount;

        VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo{};
        std::memcpy(((char*)(&depthStencilStateInfo))+selfDefStructureOffset,createInfo.pDepthStencilState,sizeof(PipelineDepthStencilStateCreateInfo));
        depthStencilStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        VkPipelineMultisampleStateCreateInfo multisampleStateInfo{};
        std::memcpy(((char*)(&multisampleStateInfo))+selfDefStructureOffset,createInfo.pMultisampleState,sizeof(PipelineMultisampleStateCreateInfo));
        multisampleStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
        std::memcpy(((char*)(&rasterizationStateInfo))+selfDefStructureOffset,createInfo.pRasterizationState,sizeof(PipelineRasterizationStateCreateInfo));
        rasterizationStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

        VkPipelineTessellationStateCreateInfo tessellationStateInfo{};
        tessellationStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellationStateInfo.patchControlPoints=createInfo.pTessellationState->patchControlPoints;

        VkPipelineLayout pipelineLayout=((VulkanPipelineLayout*)createInfo.pLayout)->Get();

        VkGraphicsPipelineCreateInfo info{};
        info.sType=VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.subpass=createInfo.subpass;
        info.renderPass=((VulkanRenderPass*)createInfo.renderPass)->Get();
        info.stageCount=stages.size();
        info.pStages=stages.data();
        info.pDynamicState=&dynamicStateInfo;
        info.pVertexInputState=&vertexInputStateInfo;
        info.pColorBlendState=&blendStateInfo;
        info.pViewportState=&viewportStateInfo;
        info.pInputAssemblyState=&inputAssemblyStateInfo;
        info.basePipelineHandle=createInfo.pBasePipelineHandle==nullptr?VK_NULL_HANDLE:((VulkanPipeline*)createInfo.pBasePipelineHandle)->Get();
        info.pDepthStencilState=&depthStencilStateInfo;
        info.pMultisampleState=&multisampleStateInfo;
        info.layout=pipelineLayout;
        info.basePipelineIndex=0;
        info.pTessellationState=&tessellationStateInfo;

        VkPipeline pipeline{};
        auto res=vkCreateGraphicsPipelines(device,VK_NULL_HANDLE,1,&info,nullptr,&pipeline);
        VK_CHECK(res,"failed to create graphic pipeline !");
        graphicPipeline=new VulkanPipeline{};
        ((VulkanPipeline*)graphicPipeline)->Set(pipeline);
    }

    std::vector<DescriptorSetInfo> VulkanGraphicInterface::CreateGraphicPipeline(const std::string &shaderSources,
        const std::vector<Z::ShaderStageFlag> &stageFlags, Pipeline *&graphicPipeline,
        RenderPassInterface *renderPassInterface,std::vector<DescriptorSetLayout*>& descriptorSetLayout,PipelineLayout*& pipelineLayout,
        GraphicPipelineCreateInfo* createInfo,bool insertInnerSetLayout) {
            GraphicPipelineCreateInfo info{};
            std::vector<DescriptorSetInfo> setInfos;
            if(createInfo!=nullptr)
                info=*createInfo;
            auto shaderInfo=VulkanUtils::ReflectShader(shaderSources,stageFlags);
            auto descriptorLayout=VulkanUtils::CreateDescriptorSetLayout(device,shaderInfo.descriptorInfos,insertInnerSetLayout?2:0);
            //descriptorLayout.insert(descriptorLayout.begin(),((VulkanDescriptorSetLayout*)innerSetLayouts)->Get());
            if(insertInnerSetLayout)
                for(int i=0;i<innerSetLayouts.size();++i){
                    descriptorLayout.insert(descriptorLayout.begin()+i,((VulkanDescriptorSetLayout*)innerSetLayouts[i])->Get());
                }
            descriptorSetLayout.resize(descriptorLayout.size());
            for(int i=0;i<descriptorLayout.size();++i){
                descriptorSetLayout[i]=new VulkanDescriptorSetLayout{};
                ((VulkanDescriptorSetLayout*)descriptorSetLayout[i])->Set(descriptorLayout[i]);
            }
            for(int i=0;i<shaderInfo.descriptorInfos.size();++i){
                const auto& lInfo=shaderInfo.descriptorInfos[i];
                const auto& bindings=lInfo.bindings;
                const auto& names = lInfo.bindingNames;
                setInfos.emplace_back();
                auto&back=setInfos.back();
                
                back.set=i;
                back.bindings.resize(bindings.size());
                for(int j=0;j<bindings.size();++j){
                    auto & bind=back.bindings[j];
                    bind.binding=bindings[j].binding;
                    bind.name=names[j];
                    bind.type=(DescriptorType)bindings[j].descriptorType;
                }
            }
            if(!pipelineLayout){
                VkPipelineLayoutCreateInfo layoutInfo{};
                layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                layoutInfo.setLayoutCount = descriptorLayout.size();
                layoutInfo.pSetLayouts = descriptorLayout.data();
                if (shaderInfo.pushConstantRange.size > 0) {
                    layoutInfo.pPushConstantRanges = &shaderInfo.pushConstantRange;
                    layoutInfo.pushConstantRangeCount = 1;
                } else {
                    layoutInfo.pushConstantRangeCount = 0;
                }
                VkPipelineLayout layout;
                auto res = vkCreatePipelineLayout(device, &layoutInfo, nullptr, &layout);
                VK_CHECK(res, "failed to create pipeline layout !");
                pipelineLayout = new VulkanPipelineLayout{};
                ((VulkanPipelineLayout *) pipelineLayout)->Set(layout);
            }

            std::vector<VkPipelineShaderStageCreateInfo> stages(shaderInfo.irCode.size());
            std::vector<VkShaderModule> modules(shaderInfo.irCode.size());
            for(int i=0;i<stages.size();++i) {
                VkShaderModuleCreateInfo moduleInfo{};
                moduleInfo.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                moduleInfo.pCode=shaderInfo.irCode[i].data();
                moduleInfo.codeSize=shaderInfo.irCode[i].size()*4;
                VkShaderModule module;
                auto res=vkCreateShaderModule(device,&moduleInfo,nullptr,&module);
                VK_CHECK(res,"failed to create shader module !");
                modules[i]=module;
                stages[i].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                stages[i].stage=(VkShaderStageFlagBits)stageFlags[i];
                stages[i].module=module;
                stages[i].pName="main";
            }
            static auto vertexDescription=MeshDescription::GetAttributeDescription();
            static auto vertexBinding=MeshDescription::GetBindingDescription();
            VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{};
            vertexInputStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputStateInfo.vertexBindingDescriptionCount=0;
            vertexInputStateInfo.vertexAttributeDescriptionCount=0;
            if(shaderInfo.vertexInput){
                if(info.pVertexInputState==nullptr){
                    vertexInputStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                    vertexInputStateInfo.vertexAttributeDescriptionCount=vertexDescription.size();
                    vertexInputStateInfo.pVertexAttributeDescriptions=(VkVertexInputAttributeDescription*)vertexDescription.data();
                    vertexInputStateInfo.vertexBindingDescriptionCount=vertexBinding.size();
                    vertexInputStateInfo.pVertexBindingDescriptions=(VkVertexInputBindingDescription*)vertexBinding.data();
                }else{
                    vertexInputStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                    vertexInputStateInfo.vertexAttributeDescriptionCount=info.pVertexInputState->vertexAttributeDescriptionCount;
                    vertexInputStateInfo.pVertexAttributeDescriptions=(VkVertexInputAttributeDescription*)info.pVertexInputState->pVertexAttributeDescriptions;
                    vertexInputStateInfo.vertexBindingDescriptionCount=info.pVertexInputState->vertexBindingDescriptionCount;
                    vertexInputStateInfo.pVertexBindingDescriptions=(VkVertexInputBindingDescription*)info.pVertexInputState->pVertexBindingDescriptions;
                }
            }
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo{};
            if(info.pInputAssemblyState==nullptr){
                inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                inputAssemblyStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;
            }else{
                inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                inputAssemblyStateInfo.topology = (VkPrimitiveTopology)info.pInputAssemblyState->topology;
                inputAssemblyStateInfo.primitiveRestartEnable = info.pInputAssemblyState->primitiveRestartEnable;
            }
            static VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
            VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
            if(info.pDynamicState==nullptr){
                dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicStateInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
                dynamicStateInfo.pDynamicStates = dynamicStates;
            }else{
                dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicStateInfo.dynamicStateCount = info.pDynamicState->dynamicStateCount;
                dynamicStateInfo.pDynamicStates = (VkDynamicState*)info.pDynamicState->pDynamicStates;
            }
            VkPipelineViewportStateCreateInfo viewportStateInfo{};
            if(info.pViewportState==nullptr){
                viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportStateInfo.viewportCount = 1;
                viewportStateInfo.scissorCount = 1;
            }else{
                viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportStateInfo.viewportCount = info.pViewportState->viewportCount;
                viewportStateInfo.scissorCount = info.pViewportState->scissorCount;
                viewportStateInfo.pScissors = (VkRect2D*)info.pViewportState->pScissors;
                viewportStateInfo.pViewports = (VkViewport*)info.pViewportState->pViewports;
            }
            std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment{};
            VkPipelineColorBlendStateCreateInfo blendStateInfo{};
            if(info.pColorBlendState==nullptr){
                colorBlendAttachment.resize(shaderInfo.fragmentOutputs.size());
                for(int i=0;i<shaderInfo.fragmentOutputs.size();++i){
                    colorBlendAttachment[i].blendEnable=false;
                    colorBlendAttachment[i].colorWriteMask=VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT;
                }
                blendStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                blendStateInfo.logicOpEnable=VK_FALSE;
                blendStateInfo.logicOp=VK_LOGIC_OP_COPY;
                blendStateInfo.attachmentCount=colorBlendAttachment.size();
                blendStateInfo.pAttachments=colorBlendAttachment.data();
            }else{
                blendStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                blendStateInfo.attachmentCount=info.pColorBlendState->attachmentCount;
                blendStateInfo.logicOp=(VkLogicOp)info.pColorBlendState->logicOp;
                blendStateInfo.logicOpEnable=info.pColorBlendState->logicOpEnable;
                blendStateInfo.pAttachments=(VkPipelineColorBlendAttachmentState*)info.pColorBlendState->pAttachments;
                for(int i=0;i<4;++i)
                    blendStateInfo.blendConstants[i]=info.pColorBlendState->blendConstants[i];
            }
            VkPipelineMultisampleStateCreateInfo multisampleStateInfo{};
            if(info.pMultisampleState==nullptr){
                multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                multisampleStateInfo.sampleShadingEnable = VK_FALSE;
                multisampleStateInfo.rasterizationSamples = defaultSampleFlag;
            }else{
                multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                multisampleStateInfo.sampleShadingEnable = info.pMultisampleState->sampleShadingEnable;
                multisampleStateInfo.rasterizationSamples = (VkSampleCountFlagBits)info.pMultisampleState->rasterizationSamples;
                multisampleStateInfo.minSampleShading = info.pMultisampleState->minSampleShading;
                multisampleStateInfo.alphaToCoverageEnable = info.pMultisampleState->alphaToCoverageEnable;
                multisampleStateInfo.alphaToOneEnable = info.pMultisampleState->alphaToOneEnable;
            }
            VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
            if(info.pRasterizationState==nullptr){
                rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                rasterizationStateInfo.depthClampEnable = VK_FALSE;
                rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
                rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
                rasterizationStateInfo.lineWidth = 1.0f;
                rasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
                rasterizationStateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                rasterizationStateInfo.depthBiasEnable = VK_FALSE;
                rasterizationStateInfo.depthBiasConstantFactor = 0.0f;
                rasterizationStateInfo.depthBiasClamp = 0.0f;
                rasterizationStateInfo.depthBiasSlopeFactor = 0.0f;
            }else{
                rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                rasterizationStateInfo.depthClampEnable = info.pRasterizationState->depthClampEnable;
                rasterizationStateInfo.rasterizerDiscardEnable = info.pRasterizationState->rasterizerDiscardEnable;
                rasterizationStateInfo.polygonMode = (VkPolygonMode)info.pRasterizationState->polygonMode;
                rasterizationStateInfo.lineWidth = info.pRasterizationState->lineWidth;
                rasterizationStateInfo.cullMode = (VkCullModeFlags)info.pRasterizationState->cullMode;
                rasterizationStateInfo.frontFace = (VkFrontFace)info.pRasterizationState->frontFace;
                rasterizationStateInfo.depthBiasEnable = info.pRasterizationState->depthBiasEnable;
                rasterizationStateInfo.depthBiasConstantFactor = info.pRasterizationState->depthBiasConstantFactor;
                rasterizationStateInfo.depthBiasClamp = info.pRasterizationState->depthBiasClamp;
                rasterizationStateInfo.depthBiasSlopeFactor = info.pRasterizationState->depthBiasSlopeFactor;
            }
            VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo{};
            if(info.pDepthStencilState==nullptr){
                depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                depthStencilStateInfo.depthTestEnable = VK_TRUE;
                depthStencilStateInfo.depthWriteEnable = VK_TRUE;
                depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
                depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
                depthStencilStateInfo.stencilTestEnable = VK_FALSE;
                depthStencilStateInfo.minDepthBounds = 0.0f;
                depthStencilStateInfo.maxDepthBounds = 1.0f;
            }else{
                depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                depthStencilStateInfo.depthTestEnable = info.pDepthStencilState->depthTestEnable;
                depthStencilStateInfo.depthWriteEnable = info.pDepthStencilState->depthWriteEnable;
                depthStencilStateInfo.depthCompareOp = (VkCompareOp)info.pDepthStencilState->depthCompareOp;
                depthStencilStateInfo.depthBoundsTestEnable = info.pDepthStencilState->depthBoundsTestEnable;
                depthStencilStateInfo.stencilTestEnable = info.pDepthStencilState->stencilTestEnable;
                depthStencilStateInfo.front = *(VkStencilOpState*)&info.pDepthStencilState->front;
                depthStencilStateInfo.back = *(VkStencilOpState*)&info.pDepthStencilState->back;
                depthStencilStateInfo.minDepthBounds = info.pDepthStencilState->minDepthBounds;
                depthStencilStateInfo.maxDepthBounds = info.pDepthStencilState->maxDepthBounds;
            }

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType=VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount=stages.size();
            pipelineInfo.pStages=stages.data();
            pipelineInfo.pVertexInputState=&vertexInputStateInfo;
            pipelineInfo.pInputAssemblyState=&inputAssemblyStateInfo;
            pipelineInfo.pViewportState=&viewportStateInfo;
            pipelineInfo.pRasterizationState=&rasterizationStateInfo;
            pipelineInfo.pMultisampleState=&multisampleStateInfo;
            pipelineInfo.pDepthStencilState=&depthStencilStateInfo;
            pipelineInfo.pColorBlendState=&blendStateInfo;
            pipelineInfo.pDynamicState=&dynamicStateInfo;
            pipelineInfo.layout=((VulkanPipelineLayout*)pipelineLayout)->Get();
            pipelineInfo.renderPass=((VulkanRenderPass*)renderPassInterface)->Get();
            pipelineInfo.subpass=info.subpass<0?0:info.subpass;
            pipelineInfo.basePipelineHandle=VK_NULL_HANDLE;
            pipelineInfo.basePipelineIndex=-1;
            VkPipeline pipeline;
            auto res=vkCreateGraphicsPipelines(device,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&pipeline);
            VK_CHECK(res,"failed to create graphic pipeline !");
            graphicPipeline=new VulkanPipeline{};
            ((VulkanPipeline*)graphicPipeline)->Set(pipeline);
            for(auto module:modules) {
                vkDestroyShaderModule(device,module,nullptr);
            }
            return setInfos;
    }

    void VulkanGraphicInterface::DestroyRenderPass(RenderPassInterface *renderPassInterface) {
        vkDestroyRenderPass(device,((VulkanRenderPass*)renderPassInterface)->Get(),nullptr);
        delete renderPassInterface;
    }

    std::vector<Z::Framebuffer *> VulkanGraphicInterface::CreateDefaultFrameBuffers(
        RenderPassInterface *renderPassInterface) {
        std::vector<Z::Framebuffer*> result(swapchainImageViews.size());
        VkFramebufferCreateInfo createInfo{};
        createInfo.sType=VK_INFO(FRAMEBUFFER,CREATE);
        createInfo.height=swapchainExtent.height;
        createInfo.width=swapchainExtent.width;
        createInfo.attachmentCount=1;
        createInfo.layers=1;
        createInfo.renderPass=((VulkanRenderPass*)renderPassInterface)->Get();
        createInfo.flags=0;
        createInfo.pNext=nullptr;
        for(int i=0;i<result.size();++i) {
            VkFramebuffer buffer{};
            createInfo.pAttachments=&swapchainImageViews[i];
            auto res=vkCreateFramebuffer(device,&createInfo,nullptr,&buffer);
            VK_CHECK(res,"failed to create frame buffers !");
            auto framebuffer=new VulkanFramebuffer{};
            framebuffer->Set(buffer);
            result[i]=framebuffer;
        }
        return std::move(result);
    }

    void VulkanGraphicInterface::DestroyFrameBuffer(Z::Framebuffer *framebuffer) {
        vkDestroyFramebuffer(device,((VulkanFramebuffer*)framebuffer)->Get(),nullptr);
        delete framebuffer;
    }

    void VulkanGraphicInterface::DestroyPipeline(Pipeline *pipeline) {
        vkDestroyPipeline(device,((VulkanPipeline*)pipeline)->Get(),nullptr);
        delete pipeline;
    }

    void VulkanGraphicInterface::DestroyPipelineLayout(PipelineLayout *pipelineLayout) {
        vkDestroyPipelineLayout(device,((VulkanPipelineLayout*)pipelineLayout)->Get(),nullptr);
        delete pipelineLayout;
    }

    void VulkanGraphicInterface::DestroyImage(Image * image , DeviceMemory* memory , ImageView* view) {
        if(view!=nullptr) {
            vkDestroyImageView(device, ((VulkanImageView *) view)->Get(), nullptr);
            delete view;
        }
        vkFreeMemory(device,((VulkanDeviceMemory*)memory)->Get(),nullptr);
        delete memory;
        vkDestroyImage(device,((VulkanImage*)image)->Get(),nullptr);
        delete image;
    }

    void VulkanGraphicInterface::DestroyImageView(ImageView* view){
        vkDestroyImageView(device,((VulkanImageView*)view)->Get(), nullptr);
        delete view;
    }

    void VulkanGraphicInterface::DestroyBuffer(Buffer *buffer, DeviceMemory *memory) {
        vkFreeMemory(device,((VulkanDeviceMemory*)memory)->Get(),nullptr);
        delete memory;
        vkDestroyBuffer(device,((VulkanBuffer*)buffer)->Get(),nullptr);
        delete buffer;
    }

    void VulkanGraphicInterface::DestroyDescriptorSetLayout(DescriptorSetLayout *descriptorSetLayout) {
        vkDestroyDescriptorSetLayout(device,((VulkanDescriptorSetLayout*)descriptorSetLayout)->Get(),nullptr);
        delete descriptorSetLayout;
    }

    void VulkanGraphicInterface::CreateDescriptorSetLayout(const DescriptorSetLayoutCreateInfo &info,
                                                           DescriptorSetLayout *&descriptorSetLayout) {
        VkDescriptorSetLayoutCreateInfo createInfo{};
        createInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.bindingCount=info.bindingCount;
        std::vector<VkDescriptorSetLayoutBinding> bindings(info.bindingCount);
        for(int i=0;i<info.bindingCount;++i) {
            auto&binding=bindings[i];
            auto&infoBinding=info.pBindings[i];
            binding.binding=infoBinding.binding;
            binding.descriptorCount=infoBinding.descriptorCount;
            binding.descriptorType=(VkDescriptorType)infoBinding.descriptorType;
            binding.stageFlags=(VkShaderStageFlags)infoBinding.stageFlags;
            binding.pImmutableSamplers=nullptr;
        }
        createInfo.pBindings=bindings.data();
        VkDescriptorSetLayout layout;
        auto res=vkCreateDescriptorSetLayout(device,&createInfo,nullptr,&layout);
        VK_CHECK(res,"failed to create descriptor set layout !");
        descriptorSetLayout=new VulkanDescriptorSetLayout{};
        ((VulkanDescriptorSetLayout*)descriptorSetLayout)->Set(layout);
    }

    void VulkanGraphicInterface::AllocateDescriptorSet(const DescriptorSetAllocateInfo &info,
                                                       DescriptorSet *&descriptorSet) {
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType=VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool= info.descriptorPool!= nullptr? ((VulkanDescriptorPool*)info.descriptorPool)->Get() : descriptorPool;
        allocateInfo.descriptorSetCount=info.descriptorSetCount;
        std::vector<VkDescriptorSetLayout> layouts(info.descriptorSetCount);
        for(int i=0;i<info.descriptorSetCount;++i) {
            layouts[i]=((VulkanDescriptorSetLayout*)(info.pSetLayouts+i))->Get();
        }
        allocateInfo.pSetLayouts=layouts.data();
        VkDescriptorSet set;
        auto res=vkAllocateDescriptorSets(device,&allocateInfo,&set);
        VK_CHECK(res,"failed to allocate descriptor set !");
        descriptorSet=new VulkanDescriptorSet{};
        ((VulkanDescriptorSet*)descriptorSet)->Set(set);
    }

    void VulkanGraphicInterface::FreeDescriptorSet(DescriptorSet *descriptorSet) {
        vkFreeDescriptorSets(device,descriptorPool,1,((VulkanDescriptorSet*)descriptorSet)->GetPtr());
        delete descriptorSet;
    }

    void VulkanGraphicInterface::CreateCubeMap(const ImageInfo &info, Image *&image, DeviceMemory *&memory,
                                               ImageView *&imageView, const std::array<void *, 6> &pixelData) {
        auto pixelSize=VulkanUtils::GetPixelSizeFromFormat((VkFormat)info.format);
        VkImageCreateInfo imageInfo{};
        imageInfo.sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.arrayLayers=6;
        imageInfo.extent.width=info.extent.width;
        imageInfo.extent.height=info.extent.height;
        imageInfo.extent.depth=1;
        imageInfo.format=(VkFormat)info.format;
        imageInfo.imageType=VK_IMAGE_TYPE_2D;
        imageInfo.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.mipLevels=1;
        imageInfo.samples=VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.tiling=VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage=VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.flags=VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        imageInfo.pNext=nullptr;
        imageInfo.queueFamilyIndexCount=0;
        imageInfo.pQueueFamilyIndices=nullptr;
        VkImage imageHandle;
        auto res=vkCreateImage(device,&imageInfo,nullptr,&imageHandle);
        VK_CHECK(res,"failed to create image !");
        image=new VulkanImage{};
        ((VulkanImage*)image)->Set(imageHandle);
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device,imageHandle,&memoryRequirements);
        VkMemoryAllocateInfo allocateInfo{};
        allocateInfo.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize=memoryRequirements.size;
        allocateInfo.memoryTypeIndex=VulkanUtils::FindMemoryType(physicalDevice,memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        VkDeviceMemory memoryHandle;
        res=vkAllocateMemory(device,&allocateInfo,nullptr,&memoryHandle);
        VK_CHECK(res,"failed to allocate memory !");
        vkBindImageMemory(device,imageHandle,memoryHandle,0);
        memory=new VulkanDeviceMemory{};
        ((VulkanDeviceMemory*)memory)->Set(memoryHandle);
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType=VK_IMAGE_VIEW_TYPE_CUBE;
        viewInfo.format=(VkFormat)info.format;
        viewInfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel=0;
        viewInfo.subresourceRange.levelCount=1;
        viewInfo.subresourceRange.baseArrayLayer=0;
        viewInfo.subresourceRange.layerCount=6;
        viewInfo.image=imageHandle;
        viewInfo.pNext=nullptr;
        viewInfo.flags=0;
        viewInfo.components.r=VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g=VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b=VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a=VK_COMPONENT_SWIZZLE_IDENTITY;
        VkImageView viewHandle;
        res=vkCreateImageView(device,&viewInfo,nullptr,&viewHandle);
        VK_CHECK(res,"failed to create image view !");
        imageView=new VulkanImageView{};
        ((VulkanImageView*)imageView)->Set(viewHandle);

        VkBufferCreateInfo stagingBufferInfo{};
        stagingBufferInfo.sType=VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        stagingBufferInfo.size=info.extent.width*info.extent.height*pixelSize*6;
        stagingBufferInfo.usage=VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        stagingBufferInfo.sharingMode=VK_SHARING_MODE_EXCLUSIVE;
        VkBuffer stagingBuffer;
        res=vkCreateBuffer(device,&stagingBufferInfo,nullptr,&stagingBuffer);
        VK_CHECK(res,"failed to create staging buffer !");
        VkMemoryRequirements stagingMemoryRequirements;
        vkGetBufferMemoryRequirements(device,stagingBuffer,&stagingMemoryRequirements);
        VkMemoryAllocateInfo stagingAllocateInfo{};
        stagingAllocateInfo.sType=VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        stagingAllocateInfo.allocationSize=stagingMemoryRequirements.size;
        stagingAllocateInfo.memoryTypeIndex=VulkanUtils::FindMemoryType(physicalDevice,stagingMemoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        VkDeviceMemory stagingMemory;
        res=vkAllocateMemory(device,&stagingAllocateInfo,nullptr,&stagingMemory);
        VK_CHECK(res,"failed to allocate staging memory !");
        vkBindBufferMemory(device,stagingBuffer,stagingMemory,0);
        void* data;
        res=vkMapMemory(device,stagingMemory,0,stagingMemoryRequirements.size,0,&data);
        VK_CHECK(res,"failed to map memory !");
        for(int i=0;i<6;++i) {
            std::memcpy((char*)data+info.extent.width*info.extent.height*pixelSize*i,pixelData[i],info.extent.width*info.extent.height*pixelSize);
        }
        vkUnmapMemory(device,stagingMemory);

        std::vector<VkBufferImageCopy> copyRegions(6);
        for(int i=0;i<6;++i) {
            copyRegions[i].bufferOffset=info.extent.width*info.extent.height*pixelSize*i;
            copyRegions[i].bufferRowLength=0;
            copyRegions[i].bufferImageHeight=0;
            copyRegions[i].imageSubresource.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegions[i].imageSubresource.baseArrayLayer=i;
            copyRegions[i].imageSubresource.layerCount=1;
            copyRegions[i].imageSubresource.mipLevel=0;
            copyRegions[i].imageOffset={0,0,0};
            copyRegions[i].imageExtent={info.extent.width,info.extent.height,1};
        }
        auto commandBuffer=BeginOnceSubmit();
        VkImageMemoryBarrier barrier{};
        barrier.sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout=VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout=VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;
        barrier.image=imageHandle;
        barrier.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel=0;
        barrier.subresourceRange.levelCount=1;
        barrier.subresourceRange.baseArrayLayer=0;
        barrier.subresourceRange.layerCount=6;
        barrier.srcAccessMask=0;
        barrier.dstAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;
        vkCmdPipelineBarrier(commandBuffer,VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT,0,0,nullptr,0,nullptr,1,&barrier);
        vkCmdCopyBufferToImage(commandBuffer,stagingBuffer,imageHandle,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,copyRegions.size(),copyRegions.data());
        barrier.oldLayout=VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout=VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask=VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask=VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(commandBuffer,VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,0,0,nullptr,0,nullptr,1,&barrier);
        EndOnceSubmit(commandBuffer);
        vkFreeMemory(device,stagingMemory,nullptr);
        vkDestroyBuffer(device,stagingBuffer,nullptr);
    }

    void VulkanGraphicInterface::WriteDescriptorSets(const WriteDescriptorSet *pWrites,uint32 writeCount) {
        std::vector<VkWriteDescriptorSet> writes(writeCount);
        std::vector<VkDescriptorImageInfo> imageInfos(writeCount);
        std::vector<VkDescriptorBufferInfo> bufferInfos(writeCount);
        std::vector<VkBufferView> bufferViews(writeCount);
        for(int i=0;i<writeCount;++i) {
            auto& write=writes[i];
            write.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.descriptorCount=pWrites[i].descriptorCount;
            write.descriptorType=(VkDescriptorType)pWrites[i].descriptorType;
            write.dstArrayElement=pWrites[i].dstArrayElement;
            write.dstBinding=pWrites[i].dstBinding;
            write.dstSet=((VulkanDescriptorSet*)pWrites[i].dstSet)->Get();
            if(pWrites[i].pImageInfo!=nullptr){
                write.pImageInfo=&imageInfos[i];
                imageInfos[i].imageView=((VulkanImageView*)pWrites[i].pImageInfo->imageView)->Get();
                imageInfos[i].imageLayout=(VkImageLayout)pWrites[i].pImageInfo->imageLayout;
                imageInfos[i].sampler=pWrites[i].pImageInfo->sampler?((VulkanSampler*)pWrites[i].pImageInfo->sampler)->Get():((VulkanSampler*)defaultLinearSampler)->Get();
            }
            if(pWrites[i].pBufferInfo!=nullptr){
                write.pBufferInfo=&bufferInfos[i];
                bufferInfos[i].buffer=((VulkanBuffer*)pWrites[i].pBufferInfo->buffer)->Get();
                bufferInfos[i].offset=pWrites[i].pBufferInfo->offset;
                bufferInfos[i].range=pWrites[i].pBufferInfo->range;
            }
            if(pWrites[i].pTexelBufferView!=nullptr){
                write.pTexelBufferView=&bufferViews[i];
                bufferViews[i]=((VulkanBufferView*)pWrites[i].pTexelBufferView)->Get();
            }
            write.pNext=nullptr;
        }
        vkUpdateDescriptorSets(device,writes.size(),writes.data(),0,nullptr);
    }

    void VulkanGraphicInterface::MapMemory(DeviceMemory *memory, uint64 offset, uint64 size, void *&data) {
        auto res=vkMapMemory(device,((VulkanDeviceMemory*)memory)->Get(),offset,size,0,&data);
        VK_CHECK(res,"failed to map memory !");
    }

    void VulkanGraphicInterface::UnMapMemory(DeviceMemory *memory) {
        vkUnmapMemory(device,((VulkanDeviceMemory*)memory)->Get());
    }

    void VulkanGraphicInterface::CopyImageToBuffer(Z::Image *image, Z::Buffer *buffer, Z::ImageLayout layout,
                                                   const Z::Rect2D &rect, ImageAspectFlag aspectFlag,
                                                   uint32 baseLayer , uint32 layerCount,
                                                   uint32 baseLevel) {
        VkBufferImageCopy copy{};
        copy.imageSubresource.layerCount=layerCount;
        copy.imageSubresource.baseArrayLayer=baseLayer;
        copy.imageSubresource.mipLevel=baseLevel;
        copy.imageSubresource.aspectMask = (VkImageAspectFlags)aspectFlag;
        copy.bufferImageHeight=rect.extent.height;
        copy.bufferRowLength=0;
        copy.imageExtent={rect.extent.width,rect.extent.height,1};
        copy.imageOffset={rect.offset.x,rect.offset.y,0};
        auto commandBuffer = BeginOnceSubmit();

        ImageMemoryBarrier barrier{};
        barrier.subresourceRange={ImageAspectFlag::COLOR,baseLevel,1,baseLayer,layerCount};
        barrier.srcAccessMask = AccessFlags::COLOR_ATTACHMENT_WRITE;
        barrier.dstAccessMask = AccessFlags::TRANSFER_READ;
        barrier.srcQueueFamilyIndex=familyIndices.graphics.value();
        barrier.dstQueueFamilyIndex=familyIndices.graphics.value();
        barrier.image=image;
        barrier.newLayout=ImageLayout::TRANSFER_SRC_OPTIMAL;
        barrier.oldLayout=ImageLayout::TRANSFER_SRC_OPTIMAL;

        PipelineBarrier(commandBuffer,PipelineStageFlags::ALL_COMMANDS,PipelineStageFlags::TRANSFER,
                        DependencyFlags::BY_REGION,0,nullptr,0, nullptr,
                        1,&barrier);

        vkCmdCopyImageToBuffer(commandBuffer,((VulkanImage*)image)->Get(),(VkImageLayout)layout,((VulkanBuffer*)buffer)->Get(),
                               1,&copy);
        EndOnceSubmit(commandBuffer);
    }

    void VulkanGraphicInterface::SetImageLayout(Image* image , ImageLayout oldLayout , ImageLayout newLayout ,
                                        PipelineStageFlags srcStage , PipelineStageFlags dstStage ,
                                        bool useInnerRenderCommandBuffer , CommandBuffer* buffer ,
                                        int baseLayer , int layerCount  , int baseMip  , int mipLevels , ImageAspectFlag aspectFlag ) {
        bool onceSubmit = buffer== nullptr && !useInnerRenderCommandBuffer ;
        VkCommandBuffer vkCommandBuffer;
        if(useInnerRenderCommandBuffer)
            vkCommandBuffer=commandBuffers[currentFrameIndex];
        else if(onceSubmit){
            vkCommandBuffer=BeginOnceSubmit();
        }else{
            vkCommandBuffer=((VulkanCommandBuffer*)buffer)->Get();
        }
        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask=(VkImageAspectFlags)aspectFlag;
        subresourceRange.baseMipLevel=baseMip;
        subresourceRange.levelCount=mipLevels;
        subresourceRange.layerCount=layerCount;
        subresourceRange.baseArrayLayer=baseLayer;
        VulkanUtils::setImageLayout(vkCommandBuffer,((VulkanImage*)image)->Get(),
                                    (VkImageLayout)oldLayout,(VkImageLayout)newLayout,subresourceRange,
                                    (VkPipelineStageFlags)srcStage,(VkPipelineStageFlags)dstStage);
        if(onceSubmit)
            EndOnceSubmit(vkCommandBuffer);
        else if(!useInnerRenderCommandBuffer)
            EndOnceSubmit(buffer);
    }

    void VulkanGraphicInterface::CreateDefaultSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType=VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.addressModeU=VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV=VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW=VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable=VK_TRUE;
        samplerInfo.borderColor=VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.compareEnable=VK_FALSE;
        samplerInfo.compareOp=VK_COMPARE_OP_ALWAYS;
        samplerInfo.magFilter=VK_FILTER_LINEAR;
        samplerInfo.minFilter=VK_FILTER_LINEAR;
        samplerInfo.maxAnisotropy=16;
        samplerInfo.maxLod=FLT_MAX;
        samplerInfo.minLod=0;
        samplerInfo.mipLodBias=0;
        samplerInfo.mipmapMode=VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.unnormalizedCoordinates=VK_FALSE;
        VkSampler linear;
        auto res=vkCreateSampler(device,&samplerInfo,nullptr,&linear);
        VK_CHECK(res,"failed to create default sampler !");
        defaultLinearSampler=new VulkanSampler{};
        ((VulkanSampler*)defaultLinearSampler)->Set(linear);
        samplerInfo.magFilter=VK_FILTER_NEAREST;
        samplerInfo.minFilter=VK_FILTER_NEAREST;
        samplerInfo.mipmapMode=VK_SAMPLER_MIPMAP_MODE_NEAREST;
        VkSampler nearest;
        res=vkCreateSampler(device,&samplerInfo,nullptr,&nearest);
        VK_CHECK(res,"failed to create default sampler !");
        defaultNearestSampler=new VulkanSampler{};
        ((VulkanSampler*)defaultNearestSampler)->Set(nearest);
    }

    void VulkanGraphicInterface::BindPipeline(PipelineBindPoint bindPoint, Pipeline *pipeline) {
        vkCmdBindPipeline(commandBuffers[currentFrameIndex],(VkPipelineBindPoint)bindPoint,((VulkanPipeline*)pipeline)->Get());
    }

    void VulkanGraphicInterface::BindPipeline(Z::CommandBuffer *buffer, Z::PipelineBindPoint bindPoint,
                                              Z::Pipeline *pipeline) {
        vkCmdBindPipeline(((VulkanCommandBuffer*)buffer)->Get(),(VkPipelineBindPoint)bindPoint,((VulkanPipeline*)pipeline)->Get());
    }

    void VulkanGraphicInterface::DrawIndexed(uint32 indexCount, uint32 instanceCount,
                                             uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance) {
        vkCmdDrawIndexed(commandBuffers[currentFrameIndex],indexCount,instanceCount,firstIndex,vertexOffset,firstInstance);
    }

    void VulkanGraphicInterface::DrawIndexed(CommandBuffer* commandBuffer,uint32 indexCount, uint32 instanceCount,
                                             uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance) {
        vkCmdDrawIndexed(((VulkanCommandBuffer*)commandBuffer)->Get(),indexCount,instanceCount,firstIndex,vertexOffset,firstInstance);
    }

    void
    VulkanGraphicInterface::BindDescriptorSets(PipelineBindPoint bindPoint, PipelineLayout *layout, uint32 firstSet,
                                               const std::vector<DescriptorSet*>&descriptorSets) {
        std::vector<VkDescriptorSet> sets(descriptorSets.size());
        for(int i=0;i<descriptorSets.size();++i) {
            sets[i]=((VulkanDescriptorSet*)(descriptorSets[i]))->Get();
        }
        vkCmdBindDescriptorSets(commandBuffers[currentFrameIndex],(VkPipelineBindPoint)bindPoint,((VulkanPipelineLayout*)layout)->Get(),firstSet,descriptorSets.size(),sets.data(),0,nullptr);

    }

    void VulkanGraphicInterface::BindDescriptorSets(Z::CommandBuffer *buffer, Z::PipelineBindPoint bindPoint,
                                                    Z::PipelineLayout *layout, Z::uint32 firstSet,
                                                    const std::vector<DescriptorSet *> &descriptorSets) {
        std::vector<VkDescriptorSet> sets(descriptorSets.size());
        for(int i=0;i<descriptorSets.size();++i) {
            sets[i]=((VulkanDescriptorSet*)(descriptorSets[i]))->Get();
        }
        vkCmdBindDescriptorSets(((VulkanCommandBuffer*)buffer)->Get(),(VkPipelineBindPoint)bindPoint,((VulkanPipelineLayout*)layout)->Get(),firstSet,descriptorSets.size(),sets.data(),0,nullptr);

    }

    void VulkanGraphicInterface::PushConstant(PipelineLayout *layout, ShaderStageFlag stageFlags, uint32 offset,
                                              uint32 size, const void *data) {
        vkCmdPushConstants(commandBuffers[currentFrameIndex],((VulkanPipelineLayout*)layout)->Get(),(VkShaderStageFlags)stageFlags,offset,size,data);
    }

    void VulkanGraphicInterface::PushConstant(CommandBuffer* buffer,PipelineLayout *layout, ShaderStageFlag stageFlags, uint32 offset,
                                              uint32 size, const void *data) {
        vkCmdPushConstants(((VulkanCommandBuffer*)buffer)->Get(),((VulkanPipelineLayout*)layout)->Get(),(VkShaderStageFlags)stageFlags,offset,size,data);
    }

    void VulkanGraphicInterface::SetViewPort(const Z::Viewport &viewport) {
        VkViewport vkViewport{};
        vkViewport=*((VkViewport*)&viewport);
        vkCmdSetViewport(commandBuffers[currentFrameIndex],0,1,&vkViewport);
    }

    void VulkanGraphicInterface::SetViewPort(Z::CommandBuffer *buffer, const Z::Viewport &viewport) {
        VkViewport vkViewport{};
        vkViewport=*((VkViewport*)&viewport);
        vkCmdSetViewport(((VulkanCommandBuffer*)buffer)->Get(),0,1,&vkViewport);
    }

    void VulkanGraphicInterface::SetScissor(const Z::Rect2D &scissor) {
        VkRect2D vkScissor{};
        vkScissor=*((VkRect2D*)&scissor);
        vkCmdSetScissor(commandBuffers[currentFrameIndex],0,1,&vkScissor);
    }

    void VulkanGraphicInterface::SetScissor(Z::CommandBuffer *buffer, const Z::Rect2D &scissor) {
        VkRect2D vkScissor{};
        vkScissor=*((VkRect2D*)&scissor);
        vkCmdSetScissor(((VulkanCommandBuffer*)buffer)->Get(),0,1,&vkScissor);
    }

    void VulkanGraphicInterface::BindVertexBuffer(Buffer ** buffers,uint32 firstBinding,uint32 bindingCount,uint32 offset){
        std::vector<VkBuffer> vkBuffers(bindingCount);
        std::vector<VkDeviceSize> offsets(bindingCount);
        for(int i=0;i<bindingCount;++i) {
            vkBuffers[i]=((VulkanBuffer*)buffers[i])->Get();
            offsets[i]=offset;
        }
        vkCmdBindVertexBuffers(commandBuffers[currentFrameIndex],firstBinding,bindingCount,vkBuffers.data(),offsets.data());
    }

    void VulkanGraphicInterface::BindVertexBuffer(CommandBuffer* commandBuffer,Buffer ** buffers,uint32 firstBinding,uint32 bindingCount,uint32 offset){
        std::vector<VkBuffer> vkBuffers(bindingCount);
        std::vector<VkDeviceSize> offsets(bindingCount);
        for(int i=0;i<bindingCount;++i) {
            vkBuffers[i]=((VulkanBuffer*)buffers[i])->Get();
            offsets[i]=offset;
        }
        vkCmdBindVertexBuffers(((VulkanCommandBuffer*)commandBuffer)->Get(),firstBinding,bindingCount,vkBuffers.data(),offsets.data());
    }

    void VulkanGraphicInterface::BindIndexBuffer(Buffer *buffer,uint32 offset,IndexType indexType){
        vkCmdBindIndexBuffer(commandBuffers[currentFrameIndex],((VulkanBuffer*)buffer)->Get(),offset,(VkIndexType)indexType);
    }

    void VulkanGraphicInterface::BindIndexBuffer(CommandBuffer* commandBuffer,Buffer *buffer,uint32 offset,IndexType indexType){
        vkCmdBindIndexBuffer(((VulkanCommandBuffer*)commandBuffer)->Get(),((VulkanBuffer*)buffer)->Get(),offset,(VkIndexType)indexType);
    }

    void
    VulkanGraphicInterface::Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) {
        vkCmdDraw(commandBuffers[currentFrameIndex],vertexCount,instanceCount,firstVertex,firstInstance);
    }

    void
    VulkanGraphicInterface::Draw(CommandBuffer* buffer,uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) {
        vkCmdDraw(((VulkanCommandBuffer*)buffer)->Get(),vertexCount,instanceCount,firstVertex,firstInstance);
    }

    Sampler *VulkanGraphicInterface::GetDefaultSampler(SamplerType samplerType) {
        if(samplerType==SamplerType::Linear)
            return defaultLinearSampler;
        else
            return defaultNearestSampler;
    }

    void
    VulkanGraphicInterface::BindDescriptorSet(PipelineBindPoint bindPoint, PipelineLayout *layout, DescriptorSet *set) {
        vkCmdBindDescriptorSets(commandBuffers[currentFrameIndex],(VkPipelineBindPoint)bindPoint,((VulkanPipelineLayout*)layout)->Get(),
                                0,1,((VulkanDescriptorSet*)set)->GetPtr(),0,nullptr);
    }

    void VulkanGraphicInterface::DestroySwapchain() {

        int width=0,height=0;
        auto windowPtr=(GLFWwindow*)Application::Get().GetWindow().GetNativeWindow();
        while(width==0||height==0){
            glfwPollEvents();
            glfwGetFramebufferSize(windowPtr,&width,&height);
        }
        vkDeviceWaitIdle(device);
        for(const auto&view:swapchainImageViews){
            vkDestroyImageView(device,view, nullptr);
        }
        swapchainImageViews.clear();
        swapchainImages.clear();
        vkDestroySwapchainKHR(device,swapchain, nullptr);
    }

    void VulkanGraphicInterface::FreeDescriptorSet(void *descriptorSet) {
        vkFreeDescriptorSets(device,descriptorPool,1,(VkDescriptorSet*)descriptorSet);
    }

    void VulkanGraphicInterface::CreatePipelineLayout(const PipelineLayoutCreateInfo &info, PipelineLayout *&layout) {

        std::vector<VkDescriptorSetLayout> setLayouts;
        setLayouts.resize(info.setLayoutCount);
        for(int i=0;i<info.setLayoutCount;++i){
            setLayouts[i]=((VulkanDescriptorSetLayout*)info.pSetLayouts[i])->Get();
        }

        VkPipelineLayoutCreateInfo createInfo{};
        createInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.pushConstantRangeCount=info.pushConstantRangeCount;
        createInfo.pPushConstantRanges=(VkPushConstantRange*)info.pPushConstantRanges;
        createInfo.pSetLayouts=setLayouts.data();
        createInfo.setLayoutCount=info.setLayoutCount;

        VkPipelineLayout pipelineLayout;
        auto res= vkCreatePipelineLayout(device,&createInfo, nullptr,&pipelineLayout);
        VK_CHECK(res,"failed to create pipeline layout !");
        layout=new VulkanPipelineLayout;
        ((VulkanPipelineLayout*)layout)->Set(pipelineLayout);
    }

    void VulkanGraphicInterface::CreateFence(const FenceCreateInfo &info, Fence *&fence) {
        VkFence vkFence{};
        VkFenceCreateInfo createInfo{};
        createInfo.sType=VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags=(VkFenceCreateFlags)info.flags;

        auto res = vkCreateFence(device,&createInfo, nullptr,&vkFence);
        VK_CHECK(res,"failed to create fence !");
        fence=new VulkanFence;
        ((VulkanFence*)fence)->Set(vkFence);
    }

    void VulkanGraphicInterface::zCreateSemaphore(const SemaphoreCreateInfo &info, Semaphore *&semaphore) {
        VkSemaphoreCreateInfo createInfo{};
        createInfo.sType=VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        VkSemaphore vkSemaphore;
        auto res = vkCreateSemaphore(device,&createInfo, nullptr,&vkSemaphore);
        VK_CHECK(res,"failed to create semaphore !");
        semaphore=new VulkanSemaphore;
        ((VulkanSemaphore*)semaphore)->Set(vkSemaphore);
    }

    void VulkanGraphicInterface::DestroyFence(Fence *fence) {
        vkDestroyFence(device,((VulkanFence*)fence)->Get(), nullptr);
        delete fence;
    }

    void VulkanGraphicInterface::DestroySemaphore(Semaphore *semaphore) {
        vkDestroySemaphore(device,((VulkanSemaphore*)semaphore)->Get(), nullptr);
        delete semaphore;
    }

    void VulkanGraphicInterface::WaitForFences(Fence *fence) {
        auto res = vkWaitForFences(device,1,((VulkanFence*)fence)->GetPtr(),VK_FALSE,UINT64_MAX);
        VK_CHECK(res,"failed to wait fences !");
    }

    void VulkanGraphicInterface::AllocateCommandBuffer(const CommandBufferAllocateInfo &info,
                                                       CommandBuffer *&commandBuffer) {
        VkCommandBufferAllocateInfo  allocateInfo{};
        allocateInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandBufferCount=1;
        allocateInfo.commandPool=transientCommandPool;
        allocateInfo.level=(VkCommandBufferLevel)info.level;
        VkCommandBuffer buffer;
        auto res = vkAllocateCommandBuffers(device,&allocateInfo,&buffer);
        VK_CHECK(res,"failed to allocate command buffer !");
        commandBuffer = new VulkanCommandBuffer;
        ((VulkanCommandBuffer*)commandBuffer)->Set(buffer);
    }

    void VulkanGraphicInterface::FreeCommandBuffer(Z::CommandBuffer *buffer) {
        vkFreeCommandBuffers(device,transientCommandPool,1,((VulkanCommandBuffer*)buffer)->GetPtr());
        delete buffer;
    }

    void VulkanGraphicInterface::BeginCommandBuffer(const Z::CommandBufferBeginInfo &info, Z::CommandBuffer *buffer) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags=(VkCommandBufferUsageFlagBits)info.flags;
        beginInfo.pInheritanceInfo= nullptr;
        vkBeginCommandBuffer(((VulkanCommandBuffer*)buffer)->Get(),&beginInfo);
    }

    void VulkanGraphicInterface::EndCommandBuffer(CommandBuffer *buffer) {
        vkEndCommandBuffer(((VulkanCommandBuffer*)buffer)->Get());
    }

    CommandBuffer *VulkanGraphicInterface::BeginOnceCommandSubmit() {
        auto buffer = BeginOnceSubmit();
        CommandBuffer* res = new VulkanCommandBuffer;
        ((VulkanCommandBuffer*)res)->Set(buffer);
        return res;
    }

    void VulkanGraphicInterface::EndOnceSubmit(CommandBuffer *buffer) {
        EndOnceSubmit(((VulkanCommandBuffer*)buffer)->Get());
        delete buffer;
    }

    void VulkanGraphicInterface::PipelineBarrier(VkCommandBuffer buffer, PipelineStageFlags srcStageMask,
                                                 PipelineStageFlags dstStageMask, DependencyFlags dependencyFlags,
                                                 uint32 memoryBarrierCount, const zMemoryBarrier *pMemoryBarriers,
                                                 uint32_t bufferMemoryBarrierCount,
                                                 const BufferMemoryBarrier *pBufferMemoryBarriers,
                                                 uint32_t imageMemoryBarrierCount, const ImageMemoryBarrier *pImageBarrier) {
        std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers(bufferMemoryBarrierCount);
        std::vector<VkImageMemoryBarrier> imageMemoryBarriers(imageMemoryBarrierCount);
        std::vector<VkMemoryBarrier> memoryBarriers(memoryBarrierCount);

        for(int i=0;i<bufferMemoryBarrierCount;++i){
            auto& barrier = bufferMemoryBarriers[i];
            auto& srcBarrier=pBufferMemoryBarriers[i];
            barrier.buffer=((VulkanBuffer*)srcBarrier.buffer)->Get();
            barrier.sType=VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            barrier.size=srcBarrier.size;
            barrier.offset=srcBarrier.offset;
            barrier.dstAccessMask=(VkAccessFlags)srcBarrier.dstAccessMask;
            barrier.srcAccessMask=(VkAccessFlags)srcBarrier.srcAccessMask;
            barrier.dstQueueFamilyIndex=srcBarrier.dstQueueFamilyIndex;
            barrier.srcQueueFamilyIndex=srcBarrier.srcQueueFamilyIndex;
            barrier.pNext= nullptr;
        }
        for(int i=0;i<imageMemoryBarrierCount;++i){
            auto& barrier = imageMemoryBarriers[i];
            auto& srcBarrier = pImageBarrier[i];
            barrier.sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.subresourceRange=*(VkImageSubresourceRange*)&srcBarrier.subresourceRange;
            barrier.srcAccessMask=(VkAccessFlags)srcBarrier.srcAccessMask;
            barrier.dstAccessMask=(VkAccessFlags)srcBarrier.dstAccessMask;
            barrier.srcQueueFamilyIndex=srcBarrier.srcQueueFamilyIndex;
            barrier.dstQueueFamilyIndex=srcBarrier.dstQueueFamilyIndex;
            barrier.image=((VulkanImage*)srcBarrier.image)->Get();
            barrier.newLayout=(VkImageLayout)srcBarrier.newLayout;
            barrier.oldLayout=(VkImageLayout)srcBarrier.oldLayout;
        }
        for(int i=0;i<memoryBarrierCount;++i){
            auto& barrier = memoryBarriers[i];
            auto& srcBarrier = pMemoryBarriers[i];
            barrier.sType=VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.srcAccessMask=(VkAccessFlags)srcBarrier.srcAccessMask;
            barrier.dstAccessMask=(VkAccessFlags)srcBarrier.dstAccessMask;
        }
        vkCmdPipelineBarrier(buffer,(VkPipelineStageFlags)srcStageMask,(VkPipelineStageFlags)dstStageMask,(VkDependencyFlags)dependencyFlags,
                             memoryBarrierCount,memoryBarriers.data(),
                             bufferMemoryBarrierCount,bufferMemoryBarriers.data(),
                             imageMemoryBarrierCount,imageMemoryBarriers.data());
    }

} // Z