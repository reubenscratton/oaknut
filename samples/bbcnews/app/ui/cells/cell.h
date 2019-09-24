//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "../../model/_module.h"
#include "../modules/cellsmodule.h"

//#import "BNLabelInfo.h"
//#import "BNCellAccessibilityDelegate.h"

class BNCell : public View { // <BNCellAccessibilityDelegate>
public:
    
    BNCellsModule* _module;
    int _cellIndex;
    bool _usesScrollviewOffset;
    bool _isOnScreen;
    int _prefetchLevel;
    bool _isDummy;
    EDGEINSETS _margins;
    
    BNCell(BNCellsModule* module);
    
    // Runtime type determinants
    virtual bool isCellItem() { return false; }
    
    virtual void setItem(BNItem* item);
    virtual void setRelationship(BNRelationship* rel);

    virtual void setPrefetchLevel(int prefetchLevel);

    //virtual void adviseScrollviewOffset(float offset);
    virtual void onIsInitialContent();
    virtual void onSuperviewDidAppear(bool viewControllerIsMovingToParent);
    virtual void onSuperviewDidDisappear(bool viewControllerIsMovingFromParent);
    virtual void setDummyInfo(const string& text);

protected:
    RECT _frame;

    friend class BNCellsModule;
    
    void layout(RECT constraint) override;
};
