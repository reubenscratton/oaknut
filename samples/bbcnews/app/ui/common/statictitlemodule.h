//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "module.h"
#include "label.h"

class BNStaticTitleModule : public BNModule {
public:
    
    BNStaticTitleModule(const variant& json);

    // Cloning
    BNStaticTitleModule(BNStaticTitleModule* source);
    BNModule* clone() override;

    bool _visible;
    sp<BNLabel> _label;
    string _contentId;
    string _text;
    COLOR _titleColor;
    const style* _style;
    int _numLines;

    //virtual void createLabelView(View* superview);
    //virtual void removeLabelView();
    virtual void onTitleTapped();
    //virtual bool showBackgroundForLinks();
    
    bool isStaticTitleModule() override { return true; }
    
protected:
    void addToView(View* parent) override;
    //void layoutWithContainingRect(const RECT& rect) override;
    //void updateSubviews(View* superview) override;
    //void removeAllViews() override;
};
