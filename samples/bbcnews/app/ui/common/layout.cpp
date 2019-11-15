//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "layout.h"


BNLayout::BNLayout(const variant& json) {
    _emptyView = json.stringVal("emptyView");
    _rootContainer = (BNContainerModule*)Object::createByName(json.stringVal("class"),json);
}

BNLayout::BNLayout(BNLayout* source) {
    _emptyView = source->_emptyView;
    _rootContainer = (BNContainerModule*)source->_rootContainer->clone();
}

BNLayout* BNLayout::clone() {
    return new BNLayout(this);
}

//- (NSString*)debugDescription {
//	return [NSString stringWithFormat:@"%@ %@ %@ %@",self.orientation,self.site,self.content,self.format];
//}
