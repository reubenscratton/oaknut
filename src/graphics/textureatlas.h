//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class AtlasNode : public Object {
public:
    class AtlasPage* page;
    int key;
    RECT rect;
    AtlasNode* left;
    AtlasNode* right;
    bool filled;
    
    AtlasNode(AtlasPage* page);
    AtlasNode* insertRect(RECT* r);
};

/**
 * TextureAtlas - a texture containing many small images, e.g. font glyphs, button assets, etc
 */
class AtlasPage : public Object {
public:
    sp<Bitmap> _bitmap;
    AtlasNode* start_node;
    
    AtlasPage(int width, int height, int bitmapFormat);
    //AtlasNode* pack(Bitmap* bmp);
    AtlasNode* reserve(int w, int h);
    class BitmapProvider* importAsset(const string& assetPath);
    void sendToGpu();
    
};

class Atlas : public Object {
public:
    vector<sp<AtlasPage>> pages;
    vector<sp<AtlasNode>> nodes;
    int width;
    int height;
    int bitmapFormat;
    int glfilter;
    
    Atlas(int width, int height, int bitmapFormat);
    AtlasNode* reserve(int w, int h, int padding);
	AtlasPage* lastPage();
};

