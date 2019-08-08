//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class TextRenderOp : public RenderOp {
public:
    vector<RECT> _rects;
    vector<RECT> _rectsTex;
    vector<COLOR> _forecolors;
    AtlasPage* _atlasPage;
    
    TextRenderOp(AtlasPage* atlasPage);


    void addGlyph(Glyph* glyph, const RECT& rect, COLOR forecolor);
    void reset();
    
    // Overrides
    bool canMergeWith(const RenderOp* op) override;
    void prepareToRender(Renderer* renderer, Surface* surface) override;
    int numQuads() override;
    void asQuads(QUAD* quad) override;
    void validateShader(Renderer* renderer) override;
};




