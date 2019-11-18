//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "../../model/_module.h"

class BNContainerModule;
class BNModule;
class BNCell;

class IBNModuleHolder {
public:
    virtual void invalidateModuleSize(BNModule* module)=0;
};


class BNModule : public Object {
public:
    variant _json;
    vector<string> _modes;
    BNContainerModule* _container;
    IBNModuleHolder* _moduleHolder; // only non-nil on root module
    sp<BNContent> _contentObject;
    EDGEINSETS _padding;
    EDGEINSETS _textPadding;
    RECT _frame;
    
    BNModule(const variant& json);
    
    /**
     Cloning
     */
protected:
    BNModule(BNModule* source);
public:
    virtual BNModule* clone();

    /*
     Configure self and any submodules with the given content.
     */
    virtual void updateLayoutWithContentObject(BNContent* contentObject);

    
    virtual void addToView(View* superview);
    /*
     Layout module and submodules contents within the bounds provided. Implementations must
     measure and position their contents and update the bounds property. Note
     that measuring is expected to be a fast operation, i.e. no UIView creation
     or nib deserialization should be done.
     */
    //virtual void layoutWithContainingRect(const RECT& bounds);
    //virtual void extendToHeight(float height);


    virtual float getState();
    virtual void applyState(float state);
    virtual void addItemsToArray(vector<BNBaseModel*>& array);
    virtual void onIsInitialContent();
    virtual void onSuperviewDidAppear(bool viewControllerIsMovingToParent);
    virtual void onSuperviewDidDisappear(bool viewControllerIsMovingFromParent);
    virtual void cleanup();
    virtual void invalidateSize();

    bool isOnScreen() const { return _isOnScreen; }
    virtual void setIsOnScreen(bool isOnScreen) {_isOnScreen=isOnScreen;}
    
    COLOR getBackgroundColor() const;
    void setBackgroundColor(COLOR color);

    virtual bool isContainer() { return false; }
    virtual bool isCellsModule() { return false; }
    virtual bool isStaticTitleModule() { return false; }
    virtual bool isLinearLayout() { return false; }

protected:
    bool _isOnScreen;
    COLOR _backgroundColor;

    static EDGEINSETS edgeInsetsVal(const variant& v, const string& field);
};
