//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class ImageView : public View, IURLRequestDelegate {
public:

	string _url;
    ObjPtr<TextureRenderOp> _renderOp;
    ObjPtr<AtlasNode> _atlasNode;
	bool _errorDisplay;
	TIMESTAMP _startLoadTime;
	bool _useFadeEffect;
    bool _useSharedTexture;
    SIZE _sharedTextureSize;
    RECT _rectTex;
	
	ImageView();
	void setImageUrl(const string& url);
    void setImageAsset(const string& assetPath);
    void setBitmap(Bitmap* bitmap);
    void setImageNode(AtlasNode* node);
	
	// Overrides
	void attachToWindow(Window* window) override;
	void detachFromWindow() override;
	void onEffectiveTintColorChanged() override;
	void layout() override;
    bool applyStyleValue(const string& name, StyleValue* value) override;

	// Internal helpers
	void loadImage();
	
	// IURLRequestDelegate
	void onUrlRequestLoad(URLData* data) override;

};


