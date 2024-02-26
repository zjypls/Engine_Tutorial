//
// Created by z on 24-1-18.
//

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
        vkQueueWaitIdle(queue);
        vkFreeCommandBuffers(device,transientCommandPool,1,&buffer);
    }

    void VulkanGraphicInterface::CreateInstance(){
        if(!VulkanUtils::checkLayerEnable(validationLayers).empty() ||
            !VulkanUtils::checkExtensionEnable({VK_EXT_DEBUG_UTILS_EXTENSION_NAME}).empty()){
            Z_CORE_WARN("Validation layer or debug utils extension is not available !");
            enableDebugUtils= false;
            enableValidationLayer= false;
        }

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

    void VulkanGraphicInterface::ReCreateSwapChain() {
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
    }

    void VulkanGraphicInterface::Shutdown() {
        //todo:destroy vulkan context

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

    void VulkanGraphicInterface::SubmitTask() {
        auto res=vkEndCommandBuffer(commandBuffers[currentFrameIndex]);
        VK_CHECK(res,"failed to end command buffer !");

        VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
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
        }
        else if (VK_SUCCESS != res)
        {
            Z_CORE_ERROR("failed to present image !");
            return;
        }

        currentFrameIndex = (currentFrameIndex + 1) % maxFlightFrames;
    }

    void VulkanGraphicInterface::WaitForFences() {
        auto res=vkWaitForFences(device,1,&frameFences[currentFrameIndex],VK_FALSE,UINT64_MAX);
        VK_CHECK(res,"failed to wait fences !");
    }

    void VulkanGraphicInterface::DeviceWaiteIdle() {
        auto res= vkDeviceWaitIdle(device);
        VK_CHECK(res,"Unknown error happened when wait idle !");
    }

    void VulkanGraphicInterface::ResetCommandPool() {
        auto res=vkResetFences(device,1,&frameFences[currentFrameIndex]);
        VK_CHECK(res,"failed to reset fence !");
        res=vkResetCommandPool(device,commandPools[currentFrameIndex],0);
        VK_CHECK(res,"failed to reset command pool !");
    }

    void VulkanGraphicInterface::BeginRenderPass(const RenderPassBeginInfo &info) {
        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType=VK_INFO(RENDER_PASS,BEGIN);
        beginInfo.framebuffer=((VulkanFramebuffer*)info.framebuffer)->Get();
        beginInfo.renderArea=*(VkRect2D*)&info.renderArea;
        //beginInfo.renderArea.extent=swapchainExtent;
        beginInfo.renderPass=((VulkanRenderPass*)info.renderPass)->Get();
        beginInfo.clearValueCount=info.clearValueCount;
        beginInfo.pClearValues=(VkClearValue*)info.pClearValues;

        vkCmdBeginRenderPass(commandBuffers[currentFrameIndex],&beginInfo,VK_SUBPASS_CONTENTS_INLINE);

    }

    void VulkanGraphicInterface::EndRenderPass() {
        vkCmdEndRenderPass(commandBuffers[currentFrameIndex]);
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
        if(info.pSubpasses!=nullptr)
        for(auto&subpass:subpasses) {
            const auto& des=info.pSubpasses[index];
            auto&attachmentRef=attachmentsReferences[index];
            auto&inputRef=inputReferences[index];
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
            subpass.colorAttachmentCount=des.colorAttachmentCount;
            subpass.inputAttachmentCount=des.inputAttachmentCount;
            subpass.pipelineBindPoint=(VkPipelineBindPoint)des.pipelineBindPoint;
            subpass.pColorAttachments=attachmentRef.data();
            subpass.pInputAttachments=inputRef.data();
            ++index;
        }
        Info.pAttachments=descriptions.data();
        Info.attachmentCount=descriptions.size();
        Info.dependencyCount=dependencies.size();
        Info.pDependencies=dependencies.data();
        Info.pSubpasses=subpasses.data();
        Info.subpassCount=subpasses.size();

        renderPassInterface=new VulkanRenderPass{};
        VkRenderPass renderpass{};
        auto res=vkCreateRenderPass(device,&Info,nullptr,&renderpass);
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

    void VulkanGraphicInterface::CreateGraphicPipeline(const std::string &shaderSources,
        const std::vector<Z::ShaderStageFlag> &stageFlags, Pipeline *&graphicPipeline,
        RenderPassInterface *renderPassInterface,DescriptorSetLayout*& descriptorSetLayout,PipelineLayout*& pipelineLayout) {
            auto shaderInfo=VulkanUtils::ReflectShader(shaderSources,stageFlags);
            auto descriptorLayout=VulkanUtils::CreateDescriptorSetLayout(device,shaderInfo.descriptorInfos);
            descriptorSetLayout=new VulkanDescriptorSetLayout{};
            ((VulkanDescriptorSetLayout*)descriptorSetLayout)->Set(descriptorLayout);
            VkPipelineLayoutCreateInfo layoutInfo{};
            layoutInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            layoutInfo.setLayoutCount=1;
            layoutInfo.pSetLayouts=&descriptorLayout;
            VkPipelineLayout layout;
            auto res=vkCreatePipelineLayout(device,&layoutInfo,nullptr,&layout);
            VK_CHECK(res,"failed to create pipeline layout !");
            pipelineLayout=new VulkanPipelineLayout{};
            ((VulkanPipelineLayout*)pipelineLayout)->Set(layout);

            std::vector<VkPipelineShaderStageCreateInfo> stages(shaderInfo.irCode.size());
            std::vector<VkShaderModule> modules(shaderInfo.irCode.size());
            for(int i=0;i<stages.size();++i) {
                VkShaderModuleCreateInfo moduleInfo{};
                moduleInfo.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                moduleInfo.pCode=shaderInfo.irCode[i].data();
                moduleInfo.codeSize=shaderInfo.irCode[i].size()*4;
                VkShaderModule module;
                res=vkCreateShaderModule(device,&moduleInfo,nullptr,&module);
                VK_CHECK(res,"failed to create shader module !");
                modules[i]=module;
                stages[i].sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                stages[i].stage=(VkShaderStageFlagBits)stageFlags[i];
                stages[i].module=module;
                stages[i].pName="main";
            }
            auto vertexDescription=MeshDescription::GetAttributeDescription();
            auto vertexBinding=MeshDescription::GetBindingDescription();
            VkPipelineVertexInputStateCreateInfo vertexInputStateInfo{};
            vertexInputStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputStateInfo.vertexAttributeDescriptionCount=vertexDescription.size();
            vertexInputStateInfo.pVertexAttributeDescriptions=(VkVertexInputAttributeDescription*)vertexDescription.data();
            vertexInputStateInfo.vertexBindingDescriptionCount=vertexBinding.size();
            vertexInputStateInfo.pVertexBindingDescriptions=(VkVertexInputBindingDescription*)vertexBinding.data();
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo{};
            inputAssemblyStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyStateInfo.topology=VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssemblyStateInfo.primitiveRestartEnable=VK_FALSE;
            VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
            VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
            dynamicStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicStateInfo.dynamicStateCount=sizeof(dynamicStates) / sizeof (dynamicStates[0]);
            dynamicStateInfo.pDynamicStates=dynamicStates;
            VkPipelineViewportStateCreateInfo viewportStateInfo{};
            viewportStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportStateInfo.viewportCount=1;
            viewportStateInfo.scissorCount=1;
            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask=VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable=VK_FALSE;
            VkPipelineColorBlendStateCreateInfo blendStateInfo{};
            blendStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            blendStateInfo.logicOpEnable=VK_FALSE;
            blendStateInfo.logicOp=VK_LOGIC_OP_COPY;
            blendStateInfo.attachmentCount=1;
            blendStateInfo.pAttachments=&colorBlendAttachment;
            VkPipelineMultisampleStateCreateInfo multisampleStateInfo{};
            multisampleStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampleStateInfo.sampleShadingEnable=VK_FALSE;
            multisampleStateInfo.rasterizationSamples=defaultSampleFlag;
            VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{};
            rasterizationStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizationStateInfo.depthClampEnable=VK_FALSE;
            rasterizationStateInfo.rasterizerDiscardEnable=VK_FALSE;
            rasterizationStateInfo.polygonMode=VK_POLYGON_MODE_FILL;
            rasterizationStateInfo.lineWidth=1.0f;
            rasterizationStateInfo.cullMode=VK_CULL_MODE_BACK_BIT;
            rasterizationStateInfo.frontFace=VK_FRONT_FACE_CLOCKWISE;
            rasterizationStateInfo.depthBiasEnable=VK_FALSE;
            rasterizationStateInfo.depthBiasConstantFactor=0.0f;
            rasterizationStateInfo.depthBiasClamp=0.0f;
            rasterizationStateInfo.depthBiasSlopeFactor=0.0f;
            VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo{};
            depthStencilStateInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencilStateInfo.depthTestEnable=VK_TRUE;
            depthStencilStateInfo.depthWriteEnable=VK_TRUE;
            depthStencilStateInfo.depthCompareOp=VK_COMPARE_OP_LESS;
            depthStencilStateInfo.depthBoundsTestEnable=VK_FALSE;
            depthStencilStateInfo.stencilTestEnable=VK_FALSE;

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
            pipelineInfo.layout=layout;
            pipelineInfo.renderPass=((VulkanRenderPass*)renderPassInterface)->Get();
            pipelineInfo.subpass=0;
            pipelineInfo.basePipelineHandle=VK_NULL_HANDLE;
            pipelineInfo.basePipelineIndex=-1;
            VkPipeline pipeline;
            res=vkCreateGraphicsPipelines(device,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&pipeline);
            VK_CHECK(res,"failed to create graphic pipeline !");
            graphicPipeline=new VulkanPipeline{};
            ((VulkanPipeline*)graphicPipeline)->Set(pipeline);
            for(auto module:modules) {
                vkDestroyShaderModule(device,module,nullptr);
            }
    }

    void VulkanGraphicInterface::DestroyRenderPass(RenderPassInterface *renderPassInterface) {
        vkDestroyRenderPass(device,((VulkanRenderPass*)renderPassInterface)->Get(),nullptr);
    }

    std::vector<Z::Framebuffer *> VulkanGraphicInterface::CreateDefaultFrameBuffers(
        RenderPassInterface *renderPassInterface) {
        std::vector<Z::Framebuffer*> result(swapchainImageViews.size());
        swapchainFrameBuffers.resize(swapchainImageViews.size());
        VkFramebufferCreateInfo createInfo{};
        createInfo.sType=VK_INFO(FRAMEBUFFER,CREATE);
        createInfo.height=swapchainExtent.height;
        createInfo.width=swapchainExtent.width;
        createInfo.attachmentCount=1;
        createInfo.layers=1;
        createInfo.renderPass=((VulkanRenderPass*)renderPassInterface)->Get();
        createInfo.flags=0;
        createInfo.pNext=nullptr;
        for(int i=0;i<swapchainImageViews.size();++i) {
            VkFramebuffer buffer{};
            createInfo.pAttachments=&swapchainImageViews[i];
            auto res=vkCreateFramebuffer(device,&createInfo,nullptr,&buffer);
            VK_CHECK(res,"failed to create frame buffers !");
            auto framebuffer=new VulkanFramebuffer{};
            framebuffer->Set(buffer);
            result[i]=framebuffer;
            swapchainFrameBuffers[i]=buffer;
        }
        return result;
    }

    void VulkanGraphicInterface::DestroyFrameBuffer(Z::Framebuffer *framebuffer) {
        vkDestroyFramebuffer(device,((VulkanFramebuffer*)framebuffer)->Get(),nullptr);
    }

    void VulkanGraphicInterface::DestroyPipeline(Pipeline *pipeline) {
        vkDestroyPipeline(device,((VulkanPipeline*)pipeline)->Get(),nullptr);
    }

    void VulkanGraphicInterface::DestroyPipelineLayout(PipelineLayout *pipelineLayout) {
        vkDestroyPipelineLayout(device,((VulkanPipelineLayout*)pipelineLayout)->Get(),nullptr);
    }

    void VulkanGraphicInterface::DestroyImage(Image * image , DeviceMemory* memory , ImageView* view) {
        if(view!=nullptr)
            vkDestroyImageView(device,((VulkanImageView*)view)->Get(),nullptr);
        vkFreeMemory(device,((VulkanDeviceMemory*)memory)->Get(),nullptr);
        vkDestroyImage(device,((VulkanImage*)image)->Get(),nullptr);
    }

    void VulkanGraphicInterface::DestroyDescriptorSetLayout(DescriptorSetLayout *descriptorSetLayout) {
        vkDestroyDescriptorSetLayout(device,((VulkanDescriptorSetLayout*)descriptorSetLayout)->Get(),nullptr);
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

} // Z