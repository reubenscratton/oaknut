//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "module.h"

class BNContainerModule : public BNModule {
public:
    
    View* _view;
    
    vector<BNModule*> _modules;
    float _topInset;
    string _displayTitle;
    

    BNContainerModule(const variant& json);

    // Cloning
    BNContainerModule(BNContainerModule* source);
    BNModule* clone() override;
    
    //virtual RECT layoutModules(vector<BNModule*> modules, RECT containingRect);
    //virtual RECT boundsAfter(RECT frame);
    //virtual void updateSubviewsForModules(vector<BNModule*> modules, View* superview);

    void updateLayoutWithContentObject(BNContent* contentObject) override;
    void addToView(View* parent) override;
    virtual View* createView();
    //void layoutWithContainingRect(const RECT& containingRect) override;
    //void updateSubviews(View* superview) override;
    //void removeAllViews() override;
    void setIsOnScreen(bool isOnScreen) override;
    void onIsInitialContent() override;
    void addItemsToArray(vector<BNBaseModel*>& array) override;
    void onSuperviewDidAppear(bool viewControllerIsMovingToParent) override;
    void onSuperviewDidDisappear(bool viewControllerIsMovingFromParent) override;
    void cleanup() override;

    bool isContainer() override { return true; }
    virtual bool isScrollableContainer() { return false; }

};
