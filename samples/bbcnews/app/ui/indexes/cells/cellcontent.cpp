//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "cellcontent.h"


BNCellContent::BNCellContent(BNCellsModule* module) : BNCell(module) {

}



void BNCellContent::onTapped() {
    
}

bool BNCellContent::handleInputEvent(INPUTEVENT* event) {
    if (event->type == INPUT_EVENT_DOWN && event->deviceType != INPUTEVENT::ScrollWheel) {
        _highlightOp = new RectRenderOp();
        _highlightOp->setFillColor(0x20000000);
        _highlightOp->setRect(getOwnRect());
        addDecorOp(_highlightOp);
        setNeedsFullRedraw();
        //return true;
    }
    else if (event->type == INPUT_EVENT_UP || event->type == INPUT_EVENT_TAP_CANCEL) {
        if (_highlightOp) {
            removeDecorOp(_highlightOp);
            _highlightOp = NULL;
        }
        //return true;
    }
    else if (event->type == INPUT_EVENT_TAP) {
        onTapped();
        return true;
    }
    return BNCell::handleInputEvent(event);
}


void BNCellContent::detachFromSurface() {
    if (_highlightOp) {
        removeDecorOp(_highlightOp);
        _highlightOp = NULL;
    }
    BNCell::detachFromSurface();
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

