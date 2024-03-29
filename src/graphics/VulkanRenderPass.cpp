#include "VulkanRenderPass.h"

namespace Blink {

    VulkanRenderPass::VulkanRenderPass(
        VulkanSwapChain* swapChain,
        VulkanDevice* device,
        VulkanPhysicalDevice* physicalDevice
    ) : swapChain(swapChain),
        device(device),
        physicalDevice(physicalDevice)
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChain->getSurfaceFormat().format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        uint32_t colorAttachmentIndex = 0;
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = colorAttachmentIndex;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        uint32_t subpassIndex = 0;
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = subpassIndex;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (device->createRenderPass(&renderPassInfo, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("Could not create render pass");
        }
    }

    VulkanRenderPass::~VulkanRenderPass() {
        device->destroyRenderPass(renderPass);
    }

    VulkanRenderPass::operator VkRenderPass() const {
        return renderPass;
    }

    void VulkanRenderPass::begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer) const {
        VkClearColorValue clearColorValue = {
            {0.0f, 0.0f, 0.0f, 1.0f}
        };

        VkClearDepthStencilValue clearDepthStencilValue{};
        clearDepthStencilValue.depth = 1.0f;
        clearDepthStencilValue.stencil = 0;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = clearColorValue;
        clearValues[1].depthStencil = clearDepthStencilValue;

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = framebuffer;
        renderPassBeginInfo.renderArea.offset = { 0, 0};
        renderPassBeginInfo.renderArea.extent = swapChain->getExtent();
        renderPassBeginInfo.clearValueCount = (uint32_t) clearValues.size();
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRenderPass::end(VkCommandBuffer commandBuffer) const {
        vkCmdEndRenderPass(commandBuffer);
    }

    VkFormat VulkanRenderPass::findDepthFormat() {
        std::vector<VkFormat> candidates = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
    };
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
        VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        return physicalDevice->findSupportedFormat(candidates, tiling, features);
    }
}
