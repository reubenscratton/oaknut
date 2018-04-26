//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


typedef struct MEASURESPEC {
    enum RefType {
        RefTypeNone,  // no measuring is done, frame must be set in code
        RefTypeAbs,  // measurement is absolute (i.e. abs field)
        RefTypeContent, // measurement is taken from intrinsic content size, plus padding
        RefTypeView, //  measure relative to another view (normally the parent)
        RefTypeAspect   // measure relative to opposite dimension
    };
    
    RefType refType;
    class View* refView;
    float refSizeMultiplier;
    float abs;
    
    MEASURESPEC(RefType refType, class View* refView, float refSizeMultiplier, float delta);

    static MEASURESPEC None();//         MEASURESPEC_Make(REFTYPE_NONE, NULL, 0, 0)
    static MEASURESPEC Abs(float x);//       MEASURESPEC_Make(REFTYPE_ABS, NULL, 0, x)
    static MEASURESPEC WrapContent();//  MEASURESPEC_Make(REFTYPE_CONTENT, NULL, 1, 0)
    static MEASURESPEC UseAspect(float x); // MEASURESPEC_Make(REFTYPE_ASPECT, NULL,  x, 0)
    static MEASURESPEC FillParent();  // MEASURESPEC_Make(REFTYPE_VIEW, NULL, 1, 0)

} MEASURESPEC;





