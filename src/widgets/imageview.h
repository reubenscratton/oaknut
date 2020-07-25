//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/**
 * @ingroup widgets
 * @brief Displays a bitmap image.
 */
class ImageView : public View {
public:
    
    enum ContentMode {
        ActualSize, // Image is shown at actual size (default)
        AspectFit,  // Image is stretched to be as large as possible without overflowing bounds
        AspectFill  // Image is stretched to fill the ImageView and overflow its bounds
    };
	
	ImageView();
	void setImage(const string& assetOrUrl);
    void setTexture(Texture* texture);
    void setBitmap(Bitmap* bitmap);
    void setImageNode(AtlasNode* node);
    ContentMode getContentMode() const;
    void setContentMode(ContentMode contentMode);
    RECT getImageRect() const;
	
	// Overrides
    void attachToSurface() override;
	void detachFromSurface() override;
	void onEffectiveTintColorChanged() override;
	void layout(RECT constraint) override;
    bool applySingleStyle(const string& name, const style& value) override;
    void updateIntrinsicSize(SIZE constrainingSize) override;
    void updateRenderOps() override;


protected:
    string _url;
    sp<Task> _task;
    sp<TextureRenderOp> _renderOp;
    sp<AtlasNode> _atlasNode;
    bool _loaded;
    ContentMode _contentMode;

    // Internal helpers
    void loadImage();
    void cancelLoad();
    
};


