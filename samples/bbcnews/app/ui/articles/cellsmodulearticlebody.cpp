//
//  Copyright (c) 2019 BBC News. All rights reserved.
//


#include "cellsmodulearticlebody.h"
#include "elementmedia.h"
#include "cell.h"
//#include "../common/imageview.h"
#include "cellmedia.h"

/*#import "BNElementText.h"
#import "BNElementAdvert.h"
#import "BNImage.h"
#import "BNAV.h"
#import "BNCellContentView.h"
*/

DECLARE_DYNCREATE(BNCellsModuleArticleBody, const variant&);

class BNCellArticleText : public BNCell {
public:
    
    LinearLayout* _ll;

    BNCellArticleText(BNCellsModule* module) : BNCell(module) {
        setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        _ll = new LinearLayout();
        _ll->_orientation = LinearLayout::Vertical;
        _ll->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
        addSubview(_ll);
    }

    void setItem(BNItem* item) override {
        _ll->removeAllSubviews();
        
        // Filter out the primary image (as it is usually presented above the article text)
        BNImage* primaryImage = nullptr;
        if (!_module->_json.boolVal("includePrimaryMedia")) {
            primaryImage = (BNImage*)item->findChildObject(BNModelTypeImage, BNRelationshipTypePlacementPrimary);
            if (!primaryImage) {
                primaryImage = (BNImage*)item->findChildObject(BNModelTypeVideo, BNRelationshipTypePlacementPrimary);
            }
        }
        
        auto textStyleDefault = app->getStyle("article.text.default");
        
        for (auto& elem : item->_elements) {
            if (elem.text.lengthInBytes()) {
                Label* label = new Label();
                label->applyStyle(*textStyleDefault);
                label->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
                label->setText(elem.text);
                _ll->addSubview(label);
            }
            if (elem.image && elem.image!=primaryImage) {
                /*BNImageView* imageView = new BNImageView();
                imageView->setBNImage(elem.image);
                imageView->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Aspect(elem.image->_height/(float)elem.image->_width));
                _ll->addSubview(imageView);*/
                BNCellMedia* mediaCell = new BNCellMedia(_module);
                mediaCell->setLayoutSize(MEASURESPEC::Fill(), MEASURESPEC::Wrap());
                mediaCell->setMediaObject(elem.image, item);
                _ll->addSubview(mediaCell);

            }
        }
    }

};

DECLARE_DYNCREATE(BNCellArticleText, BNCellsModule*);


BNCellsModuleArticleBody::BNCellsModuleArticleBody(const variant& json) : BNCellsModule(json) {
    _cellClass = "BNCellArticleText";
}


// Cloning
BNCellsModuleArticleBody::BNCellsModuleArticleBody(BNCellsModuleArticleBody* source) : BNCellsModule(source) {
}
BNModule* BNCellsModuleArticleBody::clone() {
    return new BNCellsModuleArticleBody(this);
}
void BNCellsModuleArticleBody::arrangeElements() {
}

void BNCellsModuleArticleBody::updateLayoutWithContentObject(BNContent* contentObject) {

	_item = (BNItem*)contentObject;
	arrangeElements();
	//UIColor* textColor = self.item.isMediaItem ? [UIColor contentForegroundInvColor] : [UIColor contentForegroundColor];
	
	
	


	/*int insertMpuAdvertAfterParagraph = _json.intVal("insertMpuAdvertAfterParagraph");
	int numParagraphs = 0;
	
    vector<BNCell*> cells;;
	//NSMutableAttributedString* text = [[NSMutableAttributedString alloc] init];
	vector<BNElement*> nontextElements;
	for (int i=0 ; i<elements.size() ; i++) {
		BNElement* element = elements[i];
		if (element->isElementText()) {
			[text appendAttributedString:[(BNElementText*)element toAttributedString:textColor padding:self.textPadding]];
			numParagraphs++;
			if (numParagraphs==insertMpuAdvertAfterParagraph) {
			//	BNElementAdvert* ad = [[BNElementAdvert alloc] initWithType:@"mpu"];
			//	ad.charIndex = text.length;
			//	[nontextElements addObject:ad];
			}
		} else {
			BNElementMedia* mediaElement = (BNElementMedia*)element;
			BNCellMedia* mediaCell = [mediaElement createMediaCell:self];
			[cells addObject:mediaCell];
			[nontextElements addObject:mediaElement];
		}
	}
	self.attributedText = text;
	self.nontextElements = nontextElements;
*/
	BNCellsModule::updateLayoutWithContentObject(contentObject);
        
    //_cells.insert(_cells.end(), cells.begin(), cells.end());

}

/*
- (void)insertSubview:(UIView*)superview newView:(UIView*)newView atIndex:(NSInteger)index {
	if ([newView isKindOfClass:[BNCellContentView class]]) {
		[superview addSubview:newView];
	} else {
		[superview insertSubview:newView atIndex:index];
	}
}

- (void)layoutWithContainingRect:(CGRect)rect {
	
	rect = UIEdgeInsetsInsetRect(rect, self.padding);

	// All media cells must be sized first so that the text cell can position
	// them and also know what it's own size is
	for (BNElementNonText* nte in self.nontextElements) {
		nte.frameOffset = rect.origin;
		CGFloat availableWidth = rect.size.width - (nte.marginLeft + nte.marginRight);
		
		// Size the media cell after applying margins
		CGRect cellFrame;
		cellFrame.origin.x = nte.marginLeft + availableWidth* nte.normalizedLeft;
		cellFrame.origin.y = nte.marginTop;
		cellFrame.size.width = (nte.normalizedWidth <=1.f) ? (availableWidth * nte.normalizedWidth) : nte.normalizedWidth;
		cellFrame.size.height = MAXFLOAT;
		[nte measureForContainingRect:cellFrame];
		
		// Take the measured cell frame and unapply the margins to get the whole nte frame
		CGRect frame = nte.contentFrame;
		if (nte.normalizedLeft > 0) { // i.e. right-aligned, halfwidth image
			frame.origin.x += (nte.marginLeft - 16);
			frame.size.width += 16+nte.marginRight;
		} else {
			frame.origin.x -= nte.marginLeft;
			frame.size.width += nte.marginLeft+nte.marginRight;
		}
		frame.origin.y = 0;
		frame.size.height += nte.marginTop+nte.marginBottom;
		nte.frame = frame;
	}

	// Size the text cell. This process includes positioning all the media cells relative
	// to the BNTextView bounds.
	BNCellArticleText* textCell = [self.cells firstObject];
	[textCell measureForContainingRect:rect];
	
	rect.size.height = textCell.frameSize.height;
	self.frame = UIEdgeInsetsUninsetRect(rect, self.padding);

}


- (void)arrangeElements {
	
	BOOL allImagesHalfWidth = [self.json[@"imagesAllHalfWidth"] boolValue];
	
	// Reset margins
	for (BNElement* element in self.item.elements) {
		if ([element isKindOfClass:[BNElementText class]]) {
			BNElementText* elementText = (BNElementText*)element;
			elementText.marginLeft = elementText.style.margins.left;
			elementText.marginTop = elementText.style.margins.top;
			elementText.marginRight = elementText.style.margins.right;
			elementText.marginBottom = elementText.style.margins.bottom;
		}
	}
	
	
	BNElement* element = nil;
	BNElement* elementPrev = nil;
	for (NSUInteger i = 0; i < self.item.elements.count; i++) {
		element = self.item.elements[i];
		BNImage* image = nil;
		BNImage* imagePrev = nil;
		
		// Top of article has no top margin
		if (i==0) {
			element.marginTop = 0;
		}
				
		// Default picture logic
		if ([element isKindOfClass:[BNElementMedia class]]) {
			BNElementMedia* elementMedia = (BNElementMedia*)element;
			if ([elementMedia isKindOfClass:[BNElementImage class]]) {
				image = ((BNElementImage*)elementMedia).image;
			} else {
				BNAV* av = (BNAV*)elementMedia.media;
				image = av.posterImage;
			}
			
			
			BOOL halfWidth = allImagesHalfWidth;
			BOOL absSize = NO;
			if (image) {
				NSString* positionHint = [image positionHint];
				if ([positionHint isEqualToString:@"full-width"] || positionHint==nil) {
					elementMedia.marginLeft = allImagesHalfWidth ? self.textPadding.left : 0;
					elementMedia.marginRight =  allImagesHalfWidth ? self.textPadding.right : 0;
				} else if ([positionHint isEqualToString:@"body-width"]) {
					elementMedia.marginLeft = self.textPadding.left;
					elementMedia.marginRight = self.textPadding.right;
				} else if ([positionHint isEqualToString:@"body-narrow-width"]) {
					elementMedia.marginLeft = self.textPadding.left;
					elementMedia.marginRight = self.textPadding.right;
					halfWidth = YES;
				} else if ([positionHint isEqualToString:@"abs"]) {
					elementMedia.disableForceFullWidth = YES;
					absSize = YES;
				}
			
				// Portrait images must be half-width, unless they have really long captions
				if (image.height > image.width && image.caption.length<80) {
					halfWidth = YES;
				}
			
			
				// Horizontal rules are never halfwidth
				BOOL isRule = [image isProbablyAHorizontalRule];
				if (isRule) {
					halfWidth = NO;
				}
				
			}
			
			if (!absSize) {
				if (halfWidth) {
					elementMedia.normalizedLeft = 0.5f;
					elementMedia.normalizedWidth = 0.5f;
				} else {
					elementMedia.normalizedLeft = 0.f;
					elementMedia.normalizedWidth = 1.f;
				}
			}
			
		}
		
		
		// Where text meets picture, double the spacing
		if ([elementPrev isKindOfClass:[BNElementText class]] && [element isKindOfClass:[BNElementMedia class]]) {
			elementPrev.marginBottom = MAX(8,elementPrev.marginBottom);
			elementPrev.marginBottom *= 2;
		}
		if ([element isKindOfClass:[BNElementText class]] && [elementPrev isKindOfClass:[BNElementMedia class]]) {
			element.marginTop *= 2;
			
			// If text follows a non-full-width picture then the two elements will have their
			// top edges aligned.
			if (elementPrev.normalizedWidth < 1.f) {
				elementPrev.marginBottom = 0.f;
				elementPrev.marginTop = 3; // TODO: This is an approximation... it should use font metrics
				element.marginTop = 0.f;
			}
		}
		
		// If picture meets picture, apply margins if one of them's a rule
		if (imagePrev && image) {
			if ([imagePrev isProbablyAHorizontalRule]) {
				elementPrev.marginBottom = 16.f;
			}
			if ([image isProbablyAHorizontalRule]) {
				element.marginTop = 16.f;
			}
		}
		elementPrev = element;
		imagePrev = image;
	}
	
	
	// Ensure trailing images are always full-width since they won't have no text to fill up the
	// empty space alongside them
	for (NSInteger i = self.item.elements.count-1; i>=0; i--) {
		element = self.item.elements[i];
		if (![element isKindOfClass:[BNElementMedia class]]) {
			break;
		}
		BNElementMedia* elementMedia = (BNElementMedia*)element;
		elementMedia.normalizedLeft = 0.f;
		elementMedia.normalizedWidth = 1.f;
		elementMedia.marginLeft = self.textPadding.left;
		elementMedia.marginRight = self.textPadding.right;
	}
	
	
	
	// Last element always has margin at the bottom
	if (elementPrev != nil) {
		elementPrev.marginBottom = 32;
	}
}


- (void)updateSubviews:(UIView *)superview {
	[super updateSubviews:superview];
	for (BNElementNonText* nte in self.nontextElements) {
		[nte updateSubviews:superview];
	}
}
*/

