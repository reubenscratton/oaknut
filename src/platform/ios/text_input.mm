//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_IOS

#import "app_native/AppDelegate.h"

//#define NSLOG NSLog
#define NSLOG(x,...)

@interface OaknutView (TextInput) <UITextInput, UITextInputTraits>
@end

@interface SimpleTextPos : UITextPosition <NSCopying>
@property NSInteger pos;
@end
@implementation SimpleTextPos
+(instancetype)pos:(NSInteger)pos {
    SimpleTextPos* simplePos = [SimpleTextPos new];
    simplePos.pos = pos;
    return simplePos;
}
- (id)copyWithZone:(nullable NSZone *)zone {
    SimpleTextPos* copy = [[SimpleTextPos allocWithZone:zone] init];
    copy.pos = self.pos;
    return copy;
}

@end

static UITextInputStringTokenizer* _tokenizer;
static id<UITextInputDelegate> _textInputDelegate;

@interface SimpleTextRange : UITextRange <NSCopying> {
@public SimpleTextPos *_start;
@public SimpleTextPos *_end;
}
@end
@implementation SimpleTextRange
- (UITextPosition*)start {
    return _start;
}
- (UITextPosition*)end {
    return _end;
}
- (id)copyWithZone:(nullable NSZone *)zone {
    SimpleTextRange* copy = [[SimpleTextRange allocWithZone:zone] init];
    copy->_start = self.start.copy;
    copy->_end = self.end.copy;
    return copy;
}
@end

void App::keyboardShow(bool show) {
    OaknutView* view = (OaknutView*)[UIApplication sharedApplication].delegate.window.rootViewController.view;
    if (show) {
        [view becomeFirstResponder];
    } else {
        [view resignFirstResponder];
    }
}

void App::keyboardNotifyTextChanged() {
    OaknutView* view = (OaknutView*)[UIApplication sharedApplication].delegate.window.rootViewController.view;
    if (_textInputDelegate) {
        [_textInputDelegate textWillChange: view];
        [_textInputDelegate textDidChange: view];
    }
}


@implementation OaknutView (TextInput)

- (UITextRange*)markedTextRange {
    NSLOG(@"markedTextRange");
    /*SimpleTextRange* range = [SimpleTextRange new];
    range->_start = [SimpleTextPos pos:app._window->_keyboardHandler->getSelectionStart()];
    range->_end = [SimpleTextPos pos:app._window->_keyboardHandler->getInsertionPoint()];
    NSLOG(@"markedTextRange -> %d:%d", range->_start.pos, range->_end.pos);
    return range;*/
    return nil;
}
- (NSDictionary*)markedTextStyle {
    NSLOG(@"markedTextStyle");
    return nil;
}
- (void)setMarkedTextStyle:(NSDictionary*)markedTextStyle {    
    NSLOG(@"setMarkedTextStyle %@", markedTextStyle);
}

- (id<UITextInputDelegate>)inputDelegate {
    NSLOG(@"inputDelegate");
    return _textInputDelegate;
}
- (void)setInputDelegate:(id<UITextInputDelegate>)inputDelegate {
    NSLOG(@"setInputDelegate");
    _textInputDelegate = inputDelegate;
}

- (BOOL)hasText {
    NSLOG(@"hasText");
    return app._window->_keyboardHandler->getTextLength() > 0;
}
- (void)insertText:(NSString *)text {
    NSLOG(@"insertText %@", text);
    [_textInputDelegate textWillChange:self];
    const char* cstr = [text cStringUsingEncoding:NSUTF8StringEncoding];
    int selStart = app._window->_keyboardHandler->getSelectionStart();
    int selEnd = app._window->_keyboardHandler->getInsertionPoint();
    app._window->_keyboardHandler->insertText(cstr, selStart, selEnd);
    selStart += text.length;
    app._window->_keyboardHandler->setSelectedRange(selStart, selStart);
    [_textInputDelegate textDidChange:self];
}
- (void)deleteBackward {
    NSLOG(@"deleteBackward");
    [_textInputDelegate textWillChange:self];
    app._window->_keyboardHandler->deleteBackward();
    [_textInputDelegate textDidChange:self];
}

- (UIReturnKeyType)returnKeyType {
    NSLOG(@"returnKeyType");
    return UIReturnKeySearch;
}

- (UITextPosition*)beginningOfDocument {
    NSLOG(@"beginningOfDocument -> 0");
    return [SimpleTextPos pos:0];
}

- (UITextPosition*)endOfDocument {
    NSLOG(@"endOfDocument -> %d", app._window->_keyboardHandler->getTextLength());
    return [SimpleTextPos pos:app._window->_keyboardHandler->getTextLength()];
}

- (id<UITextInputTokenizer>)tokenizer {
    NSLOG(@"tokenizer");
    if (!_tokenizer) {
        _tokenizer = [[UITextInputStringTokenizer alloc] initWithTextInput:self];
    }
    return _tokenizer;
}

- (UITextRange*)selectedTextRange {
    SimpleTextRange* range = [SimpleTextRange new];
    range->_start = [SimpleTextPos pos:app._window->_keyboardHandler->getSelectionStart()];
    range->_end = [SimpleTextPos pos:app._window->_keyboardHandler->getInsertionPoint()];
    NSLOG(@"selectedTextRange -> %d:%d", range->_start.pos, range->_end.pos);
    return range;
}
- (void)setSelectedTextRange:(UITextRange *)selectedTextRange {
    int start = (int)((SimpleTextPos*)selectedTextRange.start).pos;
    int end = (int)((SimpleTextPos*)selectedTextRange.end).pos;
    NSLOG(@"setSelectedTextRange start=%d end=%d", start, end);
    app._window->_keyboardHandler->setSelectedRange(start, end);
}

- (UITextWritingDirection)baseWritingDirectionForPosition:(nonnull UITextPosition *)position inDirection:(UITextStorageDirection)direction {
    NSLOG(@"baseWritingDirectionForPosition");
    return UITextWritingDirectionNatural;
}

- (CGRect)caretRectForPosition:(nonnull UITextPosition *)position {
    NSLOG(@"caretRectForPosition");
    return CGRectMake(0,0,10,10);
}

- (nullable UITextRange *)characterRangeAtPoint:(CGPoint)point {
    NSLOG(@"characterRangeAtPoint");
    return nil;
}

- (nullable UITextRange *)characterRangeByExtendingPosition:(nonnull UITextPosition *)position inDirection:(UITextLayoutDirection)direction {
    NSLOG(@"characterRangeByExtendingPosition");
    return nil;
}

- (nullable UITextPosition *)closestPositionToPoint:(CGPoint)point {
    NSLOG(@"closestPositionToPoint");
    return nil;
}

- (nullable UITextPosition *)closestPositionToPoint:(CGPoint)point withinRange:(nonnull UITextRange *)range {
    NSLOG(@"closestPositionToPoint");
    return nil;
}

- (NSComparisonResult)comparePosition:(nonnull UITextPosition *)position toPosition:(nonnull UITextPosition *)other {
    SimpleTextPos* pos1 = (SimpleTextPos*)position;
    SimpleTextPos* pos2 = (SimpleTextPos*)other;
    NSLOG(@"comparePosition %d %d", pos1.pos, pos2.pos);
    NSInteger diff = pos1.pos - pos2.pos;
    if (diff < 0) return NSOrderedAscending;
    if (diff > 0) return NSOrderedDescending;
    return NSOrderedSame;
}

- (CGRect)firstRectForRange:(nonnull UITextRange *)range {
    SimpleTextRange* __unused r = (SimpleTextRange*)range;
    NSLOG(@"firstRectForRange %d %d", ((SimpleTextPos*)r.start).pos, ((SimpleTextPos*)r.end).pos);
    return CGRectNull;
}

- (NSInteger)offsetFromPosition:(nonnull UITextPosition *)from toPosition:(nonnull UITextPosition *)toPosition {
    SimpleTextPos *f = (SimpleTextPos *)from;
    SimpleTextPos *t = (SimpleTextPos *)toPosition;
    NSLOG(@"offsetFromPosition %d %d", (int)f.pos, (int)t.pos);
    return (t.pos - f.pos);
}

- (nullable UITextPosition *)positionFromPosition:(nonnull UITextPosition *)position inDirection:(UITextLayoutDirection)direction offset:(NSInteger)offset {
    NSInteger p = ((SimpleTextPos*)position).pos;
    NSLOG(@"positionFromPosition %d dir=%d offset=%d", (int)p, (int)direction, (int)offset);
    switch (direction) {
        case UITextLayoutDirectionLeft:
            offset = -offset;
            break;
        case UITextLayoutDirectionRight:
            break;
        default:
            assert(0); // todo: implement up & down
            break;
    }
    NSInteger end = p + offset;
    if (end > app._window->_keyboardHandler->getTextLength() || end < 0)
        return nil;
    return [SimpleTextPos pos:end];
}

- (nullable UITextPosition *)positionFromPosition:(nonnull UITextPosition *)position offset:(NSInteger)offset {
    NSInteger p = ((SimpleTextPos*)position).pos;
    NSLOG(@"positionFromPosition %d offset=%d", (int)p, (int)offset);
    NSInteger end = p + offset;
    if (end > app._window->_keyboardHandler->getTextLength() || end < 0)
        return nil;
    return [SimpleTextPos pos:end];
}

- (nullable UITextPosition *)positionWithinRange:(nonnull UITextRange *)range farthestInDirection:(UITextLayoutDirection)direction {
    NSLOG(@"positionWithinRange");
    return nil;
}

- (void)replaceRange:(nonnull UITextRange *)range withText:(nonnull NSString *)text {
    SimpleTextRange* r = (SimpleTextRange*)range;
    int selStart = app._window->_keyboardHandler->getSelectionStart();
    int insertionPoint = app._window->_keyboardHandler->getInsertionPoint();
    int selShift = 0;
    if (insertionPoint <= r->_end.pos) {
        selShift = (int)text.length - (int)(r->_end.pos-r->_start.pos);
    }
    NSLOG(@"replaceRange %d:%d %@", (int)r->_start.pos, (int)r->_end.pos, text);
    const char* cstr = [text cStringUsingEncoding:NSUTF8StringEncoding];
    [_textInputDelegate textWillChange:self];
    app._window->_keyboardHandler->insertText(cstr, (int)r->_start.pos, (int)r->_end.pos);
    if (selShift != 0) {
        app._window->_keyboardHandler->setSelectedRange(selStart+selShift, insertionPoint+selShift);
    }
    [_textInputDelegate textDidChange:self];
}

- (nonnull NSArray *)selectionRectsForRange:(nonnull UITextRange *)range {
    NSLOG(@"selectionRectsForRange");
    return nil;
}

- (void)setBaseWritingDirection:(UITextWritingDirection)writingDirection forRange:(nonnull UITextRange *)range {
    NSLOG(@"setBaseWritingDirection");
}

- (void)setMarkedText:(nullable NSString *)markedText selectedRange:(NSRange)selectedRange {
    NSLOG(@"setMarkedText");
}

- (nullable NSString *)textInRange:(nonnull UITextRange *)range {
    SimpleTextPos* posStart = (SimpleTextPos*)range.start;
    SimpleTextPos* posEnd = (SimpleTextPos*)range.end;
    string text = app._window->_keyboardHandler->textInRange((int)posStart.pos, (int)posEnd.pos);
    NSLOG(@"textInRange %d:%d -> %s", posStart.pos, posEnd.pos, text.data());
    return [NSString stringWithUTF8String:text.data()];
}

- (nullable UITextRange *)textRangeFromPosition:(nonnull UITextPosition *)fromPosition toPosition:(nonnull UITextPosition *)toPosition {
    SimpleTextRange* range = [SimpleTextRange new];
    SimpleTextPos* f = (SimpleTextPos*)fromPosition;
    SimpleTextPos* t = (SimpleTextPos*)toPosition;
    NSInteger r1 = MIN(f.pos, t.pos);
    NSInteger r2 = MAX(t.pos, f.pos);
    range->_start = [SimpleTextPos pos:r1];
    range->_end = [SimpleTextPos pos:r2];
    NSLOG(@"textRangeFromPosition -> %d:%d", (int)r1, (int)r2);
    return range;
}


- (BOOL)shouldChangeTextInRange:(UITextRange *)range replacementText:(NSString *)text {
    SimpleTextRange* __unused r = (SimpleTextRange*)range;
    NSLOG(@"shouldChangeTextInRange %d:%d \"%@\"", (int)((SimpleTextPos*)r.start).pos, (int)((SimpleTextPos*)r.end).pos, text);
    return YES;
}

- (void)unmarkText {
    NSLOG(@"unmarkText");
}

- (UITextStorageDirection)selectionAffinity {
    return UITextStorageDirectionForward;
}
@end

#endif
