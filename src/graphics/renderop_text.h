//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

typedef struct _TEXTRENDERPARAMS {
    AtlasPage* atlasPage;
    int forecolour;
    inline bool operator<(const struct _TEXTRENDERPARAMS& y) const {
        return atlasPage < y.atlasPage && forecolour<y.forecolour;
    }
    inline bool equals(const struct _TEXTRENDERPARAMS& o) const {
        return atlasPage==o.atlasPage && forecolour==o.forecolour;
    }

    ObjPtr<class TextRenderOp> renderOp;
} TEXTRENDERPARAMS;


class TextRenderOp : public RenderOpMultiRect {
public:
    vector<RECT> _rectsTex;
    TEXTRENDERPARAMS _textRenderParams;
    
    TextRenderOp(View* view, TEXTRENDERPARAMS* textRenderParams);
    
    void addGlyph(Glyph* glyph, const RECT& rect);
    void reset();
    virtual bool canMergeWith(const RenderOp* op);
    virtual void render(Window* window, Surface* surface);
    virtual void asQuads(QUAD* quad);
};




