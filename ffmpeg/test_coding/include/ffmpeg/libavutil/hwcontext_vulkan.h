/*
 * This file is part 的 FFmpeg.
 *
 * FFmpeg is 释放 software; you can redistribute it and/or
 * mod如果y it under the terms 的 the GNU Lesser General 公共
 * License as published by the 释放 Software Foundation; either
 * version 2.1 的 the License, 或 (at your 选项) any later version.
 *
 * FFmpeg is distributed 中 the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY 或 FITNESS FOR PARTICULAR PURPOSE.  参见 the GNU
 * Lesser General 公共 License 用于 more details.
 *
 * You should have received a 复制 的 the GNU Lesser General 公共
 * License along，使用 FFmpeg; 如果 not, write 到 the 释放 Software
 * Foundation, Inc., 51 Franklin Street, F如果th Floor, Boston, M02110-1301 USA
 */

#ifndef AVUTIL_HWCONTEXT_VULKAN_H
#define AVUTIL_HWCONTEXT_VULKAN_H

#if defined(_WIN32) && !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>

#include "pixfmt.h"
#include "frame.h"
#include "hwcontext.h"

typedef struct AVVkFrame AVVkFrame;

typedef struct AVVulkanDeviceQueueFamily {
    /* Queue family index */
    int idx;
    /* 数量 的 queues 中 the queue family 中 use */
    int num;
    /* Queue family capabilities. Must be non-zero.
     * 标志 may be removed 到 indicate the queue family may not be used
     * 用于 a given purpose. */
    VkQueueFlagBits flags;
    /* Vulkan implementations are allowed 到 列表 multiple 视频 queues
     * which d如果fer 中 what they can encode 或 decode. */
    VkVideoCodecOperationFlagBitsKHR video_caps;
} AVVulkanDeviceQueueFamily;

/**
 * @file
 * API-spec如果ic header 用于 AV_HW设备_TYPE_VULKAN.
 *
 * For user-分配d pools, AVHW帧s上下文.pool must 返回 AV缓冲区Refs
 *，使用 the data 指针 设置 到 an AVVk帧.
 */

/**
 * Main Vulkan 上下文, 分配d as AVHW设备上下文.hwctx.
 * All 的 these can be 设置 before init 到 change what the 上下文 uses
 */
typedef struct AVVulkanDeviceContext {
    /**
     * Custom 内存 allocator, else NULL
     */
    const VkAllocationCallbacks *alloc;

    /**
     * 指针 到 a vk获取InstanceProcAddr loading function.
     * 如果 un设置, will dynamically load 和 use libvulkan.
     */
    PFN_vkGetInstanceProcAddr get_proc_addr;

    /**
     * Vulkan instance. Must be at least version 1.3.
     */
    VkInstance inst;

    /**
     * Physical 设备
     */
    VkPhysicalDevice phys_dev;

    /**
     * Active 设备
     */
    VkDevice act_dev;

    /**
     * 此结构体 should be 设置 到 the 设置 的 features that present 和 enabled
     * during 设备 creation. 当 a 设备 is 创建d by FFmpeg, it will 默认 to
     * enabling all that are present 的 the shaderImageGatherExtended,
     * fragmentStoresAndAtomics, shaderInt64 和 vertexPipelineStoresAndAtomics features.
     */
    VkPhysicalDeviceFeatures2 device_features;

    /**
     * Enabled instance extensions.
     * 如果 supplying your own 设备 上下文, 设置 this 到 an 数组 的 strings, with
     * each entry containing the spec如果ied Vulkan extension string 到 enable.
     * Duplicates are possible 和 accepted.
     * 如果 no extensions are enabled, 设置 these fields 到 NULL, 和 0 respectively.
     * av_vk_get_可选_instance_extensions() 可用于 到 enumerate extensions
     * that FFmpeg may use 如果 enabled.
     */
    const char * const *enabled_inst_extensions;
    int nb_enabled_inst_extensions;

    /**
     * Enabled 设备 extensions. By 默认, VK_KHR_external_内存_fd,
     * VK_EXT_external_内存_dma_buf, VK_EXT_image_drm_格式_mod如果ier,
     * VK_KHR_external_semaphore_fd 和 VK_EXT_external_内存_host are enabled 如果 found.
     * 如果 supplying your own 设备 上下文, these fields takes the same 格式 as
     * the above fields,，使用 the same conditions that duplicates are possible
     * 和 accepted, 和 that NULL 和 0 respectively means no extensions are enabled.
     * av_vk_get_可选_设备_extensions() 可用于 到 enumerate extensions
     * that FFmpeg may use 如果 enabled.
     */
    const char * const *enabled_dev_extensions;
    int nb_enabled_dev_extensions;

#if FF_API_VULKAN_SYNC_QUEUES
    /**
     * Locks a queue, preventing other threads，来自 submitting any command
     * 缓冲区 到 this queue.
     * 如果 设置 到 NULL, will be 设置 到 lavu-internal functions that utilize a
     * mutex.
     *
     * Deprecated: use VK_KHR_internally_synchronized_queues.
     */
    attribute_deprecated
    void (*lock_queue)(struct AVHWDeviceContext *ctx, uint32_t queue_family, uint32_t index);

    /**
     * Similar 到 lock_queue(), unlocks a queue. Must only be called after locking.
     *
     * Deprecated: use VK_KHR_internally_synchronized_queues.
     */
    attribute_deprecated
    void (*unlock_queue)(struct AVHWDeviceContext *ctx, uint32_t queue_family, uint32_t index);
#endif

    /**
     * Queue families used. Must be preferentially ordered. 列表 may contain
     * duplicates.
     *
     * For compatibility reasons, all the enabled queue families 列表ed above
     * (queue_family_(tx/comp/encode/decode)_index) must also be included in
     * this 列表 until they're removed after deprecation.
     */
    AVVulkanDeviceQueueFamily qf[64];
    int nb_qf;

    /* Queue creation 标志, 用于 vk获取设备Queue2. */
    VkDeviceQueueCreateFlags queue_flags;
} AVVulkanDeviceContext;

/**
 * Defines the behaviour 的 帧 allocation.
 */
typedef enum AVVkFrameFlags {
    /* Unless this 标志 is 设置, autodetected 标志 will be OR'd based 上 the
     * 设备 和 tiling during av_hw帧_ctx_init(). */
    AV_VK_FRAME_FLAG_NONE              = (1ULL << 0),

    /* Disables multiplane images.
     * This is required 到 export/import images，来自 CUDA. */
    AV_VK_FRAME_FLAG_DISABLE_MULTIPLANE = (1ULL << 2),
} AVVkFrameFlags;

/**
 * 分配d as AVHW帧s上下文.hwctx, 用于 设置 pool-spec如果ic 选项
 */
typedef struct AVVulkanFramesContext {
    /**
     * Controls the tiling 的 分配d 帧s.
     * 如果 left as VK_IMAGE_TILING_OPTIMAL (0), will use optimal tiling.
     * Can be 设置 到 VK_IMAGE_TILING_LINEAR 到 force linear images,
     * 或 VK_IMAGE_TILING_DRM_格式_MOD如果IER_EXT 到 force DMABUF-backed
     * images.
     * @note Imported 帧s，来自 other APIs ignore this.
     */
    VkImageTiling tiling;

    /**
     * Defines extra usage 的 输出 帧s. 如果 non-zero, all 标志 MUST be
     * supported by the Vk格式. Regardless, 帧s will always have the
     * following usage 标志 enabled, 如果 supported by the 格式:
     * - VK_IMAGE_USAGE_采样D_BIT
     * - VK_IMAGE_USAGE_STORAGE_BIT
     * - VK_IMAGE_USAGE_TRANSFER_SRC_BIT
     * - VK_IMAGE_USAGE_TRANSFER_DST_BIT
     */
    VkImageUsageFlagBits usage;

    /**
     * Extension data 用于 image creation.
     * 如果 DRM tiling is used, a VkImageDrm格式Mod如果ier列表创建InfoEXT 结构体
     * can be added 到 spec如果y the exact mod如果ier 到 use.
     *
     * Additional 结构体s may be added at av_hw帧_ctx_init() time,
     * which will be 释放d automatically 上 uninit(), so users must only 释放
     * any 结构体s they've 分配d themselves.
     */
    void *create_pnext;

    /**
     * Extension data 用于 内存 allocation. Must have as many entries as
     * the 数量 的 planes 的 the sw_格式.
     * This will be chained 到 VkExport内存分配Info, which is used
     * 到 make all pool images exportable 到 other APIs 如果 the necessary
     * extensions are present 中 enabled_dev_extensions.
     */
    void *alloc_pnext[AV_NUM_DATA_POINTERS];

    /**
     * combination 的 AVVk帧标志. Unless AV_VK_帧_标志_NONE is 设置,
     * autodetected 标志 will be OR'd based 上 the 设备 和 tiling during
     * av_hw帧_ctx_init().
     */
    AVVkFrameFlags flags;

    /**
     * 标志 到 设置 during image creation. 如果 un设置, 默认s to
     * VK_IMAGE_创建_ALIAS_BIT.
     */
    VkImageCreateFlags img_flags;

    /**
     * Vulkan 格式 用于 each image. MUST be compatible，使用 the 像素格式.
     * 如果 un设置, will be automatically 设置.
     * There are at most two compatible 格式s 用于 a 帧 - a multiplane
     * 格式, 和 a single-plane multi-image 格式.
     */
    VkFormat format[AV_NUM_DATA_POINTERS];

    /**
     * 数量 的 layers each image will have.
     */
    int nb_layers;

    /**
     * Locks a 帧, preventing other threads，来自 changing 帧 properties.
     * Users SHOULD only ever lock just before command submission 中 order
     * 到 get accurate 帧 properties, 和 unlock immediately after command
     * submission without waiting 用于 it 到 finish.
     *
     * 如果 un设置, will be 设置 到 lavu-internal functions that utilize a mutex.
     */
    void (*lock_frame)(struct AVHWFramesContext *fc, AVVkFrame *vkf);

    /**
     * Similar 到 lock_帧(), unlocks a 帧. Must only be called after locking.
     */
    void (*unlock_frame)(struct AVHWFramesContext *fc, AVVkFrame *vkf);
} AVVulkanFramesContext;

/*
 * 帧 结构体.
 *
 * @note the 大小 的 this 结构体 is not part 的 the ABI, 到 分配
 * you must use @av_vk_帧_alloc().
 */
struct AVVkFrame {
    /**
     * Vulkan images 到 which the 内存 is bound to.
     * May be one 用于 multiplane 格式s, 或 multiple.
     */
    VkImage img[AV_NUM_DATA_POINTERS];

    /**
     * Tiling 用于 the 帧.
     */
    VkImageTiling tiling;

    /**
     * 内存 backing the images. Either one, 或 as many as there are planes
     * 中 the sw_格式.
     * In case 的 having multiple VkImages, but one 内存, the off设置 field
     * will indicate the bound off设置 用于 each image.
     */
    VkDeviceMemory mem[AV_NUM_DATA_POINTERS];
    size_t size[AV_NUM_DATA_POINTERS];

    /**
     * OR'd 标志 用于 all 内存 分配d
     */
    VkMemoryPropertyFlagBits flags;

    /**
     * Updated after every barrier. One per VkImage.
     */
    VkAccessFlagBits2 access[AV_NUM_DATA_POINTERS];
    VkImageLayout layout[AV_NUM_DATA_POINTERS];

    /**
     * Synchronization timeline semaphores, one 用于 each VkImage.
     * Must not be 释放d manually. Must be waited 上 at every submission using
     * the 值 中 sem_值, 和 must be signalled at every submission,
     * using an incremented 值.
     */
    VkSemaphore sem[AV_NUM_DATA_POINTERS];

    /**
     * Up 到 date semaphore 值 at which each image becomes accessible.
     * One per VkImage.
     * Clients must wait 上 this 值 当 submitting a command queue,
     * 和 increment it 当 signalling.
     */
    uint64_t sem_value[AV_NUM_DATA_POINTERS];

    /**
     * Describes the binding off设置 的 each image 到 the Vk设备内存.
     * One per VkImage.
     */
    ptrdiff_t offset[AV_NUM_DATA_POINTERS];

    /**
     * Queue family 的 the images. Must be VK_QUEUE_FAMILY_IGNORED 如果
     * the image was 分配d，使用 the CONCURRENT concurrency 选项.
     * One per VkImage.
     */
    uint32_t queue_family[AV_NUM_DATA_POINTERS];

    /**
     * Internal data. Not 到 be accessed by users 中 any way.
     */
    struct AVVkFrameInternal *internal;
};

/**
 * 分配s a single AVVk帧 和 初始化s everything as 0.
 * @note Must be 释放d via av_释放()
 */
AVVkFrame *av_vk_frame_alloc(void);

/**
 * 返回 the optimal per-plane Vulkan 格式 用于 a given sw_格式,
 * one 用于 each plane.
 * 返回 NULL 上 unsupported 格式s.
 */
const VkFormat *av_vkfmt_from_pixfmt(enum AVPixelFormat p);

/**
 * 返回 an 数组 的 可选 Vulkan instance extensions that FFmpeg
 * may use 如果 enabled.
 * @note Must be 释放d via av_释放()
 */
const char **av_vk_get_optional_instance_extensions(int *count);

/**
 * 返回 an 数组 的 可选 Vulkan 设备 extensions that FFmpeg
 * may use 如果 enabled.
 * @note Must be 释放d via av_释放()
 */
const char **av_vk_get_optional_device_extensions(int *count);

#endif /* AVUTIL_HWCONTEXT_VULKAN_H */
