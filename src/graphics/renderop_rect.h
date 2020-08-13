//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class RectShader : public Shader {
public:
    struct Features {
        uint32_t tex0:2;
        uint32_t alpha:1;
        uint32_t tint:1;
        uint32_t sdf:3;
        uint32_t stroke:1;
        uint32_t _:24;
        
        Features() {
            *(uint32_t*)this = 0;
        }

        operator uint32_t() const {
            return *(uint32_t*)this;
        }
    } _features;

    static RectShader* get(Renderer* r, Features features);
    
    int16_t _u_sampler;
    int16_t _u_strokeColor;
    int16_t _u_u;
    int16_t _u_cornerRadius;
    int16_t _u_cornerRadii;
protected:
    RectShader(Renderer* renderer, Features features);
    
    string getSupportingSource() override;
    string getFragmentSource() override;
    
    int getTextureCount() override;
    Texture::Type getTextureType(int index) override;
    
    friend class ShaderFactory<RectShader>;
};

class RectRenderOp : public RenderOp {
public:
    RectRenderOp();

    // API
    COLOR getFillColor() const;
    void setFillColor(COLOR fillColor);
    COLOR getStrokeColor() const;
    void setStrokeColor(COLOR strokeColor);
    float getStrokeWidth() const;
    void setStrokeWidth(float strokeWidth);
    VECTOR4 getCornerRadii() const;
    void setCornerRadius(float cornerRadius);
    void setCornerRadii(const VECTOR4& cornerRadii);

    // Overrides
    void setAlpha(float alpha) override;
    bool canMergeWith(const RenderOp* op) override;
    void asQuads(QUAD* quad) override;
    void validateShader(RenderTask* r) override;
    void prepareToRender(RenderTask* r, class Surface* surface) override;
    
//protected:
    COLOR _strokeColor;
    float _strokeWidth;
    VECTOR4 _cornerRadii;
};


