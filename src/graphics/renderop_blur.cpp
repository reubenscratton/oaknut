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

 Future optimization: We run the same blur shader twice, but each run only uses half the varyings.
 */

static const int BLUR_RADIUS=4;


class BlurShader : public Shader {
public:
    BlurShader(Renderer* renderer) : Shader(renderer) {
        _features.textures[0] = Texture::Type::Normal;
        declareAttribute("texcoord", VariableType::Float2);
        _u_texOffset = declareUniform("texOffset", VariableType::Float2, Uniform::Usage::Vertex);
        
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
        for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
            float firstWeight = _standardGaussianWeights[i*2 + 1];
            float secondWeight = _standardGaussianWeights[i*2 + 2];
            float optimizedWeight = firstWeight + secondWeight;
            _optimizedOffsets.push_back((firstWeight * (i*2 + 1) + secondWeight * (i*2 + 2)) / optimizedWeight);
        }

        
        declareAttribute("blurCoord0", VariableType::Float2, SL_VERTEX_INPUT(texcoord) ".xy");
        for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
            string offset = string::format(SL_UNIFORM(texOffset) " * %f",  _optimizedOffsets[i]);
            declareAttribute(string::format("blurCoord%d", (i*2)+1), VariableType::Float2, SL_VERTEX_INPUT(texcoord) ".xy +" + offset);
            declareAttribute(string::format("blurCoord%d", (i*2)+2), VariableType::Float2, SL_VERTEX_INPUT(texcoord) ".xy -" + offset);
        }
        

    }
    
    string getFragmentSource() override {
        int numOptimizedOffsets = (int)_optimizedOffsets.size();
        
        //string fs =
        //"uniform vec2 texOffset;\n";
        
        
        // Inner texture loop
        string s = string::format(SL_HALF4_DECL " tmp = " SL_TEXSAMPLE_2D(texture, SL_VERTEX_OUTPUT(blurCoord0)) " * %f;\n", _standardGaussianWeights[0]);

        for (uint32_t i = 0; i < numOptimizedOffsets; i++) {
            GLfloat firstWeight = _standardGaussianWeights[i * 2 + 1];
            GLfloat secondWeight = _standardGaussianWeights[i * 2 + 2];
            GLfloat optimizedWeight = firstWeight + secondWeight;
            
            s += string::format("tmp += " SL_TEXSAMPLE_2D(texture, SL_VERTEX_OUTPUT(blurCoord)"%d") " * %f;\n",
                                i*2+1, optimizedWeight);
            s += string::format("tmp += " SL_TEXSAMPLE_2D(texture, SL_VERTEX_OUTPUT(blurCoord)"%d") " * %f;\n",
                                i*2+2, optimizedWeight);
        }
        
        // If the number of required samples exceeds the amount we can pass in via varyings, we
        // have to do dependent texture reads in the fragment shader
        uint32_t trueNumberOfOptimizedOffsets = _blurRadius / 2 + (_blurRadius % 2);
        
        
        if (trueNumberOfOptimizedOffsets > numOptimizedOffsets) {
            for (uint32_t i = numOptimizedOffsets; i < trueNumberOfOptimizedOffsets; i++) {
                GLfloat firstWeight = _standardGaussianWeights[i * 2 + 1];
                GLfloat secondWeight = _standardGaussianWeights[i * 2 + 2];
                
                GLfloat optimizedWeight = firstWeight + secondWeight;
                GLfloat optimizedOffset = (firstWeight * (i * 2 + 1) + secondWeight * (i * 2 + 2)) / optimizedWeight;
                
                s += string::format("tmp += " SL_TEXSAMPLE_2D(texture, SL_VERTEX_OUTPUT(blurCoord0)) " + " SL_UNIFORM(texOffset) " * %f;\n", optimizedOffset, optimizedWeight);
                s += string::format("tmp += " SL_TEXSAMPLE_2D(texture, SL_VERTEX_OUTPUT(blurCoord0)) " - " SL_UNIFORM(texOffset) " * %f;\n", optimizedOffset, optimizedWeight);

            }
        }
        
        s += SL_OUTPIXVAL "=tmp;\n";
        return s;
    }
    
    int _blurRadius;
    float _sigma;
    vector<float> _standardGaussianWeights;
    vector<float> _optimizedOffsets;

    int16_t _u_texOffset;
};

class PostBlurShader : public Shader {
public:
    PostBlurShader(Renderer* renderer) : Shader(renderer, Features(false,0,true,Texture::Type::Normal) ) {
        
    }
    string getFragmentSource() override {
        return
        SL_HALF3_DECL " luminanceWeighting = " SL_HALF3 "(0.2125, 0.7154, 0.0721);\n"
        // Desaturate
        SL_HALF4_DECL " tmp = mix(" SL_TEXSAMPLE_2D(texture, SL_VERTEX_OUTPUT(texcoord)) ", " SL_VERTEX_OUTPUT(color) ", " SL_HALF1 "(0.9));\n"
        SL_HALF1_DECL " lum = dot(tmp.rgb, luminanceWeighting);\n"
        SL_HALF1_DECL " lumRatio = lum * 0.05;\n"
        SL_OUTPIXVAL "  = " SL_HALF4 "(mix(" SL_HALF3 "(lum), tmp.rgb, " SL_HALF1 "(0.95)) + lumRatio, " SL_HALF1 "(1.0));\n";
    }
};





extern MATRIX4 setOrthoFrustum(float l, float r, float b, float t, float n, float f);





BlurRenderOp::BlurRenderOp() : RenderOp() {


}
BlurRenderOp::~BlurRenderOp() {
    
}

void BlurRenderOp::validateShader(Renderer* renderer) {
    if (!_alloc) {
        _alloc = renderer->allocQuads(1, NULL);
    }
    
    
    _blurShader = new BlurShader(renderer);
    _shader = new PostBlurShader(renderer);

    _tex1 = renderer->createTexture(renderer->_primarySurfaceFormat);
    _tex1->_maxMipMapLevel = 2;
    _tex1->_minFilterLinear = true;
    _tex1->_mipFilterLinear = false;
    _dirty = true;
    
    _surface1 = renderer->createPrivateSurface();
    _surface2 = renderer->createPrivateSurface();
    _surface2->_texture->_magFilterLinear = true;
}

void BlurRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    quad->tl.s = quad->bl.s = 0;
    quad->tr.s = quad->br.s = _rect.size.width / _fullSizePow2.width;
#if RENDERER_GL
    quad->tl.t = quad->tr.t =  _rect.size.height / _fullSizePow2.height;
    quad->bl.t = quad->br.t = 0;
#else
    quad->tl.t = quad->tr.t = 0 ;
    quad->bl.t = quad->br.t = _rect.size.height / _fullSizePow2.height; // 0;
#endif
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
    
    // Set up an MVP for the offscreen part
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
    BlurShader* blurShader = _blurShader.as<BlurShader>();
    renderer->setCurrentSurface(_surface1);
    renderer->setUniform(blurShader->_u_texOffset, VECTOR2(0, 1.f/_downsampledSize.height));
    renderer->drawQuads(1, _alloc->offset);
    
    // 1D blur vertically, source is surface1 and destination is surface2
    renderer->setCurrentSurface(_surface2);
    renderer->setCurrentTexture(_surface1->_texture);
    renderer->setUniform(blurShader->_u_texOffset, VECTOR2(1.f/_downsampledSize.width,0));
    renderer->drawQuads(1, _alloc->offset);
    
    renderer->setCurrentSurface(surface);
    renderer->setCurrentTexture(_surface2->_texture);
    RenderOp::prepareToRender(renderer, surface);

}







