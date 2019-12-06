//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "cellitem.h"


class BNCellItemFeature : public BNCellItem {
public:
    
    RECT _accentFrame;
    View* _accentView;
    string _imagePos;
    

    BNCellItemFeature(BNCellsModule* module) : BNCellItem(module, BNCellStyle::Feature) {
        
        _accentView = new View();
        _accentView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Abs(app->getStyleFloat("featureAccentStripHeight")));
        _accentView->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Below(_imageView, 0));
        addSubview(_accentView);
        
        _topic->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Top());
        _topic->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        _headline->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Abs(app->dp(32)));
        _headline->applyStyle((module->_cellsPerRow > 1) ? "featureHeadline"_S : "featureFullWidthHeadline"_S);
		_textAreaInsets = app->getStyle("text-insets")->edgeInsetsVal("feature");
		_showMediaGlyphInHeadline = true;
		_hideTimestamp = true;
		_imagePos = module->_json.stringVal("imagePosition");
        if (app->_defaultDisplay->sizeClass() == Display::Phone) {
            //self.summary.numLines = 3;
        }


	}
    void setRelationship(BNRelationship* relationship) override {
        BNCellItem::setRelationship(relationship);
    
        // TODO: Magazine will eventually have it's own relationship
        // secondaryType. When it does, replace this.
        string colorName;
        if (_item && _item->_modelId.hasPrefix(BNModelIdMagazine)) {
            colorName = "color.featureMagazine";
        } else if (_item && _item->_site == BNContentSiteSport) {
            colorName = "color.featureSport";
        } else if (relationship->_secondaryType == BNRelationshipTypeGroupFeatureAnalysis) {
            colorName = "color.featureAnalysis";
        } else if (relationship->_secondaryType == BNRelationshipTypeGroupAlsoInTheNews) {
            colorName = "color.bbcNewsLiveRed";
        } else {
            colorName = "color.featureFeatures";
        }
        _accentView->setBackgroundColor(app->getStyleColor(colorName));
    }

    /*
    void measureForContainingRect(const RECT& arect) override {

        RECT rect;
        rect.origin = {0,0};
        rect.size = arect.size;
        float imageWidth = rect.size.width;
        float imageHeight = 0;
        RECT textArea = rect;
        float minimumHeight = 0.f;
        if (_imagePos == "top") {
            imageHeight = (imageWidth * _imageAspect);
            textArea.origin.y += imageHeight;
        } else if (_imagePos == "left" || _imagePos == "right") {
            imageWidth *= _module->_imageWidthSpec;
            imageHeight = (imageWidth * _imageAspect);
            textArea.size.width = rect.size.width - imageWidth;
            minimumHeight = imageHeight;
            if (_imagePos == "left") {
                textArea.origin.x += imageWidth;
            } else {
                _imageOrigin = {rect.size.width - imageWidth, 0};
            }
        }
        _imageSize = {imageWidth, imageHeight};
        
        _accentFrame = {textArea.origin.x, textArea.origin.y,
            textArea.size.width, _featureAccentStripHeight};
        //textArea.origin.y += featureAccentStripHeight;
        
        textArea = textArea.copyWithInsets(_textAreaInsets);
        
        if (_item->getHomedCollection() && !_hideTopics) {
            _topic->measureForWidth(textArea.size.width, textArea.origin);
            textArea.origin.y = _topic->_bounds.bottom() + 4;
        } else {
            textArea.origin.y = textArea.origin.y + 8;
        }
        
        _headline->measureForWidth(textArea.size.width, textArea.origin);
        textArea.origin.y = _headline->_bounds.bottom() + 2; // todo
        
        
        CGFloat textBottom;
     
        if (!_summary) {
            textBottom =_headline->_bounds.bottom() + _textAreaInsets.bottom;
        } else {
            _summary->measureForWidth(textArea.size.width, textArea.origin);
            textBottom = _summary->_bounds.bottom() + _textAreaInsets.bottom;
        }
        
        float frameHeight = MAX(minimumHeight, textBottom);
        
        _frame.size = {rect.size.width, frameHeight};

    }
*/
};

DECLARE_DYNCREATE(BNCellItemFeature, BNCellsModule*);


#define FEATURE_LARGE_ALPHA 0.9f


class BNCellItemFeatureLarge : public BNCellItemFeature {
public:
    
    View* _textFrameBackgroundView;

    BNCellItemFeatureLarge(BNCellsModule* module) : BNCellItemFeature(module) {
        _imagePos = "top";
    }

/*
    void measureForContainingRect(const RECT& arect) override {
        RECT rect = arect;
        float origWidth = rect.size.width;
        rect.size.width = ((rect.size.width) / 2) - 8;
        BNCellItemFeature::measureForContainingRect(rect);
        rect.size.width = origWidth;
        _imageSize = {rect.size.width, rect.size.width*9.f/16.f};
        _frame.size = _imageSize;
        float dy = _accentFrame.origin.y - 180; // TODO: move this constant into layout
        POINT delta = {0, dy};
        _summary->adjustFrame(delta);
        _headline->adjustFrame(delta);
        _topic->adjustFrame(delta);
        _accentFrame.origin.y += delta.y;
    }

    void createView(View* superview) override {
        BNCellItemFeature::createView(superview);
	    RECT textFrame = _accentFrame;
        textFrame.size.height = _frame.size.height - textFrame.origin.y;
        _textFrameBackgroundView = new View();
        _textFrameBackgroundView->setRect(textFrame);
        // _textFrameBackgroundView->setBackgroundColor(_headline->_label->_backgroundColor);
        // _textFrameBackgroundView.alpha = FEATURE_LARGE_ALPHA;
        // _headline->_label.backgroundColor = nil;
        // self.headline.label.opaque = NO;
        // self.summary.label.backgroundColor = nil;
        // self.summary.label.opaque = NO;
 
        _view->insertSubview(_textFrameBackgroundView, _view->indexOfSubview(_imageView));
    }

    void deleteView() override {
        BNCellItemFeature::deleteView();
        _textFrameBackgroundView->removeFromParent();
        _textFrameBackgroundView = NULL;
    }

*/

};

DECLARE_DYNCREATE(BNCellItemFeatureLarge, BNCellsModule*);

