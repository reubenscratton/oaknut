//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

RenderResource::RenderResource(Renderer* renderer) : _renderer(renderer) {
}

Texture::Texture(Renderer* renderer, int format) : RenderResource(renderer) {
    _it = renderer->_textures.insert(renderer->_textures.end(), this);
    _type = Normal;
    _format = format;
}
Texture::~Texture() {
    if (_renderer) {
        _renderer->releaseTexture(this);
    }
}

Shader::Shader(Renderer* renderer) : RenderResource(renderer) {
    _it = renderer->_shaders.insert(renderer->_shaders.end(), this);
    declareAttribute("position", VariableType::Float2);
    _u_mvp = declareUniform("mvp", VariableType::Matrix4, Uniform::Vertex);
    // TODO: Not sure these are generic enough to be here....
    _u_alpha = declareUniform("alpha", VariableType::Float1);
    declareAttribute("color", VariableType::Color);
}

string Shader::getSupportingSource() {
    return "";
}
string Shader::getFragmentSource() {
    return "";
}

Shader::~Shader() {
    if (_onDestroy) {
        _onDestroy();
    }
    if (_renderer) {
        _renderer->releaseShader(this);
    }
}


int16_t Shader::Uniform::length() {
    switch (type) {
        case Color: return SL_SIZEOF_COLOR;
        case Int1:  return 4;
        case Float1: return 4;
        case Float2: return 8;
        case Float4: return 16;
        case Matrix4: return 64;
    }
    assert(0);
    return 4;
}


int16_t Shader::declareAttribute(const string& name, VariableType type, string outValue) {
    VertexShaderOutput attribute;
    attribute.isVertexAttribute = true;
    attribute.name = name;
    attribute.type = type;
    attribute.outValue = outValue;
    _vertexShaderOutputs.push_back(attribute);
    return 0;
}

int16_t Shader::declareUniform(const string& name, VariableType type, Uniform::Usage usage) {
    Uniform uniform;
    uniform.name = name;
    uniform.usage = usage;
    uniform.type = type;    
    _uniforms.push_back(uniform);
    return (int16_t)(_uniforms.size()-1);
}


string oak::sl_getTypeString(Shader::VariableType type) {
    switch (type) {
        case Shader::VariableType::Color: return SL_HALF4_DECL;
        case Shader::VariableType::Int1: return "int";
        case Shader::VariableType::Float1: return SL_FLOAT1;
        case Shader::VariableType::Float2: return SL_FLOAT2;
        case Shader::VariableType::Float4: return SL_FLOAT4;
        case Shader::VariableType::Matrix4: return SL_MATRIX4;
    }
}






RenderTask::RenderTask(Renderer* r) : RenderResource(r) {
    _currentClipValid = true;
}

Renderer::Renderer() : _quadBuffer(sizeof(QUAD), 256) {
    _primarySurfaceFormat = PIXELFORMAT_DEFAULT32;
}

/*Shader* Renderer::getStandardShader(RenderOp* op) {
    Shader* shader = _standardShaders[features];
    if (!shader) {
        shader = new Shader(this, features);
        _standardShaders[features] = shader;
    }
    return shader;    
}*/

void RenderTask::setCurrentSurface(Surface* surface) {
    if (surface == _currentSurface) {
        return;
    }
    _currentSurface = surface;
    _currentSurfaceValid = false;
}
void RenderTask::setCurrentTexture(Texture* texture) {
    if (texture == _currentTexture) {
        return;
    }
    _currentTexture = texture;
    _currentTextureValid = false;
}


void RenderTask::pushClip(const RECT& clip) {
    _currentClip = clip;
    if (_currentClip.size.width<0) _currentClip.size.width = 0;
    if (_currentClip.size.height<0) _currentClip.size.height = 0;
    if (_clips.size()) {
        _currentClip.intersectWith(_clips.top());
    }
    _clips.push(_currentClip);
    _currentClipValid = false;
    //log_dbg(">clip %d,%d %dx%d", (int)_currentClip.origin.x,(int)_currentClip.origin.y, (int)_currentClip.size.width, (int)_currentClip.size.height);
}
void RenderTask::popClip() {
    assert(_clips.size()>0);
    _clips.pop();
    if (!_clips.size()) {
        _currentClip = {0,0, _currentSurface->_size.width, _currentSurface->_size.height};
    } else {
        _currentClip = _clips.top();
    }
    _currentClipValid = false;
    //log_dbg("<clip %d,%d %dx%d", (int)_currentClip.origin.x,(int)_currentClip.origin.y, (int)_currentClip.size.width, (int)_currentClip.size.height);
}

void Renderer::reset() {
    //delete _quadBuffer; todo: fix this leak
    while (_textures.size() > 0) {
        releaseTexture((Texture*)*_textures.begin());
    }
    while (_shaders.size() > 0) {
        releaseShader((Shader*)*_shaders.begin());
    }
}


template<>
void RenderTask::setUniform<int>(int16_t uniformIndex, const int& val) {
    setUniformData(uniformIndex, &val, sizeof(val));
}
template<>
void RenderTask::setUniform<float>(int16_t uniformIndex, const float& val) {
    setUniformData(uniformIndex, &val, sizeof(val));
}
template<>
void RenderTask::setUniform<POINT>(int16_t uniformIndex, const POINT& val) {
    setUniformData(uniformIndex, &val, sizeof(val));
}
template<>
void RenderTask::setUniform<MATRIX4>(int16_t uniformIndex, const MATRIX4& val) {
    setUniformData(uniformIndex, val.get(), 16*sizeof(float));
}
static inline uint16_t make_half(float f) {
    uint32_t x = *((uint32_t*)&f);
    int e = ((x&0x7f800000U) >> 23) -127; // undo 32-bit bias to get the real exponent
    e = MIN(15, MAX(-15,e)) + 15; // clamp to 5-bit range and add the 16-bit bias
    uint16_t h = ((x>>16)&0x8000U)
               | (e<<10) // exponent is 5 bits, down from 8
               | ((x>>13)&0x03ffU); // significand is 10 bits, truncated from 23
    return h;
}

template<>
void RenderTask::setUniform<COLOR>(int16_t uniformIndex, const COLOR& val) {
    auto& uniform = _currentShader->_uniforms[uniformIndex];
    if (uniform.cachedColorVal == val) {
        return;
    }
    uniform.cachedColorVal = val;
#if RENDERER_METAL
    uint16_t halfs[4];
    halfs[0] = make_half((val&0xff)/255.0f);
    halfs[1] = make_half(((val&0xff00)>>8)/255.0f);
    halfs[2] = make_half(((val&0xff0000)>>16)/255.0f);
    halfs[3] = make_half(((val&0xff000000)>>24)/255.0f);
    setUniformData(uniformIndex, halfs, sizeof(halfs));
#else
    float c[4];
    c[0] = (val&0xff)/255.0f;
    c[1] = ((val&0xff00)>>8)/255.0f;
    c[2] = ((val&0xff0000)>>16)/255.0f;
    c[3] = ((val&0xff000000)>>24)/255.0f;
    setUniformData(uniformIndex, c, sizeof(c));
#endif
    
}
template<>
void RenderTask::setUniform<SIZE>(int16_t uniformIndex, const SIZE& val) {
    setUniformData(uniformIndex, &val, sizeof(val));
}
template<>
void RenderTask::setUniform<VECTOR2>(int16_t uniformIndex, const VECTOR2& val) {
    setUniformData(uniformIndex, &val, sizeof(val));
}
template<>
void RenderTask::setUniform<VECTOR4>(int16_t uniformIndex, const VECTOR4& val) {
    setUniformData(uniformIndex, &val, sizeof(val));
}



void Renderer::releaseTexture(Texture* tex) {
    if (tex->_renderer) {
        assert(tex->_renderer == this);
        _textures.erase(tex->_it);
        tex->_renderer = NULL;
    }
}

void Renderer::releaseShader(Shader* shader) {
    if (shader->_renderer) {
        assert(shader->_renderer == this);
        if (shader->_shaderState) {
            deleteShaderState(shader->_shaderState);
            shader->_shaderState = NULL;
        }
        _shaders.erase(shader->_it);
        shader->_renderer = NULL;
    }
}

void Renderer::createTextureForBitmap(Bitmap* bitmap) {
    bitmap->_texture = createTexture(bitmap->_format);
    bitmap->_texture->_bitmap = bitmap;
    bitmap->_texture->_minFilterLinear = !bitmap->_sampleNearest;
    bitmap->_texture->_magFilterLinear = !bitmap->_sampleNearest;
    bitmap->_texture->setSize({bitmap->_width, bitmap->_height});
    bitmap->_texture->_needsUpload = true;
}

void RenderTask::bindBitmap(Bitmap* bitmap) {
    assert(bitmap);
    if (!bitmap->_texture) {
        _renderer->createTextureForBitmap(bitmap);
    }
    setCurrentTexture(bitmap->_texture);
}

void RenderTask::setCurrentShader(Shader *shader) {
    if (_currentShader != shader) {
        if (!shader->_shaderState) {
            shader->_shaderState = _renderer->createShaderState(shader);
        }
        _currentShader = shader;
        _currentShaderValid = false;
    }
}

ItemPool::Alloc* Renderer::allocQuads(int num, ItemPool::Alloc* existingAlloc) {
    return _quadBuffer.alloc(num, existingAlloc);
}


void Renderer::invalidateQuads(ItemPool::Alloc* alloc) {
    uint8_t* lo = alloc->addr();
    uint8_t* hi = lo + alloc->cb();
    if (!_quadBufferDirtyLo) {
        _quadBufferDirtyLo = lo;
        _quadBufferDirtyHi = hi;
    } else {
        _quadBufferDirtyLo = MIN(_quadBufferDirtyLo, lo);
        _quadBufferDirtyHi = MAX(_quadBufferDirtyHi, hi);
    }
}



