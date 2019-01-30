//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

RenderResource::RenderResource(Renderer* renderer) : _renderer(renderer) {
}

Texture::Texture(Renderer* renderer) : RenderResource(renderer) {
    _it = renderer->_textures.insert(renderer->_textures.end(), this);
}
Texture::~Texture() {
    if (_renderer) {
        _renderer->releaseTexture(this);
    }
}

Shader::Shader(Renderer* renderer) : RenderResource(renderer) {
    _it = renderer->_shaders.insert(renderer->_shaders.end(), this);
    _u_mvp = declareUniform("mvp", Uniform::Matrix4, Uniform::Vertex);
}
Shader::~Shader() {
    if (_renderer) {
        _renderer->releaseShader(this);
    }
}

string Shader::getUniformFields(Shader::Uniform::Usage usage) {
    string s="";
    for (auto& uniform : _uniforms) {
        if (usage == uniform.usage) {
            switch (uniform.type) {
                case Shader::Uniform::Int1: s+="int"; break;
                case Shader::Uniform::Float1: s+="float"; break;
                case Shader::Uniform::Float2: s+="float2"; break;
                case Shader::Uniform::Float4: s+="float4"; break;
                case Shader::Uniform::Matrix4: s+="float4x4"; break;
            }
            s+= " ";
            s+= uniform.name;
            s+= ";\n";
        }
    }
    return s;
}

int16_t Shader::Uniform::length() {
    switch (type) {
        case Int1:  return 4;
        case Float1: return 4;
        case Float2: return 8;
        case Float4: return 16;
        case Matrix4: return 64;
    }
    assert(0);
    return 4;
}

int16_t Shader::declareUniform(const char* name, Uniform::Type type, Uniform::Usage usage) {
    Uniform uniform;
    uniform.name = name;
    uniform.usage = usage;
    uniform.type = type;    
    _uniforms.push_back(uniform);
    return (int16_t)(_uniforms.size()-1);
}



StandardShader::StandardShader(Renderer* renderer, ShaderFeatures features) : Shader(renderer), _features(features) {
    if (_features.sampler0 != 0) {
        _u_sampler = declareUniform("texture", Uniform::Int1);
    }
    if (_features.alpha) {
        _u_alpha = declareUniform("alpha", Uniform::Float1);
    }
    if (_features.roundRect) {
        _u_strokeColor = declareUniform("strokeColor", Uniform::Float4);
        _u_u = declareUniform("u", Uniform::Float4);
        if (_features.roundRect == SHADER_ROUNDRECT_1) {
            _u_radius = declareUniform("radius", Uniform::Float1);
        } else {
            _u_radii = declareUniform("radii", Uniform::Float4);
        }
    }

}




Renderer::Renderer(Window* window) : _window(window), _quadBuffer(sizeof(QUAD), 256) {
}

Shader* Renderer::getStandardShader(ShaderFeatures features) {
    Shader* shader = _standardShaders[features.all];
    if (!shader) {
        shader = new StandardShader(this, features);
        _standardShaders[features.all] = shader;
    }
    return shader;    
}


void Renderer::reset() {
    _doneInit = false;
    //delete _quadBuffer; todo: fix this leak
    while (_textures.size() > 0) {
        releaseTexture((Texture*)*_textures.begin());
    }
    while (_shaders.size() > 0) {
        releaseShader((Shader*)*_shaders.begin());
    }
}


template<>
void Renderer::setUniform<int>(int16_t uniformIndex, const int& val) {
    setUniformData(uniformIndex, &val, sizeof(val));
}
template<>
void Renderer::setUniform<float>(int16_t uniformIndex, const float& val) {
    setUniformData(uniformIndex, &val, sizeof(val));
}
template<>
void Renderer::setUniform<MATRIX4>(int16_t uniformIndex, const MATRIX4& val) {
    setUniformData(uniformIndex, val.get(), 16*sizeof(float));
}
template<>
void Renderer::setUniform<COLOR>(int16_t uniformIndex, const COLOR& val) {
    float c[4];
    c[3] = ((val&0xff000000)>>24)/255.0f;
    c[2] = (val&0xff)/255.0f;
    c[1] = ((val&0xff00)>>8)/255.0f;
    c[0] = ((val&0xff0000)>>16)/255.0f;
    setUniformData(uniformIndex, c, sizeof(c));
}
template<>
void Renderer::setUniform<VECTOR2>(int16_t uniformIndex, const VECTOR2& val) {
    setUniformData(uniformIndex, &val, sizeof(val));
}
template<>
void Renderer::setUniform<VECTOR4>(int16_t uniformIndex, const VECTOR4& val) {
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
    bitmap->_texture = createTexture();
    bitmap->_texture->_bitmap = bitmap;
    bitmap->_texture->_format = bitmap->_format;
    bitmap->_texture->_minFilterLinear = !bitmap->_sampleNearest;
    bitmap->_texture->_magFilterLinear = !bitmap->_sampleNearest;
    bitmap->_texture->resize(bitmap->_width, bitmap->_height);
    bitmap->_texture->_needsUpload = true;
}

void Renderer::bindBitmap(Bitmap* bitmap) {
    assert(bitmap);
    if (!bitmap->_texture) {
        createTextureForBitmap(bitmap);
    }
    setCurrentTexture(bitmap->_texture);
}

void Renderer::setCurrentShader(Shader *shader) {
    if (_currentShader != shader) {
        if (!shader->_shaderState) {
            shader->_shaderState = createShaderState(shader);
        }
        _currentShader = shader;
        bindCurrentShader();
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



