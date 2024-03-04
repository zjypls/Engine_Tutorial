//
// Created by z on 24-2-24.
//
#include "Include/stb/stb_image.h"

#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Utils/ZUtils.h"
#include "Z/Renderer/Passes/SkyboxPass.h"
#include "Z/Renderer/RenderResource.h"
namespace Z {
    void SkyboxPass::Init(RenderPassInitInfo *info) {
        framebuffer.renderPass = ((SkyboxPassInitInfo *) info)->renderpass;
        Context = info->graphicInterface;
    }

    void SkyboxPass::draw() {
    }

    void SkyboxPass::clear() {
    }

} // Z