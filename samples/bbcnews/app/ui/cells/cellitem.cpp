//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellitem.h"
#include "../modules/cellsmodule.h"
#include "../../model/_module.h"
//#import "BNTopicLabelInfo.h"
//#import "BNOfflineManager.h"
//#import "BNContainerModule.h"
//#import "BNHorizontalStack.h"


//@interface BNCellItem () <BNURLRequestDelegate>

BNCellItem::BNCellItem(BNCellsModule* module) : BNCellContent(module) {
    _imageView = new BNImageView();
    addSubview(_imageView);
    _textFrame = new View();
    addSubview(_textFrame);
    _headline = new BNLabel();
    _textFrame->addSubview(_headline);
    int numLines = _module->_json.intVal("numLines");
    _headline->setMaxLines(numLines);
    _headline->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
    _showMediaGlyphInHeadline = _module->_showMediaGlyphInHeadline;
    _hideTopics = _module->_hideTopics;
    _imageAspect = _module->_json.floatVal("imageAspect");
    if (_imageAspect<=0.) {
        _imageAspect = 9.f/16.f;
    }
    if (_module->_summaries) {
        _summary = new BNLabel();
        _summary->applyStyle("summaryText");
        _summary->_useFullWidth = true;
        _textFrame->addSubview(_summary);
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
    setBackgroundColor(app->getStyleColor(_inverseColorScheme ? "color.contentBackgroundInv":"color.contentBackground"));

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
    
    if (!_hideTopics) {
        bool isLong = _module->_json.boolVal("long_timestamp");
        _topic = new BNTopicLabel(_item, _module->_maxTopics, _inverseColorScheme,
                                  _hideTimestamp, _module->_contentObject.as<BNCollection>(), isLong);
        _topic->_useFullWidth = true;
        _textFrame->addSubview(_topic);
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
    app->log("TODO: BNCellItem::onTapped");
    /*
	BNNavigationController* navController = [BNNavigationController get];
	
	// Dummy items open the associated collection
	if (_isDummy) {
		BNCollection* col = [self getOwningCollection];
		if (col) {
			[navController openItem:col withinCollection:nil andTitle:nil];
		}
		return;
	}

	// Get a ref. to the layout's root module
	BNContainerModule* rootModule = _module.container;
	while (rootModule.container) {
		rootModule = rootModule.container;
	}
	NSString* displayTitle = rootModule.displayTitle;

	// Build an array of all the cells in the layout
	NSMutableArray* allCells = [NSMutableArray new];

	// Are we an AV carousel?
	if ([_module isKindOfClass:[BNCellsModule class]]) {
		BNCellsModule* cellsModule = (BNCellsModule*)_module;
		if ([cellsModule.cellClass isEqualToString:@"BNCellItemAV"]) {
			[cellsModule addItemsToArray:allCells];
		}
	}
	
	// This module is *not* an AV carousel, therefore we want all cells *except* the carousel's.
	if (!allCells.count) {
		NSMutableArray* modules = [NSMutableArray new];
		[modules addObject:rootModule];
		while (modules.count) {
			BNModule* module = modules[0];
			[modules removeObjectAtIndex:0];
			if ([module isKindOfClass:[BNCellsModule class]]) {
				BNCellsModule* cellsModule = (BNCellsModule*)module;
				if (![cellsModule.cellClass isEqualToString:@"BNCellItemAV"]) {
					[cellsModule addCellsToArray:allCells];
				}
			}
			if ([module isKindOfClass:[BNContainerModule class]]) {
				[modules addObjectsFromArray:((BNContainerModule*)module).modules];
			}
		}
	}
	
	// If user tapped a related story, create a vaguely intelligent/relevant title
	if ([NSStringFromClass(_class) isEqualToString:@"BNCellItemRelatedStory"]) {
		displayTitle = _item.homedCollection.name;
		allCells = @[_item].mutableCopy;
	}
	
	// Remove any VIVO Live Event pages
	for (int i=0 ; i<allCells.count ; i++) {
		BNBaseModel* item = allCells[i];
		if ([item.modelType isEqualToString:BNModelType.liveEventExt]) {
			[allCells removeObjectAtIndex:i--];
		}
	}
	if (!allCells.count) {
		return;
	}
	
	[[BNAManager sharedInstance] setPageLoadInteraction:BNALoadInteractionValueTap referringObject:BNAReferringObjectValueFromInlineLink];
	[[BNAManager sharedInstance] userActionEventOfType:BNAActionTypeClick name:BNAActionNameInlineLink labels:nil];
	
	// Appalling hack
	if ([displayTitle isEqualToString:@"By Topic"] || [displayTitle isEqualToString:@"By Time"]) {
		BNCollection* col = [self getOwningCollection];
		displayTitle = col?col.name:@"";
	}

	[navController openItem:_item withinCollection:allCells andTitle:displayTitle];
     */
}

void BNCellItem::setPrefetchLevel(int level) {
    if (_prefetchLevel == level) {
        return;
    }
    _prefetchLevel = level;
	if (level == 1) {
		app->log("Prefetching %s", _item->_modelId.data());
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
