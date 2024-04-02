//
// Created by z on 24-1-16.
//

#ifndef ENGINEALL_RENDERINTERFACE_H
#define ENGINEALL_RENDERINTERFACE_H

#include <optional>

#include "Z/Core/Core.h"

namespace Z {
    class Buffer {};
    class CommandBuffer {};
    class CommandPool {};
    class DescriptorPool {};
    class DescriptorSet {};
    class DescriptorSetLayout {};
    class DeviceMemory {};
    class Framebuffer {};
    class Image {};
    class ImageView {};
    class BufferView {};
    class Queue {};
    class ShaderModule {};
    class PipelineLayout{};
    class Pipeline{};
    class RenderPassInterface{};
    class Sampler{};


    struct ImageInfo;
    struct Extent2D;
    struct Offset2D;
    struct Rect2D;
    struct Extent3D;
    struct Viewport;
    union  ClearColorValue;
    struct ClearDepthStencilValue;
    union  ClearValue;
    struct BufferInfo;
    struct SwapChainInfo;
    struct RenderPassCreateInfo;
    struct GraphicPipelineCreateInfo;
    struct RenderPassBeginInfo;
    struct PipelineShaderStageCreateInfo;
    struct StencilOpState;


    using ResourceSize=uint64;


    //same value as vulkan define
    enum class ImageLayout {
        UNDEFINED = 0,
        GENERAL = 1,
        COLOR_ATTACHMENT_OPTIMAL = 2,
        DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
        DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
        SHADER_READ_ONLY_OPTIMAL = 5,
        TRANSFER_SRC_OPTIMAL = 6,
        TRANSFER_DST_OPTIMAL = 7,
        PREINITIALIZED = 8,
        DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL = 1000117000,
        DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL = 1000117001,
        DEPTH_ATTACHMENT_OPTIMAL = 1000241000,
        DEPTH_READ_ONLY_OPTIMAL = 1000241001,
        STENCIL_ATTACHMENT_OPTIMAL = 1000241002,
        STENCIL_READ_ONLY_OPTIMAL = 1000241003,
        READ_ONLY_OPTIMAL = 1000314000,
        ATTACHMENT_OPTIMAL = 1000314001,
        PRESENT_SRC = 1000001002,
        VIDEO_DECODE_DST = 1000024000,
        VIDEO_DECODE_SRC = 1000024001,
        VIDEO_DECODE_DPB = 1000024002,
        SHARED_PRESENT = 1000111000,
        FRAGMENT_DENSITY_MAP_OPTIMAL = 1000218000,
        FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL = 1000164003,
        ATTACHMENT_FEEDBACK_LOOP_OPTIMAL = 1000339000,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class ShaderStageFlag {
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
    enum class PipelineShaderStageCreateFlag {
        ALLOW_VARYING_SUBGROUP_SIZE_BIT = 0x00000001,
        REQUIRE_FULL_SUBGROUPS_BIT = 0x00000002,
        MAX_ENUM = 0x7FFFFFFF
    } ;
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
    enum class SampleCountFlagBits {
        e1_BIT = 0x00000001,
        e2_BIT = 0x00000002,
        e4_BIT = 0x00000004,
        e8_BIT = 0x00000008,
        e16_BIT = 0x00000010,
        e32_BIT = 0x00000020,
        e64_BIT = 0x00000040,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class AttachmentLoadOp {
        LOAD = 0,
        CLEAR = 1,
        DONT_CARE = 2,
        NONE_EXT = 1000400000,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class AttachmentStoreOp {
        STORE = 0,
        DONT_CARE = 1,
        NONE = 1000301000,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class PipelineBindPoint {
        GRAPHICS = 0,
        COMPUTE = 1,
        RAY_TRACING = 1000165000,
        SUBPASS_SHADING_HUAWEI = 1000369003,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class PipelineStageFlags {
        TOP_OF_PIPE = 0x00000001,
        DRAW_INDIRECT = 0x00000002,
        VERTEX_INPUT = 0x00000004,
        VERTEX_SHADER = 0x00000008,
        TESSELLATION_CONTROL_SHADER = 0x00000010,
        TESSELLATION_EVALUATION_SHADER = 0x00000020,
        GEOMETRY_SHADER = 0x00000040,
        FRAGMENT_SHADER = 0x00000080,
        EARLY_FRAGMENT_TESTS = 0x00000100,
        LATE_FRAGMENT_TESTS = 0x00000200,
        COLOR_ATTACHMENT_OUTPUT = 0x00000400,
        COMPUTE_SHADER = 0x00000800,
        TRANSFER = 0x00001000,
        BOTTOM_OF_PIPE = 0x00002000,
        HOST = 0x00004000,
        ALL_GRAPHICS = 0x00008000,
        ALL_COMMANDS = 0x00010000,
        NONE = 0,
        TRANSFORM_FEEDBACK = 0x01000000,
        CONDITIONAL_RENDERING = 0x00040000,
        ACCELERATION_STRUCTURE_BUILD = 0x02000000,
        RAY_TRACING_SHADER_KHR = 0x00200000,
        FRAGMENT_DENSITY_PROCESS = 0x00800000,
        FRAGMENT_SHADING_RATE_ATTACHMENT = 0x00400000,
        COMMAND_PREPROCESS = 0x00020000,
        TASK_SHADER = 0x00080000,
        MESH_SHADER = 0x00100000,
        FLAGS_MAX_ENUM = 0x7FFFFFFF
    };
    enum class AccessFlags {
        INDIRECT_COMMAND_READ = 0x00000001,
        INDEX_READ = 0x00000002,
        VERTEX_ATTRIBUTE_READ = 0x00000004,
        UNIFORM_READ = 0x00000008,
        INPUT_ATTACHMENT_READ = 0x00000010,
        SHADER_READ = 0x00000020,
        SHADER_WRITE = 0x00000040,
        COLOR_ATTACHMENT_READ = 0x00000080,
        COLOR_ATTACHMENT_WRITE = 0x00000100,
        DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200,
        DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400,
        TRANSFER_READ = 0x00000800,
        TRANSFER_WRITE = 0x00001000,
        HOST_READ = 0x00002000,
        HOST_WRITE = 0x00004000,
        MEMORY_READ = 0x00008000,
        MEMORY_WRITE = 0x00010000,
        NONE = 0,
        TRANSFORM_FEEDBACK_WRITE = 0x02000000,
        TRANSFORM_FEEDBACK_COUNTER_READ = 0x04000000,
        TRANSFORM_FEEDBACK_COUNTER_WRITE = 0x08000000,
        CONDITIONAL_RENDERING_READ = 0x00100000,
        COLOR_ATTACHMENT_READ_NONCOHERENT = 0x00080000,
        ACCELERATION_STRUCTURE_READ = 0x00200000,
        ACCELERATION_STRUCTURE_WRITE = 0x00400000,
        FRAGMENT_DENSITY_MAP_READ = 0x01000000,
        FRAGMENT_SHADING_RATE_ATTACHMENT_READ = 0x00800000,
        COMMAND_PREPROCESS_READ = 0x00020000,
        COMMAND_PREPROCESS_WRITE = 0x00040000,
        FLAGS_MAX_ENUM = 0x7FFFFFFF
    };
    enum class SubpassContents : uint32{
        INLINE = 0,
        SECONDARY_COMMAND_BUFFERS = 1,
        INLINE_AND_SECONDARY_COMMAND_BUFFERS_EXT = 1000451000,
        MAX_ENUM = 0x7FFFFFFF,
        EXTERNAL = ~0U
    };
    enum class DependencyFlags {
        BY_REGION = 0x00000001,
        DEVICE_GROUP = 0x00000004,
        VIEW_LOCAL = 0x00000002,
        FEEDBACK_LOOP = 0x00000008,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class VertexInputRate {
        VERTEX = 0,
        INSTANCE = 1,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class PrimitiveTopology {
        POINT_LIST = 0,
        LINE_LIST = 1,
        LINE_STRIP = 2,
        TRIANGLE_LIST = 3,
        TRIANGLE_STRIP = 4,
        TRIANGLE_FAN = 5,
        LINE_LIST_WITH_ADJACENCY = 6,
        LINE_STRIP_WITH_ADJACENCY = 7,
        TRIANGLE_LIST_WITH_ADJACENCY = 8,
        TRIANGLE_STRIP_WITH_ADJACENCY = 9,
        PATCH_LIST = 10,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class PolygonMode {
        FILL = 0,
        LINE = 1,
        POINT = 2,
        FILL_RECTANGLE_NV = 1000153000,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class CullModeFlag {
        NONE = 0,
        FRONT = 0x00000001,
        BACK = 0x00000002,
        FRONT_AND_BACK = 0x00000003,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class FrontFace {
        COUNTER_CLOCKWISE = 0,
        CLOCKWISE = 1,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class CompareOp {
        NEVER = 0,
        LESS = 1,
        EQUAL = 2,
        LESS_OR_EQUAL = 3,
        GREATER = 4,
        NOT_EQUAL = 5,
        GREATER_OR_EQUAL = 6,
        ALWAYS = 7,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class StencilOp {
        KEEP = 0,
        ZERO = 1,
        REPLACE = 2,
        INCREMENT_AND_CLAMP = 3,
        DECREMENT_AND_CLAMP = 4,
        INVERT = 5,
        INCREMENT_AND_WRAP = 6,
        DECREMENT_AND_WRAP = 7,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class LogicOp {
        CLEAR = 0,
        AND = 1,
        AND_REVERSE = 2,
        COPY = 3,
        AND_INVERTED = 4,
        NO_OP = 5,
        XOR = 6,
        OR = 7,
        NOR = 8,
        EQUIVALENT = 9,
        INVERT = 10,
        OR_REVERSE = 11,
        COPY_INVERTED = 12,
        OR_INVERTED = 13,
        NAND = 14,
        SET = 15,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class BlendFactor {
        ZERO = 0,
        ONE = 1,
        SRC_COLOR = 2,
        ONE_MINUS_SRC_COLOR = 3,
        DST_COLOR = 4,
        ONE_MINUS_DST_COLOR = 5,
        SRC_ALPHA = 6,
        ONE_MINUS_SRC_ALPHA = 7,
        DST_ALPHA = 8,
        ONE_MINUS_DST_ALPHA = 9,
        CONSTANT_COLOR = 10,
        ONE_MINUS_CONSTANT_COLOR = 11,
        CONSTANT_ALPHA = 12,
        ONE_MINUS_CONSTANT_ALPHA = 13,
        SRC_ALPHA_SATURATE = 14,
        SRC1_COLOR = 15,
        ONE_MINUS_SRC1_COLOR = 16,
        SRC1_ALPHA = 17,
        ONE_MINUS_SRC1_ALPHA = 18,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class BlendOp {
        ADD = 0,
        SUBTRACT = 1,
        REVERSE_SUBTRACT = 2,
        MIN = 3,
        MAX = 4,
        ZERO_EXT = 1000148000,
        SRC_EXT = 1000148001,
        DST_EXT = 1000148002,
        SRC_OVER_EXT = 1000148003,
        DST_OVER_EXT = 1000148004,
        SRC_IN_EXT = 1000148005,
        DST_IN_EXT = 1000148006,
        SRC_OUT_EXT = 1000148007,
        DST_OUT_EXT = 1000148008,
        SRC_ATOP_EXT = 1000148009,
        DST_ATOP_EXT = 1000148010,
        XOR_EXT = 1000148011,
        MULTIPLY_EXT = 1000148012,
        SCREEN_EXT = 1000148013,
        OVERLAY_EXT = 1000148014,
        DARKEN_EXT = 1000148015,
        LIGHTEN_EXT = 1000148016,
        COLORDODGE_EXT = 1000148017,
        COLORBURN_EXT = 1000148018,
        HARDLIGHT_EXT = 1000148019,
        SOFTLIGHT_EXT = 1000148020,
        DIFFERENCE_EXT = 1000148021,
        EXCLUSION_EXT = 1000148022,
        INVERT_EXT = 1000148023,
        INVERT_RGB_EXT = 1000148024,
        LINEARDODGE_EXT = 1000148025,
        LINEARBURN_EXT = 1000148026,
        VIVIDLIGHT_EXT = 1000148027,
        LINEARLIGHT_EXT = 1000148028,
        PINLIGHT_EXT = 1000148029,
        HARDMIX_EXT = 1000148030,
        HSL_HUE_EXT = 1000148031,
        HSL_SATURATION_EXT = 1000148032,
        HSL_COLOR_EXT = 1000148033,
        HSL_LUMINOSITY_EXT = 1000148034,
        PLUS_EXT = 1000148035,
        PLUS_CLAMPED_EXT = 1000148036,
        PLUS_CLAMPED_ALPHA_EXT = 1000148037,
        PLUS_DARKER_EXT = 1000148038,
        MINUS_EXT = 1000148039,
        MINUS_CLAMPED_EXT = 1000148040,
        CONTRAST_EXT = 1000148041,
        INVERT_OVG_EXT = 1000148042,
        RED_EXT = 1000148043,
        GREEN_EXT = 1000148044,
        BLUE_EXT = 1000148045,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class ColorComponentFlag {
        R = 0x00000001,
        G = 0x00000002,
        B = 0x00000004,
        A = 0x00000008,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class DynamicState {
        VIEWPORT = 0,
        SCISSOR = 1,
        LINE_WIDTH = 2,
        DEPTH_BIAS = 3,
        BLEND_CONSTANTS = 4,
        DEPTH_BOUNDS = 5,
        STENCIL_COMPARE_MASK = 6,
        STENCIL_WRITE_MASK = 7,
        STENCIL_REFERENCE = 8,
        CULL_MODE = 1000267000,
        FRONT_FACE = 1000267001,
        PRIMITIVE_TOPOLOGY = 1000267002,
        VIEWPORT_WITH_COUNT = 1000267003,
        SCISSOR_WITH_COUNT = 1000267004,
        VERTEX_INPUT_BINDING_STRIDE = 1000267005,
        DEPTH_TEST_ENABLE = 1000267006,
        DEPTH_WRITE_ENABLE = 1000267007,
        DEPTH_COMPARE_OP = 1000267008,
        DEPTH_BOUNDS_TEST_ENABLE = 1000267009,
        STENCIL_TEST_ENABLE = 1000267010,
        STENCIL_OP = 1000267011,
        RASTERIZER_DISCARD_ENABLE = 1000377001,
        DEPTH_BIAS_ENABLE = 1000377002,
        PRIMITIVE_RESTART_ENABLE = 1000377004,
        VIEWPORT_W_SCALING_NV = 1000087000,
        DISCARD_RECTANGLE_EXT = 1000099000,
        DISCARD_RECTANGLE_ENABLE_EXT = 1000099001,
        DISCARD_RECTANGLE_MODE_EXT = 1000099002,
        SAMPLE_LOCATIONS_EXT = 1000143000,
        RAY_TRACING_PIPELINE_STACK_SIZE_KHR = 1000347000,
        VIEWPORT_SHADING_RATE_PALETTE_NV = 1000164004,
        VIEWPORT_COARSE_SAMPLE_ORDER_NV = 1000164006,
        EXCLUSIVE_SCISSOR_ENABLE_NV = 1000205000,
        EXCLUSIVE_SCISSOR_NV = 1000205001,
        FRAGMENT_SHADING_RATE_KHR = 1000226000,
        VERTEX_INPUT_EXT = 1000352000,
        PATCH_CONTROL_POINTS_EXT = 1000377000,
        LOGIC_OP_EXT = 1000377003,
        COLOR_WRITE_ENABLE_EXT = 1000381000,
        DEPTH_CLAMP_ENABLE_EXT = 1000455003,
        POLYGON_MODE_EXT = 1000455004,
        RASTERIZATION_SAMPLES_EXT = 1000455005,
        SAMPLE_MASK_EXT = 1000455006,
        ALPHA_TO_COVERAGE_ENABLE_EXT = 1000455007,
        ALPHA_TO_ONE_ENABLE_EXT = 1000455008,
        LOGIC_OP_ENABLE_EXT = 1000455009,
        COLOR_BLEND_ENABLE_EXT = 1000455010,
        COLOR_BLEND_EQUATION_EXT = 1000455011,
        COLOR_WRITE_MASK_EXT = 1000455012,
        TESSELLATION_DOMAIN_ORIGIN_EXT = 1000455002,
        RASTERIZATION_STREAM_EXT = 1000455013,
        CONSERVATIVE_RASTERIZATION_MODE_EXT = 1000455014,
        EXTRA_PRIMITIVE_OVERESTIMATION_SIZE_EXT = 1000455015,
        DEPTH_CLIP_ENABLE_EXT = 1000455016,
        SAMPLE_LOCATIONS_ENABLE_EXT = 1000455017,
        COLOR_BLEND_ADVANCED_EXT = 1000455018,
        PROVOKING_VERTEX_MODE_EXT = 1000455019,
        LINE_RASTERIZATION_MODE_EXT = 1000455020,
        LINE_STIPPLE_ENABLE_EXT = 1000455021,
        DEPTH_CLIP_NEGATIVE_ONE_TO_ONE_EXT = 1000455022,
        VIEWPORT_W_SCALING_ENABLE_NV = 1000455023,
        VIEWPORT_SWIZZLE_NV = 1000455024,
        COVERAGE_TO_COLOR_ENABLE_NV = 1000455025,
        COVERAGE_TO_COLOR_LOCATION_NV = 1000455026,
        COVERAGE_MODULATION_MODE_NV = 1000455027,
        COVERAGE_MODULATION_TABLE_ENABLE_NV = 1000455028,
        COVERAGE_MODULATION_TABLE_NV = 1000455029,
        SHADING_RATE_IMAGE_ENABLE_NV = 1000455030,
        REPRESENTATIVE_FRAGMENT_TEST_ENABLE_NV = 1000455031,
        COVERAGE_REDUCTION_MODE_NV = 1000455032,
        ATTACHMENT_FEEDBACK_LOOP_ENABLE_EXT = 1000524000,
        LINE_STIPPLE_KHR = 1000259000,
        MAX_ENUM = 0x7FFFFFFF
    } ;
    enum class ImageViewType{
        e1D = 0,
        e2D = 1,
        e3D = 2,
        eCUBE = 3,
        e1D_ARRAY = 4,
        e2D_ARRAY = 5,
        eCUBE_ARRAY = 6,
        eMAX_ENUM = 0x7FFFFFFF
    };
    enum class ComponentSwizzle{
        IDENTITY = 0,
        ZERO = 1,
        ONE = 2,
        R = 3,
        G = 4,
        B = 5,
        A = 6,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class ImageAspectFlag{
        COLOR = 0x00000001,
        DEPTH = 0x00000002,
        STENCIL = 0x00000004,
        METADATA = 0x00000008,
        PLANE_0 = 0x00000010,
        PLANE_1 = 0x00000020,
        PLANE_2 = 0x00000040,
        MEMORY_PLANE_0_EXT = 0x00000080,
        MEMORY_PLANE_1_EXT = 0x00000100,
        MEMORY_PLANE_2_EXT = 0x00000200,
        MEMORY_PLANE_3_EXT = 0x00000400,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class DescriptorType{
        SAMPLER = 0,
        COMBINED_IMAGE_SAMPLER = 1,
        SAMPLED_IMAGE = 2,
        STORAGE_IMAGE = 3,
        UNIFORM_TEXEL_BUFFER = 4,
        STORAGE_TEXEL_BUFFER = 5,
        UNIFORM_BUFFER = 6,
        STORAGE_BUFFER = 7,
        UNIFORM_BUFFER_DYNAMIC = 8,
        STORAGE_BUFFER_DYNAMIC = 9,
        INPUT_ATTACHMENT = 10,
        INLINE_UNIFORM_BLOCK_EXT = 1000138000,
        ACCELERATION_STRUCTURE_KHR = 1000165000,
        MAX_ENUM = 0x7FFFFFFF
    };
    enum class SamplerType{
        Linear = 0,
        Nearest = 1,
    };
    enum class IndexType {
        UINT16 = 0,
        UINT32 = 1,
        NONE = 1000165000,
        UINT8 = 1000265000,
        MAX_ENUM = 0x7FFFFFFF
    } ;

    //operator overloading for binary operation |
    inline DynamicState operator|(DynamicState a,DynamicState b){
        return static_cast<DynamicState>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline ImageAspectFlag operator|(ImageAspectFlag a,ImageAspectFlag b){
        return static_cast<ImageAspectFlag>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline ImageLayout operator|(ImageLayout a,ImageLayout b){
        return static_cast<ImageLayout>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline ShaderStageFlag operator|(ShaderStageFlag a,ShaderStageFlag b){
        return static_cast<ShaderStageFlag>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline PipelineStageFlags operator|(PipelineStageFlags a,PipelineStageFlags b){
        return static_cast<PipelineStageFlags>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline AccessFlags operator|(AccessFlags a,AccessFlags b){
        return static_cast<AccessFlags>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline ColorComponentFlag operator|(ColorComponentFlag a,ColorComponentFlag b){
        return static_cast<ColorComponentFlag>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline BlendFactor operator|(BlendFactor a,BlendFactor b){
        return static_cast<BlendFactor>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline BlendOp operator|(BlendOp a,BlendOp b){
        return static_cast<BlendOp>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline CullModeFlag operator|(CullModeFlag a,CullModeFlag b){
        return static_cast<CullModeFlag>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline SampleCountFlagBits operator|(SampleCountFlagBits a,SampleCountFlagBits b){
        return static_cast<SampleCountFlagBits>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline ImageUsageFlag operator|(ImageUsageFlag a,ImageUsageFlag b){
        return static_cast<ImageUsageFlag>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline ImageUsageFlag operator|= (ImageUsageFlag& a,ImageUsageFlag b){
        a = a|b;
        return a;
    }

    inline MemoryPropertyFlag operator|(MemoryPropertyFlag a,MemoryPropertyFlag b){
        return static_cast<MemoryPropertyFlag>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline ImageCreateFlag operator|(ImageCreateFlag a,ImageCreateFlag b){
        return static_cast<ImageCreateFlag>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline BufferUsageFlag operator|(BufferUsageFlag a,BufferUsageFlag b){
        return static_cast<BufferUsageFlag>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline ImageTiling operator|(ImageTiling a,ImageTiling b){
        return static_cast<ImageTiling>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    inline Format operator|(Format a,Format b){
        return static_cast<Format>(static_cast<uint32>(a)|static_cast<uint32>(b));
    }

    struct Extent2D{
        uint32 width,height;
    };
    struct Offset2D {
        int32 x,y;
    };
    struct Rect2D {
        Offset2D offset;
        Extent2D extent;
    };
    struct Extent3D{
        uint32 width,height,depth;
    };
    struct Viewport {
        float    x;
        float    y;
        float    width;
        float    height;
        float    minDepth;
        float    maxDepth;
    } ;
    struct ImageInfo {
        uint32              arrayLayers,mipMapLevels;
        Extent3D            extent;
        ImageLayout         initialLayout;
        Format              format;
        ImageTiling         tilling;
        ImageUsageFlag      usageFlag;
        MemoryPropertyFlag  memoryPropertyFlag;
        ImageCreateFlag     createFlag;
        SampleCountFlagBits sampleCount;
    };
    struct ImageSubresourceRange {
        ImageAspectFlag aspectMask;
        uint32          baseMipLevel;
        uint32          levelCount;
        uint32          baseArrayLayer;
        uint32          layerCount;
    };
    struct ComponentMapping {
        ComponentSwizzle r;
        ComponentSwizzle g;
        ComponentSwizzle b;
        ComponentSwizzle a;
    };
    struct ImageViewInfo {
        Image*                image;
        ImageViewType         viewType;
        Format                format;
        ComponentMapping      components;
        ImageSubresourceRange subresourceRange;
    };
    struct BufferInfo{
        ResourceSize          size;
        BufferUsageFlag       usage;
        MemoryPropertyFlag    properties;
    };
    struct FramebufferInfo {
        RenderPassInterface* renderPass;
        uint32               attachmentCount;
        ImageView**          pAttachments;
        Extent2D             extent;
        uint32               layers;
    };
    struct SwapChainInfo {
        Format   swapchainImageFormat;
        uint32   imageCount;
        uint32   minImageCount;
        uint32   maxImageCount;
        Extent2D swapchainExtent;
    };
    struct AttachmentReference {
        uint32         attachment;
        ImageLayout    layout;
    };
    struct AttachmentDescription {
        Format                        format;
        SampleCountFlagBits           samples;
        AttachmentLoadOp              loadOp;
        AttachmentStoreOp             storeOp;
        AttachmentLoadOp              stencilLoadOp;
        AttachmentStoreOp             stencilStoreOp;
        ImageLayout                   initialLayout;
        ImageLayout                   finalLayout;
    };
    struct SubpassDescription {
        PipelineBindPoint             pipelineBindPoint;
        uint32                        inputAttachmentCount;
        const AttachmentReference*    pInputAttachments;
        uint32                        colorAttachmentCount;
        const AttachmentReference*    pColorAttachments;
        const AttachmentReference*    pResolveAttachments;
        const AttachmentReference*    pDepthStencilAttachment;
        uint32                        preserveAttachmentCount;
        const uint32*                 pPreserveAttachments;
    };
    struct SubpassDependency {
        SubpassContents       srcSubpass;
        SubpassContents       dstSubpass;
        PipelineStageFlags    srcStageMask;
        PipelineStageFlags    dstStageMask;
        AccessFlags           srcAccessMask;
        AccessFlags           dstAccessMask;
        DependencyFlags       dependencyFlags;
    };
    struct RenderPassCreateInfo {
        const void*                     pNext;
        uint32                          attachmentCount;
        const AttachmentDescription*    pAttachments;
        uint32                          subpassCount;
        const SubpassDescription*       pSubpasses;
        uint32                          dependencyCount;
        const SubpassDependency*        pDependencies;
    };
    union  ClearColorValue {
        float       f32[4];
        int32       i32[4];
        uint32      ui32[4];
    };
    struct ClearDepthStencilValue {
        float       depth;
        uint32      stencil;
    };
    union ClearValue {
        ClearColorValue           color;
        ClearDepthStencilValue    depthStencil;
    };
    struct RenderPassBeginInfo {
        const void*            pNext;
        RenderPassInterface*   renderPass;
        Framebuffer*           framebuffer;
        Rect2D                 renderArea;
        uint32                 clearValueCount;
        const ClearValue*      pClearValues;
    };
    struct SpecializationMapEntry {
        uint32           constantID;
        uint32           offset;
        ResourceSize     size;
    } ;
    struct SpecializationInfo {
        uint32                             mapEntryCount;
        const SpecializationMapEntry*      pMapEntries;
        ResourceSize                       dataSize;
        const void*                        pData;
    };
    struct PipelineShaderStageCreateInfo{
        const void*                         pNext;
        PipelineShaderStageCreateFlag       flags;
        ShaderStageFlag                     stage;
        ShaderModule*                       module;
        const char*                         pName;
        const SpecializationInfo*           pSpecializationInfo;
    };
    struct VertexInputBindingDescription {
        uint32             binding;
        uint32             stride;
        VertexInputRate    inputRate;
    } ;
    struct VertexInputAttributeDescription {
        uint32    location;
        uint32    binding;
        Format    format;
        uint32    offset;
    } ;
    struct PipelineVertexInputStateCreateInfo {
        uint32                                    vertexBindingDescriptionCount;
        const VertexInputBindingDescription*      pVertexBindingDescriptions;
        uint32                                    vertexAttributeDescriptionCount;
        const VertexInputAttributeDescription*    pVertexAttributeDescriptions;
    } ;
    struct PipelineInputAssemblyStateCreateInfo {
        PrimitiveTopology              topology;
        bool                           primitiveRestartEnable;
    } ;
    struct PipelineTessellationStateCreateInfo {
        uint32                         patchControlPoints;
    } ;
    struct PipelineViewportStateCreateInfo {
        uint32                              viewportCount;
        const Viewport*                     pViewports;
        uint32                              scissorCount;
        const Rect2D*                       pScissors;
    };
    struct PipelineRasterizationStateCreateInfo {
        bool                                 depthClampEnable;
        bool                                 rasterizerDiscardEnable;
        PolygonMode                          polygonMode;
        CullModeFlag                         cullMode;
        FrontFace                            frontFace;
        bool                                 depthBiasEnable;
        float                                depthBiasConstantFactor;
        float                                depthBiasClamp;
        float                                depthBiasSlopeFactor;
        float                                lineWidth;
    } ;
    struct PipelineMultisampleStateCreateInfo {
        using SampleMask=uint32;
        SampleCountFlagBits                  rasterizationSamples;
        bool                                 sampleShadingEnable;
        float                                minSampleShading;
        const SampleMask*                    pSampleMask;
        bool                                 alphaToCoverageEnable;
        bool                                 alphaToOneEnable;
    } ;
    struct StencilOpState {
        StencilOp    failOp;
        StencilOp    passOp;
        StencilOp    depthFailOp;
        CompareOp    compareOp;
        uint32       compareMask;
        uint32       writeMask;
        uint32       reference;
    } ;
    struct PipelineDepthStencilStateCreateInfo {
        bool                                  depthTestEnable;
        bool                                  depthWriteEnable;
        CompareOp                               depthCompareOp;
        bool                                  depthBoundsTestEnable;
        bool                                  stencilTestEnable;
        StencilOpState                          front;
        StencilOpState                          back;
        float                                     minDepthBounds;
        float                                     maxDepthBounds;
    } ;
    struct PipelineColorBlendAttachmentState {
        bool                 blendEnable;
        BlendFactor            srcColorBlendFactor;
        BlendFactor            dstColorBlendFactor;
        BlendOp                colorBlendOp;
        BlendFactor            srcAlphaBlendFactor;
        BlendFactor            dstAlphaBlendFactor;
        BlendOp                alphaBlendOp;
        ColorComponentFlag    colorWriteMask;
    } ;
    struct PipelineColorBlendStateCreateInfo {
        bool                                      logicOpEnable;
        LogicOp                                     logicOp;
        uint32                                      attachmentCount;
        const PipelineColorBlendAttachmentState*    pAttachments;
        float                                         blendConstants[4];
    } ;
    struct PipelineDynamicStateCreateInfo {
        uint32                             dynamicStateCount;
        const DynamicState*                pDynamicStates;
    } ;
    struct GraphicPipelineCreateInfo{
        const void*                                      pNext;
        uint32                                           stageCount;
        const PipelineShaderStageCreateInfo*             pStages;
        const PipelineVertexInputStateCreateInfo*        pVertexInputState;
        const PipelineInputAssemblyStateCreateInfo*      pInputAssemblyState;
        const PipelineTessellationStateCreateInfo*       pTessellationState;
        const PipelineViewportStateCreateInfo*           pViewportState;
        const PipelineRasterizationStateCreateInfo*      pRasterizationState;
        const PipelineMultisampleStateCreateInfo*        pMultisampleState;
        const PipelineDepthStencilStateCreateInfo*       pDepthStencilState;
        const PipelineColorBlendStateCreateInfo*         pColorBlendState;
        const PipelineDynamicStateCreateInfo*            pDynamicState;
        PipelineLayout*                                  pLayout;
        RenderPassInterface*                             renderPass;
        int32                                            subpass=-1;
        Pipeline*                                        pBasePipelineHandle;
        int32                                            basePipelineIndex;
    };
    struct ShaderModuleCreateInfo {
        uint64                       codeSize;
        const uint32*                pCode;
    };
    struct DescriptorSetBindingInfo{
        uint32             binding;
        DescriptorType     type;
        std::string        name;
    };
    struct DescriptorSetInfo{
        std::vector<DescriptorSetBindingInfo> bindnings;
        uint32                                set;
        ShaderStageFlag                       stage;
    };
    struct DescriptorSetLayoutBinding {
        uint32             binding;
        DescriptorType     descriptorType;
        uint32             descriptorCount;
        ShaderStageFlag    stageFlags;
        const Sampler*     pImmutableSamplers;
    };
    struct DescriptorSetLayoutCreateInfo {
        const void*                       pNext;
        uint32                            bindingCount;
        const DescriptorSetLayoutBinding* pBindings;
    };
    struct DescriptorSetAllocateInfo {
        const void*             pNext;
        DescriptorPool*         descriptorPool;
        uint32                  descriptorSetCount;
        const DescriptorSetLayout* pSetLayouts;
    };
    struct DescriptorImageInfo {
        const Sampler*      sampler;
        ImageView*          imageView;
        ImageLayout         imageLayout;
    };
    struct DescriptorBufferInfo {
        Buffer*             buffer;
        ResourceSize        offset;
        ResourceSize        range;
    };
    struct WriteDescriptorSet {
        const void*             pNext;
        DescriptorSet*          dstSet;
        uint32                  dstBinding;
        uint32                  dstArrayElement;
        uint32                  descriptorCount;
        DescriptorType          descriptorType;
        const DescriptorImageInfo* pImageInfo;
        const DescriptorBufferInfo*    pBufferInfo;
        const BufferView*              pTexelBufferView;
    };
}

#endif //ENGINEALL_RENDERINTERFACE_H
