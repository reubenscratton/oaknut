//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#include "statictitlemodule.h"
//#import "NSDate+Formatter.h"
//#import "BNStyles.h"
//#import "BNPolicy.h"

class BNCopyrightModule : public BNStaticTitleModule {
public:

    BNCopyrightModule(const variant& json) : BNStaticTitleModule(json) {
        _style = app->getStyle("copyrightNotice");
    }

    void updateLayoutWithContentObject(BNContent* contentObject) override {
        /*NSString *yearString = [NSDate longYearStringFromTimeStamp:contentObject.lastUpdated];
        if ([contentObject.modelId isEqualToString:modelIdLive]) {
            yearString = [NSDate longYearStringFromTimeStamp:[NSDate date].timeIntervalSince1970];
        }
        NSString *copyrightInfo = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"NSHumanReadableCopyright"];
        self.titleColor = [contentObject isMediaItem] ? [UIColor contentForegroundInvColor] : [UIColor contentForegroundColor];
        self.text = [NSString stringWithFormat:copyrightInfo,yearString];*/
        _text = "TODO: copyright";
    }

};

DECLARE_DYNCREATE(BNCopyrightModule, const variant&);



