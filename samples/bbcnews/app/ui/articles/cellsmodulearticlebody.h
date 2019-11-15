//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>
#include "../../ui/common/cellsmodule.h"
#include "../../model/_module.h"

class BNCellsModuleArticleBody : public BNCellsModule {
public:
    BNCellsModuleArticleBody(const variant& json);
    
    // Cloning
    BNCellsModuleArticleBody(BNCellsModuleArticleBody* source);
    BNModule* clone() override;

    sp<BNItem> _item;
    // @property (nonatomic) NSAttributedString* attributedText;
    // @property (nonatomic) NSArray* nontextElements;

    // - (void)realLayoutWithContainingRect:(CGRect)rect;

    void updateLayoutWithContentObject(BNContent* contentObject) override;
    
    void arrangeElements();
};
