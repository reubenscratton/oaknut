//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "cellcontent.h"


BNCellContent::BNCellContent(BNCellsModule* module) : BNCell(module) {

    setBackgroundColor(app->getStyleColor(_inverseColorScheme ? "color.contentBackgroundInv":"color.contentBackground"));
}



void BNCellContent::onTapped() {
    
}


/*todo
 - (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
 self.highlightView = [[UIView alloc] initWithFrame:self.bounds];
 self.highlightView.backgroundColor = [UIColor colorWithWhite:0.25f alpha:0.25f];
 [self addSubview:self.highlightView];
 }
 
 - (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
 }
 
 - (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
 [self.cell onTapped];
 [self.highlightView removeFromSuperview];
 }
 
 - (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
 [self.highlightView removeFromSuperview];
 }
 
 */



/*#pragma mark - BNCellAccessibilityDelegate

- (BOOL)isAccessibilityElement
{
	return YES;
}*/

