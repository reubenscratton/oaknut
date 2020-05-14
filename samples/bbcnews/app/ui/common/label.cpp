//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "label.h"
//#import "BNStyles.h"
//#import "BNLayoutManager.h"


/*@interface BNLabel ()
@property (nonatomic) NSLayoutManager *layoutManager;
@property (nonatomic) NSTextContainer *textContainer;
@property (nonatomic) NSTextStorage *textStorage;
@end
*/

/*
- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        [self commonInit];
    }
    return self;
}


- (void)commonInit {
	
	// Create TextKit objects
	self.textContainer = [[NSTextContainer alloc] init];
	self.textContainer.lineFragmentPadding = 0;
	self.textContainer.maximumNumberOfLines = 0;//self.numberOfLines;
	self.textContainer.lineBreakMode = NSLineBreakByTruncatingTail; // only affects last line of multiline AFAICS
	self.textContainer.size = self.frame.size;
	self.layoutManager = [[BNLayoutManager alloc] init];
	self.layoutManager.delegate = self;
	[self.layoutManager addTextContainer:self.textContainer];
	[self.textContainer setLayoutManager:self.layoutManager];
	self.textStorage = [[NSTextStorage alloc] init];
	[self.textStorage addLayoutManager:self.layoutManager];
	[self.layoutManager setTextStorage:self.textStorage];
}


- (NSAttributedString*)attributedText {
	return self.textStorage;
}
- (void)setAttributedText:(NSAttributedString *)attributedText {
	[self.textStorage setAttributedString:attributedText];
}



- (void)setFrame:(CGRect)frame {
    [super setFrame:frame];
	self.textContainer.size = frame.size;
}


-(void)drawRect:(CGRect)rect {

	rect.origin.y += self.paragraphSpacingBefore;
	
    // Drawing code
    NSRange glyphRange = [self.layoutManager glyphRangeForTextContainer:self.textContainer];
    //[self.layoutManager drawBackgroundForGlyphRange:glyphRange atPoint:rect.origin];
    [self.layoutManager drawGlyphsForGlyphRange:glyphRange atPoint:rect.origin];
	
	// Useful for debugging text exclusion rects
	//
	//UIBezierPath* p = [self.textContainer.exclusionPaths firstObject];
	//if (p) {
	//	CGContextRef r = UIGraphicsGetCurrentContext();
	//	CGContextAddPath(r, p.CGPath);
	//	CGContextSetStrokeColorWithColor(r,[UIColor redColor].CGColor);
	//	CGContextStrokePath(r);
	//}
}



- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	CGPoint touchLocation = [[touches anyObject] locationInView:self];
	BNLayoutManager* layoutManager = ((BNLayoutManager*)self.layoutManager);
	if (![layoutManager touchesBegan:touchLocation textContainer:self.textContainer]) {
		[super touchesBegan:touches withEvent:event];
	} else {
		[self setNeedsDisplay];
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	BNLayoutManager* layoutManager = ((BNLayoutManager*)self.layoutManager);
	[layoutManager touchesCancelled];
	[super touchesCancelled:touches withEvent:event];
	[self setNeedsDisplay];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	BNLayoutManager* layoutManager = ((BNLayoutManager*)self.layoutManager);
	[layoutManager touchesEnded];
	[super touchesEnded:touches withEvent:event];
	[self setNeedsDisplay];
}

- (BOOL)isAccessibilityElement {
    return YES;
}

- (NSString *)accessibilityIdentifier {
    return @"bn-label";
}

- (NSString *)accessibilityLabel {
    return self.attributedText.string;
}

*/


void BNLabel::setBottomRightExclusionSize(const SIZE& size) {
    RECT exclusionRect = getOwnRect(); // padded?
    exclusionRect.origin.x = exclusionRect.right() - size.width;
    exclusionRect.origin.y = exclusionRect.bottom() - size.height;
    exclusionRect.size = size;
    log("TODO: implement text layout exclusion paths");
    //self.textContainer.exclusionPaths = @[[UIBezierPath bezierPathWithRect:exclusionRect]];
}




static attributed_string s_videoPrefixString;
static attributed_string s_audioPrefixString;



/*+ (void)load {
 UIFont* font = [UIFont fontWithName:@"bbc-news-beta" size:IS_IPAD?16:11];
 s_videoPrefixString = [[NSAttributedString alloc] initWithString:@" " attributes:@{NSFontAttributeName:font}];
 s_audioPrefixString = [[NSAttributedString alloc] initWithString:@" " attributes:@{NSFontAttributeName:font}];
 }*/




/*    if (mediaGlyph != kNone) {
        
        log("TODO!");
        _attributedString.insert(0, " ");
        Font* bbcNewsFont = Font::get("bbc-news", app->dp(11));
        _attributedString.setAttribute(Attribute::font(bbcNewsFont), 0, 1);
  */
        /*
         NSDictionary* extraAttrs = @{
         NSForegroundColorAttributeName: attrs[NSForegroundColorAttributeName],
         NSParagraphStyleAttributeName: attrs[NSParagraphStyleAttributeName]
         };
         NSMutableAttributedString* s = self.attributedString.mutableCopy;
         [s insertAttributedString:((mediaGlyph==kAudio) ? s_audioPrefixString : s_videoPrefixString) atIndex:0];
         [s addAttributes:extraAttrs range:NSMakeRange(0,2)];
         self.attributedString = s;*/



/*- (void)setAttributedString:(NSAttributedString*)attributedString {
 if (self.useDynamicText) {
 attributedString = [BNStyles applyDynamicTextSize:attributedString];
 }
 _attributedString = attributedString;
 
 NSUInteger length = attributedString.length;
 self.paragraphSpacing = 0;
 self.paragraphSpacingBefore = 0;
 
 
 // Get spacing at top and bottom which boundingRectEtc forgets to apply
 if (length > 0) {
 NSParagraphStyle* styleAtTop = [attributedString attribute:NSParagraphStyleAttributeName atIndex:0 effectiveRange:nil];
 self.paragraphSpacingBefore = styleAtTop.paragraphSpacingBefore;
 NSParagraphStyle* styleAtBottom = [attributedString attribute:NSParagraphStyleAttributeName atIndex:length-1 effectiveRange:nil];
 self.paragraphSpacing = styleAtBottom.paragraphSpacing;
 
 // Line height
 UIFont* font = [attributedString attribute:NSFontAttributeName atIndex:length-1 effectiveRange:nil];
 if (font) {
 self.lineHeight = font.lineHeight;
 }
 }
 }*/


/*
void BNLabel::createButton(View* superview) {
    assert(!_label); // ooops
    
    Button* button = new Button();
    button->setRect(_bounds);
    button->setText(_attributedString);
    
    log("TODO: finish createButton()");
*/    //NSMutableAttributedString *selectedTitle = [[NSMutableAttributedString alloc] initWithAttributedString:self.attributedString];
    //[selectedTitle addAttribute:NSForegroundColorAttributeName value:[UIColor darkGrayColor] range:NSMakeRange(0, self.attributedString.length)];
    //[button setAttributedTitle:selectedTitle forState:UIControlStateHighlighted];
    
    /*button.titleLabel.numberOfLines = self.numLines;
     button.titleLabel.lineBreakMode = NSLineBreakByTruncatingTail;
     button.backgroundColor = superview.backgroundColor;
     button.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
     
     UIImage *chevronImage = [UIImage imageNamed:@"forwardChevron"];
     [button setImage:chevronImage forState:UIControlStateNormal];
     
     CGSize imageSize = button.imageView.frame.size;
     
     button.titleEdgeInsets = UIEdgeInsetsMake(0, -button.imageView.frame.size.width, 0, button.imageView.frame.size.width);
     button.imageEdgeInsets = UIEdgeInsetsMake((imageSize.height/2)-2 , button.titleLabel.frame.size.width, 0, -button.titleLabel.frame.size.width);
     */
/*
    _label = button;
    superview->addSubview(button);
}*/

/*
void BNLabelInfo::adjustFrame(POINT originDelta) {
    _bounds.origin += originDelta;
    if (_label) {
        RECT rect = _label->getRect();
        rect.origin += originDelta;
        _label->setRect(rect);
    }
}*/

bool BNLabel::applySingleStyle(const string& name, const style& value) {
    if (name=="forecolor-inv") {
        //setTextColor(value.colorVal());
        return true;
    }
    return Label::applySingleStyle(name, value);
}


bool BNLabel::getUseDynamicText() const {
    return _useDynamicText;
}

void BNLabel::setUseDynamicText(bool useDynamicText) {
    if (_useDynamicText != useDynamicText) {
        _useDynamicText = useDynamicText;
        if (useDynamicText) {
            log("TODO: implement setUseDynamicText properly");
            //[self setAttributedString:self.attributedString];
        }
    }
}



