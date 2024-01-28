//
// Created by z on 24-1-16.
//

#ifndef ENGINEALL_RENDERRESOURCE_H
#define ENGINEALL_RENDERRESOURCE_H

#include <optional>

#include "Z/Core/Core.h"

namespace Z {
    class Buffer {};
    class CommandBuffer {};
    class CommandPool {};
    class DescriptorPool {};
    class DescriptorSet {};
    class DescriptorSetLayout {};
    class Device {};
    class DeviceMemory {};
    class Framebuffer {};
    class Image {};
    class ImageView {};
    class GraphicContext {};
    class Queue {};
    class PhysicalDevice {};
    class ShaderModule {};


    struct ImageInfo;
    struct Extent2D;
    struct Extent3D;
    struct BufferInfo;
    struct SwapChainInfo;


    using ResourceSize=uint64;

    //same value as vulkan define
    enum class ImageLayout {
        UNDEFINED = 0,
        GENERAL = 1,
        COLOR_ATTACHMENT = 2,
        DEPTH_STENCIL_ATTACHMENT = 3,
        DEPTH_STENCIL_READ_ONLY = 4,
        SHADER_READ_ONLY = 5,
        TRANSFER_SRC = 6,
        TRANSFER_DST = 7,
        PREINITIALIZED = 8,
        DEPTH_READ_ONLY_STENCIL_ATTACHMENT = 1000117000,
        DEPTH_ATTACHMENT_STENCIL_READ_ONLY = 1000117001,
        DEPTH_ATTACHMENT = 1000241000,
        DEPTH_READ_ONLY = 1000241001,
        STENCIL_ATTACHMENT = 1000241002,
        STENCIL_READ_ONLY = 1000241003,
        READ_ONLY = 1000314000,
        ATTACHMENT = 1000314001,
        PRESENT_SRC = 1000001002,
        VIDEO_DECODE_DST = 1000024000,
        VIDEO_DECODE_SRC = 1000024001,
        VIDEO_DECODE_DPB = 1000024002,
        SHARED_PRESENT = 1000111000,
        FRAGMENT_DENSITY_MAP = 1000218000,
        FRAGMENT_SHADING_RATE_ATTACHMENT = 1000164003,
        VIDEO_ENCODE_DST = 1000299000,
        VIDEO_ENCODE_SRC = 1000299001,
        VIDEO_ENCODE_DPB = 1000299002,
        ATTACHMENT_FEEDBACK_LOOP = 1000339000,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class ShaderStage {
        VERTEX = 0x00000001,
        TESSELLATION_CONTROL = 0x00000002,
        TESSELLATION_EVALUATION = 0x00000004,
        GEOMETRY = 0x00000008,
        FRAGMENT = 0x00000010,
        COMPUTE = 0x00000020,
        ALL_GRAPHICS = 0x0000001F,
        ALL = 0x7FFFFFFF,
        RAYGEN = 0x00000100,
        ANY_HIT = 0x00000200,
        CLOSEST_HIT = 0x00000400,
        MISS = 0x00000800,
        INTERSECTION = 0x00001000,
        CALLABLE = 0x00002000,
        TASK = 0x00000040,
        MESH = 0x00000080,
        SUBPASS_SHADING = 0x00004000,
        CLUSTER_CULLING = 0x00080000,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class Format {
        UNDEFINED = 0,
        R8_UNORM = 9,
        R8_SNORM = 10,
        R8_USCALED = 11,
        R8_SSCALED = 12,
        R8_UINT = 13,
        R8_SINT = 14,
        R8_SRGB = 15,
        R8G8_UNORM = 16,
        R8G8_SNORM = 17,
        R8G8_USCALED = 18,
        R8G8_SSCALED = 19,
        R8G8_UINT = 20,
        R8G8_SINT = 21,
        R8G8_SRGB = 22,
        R8G8B8_UNORM = 23,
        R8G8B8_SNORM = 24,
        R8G8B8_USCALED = 25,
        R8G8B8_SSCALED = 26,
        R8G8B8_UINT = 27,
        R8G8B8_SINT = 28,
        R8G8B8_SRGB = 29,
        B8G8R8_UNORM = 30,
        B8G8R8_SNORM = 31,
        B8G8R8_USCALED = 32,
        B8G8R8_SSCALED = 33,
        B8G8R8_UINT = 34,
        B8G8R8_SINT = 35,
        B8G8R8_SRGB = 36,
        R8G8B8A8_UNORM = 37,
        R8G8B8A8_SNORM = 38,
        R8G8B8A8_USCALED = 39,
        R8G8B8A8_SSCALED = 40,
        R8G8B8A8_UINT = 41,
        R8G8B8A8_SINT = 42,
        R8G8B8A8_SRGB = 43,
        B8G8R8A8_UNORM = 44,
        B8G8R8A8_SNORM = 45,
        B8G8R8A8_USCALED = 46,
        B8G8R8A8_SSCALED = 47,
        B8G8R8A8_UINT = 48,
        B8G8R8A8_SINT = 49,
        B8G8R8A8_SRGB = 50,
        R16_UNORM = 70,
        R16_SNORM = 71,
        R16_USCALED = 72,
        R16_SSCALED = 73,
        R16_UINT = 74,
        R16_SINT = 75,
        R16_SFLOAT = 76,
        R16G16_UNORM = 77,
        R16G16_SNORM = 78,
        R16G16_USCALED = 79,
        R16G16_SSCALED = 80,
        R16G16_UINT = 81,
        R16G16_SINT = 82,
        R16G16_SFLOAT = 83,
        R16G16B16_UNORM = 84,
        R16G16B16_SNORM = 85,
        R16G16B16_USCALED = 86,
        R16G16B16_SSCALED = 87,
        R16G16B16_UINT = 88,
        R16G16B16_SINT = 89,
        R16G16B16_SFLOAT = 90,
        R16G16B16A16_UNORM = 91,
        R16G16B16A16_SNORM = 92,
        R16G16B16A16_USCALED = 93,
        R16G16B16A16_SSCALED = 94,
        R16G16B16A16_UINT = 95,
        R16G16B16A16_SINT = 96,
        R16G16B16A16_SFLOAT = 97,
        R32_UINT = 98,
        R32_SINT = 99,
        R32_SFLOAT = 100,
        R32G32_UINT = 101,
        R32G32_SINT = 102,
        R32G32_SFLOAT = 103,
        R32G32B32_UINT = 104,
        R32G32B32_SINT = 105,
        R32G32B32_SFLOAT = 106,
        R32G32B32A32_UINT = 107,
        R32G32B32A32_SINT = 108,
        R32G32B32A32_SFLOAT = 109,
        R64_UINT = 110,
        R64_SINT = 111,
        R64_SFLOAT = 112,
        R64G64_UINT = 113,
        R64G64_SINT = 114,
        R64G64_SFLOAT = 115,
        R64G64B64_UINT = 116,
        R64G64B64_SINT = 117,
        R64G64B64_SFLOAT = 118,
        R64G64B64A64_UINT = 119,
        R64G64B64A64_SINT = 120,
        R64G64B64A64_SFLOAT = 121,
        D16_UNORM = 124,
        D32_SFLOAT = 126,
        S8_UINT = 127,
        D16_UNORM_S8_UINT = 128,
        D24_UNORM_S8_UINT = 129,
        D32_SFLOAT_S8_UINT = 130,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class ImageType {
        IMAGE_1D = 0,
        IMAGE_2D = 1,
        IMAGE_3D = 2,
        IMAGE_MAX_ENUM = 0x7FFFFFFF
    };
    enum class ImageTiling {
        OPTIMAL = 0,
        LINEAR = 1,
        DRM_FORMAT_MODIFIER_EXT = 1000158000,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class ImageUsageFlag {
        TRANSFER_SRC = 0x00000001,
        TRANSFER_DST = 0x00000002,
        SAMPLED = 0x00000004,
        STORAGE = 0x00000008,
        COLOR_ATTACHMENT = 0x00000010,
        DEPTH_STENCIL_ATTACHMENT = 0x00000020,
        TRANSIENT_ATTACHMENT = 0x00000040,
        INPUT_ATTACHMENT = 0x00000080,
        FRAGMENT_DENSITY_MAP = 0x00000200,
        FRAGMENT_SHADING_RATE_ATTACHMENT = 0x00000100,
        HOST_TRANSFER = 0x00400000,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class MemoryPropertyFlag {
        DEVICE_LOCAL = 0x00000001,
        HOST_VISIBLE = 0x00000002,
        HOST_COHERENT = 0x00000004,
        HOST_CACHED = 0x00000008,
        LAZILY_ALLOCATED = 0x00000010,
        PROTECTED = 0x00000020,
        DEVICE_COHERENT_AMD = 0x00000040,
        DEVICE_UNCACHED_AMD = 0x00000080,
        RDMA_CAPABLE_NV = 0x00000100,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class ImageCreateFlag {
        eSPARSE_BINDING = 0x00000001,
        eSPARSE_RESIDENCY = 0x00000002,
        eSPARSE_ALIASED = 0x00000004,
        eMUTABLE_FORMAT = 0x00000008,
        eCUBE_COMPATIBLE = 0x00000010,
        eALIAS = 0x00000400,
        eSPLIT_INSTANCE_BIND_REGIONS = 0x00000040,
        e2D_ARRAY_COMPATIBLE = 0x00000020,
        eBLOCK_TEXEL_VIEW_COMPATIBLE = 0x00000080,
        eEXTENDED_USAGE = 0x00000100,
        ePROTECTED = 0x00000800,
        eDISJOINT = 0x00000200,
        eCORNER_SAMPLED_NV = 0x00002000,
        eSAMPLE_LOCATIONS_COMPATIBLE_DEPTH = 0x00001000,
        eSUBSAMPLED = 0x00004000,
        eDESCRIPTOR_BUFFER_CAPTURE_REPLAY = 0x00010000,
        eMULTISAMPLED_RENDER_TO_SINGLE_SAMPLED = 0x00040000,
        e2D_VIEW_COMPATIBLE = 0x00020000,
        eMAX_ENUM = 0x7FFFFFFF
    } ;
    enum class BufferUsageFlag {
        TRANSFER_SRC = 0x00000001,
        TRANSFER_DST = 0x00000002,
        UNIFORM_TEXEL_BUFFER = 0x00000004,
        STORAGE_TEXEL_BUFFER = 0x00000008,
        UNIFORM_BUFFER = 0x00000010,
        STORAGE_BUFFER = 0x00000020,
        INDEX_BUFFER = 0x00000040,
        VERTEX_BUFFER = 0x00000080,
        INDIRECT_BUFFER = 0x00000100,
        SHADER_DEVICE_ADDRESS = 0x00020000,
        TRANSFORM_FEEDBACK_BUFFER = 0x00000800,
        TRANSFORM_FEEDBACK_COUNTER_BUFFER = 0x00001000,
        CONDITIONAL_RENDERING = 0x00000200,
        ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY = 0x00080000,
        ACCELERATION_STRUCTURE_STORAGE = 0x00100000,
        SHADER_BINDING_TABLE = 0x00000400,
        SAMPLER_DESCRIPTOR_BUFFER = 0x00200000,
        RESOURCE_DESCRIPTOR_BUFFER = 0x00400000,
        PUSH_DESCRIPTORS_DESCRIPTOR_BUFFER = 0x04000000,
        MICROMAP_BUILD_INPUT_READ_ONLY = 0x00800000,
        MICROMAP_STORAGE = 0x01000000,
        MAX_ENUM = 0x7FFFFFFF
    } ;

    struct Extent2D{
        uint32 width,height;
    };
    struct Extent3D{
        uint32 width,height,depth;
    };
    struct ImageInfo {
        uint32 arrayLayers,mipMapLevels;
        Extent3D extent;
        ImageLayout initialLayout;
        Format format;
        ImageTiling tilling;
        ImageUsageFlag usageFlag;
        MemoryPropertyFlag memoryPropertyFlag;
        ImageCreateFlag createFlag;
    };
    struct BufferInfo{
        ResourceSize          size;
        BufferUsageFlag    usage;
        MemoryPropertyFlag properties;
    };

    struct SwapChainInfo {
        Format swapchainImageFormat;
        uint32 imageCount;
        uint32 minImageCount;
        uint32 maxImageCount;
    };

}

#endif //ENGINEALL_RENDERRESOURCE_H
