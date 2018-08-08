//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef struct _TEXTRENDERPARAMS {
    AtlasPage* atlasPage;
    int forecolor;
    inline bool operator<(const struct _TEXTRENDERPARAMS& y) const {
        if (atlasPage < y.atlasPage) return true;
        if (atlasPage > y.atlasPage) return false;
        return forecolor<y.forecolor;
    }
    inline bool operator==(const struct _TEXTRENDERPARAMS& o) const {
        return atlasPage==o.atlasPage && forecolor==o.forecolor;
    }

    ObjPtr<class TextRenderOp> renderOp;
} TEXTRENDERPARAMS;


class TextRenderOp : public RenderOp {
public:
    vector<RECT> _rects;
    vector<RECT> _rectsTex;
    TEXTRENDERPARAMS _textRenderParams;
    
    TextRenderOp(View* view, const TEXTRENDERPARAMS* textRenderParams);


    void addGlyph(Glyph* glyph, const RECT& rect);
    void reset();
    
    // Overrides
    bool canMergeWith(const RenderOp* op) override;
    void render(Window* window, Surface* surface) override;
    int numQuads() override;
    void asQuads(QUAD* quad) override;
    void validateShader() override;
};




