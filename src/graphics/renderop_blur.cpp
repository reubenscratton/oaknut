//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

/*
 Blur render op and custom shader. The blur is implemented as follows:
 
 1. Maintain 3 private textures, 1 one full-size and 2 quarter-sized.
 2. Copy the area to be blurred from the framebuffer to texture 0 (the full-sized one).
 3. Generate 2 levels of mipmap on texture 0, 1/2 size and 1/4 size.
 4. Using texture 0 as source and texture 1 as target, run 1D blur shader horizontally
 5. Using texture 1 as source and texture 2 as target, run 1D blur shader vertically
 6. Upscale from texture 2 to the framebuffer with a custom desaturation effect

 */

BlurShader::BlurShader(Renderer* renderer, BlurRenderOp* op) : Shader(renderer), _op(op) {
    _u_sampler = declareUniform("texture", VariableType::Int1);
    _u_texOffset = declareUniform("texOffset", VariableType::Float2, Uniform::Usage::Vertex);
}

PostBlurShader::PostBlurShader(Renderer* renderer) : Shader(renderer) {
}


extern MATRIX4 setOrthoFrustum(float l, float r, float b, float t, float n, float f);


static const int BLUR_RADIUS=4;            // 12



BlurRenderOp::BlurRenderOp() : RenderOp() {

    _blurRadius = BLUR_RADIUS;
    _sigma = BLUR_RADIUS;
    int32_t blurRadiusInPixels = round(_blurRadius);
    uint32_t calculatedSampleRadius = 0;
    if (blurRadiusInPixels >= 1) {
        float minimumWeightToFindEdgeOfSamplingArea = 1.0/256.0;
        calculatedSampleRadius = floor(sqrt(-2.0 * pow(blurRadiusInPixels, 2.0) * log(minimumWeightToFindEdgeOfSamplingArea * sqrt(2.0 * M_PI * pow(blurRadiusInPixels, 2.0))) ));
        calculatedSampleRadius += calculatedSampleRadius % 2;
    }
    _blurRadius = calculatedSampleRadius;

}
BlurRenderOp::~BlurRenderOp() {
    
}

void BlurRenderOp::validateShader(Renderer* renderer) {
    if (!_alloc) {
        _alloc = renderer->allocQuads(1, NULL);
    }
    
    // First, generate the normal Gaussian weights for a given sigma
    _standardGaussianWeights.clear();
    float sumOfWeights = 0.0;
    for (uint32_t i = 0; i < _blurRadius + 1; i++) {
        float w = (1.0 / sqrt(2.0 * M_PI * pow(_sigma, 2.0))) * exp(-pow(i, 2.0) / (2.0 * pow(_sigma, 2.0)));
        sumOfWeights += ((i==0)?1:2) * w;
        _standardGaussianWeights.push_back(w);
    }
    
    
    // Next, normalize these weights to prevent the clipping of the Gaussian curve
    // at the end of the discrete samples from reducing luminance
    for (uint32_t i = 0; i < _blurRadius + 1; i++) {
        _standardGaussianWeights[i] /= sumOfWeights;
    }
    
    // Work out how many offsets can be passed via vertex interpolation (varyings)
    int maxOptimizedOffsets = renderer->getIntProperty(Renderer::MaxVaryingFloats);
    maxOptimizedOffsets -= 4; // float4 position is the only other varying
    
    
    // From these weights we calculate the offsets to read interpolated values from
    uint32_t numOptimizedOffsets = MIN(_blurRadius / 2 + (_blurRadius % 2), maxOptimizedOffsets);
    _optimizedOffsets.clear();
    for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
        float firstWeight = _standardGaussianWeights[i*2 + 1];
        float secondWeight = _standardGaussianWeights[i*2 + 2];
        float optimizedWeight = firstWeight + secondWeight;
        _optimizedOffsets.push_back((firstWeight * (i*2 + 1) + secondWeight * (i*2 + 2)) / optimizedWeight);
    }
    
    
    _blurShader = new BlurShader(renderer, this);
    _shader = new PostBlurShader(renderer);

    _tex1 = renderer->createTexture();
    _tex1->_maxMipMapLevel = 2;
    _tex1->_minFilterLinear = true;
    _tex1->_mipFilterLinear = false;

    _surface1 = renderer->createPrivateSurface();
    _surface2 = renderer->createPrivateSurface();
    _surface2->_texture->_magFilterLinear = true;
}

void BlurRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    quad->tl.s = quad->bl.s = 0;
    quad->tl.t = quad->tr.t = _rect.size.height / _fullSizePow2.height;
    quad->tr.s = quad->br.s = _rect.size.width / _fullSizePow2.width;
    quad->bl.t = quad->br.t = 0;
}


void BlurRenderOp::setRect(const RECT &rect) {
    SIZE prevSize = _rect.size;
    RenderOp::setRect(rect);
    if (prevSize.width == rect.size.width && prevSize.height==rect.size.height) {
        return;
    }
    
    _downsampledSize.width = (int)(_rect.size.width/4);
    _downsampledSize.height = (int)(_rect.size.height/4);

    // Round fullsize tex up to pow2
    _fullSizePow2.width = (int)_rect.size.width;
    _fullSizePow2.height = (int)_rect.size.height;
#define log2f(x) (logf(x)*1.4426950408889634)
    _fullSizePow2.width = 1 << (int)(ceil(log2f(_fullSizePow2.width)));
    _fullSizePow2.height = 1 << (int)(ceil(log2f(_fullSizePow2.height)));
    
    _dirty = true;
    
    _mvp = setOrthoFrustum(0,_downsampledSize.width,0,_downsampledSize.height,-1,1);

    
}





void BlurRenderOp::prepareToRender(Renderer* renderer, Surface* surface) {
    
    if (_dirty) {
        _dirty = false;

        _tex1->resize(_fullSizePow2.width, _fullSizePow2.height);
        _surface1->setSize({static_cast<float>(_downsampledSize.width), static_cast<float>(_downsampledSize.height)});
        _surface2->setSize({static_cast<float>(_downsampledSize.width), static_cast<float>(_downsampledSize.height)});
        
        QUAD* quad = (QUAD*)_alloc->addr();
        float dw = _downsampledSize.width;
        float dh = _downsampledSize.height;
        quad->tl = {0,   0, 0, 1, 0};
        quad->tr = {dw,  0, 1, 1, 0};
        quad->bl = {0,  dh, 0, 0, 0};
        quad->br = {dw, dh, 1, 0, 0};
        renderer->uploadQuad(_alloc);
    }
    
    // Grab the region to be blurred into tex1
    RECT rect = surfaceRect();
    renderer->setCurrentTexture(_tex1);
    renderer->copyFromCurrent(rect, _tex1, {0,0});
    
    // Generate 2 levels of mipmap to scale down by 1/4
    renderer->generateMipmaps(_tex1);

    // Prepare to render to offscreen surface
    renderer->setCurrentShader(_blurShader);
    renderer->setCurrentBlendMode(BLENDMODE_NONE);
    renderer->setUniform(_blurShader->_u_mvp, _mvp);

    // 1D blur horizontally, source is 1/4-sized mipmap in tex1 and destination is surface1
    renderer->setCurrentSurface(_surface1);
    renderer->setUniform(_blurShader->_u_texOffset, VECTOR2(0, 1.f/_downsampledSize.height));
    renderer->drawQuads(1, _alloc->offset);
    
    // 1D blur vertically, source is surface1 and destination is surface2
    renderer->setCurrentSurface(_surface2);
    renderer->setCurrentTexture(_surface1->_texture);
    renderer->setUniform(_blurShader->_u_texOffset, VECTOR2(1.f/_downsampledSize.width,0));
    renderer->drawQuads(1, _alloc->offset);
    
    renderer->setCurrentSurface(surface);
    renderer->setCurrentTexture(_surface2->_texture);
    RenderOp::prepareToRender(renderer, surface);

}







