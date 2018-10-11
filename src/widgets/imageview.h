//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class ImageView : public View {
public:
    
    enum ContentMode {
        ActualSize, // Image is shown at actual size (default)
        AspectFit,  // Image is stretched to be as large as possible without overflowing bounds
        AspectFill  // Image is stretched to fill the ImageView and overflow its bounds
    };
	
	ImageView();
	void setImageUrl(const string& url);
    void setImageAsset(const string& assetPath);
    void setBitmap(Bitmap* bitmap);
    void setImageNode(AtlasNode* node);
    ContentMode getContentMode() const;
    void setContentMode(ContentMode contentMode);
	
	// Overrides
	void attachToWindow(Window* window) override;
	void detachFromWindow() override;
	void onEffectiveTintColorChanged() override;
	void layout() override;
    bool applyStyleValue(const string& name, const StyleValue* value) override;
    void updateContentSize(SIZE constrainingSize) override;
    void updateRenderOps() override;


protected:
    string _url;
    string _assetPath;
    ObjPtr<URLRequest> _request;
    ObjPtr<TextureRenderOp> _renderOp;
    ObjPtr<AtlasNode> _atlasNode;
    bool _loaded;
    TIMESTAMP _startLoadTime;
    bool _useFadeEffect;
    bool _useSharedTexture;
    SIZE _sharedTextureSize;
    RECT _rectTex;
    ContentMode _contentMode;

    // Internal helpers
    void loadImage();
    void cancelLoad();
    
};


