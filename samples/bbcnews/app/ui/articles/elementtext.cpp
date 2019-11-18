//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "elementtext.h"
#include "texttraits.h"
//#import "BNStyles.h"


BNElementText::BNElementText(const string& styleName) {
    _styleName = styleName;
    app->log("todo: element style fetch");
    //_style = app->getStyleValue(styleName);
}


attributed_string BNElementText::toAttributedString(COLOR textColor, EDGEINSETS padding) {
	string text = _text;
	text.append('\n');
	
	// Create basic string with just the font
    attributed_string str(text);
    str.setAttribute(attributed_string::forecolor(textColor), 0, -1);
    app->log("todo: element attributes");
    /*str.setAttribute(Attribute::font(_style->font), 0, -1);

	// Apply text traits (the links, listitem markers etc)
	for (NSUInteger i = 0; i < self.traits.count; i++) {
		BNTextTrait *trait = self.traits[i];
		[trait applyToString:string];
	}
	
	// Apply paragraph style
	NSMutableParagraphStyle* paragraphStyle = [[NSMutableParagraphStyle alloc] init];
	paragraphStyle.lineSpacing = self.style.lineHeight - self.style.baseFont.lineHeight;
	paragraphStyle.firstLineHeadIndent = self.marginLeft + padding.left;
	paragraphStyle.headIndent = self.marginLeft + padding.left;
	paragraphStyle.tailIndent = -(self.marginRight + padding.right);
	paragraphStyle.paragraphSpacingBefore = MAX(0.f, self.marginTop - [self.style topAdjustment]);
	paragraphStyle.paragraphSpacing = self.marginBottom - [self.style bottomAdjustment];
	if (self.headIndentNumChars > 0) {
		paragraphStyle.headIndent = paragraphStyle.firstLineHeadIndent + [[[BNStyles applyDynamicTextSize:string] attributedSubstringFromRange:NSMakeRange(0, self.headIndentNumChars)] size].width;
	}
	[string addAttribute:NSParagraphStyleAttributeName value:paragraphStyle range:NSMakeRange(0, string.length)];
	*/
	return str;
}


void BNElementText::trimTrailingWhitespace() {
    _text.trim();
}
