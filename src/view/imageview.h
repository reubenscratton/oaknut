//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class ImageView : public View, IURLRequestDelegate {
public:

	string _url;
    ObjPtr<TextureRenderOp> _renderOp;
    ObjPtr<Bitmap> _bitmap;
    ObjPtr<AtlasNode> _atlasNode;
	bool _errorDisplay;
	long _startLoadTime;
	bool _useFadeEffect;
    bool _useSharedTexture;
    SIZE _sharedTextureSize;
    RECT _rectTex;
	
	ImageView();
	void setImageUrl(const string& url);
    void setImageBitmap(Bitmap* bitmap);
    void setImageNode(AtlasNode* node);
	
	// Overrides
	virtual void attachToWindow(Window* window);
	virtual void detachFromWindow();
	virtual void onEffectiveTintColourChanged();
    virtual void updateRenderOps();

	// Internal helpers
	void loadImage();
	
	// IURLRequestDelegate
	virtual void onUrlRequestLoad(URLData* data);

};


