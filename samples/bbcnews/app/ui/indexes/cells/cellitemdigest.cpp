//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "cellitem.h"
#include "../../common/durationlabel.h"

class BNCellItemDigest : public BNCellItem {
public:

    BNCellItemDigest(BNCellsModule* module) : BNCellItem(module, BNCellStyle::Digest) {
	}
};

DECLARE_DYNCREATE(BNCellItemDigest, BNCellsModule*);


