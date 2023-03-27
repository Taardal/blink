#include "VulkanRenderPass.h"

namespace Blink {

    VulkanRenderPass::VulkanRenderPass(VulkanSwapChain* swapChain, VulkanDevice* device)
        : swapChain(swapChain), device(device) {}

    VkRenderPass VulkanRenderPass::getRenderPass() const {
        return renderPass;
    }

    bool VulkanRenderPass::initialize() {
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

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        uint32_t subpassIndex = 0;
        VkSubpassDependency subpassDependency{};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = subpassIndex;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &subpassDependency;

        if (device->createRenderPass(&renderPassCreateInfo, &renderPass) != VK_SUCCESS) {
            BL_LOG_ERROR("Could not create render pass");
            return false;
        }

        return true;
    }

    void VulkanRenderPass::terminate() {
        device->destroyRenderPass(renderPass);
    }

    void VulkanRenderPass::begin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer) const {
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = framebuffer;
        renderPassBeginInfo.renderArea.offset = { 0, 0};
        renderPassBeginInfo.renderArea.extent = swapChain->getExtent();

        VkClearColorValue clearColorValue = {
                {0.0f, 0.0f, 0.0f, 1.0f}
        };

        VkClearDepthStencilValue clearDepthStencilValue{};
        clearDepthStencilValue.depth = 1.0f;
        clearDepthStencilValue.stencil = 0;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = clearColorValue;
        clearValues[1].depthStencil = clearDepthStencilValue;

        renderPassBeginInfo.clearValueCount = (uint32_t) clearValues.size();
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRenderPass::end(VkCommandBuffer commandBuffer) const {
        vkCmdEndRenderPass(commandBuffer);
    }
}
