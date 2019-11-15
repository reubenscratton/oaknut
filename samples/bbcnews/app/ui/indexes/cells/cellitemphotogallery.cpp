//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "cellitem.h"


class BNCellItemPhotoGallery : public BNCellItem {
public:
    
    BNCellItemPhotoGallery(BNCellsModule* module) : BNCellItem(module, BNCellStyle::PhotoGallery) {
        _summary = new BNLabel();
        
        /*int maxImages = _module->_json.intVal("maxImages");
        maxImages = MIN(maxImages, (int)_photoGalleryImages.size());
        if (maxImages <= 1) {
            _imageSize = {rect.size.width, rect.size.width * 9.f/16.f};
        } else {
            float imageHeight = rect.size.width / maxImages;
            _imageSize = {imageHeight, imageHeight};
        }
        
        _headline->measureForWidth(rect.size.width, {0,0});
        _headline->_bounds.origin.y = _summary->_bounds.origin.y - _headline->_bounds.size.height;
        
        _frame.size = {rect.size.width, _imageSize.height};
*/
    }

    void setRelationship(BNRelationship* relationship) override {
        BNCellItem::setRelationship(relationship);
        _photoGalleryImages = _item->photoGalleryImages();
        if (_photoGalleryImages.size() == 0) {
            auto indexImage = _item->getIndexImage();
            if (indexImage) {
                _photoGalleryImages.push_back(indexImage);
            }
        }
        _headline->applyStyle("photoGalleryCell.headline");
        _summary->applyStyle("photoGalleryCell.summary");
        
        _summary->setLayoutSize(MEASURESPEC::Fill(),MEASURESPEC::Wrap());
    }
    
/*
    void createView(View* superview) override {
        RECT imageFrame = {0, 0, _imageSize.width, _imageSize.height};
        
        // Do baseclass view creation but without the index image
        _imageSize = {0,0};
        BNCellItem::createView(superview);
        _imageSize = imageFrame.size;
        
        int maxImages = _module->_json.intVal("maxImages");
        maxImages = MIN(maxImages, (int)_photoGalleryImages.size());

        for (int i=0 ; i<maxImages ; i++) {
            BNImage* image = (BNImage*)_photoGalleryImages[i];
            BNImageView* imageView = new BNImageView();
            imageView->setRect(imageFrame);
            if (!(i&1)) {
                imageView->setBackgroundColor(0xFF7f7f7f); // TODO: style
            }
            imageView->setBNImage(image);
            _view->insertSubview(imageView, 0);
            imageFrame.origin.x += imageFrame.size.width;
        }
        
        COLOR bkgndColor = app->getStyleColor("color.cellOverlayBackground");
        _headline->_label->setBackgroundColor(bkgndColor);
        _headline->_label->setOpaque(false);
        _summary->_label->setBackgroundColor(bkgndColor);
        _summary->_label->setOpaque(false);
        _cameraIcon = new ImageView();
        _cameraIcon->setImageAsset("CameraIcon");
        _cameraIcon->setBackgroundColor(bkgndColor);
        _cameraIcon->setContentMode(ImageView::ContentMode::AspectFit);
        _cameraIcon->setGravity({GRAVITY_CENTER,GRAVITY_CENTER});
        auto foo = app->getStyle("photoGalleryCell.cameraIconPadding")->arrayVal();
        SIZE iconPadding = {foo[0].floatVal(), foo[1].floatVal()};
        _cameraIcon->setRect({0, _headline->_bounds.origin.y - iconPadding.height,
            iconPadding.width, iconPadding.height});
        _view->addSubview(_cameraIcon);
    }
    
*/
    vector<BNBaseModel*> _photoGalleryImages;
    ImageView* _cameraIcon;

};


DECLARE_DYNCREATE(BNCellItemPhotoGallery, BNCellsModule*);

