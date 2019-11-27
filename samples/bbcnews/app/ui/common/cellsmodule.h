//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "module.h"

class BNCellsModule : public BNModule {
public:
    BNCellsModule(const variant& json);
    
    // Cloning
    BNCellsModule(BNCellsModule* source);
    BNModule* clone() override;

    virtual void insertSubview(View* superview, View* newView, int index);

    void updateLayoutWithContentObject(BNContent* contentObject) override;
    void onSuperviewDidAppear(bool viewControllerIsMovingToParent) override;
    void onSuperviewDidDisappear(bool viewControllerIsMovingFromParent) override;
    //void layoutWithContainingRect(const RECT& bounds) override;
    //void extendToHeight(float height) override;
    void setIsOnScreen(bool isOnScreen) override;
    void onIsInitialContent() override;
    void addItemsToArray(vector<BNBaseModel*>& array) override;
    
    void addToView(View* parent) override;

    bool isCellsModule() override { return true; }

    const style* _titleStyle;
    string _cellClass;
    int _limit;
    int _offset;
    vector<string> _primary;
    vector<string> _secondary;
    vector<string> _format;
    EDGEINSETS _cellMargins;
    EDGEINSETS _cellPadding;
    vector<BNCell*> _cells;
    bool _usesScrollviewOffset;
    int _cellsPerRow;
    bool _allowIncompleteRows;
    int _maxTopics;
    bool _summaries;
    bool _hideTopics;
    bool _tinyTimestamps;
    bool _showMediaGlyphInHeadline;
    float _imageWidthSpec;
    int _H; // font size, as per H1, H2, H3 notation

};
