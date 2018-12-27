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
 2. glCopyTexSubImage2D from framebuffer to texture 0 (the full-sized one).
 3. Generate 2 levels of mipmap on texture 0, 1/2 size and 1/4 size.
 4. Using texture 0 as source and texture 1 as target, run 1D blur program horizontally
 5. Using texture 1 as source and texture 2 as target, run 1D blur vertically
 6. Upscale from texture 2 to the framebuffer

 Expensive, eh? My hope is that mipmap generation is cheap as chips
 
 */

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
    
    // TODO: IIRC there is a GL api for determining the max number of parameters...
#if PLATFORM_WEB
#define MAX_OPTIMIZED_OFFSETS 3
#else
#define MAX_OPTIMIZED_OFFSETS 7
#endif
    
    // From these weights we calculate the offsets to read interpolated values from
    uint32_t numOptimizedOffsets = MIN(_blurRadius / 2 + (_blurRadius % 2), MAX_OPTIMIZED_OFFSETS);
    _optimizedOffsets.clear();
    for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
        float firstWeight = _standardGaussianWeights[i*2 + 1];
        float secondWeight = _standardGaussianWeights[i*2 + 2];
        float optimizedWeight = firstWeight + secondWeight;
        _optimizedOffsets.push_back((firstWeight * (i*2 + 1) + secondWeight * (i*2 + 2)) / optimizedWeight);
    }
    
    
    rendererLoad(renderer);
    assert(_shader);
    
    _shaderValid = true;
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

    // Deliberately don't call base class cos the default mechanism is set up with the final render
    renderer->setBlendMode(BLENDMODE_NONE);
    renderer->setActiveShader(_blurShader);
    _pmvp = &surface->_mvp;
    
    if (_dirty) {
        _dirty = false;
        
        rendererResize(renderer);
        
        QUAD* quad = (QUAD*)_alloc->addr();
        float dw = _downsampledSize.width;
        float dh = _downsampledSize.height;
        quad->bl = {0, 0, 0, 0, 0};
        quad->br = {dw, 0, 1, 0, 0};
        quad->tl = {0,  dh, 0, 1, 0};
        quad->tr = {dw, dh, 1, 1, 0};
        renderer->uploadQuad(_alloc);
    }


}







