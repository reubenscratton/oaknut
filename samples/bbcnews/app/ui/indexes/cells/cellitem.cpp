//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellitem.h"
#include "../../common/cellsmodule.h"
#include "../../../model/_module.h"
//#import "BNTopicLabelInfo.h"
//#import "BNOfflineManager.h"
//#import "BNContainerModule.h"
//#import "BNHorizontalStack.h"
#include "../../../app.h"



BNCellItem::BNCellItem(BNCellsModule* module, BNCellStyle cellStyle) : BNCellContent(module) {
    _imageView = new BNImageView();
    addSubview(_imageView);
    _textFrame = new View();
    addSubview(_textFrame);
    _headline = new BNLabel();
    _textFrame->addSubview(_headline);
    int numLines = _module->_json.intVal("lines");
    if (numLines) {
        _headline->setMinLines(numLines);
        _headline->setMaxLines(numLines);
    }
    _headline->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
    _showMediaGlyphInHeadline = _module->_showMediaGlyphInHeadline;
    _imageAspect = _module->_json.floatVal("imageAspect");
    if (_imageAspect<=0.) {
        _imageAspect = 9.f/16.f;
    }
    /*if (0 && _module->_summaries) {
        _summary = new BNLabel();
        _summary->applyStyle("summaryText");
        _summary->_useFullWidth = true;
        _textFrame->addSubview(_summary);
    }*/
    
    bool hideTopics = _module->_hideTopics;
    
    if (cellStyle == BNCellStyle::TopStory) {
        if (((BBCNewsApp*)app)->_compactMode && _summary) {
            _hideTimestamp = true;
            hideTopics = true;
        }
    }
    if (cellStyle == BNCellStyle::Feature || cellStyle == BNCellStyle::Digest) {
        _hideTimestamp = true;
    }

    if (!hideTopics) {
        bool isLong = _module->_json.boolVal("long_timestamp");
        _topic = new BNTopicLabel(_module->_maxTopics, _inverseColorScheme,
                                  _hideTimestamp, isLong);
        _topic->_useFullWidth = true;
        _textFrame->addSubview(_topic);
    }

    auto textInsetsStyle = app->getStyle("text-insets");
    if (module->_H == 1) {
        _headline->applyStyle("H1");
        _textAreaInsets = textInsetsStyle->edgeInsetsVal("top-story");
    } else if (module->_H == 2) {
        _headline->applyStyle("H2");
        _textAreaInsets = textInsetsStyle->edgeInsetsVal("secondary-top-story");
        _headline->setMaxLines(_summary ? 0 : 3);
    } else {
        _headline->applyStyle("H3");
        _textAreaInsets = textInsetsStyle->edgeInsetsVal("secondary-top-story");
        _headline->setMaxLines(_summary ? 0 : 3);
    }
    
    /*if (module->_textPadding != EDGEINSETS_Zero) {
        _textAreaInsets = module->_textPadding;
    }*/
    
    setPadding(_module->_cellPadding);
    
    
    if (cellStyle == BNCellStyle::Digest) {
        _imageView->setLayoutSize(MEASURESPEC::Abs(144), MEASURESPEC::Abs(81));
        _showMediaGlyphInHeadline = false;
        hideTopics = true;
        //_headline->_numLines = _module->_json.intVal("numLines");
        _inverseColorScheme = false;
    }
    
    // Choose general orientation
    enum BNCellOrientation {
        Portrait,
        Landscape,
        LandscapeWithFullWidthHeadline
    };
    BNCellOrientation orientation;
    string strOrientation = module->_json.stringVal("orientation");
    if (strOrientation == "portrait") {
        orientation = Portrait;
    } else if (strOrientation == "landscape") {
        orientation = Landscape;
    } else if (strOrientation == "landscapeWithFullWidthHeadline") {
        orientation = LandscapeWithFullWidthHeadline;
    } else {
        if (cellStyle == BNCellStyle::Feature) {
            orientation = Portrait;
        } else {
            orientation = (module->_H == 1) ? Portrait : Landscape;
        }
    }
    
    
    
    if (orientation == LandscapeWithFullWidthHeadline) {
        setPadding(_textAreaInsets);
        _headline->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        /*_imageOrigin.x = _textAreaInsets.left;
         _imageOrigin.y = _textAreaInsets.top + _headline->_bounds.size.height + _textAreaInsets.bottom;
         
         // Image fills left half (actually slightly less than half)
         float imageWidth = 0;
         if (_module->_imageWidthSpec <= 1) {
         imageWidth = (rect.size.width * _module->_imageWidthSpec) - _textAreaInsets.left/2;
         } else {
         imageWidth = _module->_imageWidthSpec;
         }
         float imageHeight = ceilf(imageWidth * 9.f/16.f);
         _imageSize = {imageWidth, imageHeight};
         
         
         float textLeft = _imageOrigin.x + imageWidth + _textAreaInsets.left;
         float textWidth = rect.size.width - textLeft;
         _topic->measureForWidth(textWidth, {0,0});
         RECT topicRect = _topic->_bounds;
         float bottom = _imageOrigin.y + _imageSize.height;
         if (_showSummary) {
         _summary->measureForWidth(textWidth, {textLeft, _imageOrigin.y - 6});
         bottom = MAX(bottom, _summary->_bounds.bottom() + (topicRect.size.height-6));
         }
         topicRect.origin = {textLeft - 5, bottom - (topicRect.size.height-6)};
         _topic->_bounds = topicRect;
         rect.size.height = bottom;
         rect = rect.copyWithUninsets(_textAreaInsets);
         _frame.size = rect.size;*/
    } else if (orientation == Landscape) {
        
        //_imageOrigin = {_module->_cellPadding.left, _module->_cellPadding.top};
        if (app->_defaultDisplay->sizeClass() == Display::Tablet) {
            if (_module->_imageWidthSpec <= 1) {
                _imageView->setLayoutSize({MEASURESPEC::TypeRelative,NULL,_module->_imageWidthSpec,  - _textAreaInsets.left/4}, MEASURESPEC::Aspect(9.f/16.f));
            } else {
                _imageView->setLayoutSize({MEASURESPEC::TypeRelative,NULL,_module->_imageWidthSpec,0}, MEASURESPEC::Aspect(9.f/16.f));
            }
            _imageView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Aspect(9.f/16.f));
            
        } else {
            _imageView->setLayoutSize(MEASURESPEC::Abs(app->dp(144)), MEASURESPEC::Abs(app->dp(81)));
        }
        
        _textFrame->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Match(_imageView));
        _textFrame->setLayoutOrigin(ALIGNSPEC::ToRightOf(_imageView, 0),ALIGNSPEC::Top());
        
        // Image fills left half (actually slightly less than half)
        
        
        // Headline first
        _headline->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        _headline->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Top());
        
        // Summary beneath headline
        /*if (_showSummary) {
         _summary->measureForWidth(textFrame.size.width, {textFrame.origin.x, bottom});
         bottom = _summary->_bounds.bottom();
         }*/
        
        // Topic at bottom
        if (_topic) {
            _topic->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Bottom());
        }
        /*
         // If bottom of topic is higher than bottom of image, shift topic down so they're aligned
         float dy = _imageSize.height - bottom;
         if (dy > 0) {
         _topic->_bounds.origin.y += dy;
         bottom += dy;
         } else {
         bottom += dy;
         
         // Force the summary to ellipsize rather than extend below the image
         if (_showSummary) {
         _summary->_bounds.size.height += dy;
         }
         }
         
         // Overall rect is original rect with image height
         rect.size.height = bottom;
         _frame.size = rect.size;*/
    } else {
        
        _imageView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Aspect(9.f/16.f));
        _textFrame->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        _textFrame->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Below(_imageView, 0));
        
        _headline->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Top());
        if (_summary) {
            _summary->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Below(_headline, 0));
        }
        if (_topic) {
            _topic->setLayoutOrigin(ALIGNSPEC::Left(), ALIGNSPEC::Below(_headline, 0));
        }
        
    }

}

void BNCellItem::setRelationship(BNRelationship* relationship) {
    if (relationship->_childObject->isItem()) {
        setItem((BNItem*)relationship->_childObject);
    }
}

void BNCellItem::setItem(BNItem* item) {
    _item = item;


    _textFrame->setPadding(_textAreaInsets);

    if (_item->_nameOverride.length()) {
        _headline->setText(_item->_nameOverride);
    } else if (_item->_shortName.length()) {
        _headline->setText(_item->_shortName);
    } else {
        _headline->setText(_item->_name);
    }

    _inverseColorScheme = _item->isMediaItem();
    setBackgroundColor(app->getStyleColor(_inverseColorScheme ? "color.contentBackgroundInv"_S:"color.contentBackground"_S));

    _headline->setTextColor(app->getStyleColor(_inverseColorScheme ? "color.contentForegroundInv"_S:"color.contentForeground"_S));
    
    if (_summary) {
        if (_item->_summaryOverride.length()) {
            _summary->setText(_item->_summaryOverride);
        } else {
            _summary->setText(_item->_summary);
        }
    }

    BNLabel::BNMediaGlyph mediaGlyph = BNLabel::kNone;
    if (_showMediaGlyphInHeadline && _item->isMediaItem()) {
        mediaGlyph = _item->isAudio() ? BNLabel::kAudio : BNLabel::kVideo;
    }
    
    //todo: set mediaGlyph);
    _headline->_useFullWidth = true;
    
    if (_topic) {
        // bool isLong = _module->_json.boolVal("long_timestamp");
        _topic->_useFullWidth = true;
        _topic->setItem(_item, _module->_contentObject.as<BNCollection>());
    }
    
    
    BNImage* image = _item->getIndexImage();
    if (!image) {
        image = _item->getPrimaryImage();
    }
    _imageView->setBNImage(image);
    
    if (_isDummy) {
        setBackgroundColor(app->getStyleColor("color.contentBackground"));
        //_dummyLabel->createLabel(_view);
    }

}

/*
void BNCellItem::deleteView() {

    if (_reqPrefetch) {
        _reqPrefetch->cancel();
        _reqPrefetch = NULL;
    }
}
*/

BNCollection* BNCellItem::getOwningCollection() {
    sp<BNBaseModel> col = _item->findChildObject(BNModelTypeCollection, BNModelTypeItem);
    if (col) {
        return col.as<BNCollection>();
    }
    return _item->getHomedCollection();
}

void BNCellItem::onTapped() {
    BNNavController* navController = (BNNavController*)app->_window->_rootViewController->_navigationController;
	
	// Dummy items open the associated collection
	if (_isDummy) {
		BNCollection* col = getOwningCollection();
		if (col) {
			navController->openItem(col, {}, "");
		}
		return;
	}

	// Get a ref. to the layout's root module
	BNContainerModule* rootModule = _module->_container;
	while (rootModule->_container) {
		rootModule = rootModule->_container;
	}
	string displayTitle = rootModule->_displayTitle;

	// Build an array of all the cells in the layout
	vector<BNBaseModel*> allItems;

	// Are we an AV carousel?
	if (_module->isCellsModule()) {
		BNCellsModule* cellsModule = (BNCellsModule*)_module;
		if (cellsModule->_cellClass == "BNCellItemAV") {
			cellsModule->addItemsToArray(allItems);
		}
	}
	
	// This module is *not* an AV carousel, therefore we want all cells *except* the carousel's.
	if (!allItems.size()) {
        vector<BNModule*> modules = {rootModule};
		while (modules.size()) {
			BNModule* module = modules[0];
			modules.erase(modules.begin());
			if (module->isCellsModule()) {
				BNCellsModule* cellsModule = (BNCellsModule*)module;
				if (cellsModule->_cellClass != "BNCellItemAV") {
					cellsModule->addItemsToArray(allItems);
				}
			}
			if (module->isContainer()) {
                BNContainerModule* container = (BNContainerModule*)module;
				modules.insert(modules.end(), container->_modules.begin(), container->_modules.end());
			}
		}
	}
	
	// If user tapped a related story, create a vaguely intelligent/relevant title
	if (isCellItemRelatedStory()) {
		displayTitle = _item->getHomedCollection()->_name;
        allItems.clear();
		allItems.push_back(_item);
	}
	
	// Remove any VIVO Live Event pages
	for (int i=0 ; i<allItems.size() ; i++) {
		BNBaseModel* item = allItems[i];
		if (item->_modelType == BNModelTypeLiveEventExt) {
            allItems.erase(allItems.begin() + i);
            i--;
		}
	}
	if (!allItems.size()) {
		return;
	}
	
	// [[BNAManager sharedInstance] setPageLoadInteraction:BNALoadInteractionValueTap referringObject:BNAReferringObjectValueFromInlineLink];
	// [[BNAManager sharedInstance] userActionEventOfType:BNAActionTypeClick name:BNAActionNameInlineLink labels:nil];
	
	// Appalling hack
	/*if ([displayTitle isEqualToString:@"By Topic"] || [displayTitle isEqualToString:@"By Time"]) {
		BNCollection* col = [self getOwningCollection];
		displayTitle = col?col.name:@"";
	}*/

	navController->openItem(_item, allItems, displayTitle);
}

void BNCellItem::setPrefetchLevel(int level) {
    if (_prefetchLevel == level) {
        return;
    }
    _prefetchLevel = level;
	if (level == 1) {
		log_info("Prefetching %s", _item->_modelId.c_str());
        _reqPrefetch = BNURLRequest::requestContent(_item->_modelId, 0, BNURLRequest::Priority::Medium);
	} else if (level == -1) {
		//NSLog(@"Unprefetching %@", _item.modelId);
        if (_reqPrefetch) {
            _reqPrefetch->cancel();
            _reqPrefetch = NULL;
        }
	}
}

//
// BNUrlRequestDelegate
//
/*- (void)onRequestLoadedObject:(BNURLRequest*)request object:(id)object isCacheData:(BOOL)isCacheData {
}
- (void)onRequestError:(BNURLRequest*)request error:(NSError*)error httpStatus:(NSInteger)httpStatus {
	
}
*/

void BNCellItem::setDummyInfo(const string& text) {
    _isDummy = true;
    _dummyLabel = new BNLabel();
    _dummyLabel->setText(text);
    _dummyLabel->applyStyle("dummyText");
}

/*
- (NSString *)accessibilityIdentifier {
    return @"bn-cell-item";
}

- (UIAccessibilityTraits)accessibilityTraits
{
    return UIAccessibilityTraitButton;
}

- (NSString*)accessibilityLabel
{
    NSMutableString* label = _headlineText.mutableCopy;
    
    if (_isDummy) {
        return _dummyLabel.string;
    }
    
    if (_item.homedCollection && !_hideTopics) {
        
        [label appendString:@". "];
        [label appendString:_item.homedCollection.name];
    }
    
    if (_item.isMediaItem) {
        if (_item.isVideo) {
            [label insertString:@". " atIndex:0];
            [label insertString:@"Video" atIndex:0];
        }
        
        if (_item.isAudio) {
            [label insertString:@". " atIndex:0];
            [label insertString:@"Audio" atIndex:0];
        }
    }
    
    if (_summary) {
        [label appendString:@". "];
        [label appendString:_summaryText];
    }
    
    
    return label;
}

*/
