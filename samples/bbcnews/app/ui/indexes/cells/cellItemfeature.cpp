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
        _accentView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Abs(style::get("featureAccentStripHeight")->floatVal()));
        _accentView->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Below(_imageView, 0));
        addSubview(_accentView);
        
        _topic->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Top());
        _topic->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        _headline->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Abs(app->dp(32)));
        _headline->applyStyle((module->_cellsPerRow > 1) ? "featureHeadline"_S : "featureFullWidthHeadline"_S);
		_textAreaInsets = style::get("text-insets")->edgeInsetsVal("feature");
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
        _accentView->setBackgroundColor(style::get(colorName)->colorVal());
    }

};

DECLARE_DYNCREATE(BNCellItemFeature, BNCellsModule*);

