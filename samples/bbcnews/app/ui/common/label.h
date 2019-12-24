//
//  Copyright (c) 2019 BBC News. All rights reserved.
//

#pragma once
#include <oaknut.h>


class BNLabel : public Label {
public:
    
    bool applySingleStyle(const string& name, const style& value) override;

    float paragraphSpacingBefore;
    float paragraphSpacing;

    void setBottomRightExclusionSize(const SIZE& size);

    
    enum BNMediaGlyph {
        kNone=0,
        kVideo=1,
        kAudio=2
    };
    
    bool getUseDynamicText() const;
    void setUseDynamicText(bool val);
    
    bool _useFullWidth;
    SIZE _bottomRightExclusionSize;
    COLOR _textColorOverride;
protected:
    const style* _style;
    float _paragraphSpacingBefore;
    float _paragraphSpacing;
    float _headIdent;
    float _tailIndent;
    float _lineHeight;
    bool _useDynamicText;

};


