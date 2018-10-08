//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


typedef struct MEASURESPEC {
    enum Type {
        TypeNone,     // no measuring done via this measurespec, measurement is set in code
        TypeRelative, // measurement is relative to another view (normally the parent)
        TypeContent,  // measurement is taken from intrinsic content size, plus padding
        TypeAspect    // measurement relative to this view's opposite dimension
    };
    
    Type type;
    class View* ref;
    float mul;
    float con;
    
    MEASURESPEC(Type type, class View* ref, float mul, float con);
    MEASURESPEC(const class StyleValue* value, View* view);
    float calc(View* view, float parentSize, float otherSize, bool isVertical) const;
    
    static MEASURESPEC None();
    static MEASURESPEC Abs(float x);
    static MEASURESPEC Wrap();
    static MEASURESPEC Aspect(float x);
    static MEASURESPEC Fill();

} MEASURESPEC;





