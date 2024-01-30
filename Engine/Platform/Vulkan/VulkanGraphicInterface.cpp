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
                Z_CORE_INFO("Using GPU : {0}",properties.deviceName);
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

    void VulkanGraphicInterface::ReCreateSwapChain() {
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

    bool VulkanGraphicInterface::prepareBeforeRender(const std::function<void()> &funcCallAfterRecreateSwapChain) {
        auto acquireRes=vkAcquireNextImageKHR(device,swapchain,UINT64_MAX,imageAvailable[currentFrameIndex],
            VK_NULL_HANDLE,&currentSwapChainImageIndex);

        if(VK_SUBOPTIMAL_KHR==acquireRes||VK_ERROR_OUT_OF_DATE_KHR==acquireRes) {
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

        VkSemaphore semaphores[2]={imageAvailable[currentFrameIndex],imageRenderFinish[currentFrameIndex]};

        VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo         submit_info   = {};
        submit_info.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount     = 1;
        submit_info.pWaitSemaphores        = &imageAvailable[currentFrameIndex];
        submit_info.pWaitDstStageMask      = waitStages;
        submit_info.commandBufferCount     = 1;
        submit_info.pCommandBuffers        = &commandBuffers[currentFrameIndex];
        submit_info.signalSemaphoreCount = 2;
        submit_info.pSignalSemaphores = semaphores;

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

    void VulkanGraphicInterface::CreateRenderPass(const RenderPassCreateInfo &info, RenderPassInterface *&interface) {
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
            dependency.dstSubpass=dep.dstSubpass;
            dependency.srcSubpass=dep.srcSubpass;
            dependency.dstAccessMask=(VkAccessFlags)dep.dstAccessMask;
            dependency.srcAccessMask=(VkAccessFlags)dep.srcAccessMask;
            dependency.dstStageMask=(VkPipelineStageFlags)dep.dstAccessMask;
            dependency.srcAccessMask=(VkPipelineStageFlags)dep.srcAccessMask;
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

        interface=new VulkanRenderPass{};
        VkRenderPass renderpass{};
        auto res=vkCreateRenderPass(device,&Info,nullptr,&renderpass);
        VK_CHECK(res,"failed to create RenderPass !");
        ((VulkanRenderPass*)interface)->Set(renderpass);
    }
} // Z