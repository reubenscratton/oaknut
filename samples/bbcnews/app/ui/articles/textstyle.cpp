//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#import "textstyle.h"


float BNTextStyle::topAdjustment() {
    return _baseFont->_leading;// self.baseFont.lineHeight - (self.baseFont.capHeight-self.baseFont.descender);
}

float BNTextStyle::bottomAdjustment() {
    return _baseFont->_height - _baseFont->_ascent;// self.baseFont.lineHeight - self.baseFont.ascender;
}

/*
+ (instancetype)styleWithFontName:(NSString*)fontName size:(CGFloat)size lineHeight:(CGFloat)lineHeight traits:(UIFontDescriptorSymbolicTraits)traits margins:(UIEdgeInsets)margins {
	
	BNTextStyle *style = [[self class] new];
	
	
	style.baseFont = [UIFont fontWithName:fontName size:size];
	if (traits != 0) {
		style.baseFont = [UIFont fontWithDescriptor:[style.baseFont.fontDescriptor fontDescriptorWithSymbolicTraits:traits] size:0];
	}
	style.lineHeight = lineHeight;
	style.margins = margins;
	
	return style;
}*/

