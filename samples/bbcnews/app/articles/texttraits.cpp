//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "texttraits.h"


BNTextTrait::BNTextTrait(Type type, uint32_t start) {
    _type = type;
	_start = _end = start;
}
BNTextTrait::BNTextTrait(Type type, uint32_t start, uint32_t end) {
    _type = type;
    _start = start;
    _end = end;
}


void BNTextTrait::applyToString(AttributedString* string) {
    switch (_type) {
        case Bold:
            app->log("todo: bold trait");
            /*UIFont *font = [string attribute:NSFontAttributeName atIndex:self.start effectiveRange:NULL];
             if (font != nil) {
             font = [UIFont fontWithDescriptor:[[font fontDescriptor] fontDescriptorWithSymbolicTraits:UIFontDescriptorTraitBold] size:0];
             [string addAttribute:NSFontAttributeName value:font range:NSMakeRange(self.start, self.end-self.start)];
             }*/
            break;
        case Italic:
            app->log("todo: italic trait");
            /*UIFont *font = [string attribute:NSFontAttributeName atIndex:self.start effectiveRange:NULL];
             if (font != nil) {
             font = [UIFont fontWithDescriptor:[[font fontDescriptor] fontDescriptorWithSymbolicTraits:UIFontDescriptorTraitItalic] size:0];
             [string addAttribute:NSFontAttributeName value:font range:NSMakeRange(self.start, self.end-self.start)];
             }*/
            break;
        case Forecolor:
            app->log("todo: forecolor trait");
            //    [string addAttribute:NSForegroundColorAttributeName value:self.forecolor range:NSMakeRange(self.start, self.end-self.start)];
            break;
        case FontScale:
            app->log("todo: font scale trait");
            /*
             UIFont *font = [string attribute:NSFontAttributeName atIndex:self.start effectiveRange:NULL];
             if (font != nil) {
             font = [UIFont fontWithDescriptor:font.fontDescriptor size:font.pointSize * self.scale];
             [string addAttribute:NSFontAttributeName value:font range:NSMakeRange(self.start, self.end-self.start)];
             }*/
            break;
        case BaselineOffset:
            app->log("todo: baseline trait");
            /*
             UIFont *font = [string attribute:NSFontAttributeName atIndex:self.start effectiveRange:NULL];
             if (font != nil) {
             [string addAttribute:NSBaselineOffsetAttributeName value:@(font.lineHeight * self.distance) range:NSMakeRange(self.start, self.end-self.start)];
             }*/
            break;
        case Link:
            app->log("todo: link trait");
            
            /*    [super applyToString:string];
             NSRange range = NSMakeRange(self.start, self.end-self.start);
             [string addAttribute:BNLinkAttributeName value:self range:range];
             //[string addAttribute:NSForegroundColorAttributeName value:[UIColor blackColor] range:range];
             [string addAttribute:NSUnderlineStyleAttributeName value:@1 range:range];
             */
            break;
    }
}




