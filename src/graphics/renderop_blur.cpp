//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
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


#define log2f(x) (logf(x)*1.4426950408889634)
#ifndef GL_TEXTURE_MAX_LEVEL
#define GL_TEXTURE_MAX_LEVEL 0x813D
#endif


#define BLUR_RADIUS 4			// 12

#if EMSCRIPTEN
#define MAX_OPTIMIZED_OFFSETS 3
#else
#define MAX_OPTIMIZED_OFFSETS 7
#endif


class GLProgramPostBlur : public GLProgram {
public:
    
    virtual void load();
};

extern Matrix4 setOrthoFrustum(float l, float r, float b, float t, float n, float f);

static GLProgramBlur s_progBlur(BLUR_RADIUS, BLUR_RADIUS);
static GLProgramPostBlur s_progBlurPost;


BlurRenderOp::BlurRenderOp(View* view) : RenderOp(view) {
    _prog = &s_progBlurPost;
    GLint otex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &otex);

    check_gl(glGenTextures, 3, _textureIds);
    for (int i=0 ; i<3 ; i++) {
        check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[i]);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    
    // Set up a two private FBOs and bind textures 1 and 2 to them
    GLint oldFBO;
    check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &oldFBO);
    check_gl(glGenFramebuffers, 2, _fb);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb[0]);
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[1]);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureIds[1], 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb[1]);
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[2]);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureIds[2], 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, oldFBO);
    glBindTexture(GL_TEXTURE_2D, otex);
}

BlurRenderOp::~BlurRenderOp() {
    check_gl(glDeleteTextures, 3, _textureIds);
    check_gl(glDeleteFramebuffers, 2, _fb);
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
    _fullSizePow2.width = 1 << (int)(ceil(log2f(_fullSizePow2.width)));
    _fullSizePow2.height = 1 << (int)(ceil(log2f(_fullSizePow2.height)));
    
    GLint otex;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &otex);

    
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[0]);
    check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
    check_gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _fullSizePow2.width, _fullSizePow2.height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[1]);
    check_gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _downsampledSize.width, _downsampledSize.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[2]);
    check_gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, _downsampledSize.width, _downsampledSize.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, otex);
    
    _mvp = setOrthoFrustum(0,_downsampledSize.width,0,_downsampledSize.height,-1,1);

    _vertexesValid = false;
}

void BlurRenderOp::asQuads(QUAD *quad) {
    rectToSurfaceQuad(_rect, quad);
    quad->tl.s = quad->bl.s = 0;//_rectTex.left();
    quad->tl.t = quad->tr.t = _rect.size.height / _fullSizePow2.height;
    quad->tr.s = quad->br.s = _rect.size.width / _fullSizePow2.width;
    quad->bl.t = quad->br.t = 0;//_rectTex.bottom();
}


void BlurRenderOp::render(Window* window, Surface* surface) {
    RenderOp::render(window, surface);

    GLint viewport[4];
    check_gl(glGetIntegerv, GL_VIEWPORT, viewport);

    // Copy the area of framebuffer to be blurred to a private pow2 texture and
    // generate 2 levels of mipmap (ie 1/2 and 1/4 sizes)
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[0]);
    RECT wrect = _rect;
    wrect.origin.y += viewport[3] - _rect.size.height;
    check_gl(glCopyTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, wrect.origin.x, wrect.origin.y, wrect.size.width, wrect.size.height);
    check_gl(glGenerateMipmap, GL_TEXTURE_2D);

    // Switch to private FBO
    GLint oldFBO;
    check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &oldFBO);
    if (!_alloc) {
        _alloc = window->_quadBuffer->alloc(1, NULL);
    }
    if (!_vertexesValid) {
        QUAD* quad = (QUAD*)_alloc->addr();
        quad->bl = VERTEX_Make(0, 0, 0, 0, 0);
        quad->br = VERTEX_Make(_downsampledSize.width, 0, 1, 0, 0);
        quad->tl = VERTEX_Make(0,  _downsampledSize.height, 0, 1, 0);
        quad->tr = VERTEX_Make(_downsampledSize.width, _downsampledSize.height, 1, 1, 0);
        check_gl(glBufferSubData, GL_ARRAY_BUFFER, _alloc->offset*sizeof(QUAD), _alloc->count*sizeof(QUAD), _alloc->addr());
        _vertexesValid = true;
    }
    
    // 1D blur horizontally, source is 1/4-sized mipmap and destination is fb0
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb[0]);
    s_progBlur.use(window);
    s_progBlur.setMvp(_mvp);
    s_progBlur.setTexOffset(POINT_Make(0, 1.f/_downsampledSize.height));
    glViewport(0, 0, _downsampledSize.width, _downsampledSize.height);
    check_gl(glDrawElements, GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)((_alloc->offset)*6*sizeof(GLshort)));

    // 1D blur vertically
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[1]);
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, _fb[1]);
    s_progBlur.setTexOffset(POINT_Make(1.f/_downsampledSize.width, 0));
    check_gl(glDrawElements, GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)((_alloc->offset)*6*sizeof(GLshort)));

    // Switch back to rendering to the backbuffer
    check_gl(glBindFramebuffer, GL_FRAMEBUFFER, oldFBO);
    check_gl(glViewport, viewport[0], viewport[1], viewport[2], viewport[3]);
    window->_currentTexture = NULL;
    check_gl(glBindTexture, GL_TEXTURE_2D, _textureIds[2]);
    _prog->use(window);
}


GLProgramBlur::GLProgramBlur(int blurRadius, int sigma) : _blurRadius(blurRadius) {
    int32_t blurRadiusInPixels = round(_blurRadius);
    uint32_t calculatedSampleRadius = 0;
    if (blurRadiusInPixels >= 1) {
        float minimumWeightToFindEdgeOfSamplingArea = 1.0/256.0;
        calculatedSampleRadius = floor(sqrt(-2.0 * pow(blurRadiusInPixels, 2.0) * log(minimumWeightToFindEdgeOfSamplingArea * sqrt(2.0 * M_PI * pow(blurRadiusInPixels, 2.0))) ));
        calculatedSampleRadius += calculatedSampleRadius % 2;
    }
    _blurRadius = calculatedSampleRadius;
    _sigma = sigma;
}


void GLProgramBlur::load() {

	
	
	// First, generate the normal Gaussian weights for a given sigma
	GLfloat standardGaussianWeights[_blurRadius + 1];
    GLfloat sumOfWeights = 0.0;
    for (uint32_t i = 0; i < _blurRadius + 1; i++) {
        standardGaussianWeights[i] = (1.0 / sqrt(2.0 * M_PI * pow(_sigma, 2.0))) * exp(-pow(i, 2.0) / (2.0 * pow(_sigma, 2.0)));
		sumOfWeights += ((i==0)?1:2) * standardGaussianWeights[i];
    }


    // Next, normalize these weights to prevent the clipping of the Gaussian curve
	// at the end of the discrete samples from reducing luminance
    for (uint32_t i = 0; i < _blurRadius + 1; i++) {
        standardGaussianWeights[i] = standardGaussianWeights[i] / sumOfWeights;
    }
	
    // From these weights we calculate the offsets to read interpolated values from
	uint32_t numberOfOptimizedOffsets = MIN(_blurRadius / 2 + (_blurRadius % 2), MAX_OPTIMIZED_OFFSETS);
	GLfloat optimizedGaussianOffsets[numberOfOptimizedOffsets];
    for (uint32_t i = 0; i < numberOfOptimizedOffsets; i++) {
        GLfloat firstWeight = standardGaussianWeights[i*2 + 1];
        GLfloat secondWeight = standardGaussianWeights[i*2 + 2];
        GLfloat optimizedWeight = firstWeight + secondWeight;
        optimizedGaussianOffsets[i] = (firstWeight * (i*2 + 1) + secondWeight * (i*2 + 2)) / optimizedWeight;
    }
	
	
	uint32_t trueNumberOfOptimizedOffsets = _blurRadius / 2 + (_blurRadius % 2);
	

	char ach[128];
	
	string vertexShader =
        "attribute highp vec2 vPosition;\n"
        "uniform highp mat4 mvp;\n"
        "attribute vec2 texcoord;\n"
        "uniform vec2 texOffset;\n";
	
	sprintf(ach, "varying vec2 blurCoordinates[%d];\n", 1 + numberOfOptimizedOffsets * 2);
	vertexShader.append(ach);
	
	vertexShader.append(
		"void main() {\n"
        "   gl_Position = mvp * vec4(vPosition,0,1);\n"
	);
    vertexShader.append(
		"   blurCoordinates[0] = texcoord.xy;\n");
    for (uint32_t i = 0; i < numberOfOptimizedOffsets; i++) {
        sprintf(ach,
		"   blurCoordinates[%lu] = texcoord.xy + texOffset * %f;\n"
		"   blurCoordinates[%lu] = texcoord.xy - texOffset * %f;\n",
			(unsigned long)((i * 2) + 1), optimizedGaussianOffsets[i],
			(unsigned long)((i * 2) + 2), optimizedGaussianOffsets[i]);
		vertexShader.append(ach);
    }

	vertexShader.append("}\n");
	


	string fragShader =
		"uniform sampler2D texture;\n"
        "uniform vec2 texOffset;\n";
	sprintf(ach,
		"varying highp vec2 blurCoordinates[%d];\n", 1 + numberOfOptimizedOffsets * 2);
	fragShader.append(ach);

	fragShader.append(
		"void main() {\n");
	
    // Inner texture loop
    sprintf(ach, "lowp vec4 c = texture2D(texture, blurCoordinates[0]) * %f;\n", standardGaussianWeights[0]);
	fragShader.append(ach);
    for (uint32_t i = 0; i < numberOfOptimizedOffsets; i++) {
        GLfloat firstWeight = standardGaussianWeights[i * 2 + 1];
        GLfloat secondWeight = standardGaussianWeights[i * 2 + 2];
        GLfloat optimizedWeight = firstWeight + secondWeight;
        sprintf(ach, "c += texture2D(texture, blurCoordinates[%lu]) * %f;\n", (unsigned long)((i * 2) + 1), optimizedWeight);
		fragShader.append(ach);
        sprintf(ach, "c += texture2D(texture, blurCoordinates[%lu]) * %f;\n", (unsigned long)((i * 2) + 2), optimizedWeight);
		fragShader.append(ach);
    }
    
    // If the number of required samples exceeds the amount we can pass in via varyings, we have to do dependent texture reads in the fragment shader
    if (trueNumberOfOptimizedOffsets > numberOfOptimizedOffsets) {
        for (uint32_t i = numberOfOptimizedOffsets; i < trueNumberOfOptimizedOffsets; i++) {
            GLfloat firstWeight = standardGaussianWeights[i * 2 + 1];
            GLfloat secondWeight = standardGaussianWeights[i * 2 + 2];
            
            GLfloat optimizedWeight = firstWeight + secondWeight;
            GLfloat optimizedOffset = (firstWeight * (i * 2 + 1) + secondWeight * (i * 2 + 2)) / optimizedWeight;
            
            sprintf(ach, "c += texture2D(texture, blurCoordinates[0] + texOffset * %f) * %f;\n", optimizedOffset, optimizedWeight);
			fragShader.append(ach);
			sprintf(ach, "c += texture2D(texture, blurCoordinates[0] - texOffset * %f) * %f;\n", optimizedOffset, optimizedWeight);
			fragShader.append(ach);
        }
    }
	
    fragShader.append("   gl_FragColor = c;\n"
//     gl_FragColor = vec4((textureColor.rgb) + (luminanceRatio), textureColor.w);
		"}\n");

	loadShaders(vertexShader.data(), fragShader.data());
    
	_posTexOffset = glGetUniformLocation(_program, "texOffset");
}

void GLProgramBlur::setTexOffset(POINT texOffset) {
	check_gl(glUniform2f, _posTexOffset, texOffset.x, texOffset.y);
}

void GLProgramPostBlur::load() {
    loadShaders(TEXTURE_VERTEX_SHADER,
                "varying vec2 v_texcoord;\n"
                "varying lowp vec4 v_colour;\n"
                "uniform sampler2D texture;\n"
                "const lowp vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);\n"
                "void main() {\n"
                // Desaturate
                "   lowp vec4 c = mix(texture2D(texture, v_texcoord), v_colour, 0.9);\n"
                "   lowp float lum = dot(c.rgb, luminanceWeighting);\n"
                "   lowp float lumRatio = ((0.5 - lum) * 0.1);\n"
                "   gl_FragColor = vec4(mix(vec3(lum), c.rgb, 0.8) + lumRatio, 1.0);\n"
                "}\n"
                );
}
