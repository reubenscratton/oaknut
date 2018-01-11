//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include "bitmap.h"


static jclass jclassCanvas;
static jmethodID jmidCanvasConstructor;
static jmethodID jmidResize;
static jmethodID jmidGetBitmap;
static jmethodID jmidClear;
static jmethodID jmidSetFillColour;
static jmethodID jmidSetStrokeColour;
static jmethodID jmidSetStrokeWidth;
static jmethodID jmidSetTransform;
static jmethodID jmidClearTransform;
static jmethodID jmidDrawRect;
static jmethodID jmidDrawOval;
static jmethodID jmidDrawPath;
//static jmethodID jmidCommitChanges;

static jclass jclassPath;
static jmethodID jmidPathConstructor;
static jmethodID jmidPathMoveTo;
static jmethodID jmidPathLineTo;
static jmethodID jmidPathCubicTo;

class AndroidFont : public Font {
public:
    jobject _obj;

    static jclass s_jclass;
    static jmethodID s_jmidConstructor;
    static jmethodID s_jmidCreateGlyph;
    static jmethodID s_jmidDrawGlyph;

    AndroidFont(const string& fontAssetPath, float size) : Font(fontAssetPath, size) {
        JNIEnv *env = getJNIEnv();
        if (!s_jclass) {
            s_jclass = env->FindClass(PACKAGE "/Font");
            s_jclass = (jclass) env->NewGlobalRef(s_jclass);
            s_jmidConstructor = env->GetMethodID(s_jclass, "<init>", "(JLjava/lang/String;F)V");
            s_jmidCreateGlyph = env->GetMethodID(s_jclass, "createGlyph", "(JI)J");
            s_jmidDrawGlyph = env->GetMethodID(s_jclass, "drawGlyph", "(ILandroid/graphics/Bitmap;FF)V");
        }
        jstring strAssetPath = env->NewStringUTF(fontAssetPath.data());
        _obj = env->NewObject(s_jclass, s_jmidConstructor, (jlong)this, strAssetPath, dp(size));
        _obj = env->NewGlobalRef(_obj);
    }

    Glyph* createGlyph(char32_t ch, Atlas* atlas) {
        JNIEnv *env = getJNIEnv();
        Glyph* glyph = (Glyph*)env->CallLongMethod(_obj, s_jmidCreateGlyph, (jlong)atlas, (jint)ch);
        if (glyph) {
            glyph->charCode = ch;
            POINT pt = glyph->atlasNode->rect.origin;
            OSBitmap* bitmap = (OSBitmap*)glyph->atlasNode->page->_bitmap._obj;
            env->CallVoidMethod(_obj, s_jmidDrawGlyph, (jint)ch, bitmap->_androidBitmap, pt.x-glyph->bitmapLeft, pt.y+glyph->bitmapHeight+glyph->bitmapTop);
            bitmap->_needsUpload = true;
        }
        return glyph;
    }
};

jclass AndroidFont::s_jclass;
jmethodID AndroidFont::s_jmidConstructor;
jmethodID AndroidFont::s_jmidCreateGlyph;
jmethodID AndroidFont::s_jmidDrawGlyph;

Font* oakFontGet(const string& fontAssetPath, float size) {
    return new AndroidFont(fontAssetPath, size);
}

JAVA_FN(void, Font, nativeSetMetrics)(JNIEnv *env, jobject jfont, jlong cobj, jfloat ascent, jfloat descent, jfloat leading) {
    AndroidFont* font = (AndroidFont*)cobj;
    font->_ascent = ascent;
    font->_descent = descent;
    font->_leading = leading;
    font->_height = (ascent-descent) + leading;
}

JAVA_FN(jlong, Font, nativeCreateGlyph)(JNIEnv *env, jobject jfont, jlong cfont, jlong cAtlas, jint charcode, jint left, jint descent, jint width, jint height, jfloat advance) {
    Font* font = (Font*)cfont;
    Glyph *glyph = new Glyph(font, charcode, 0);
    glyph->advance.width = advance;
    glyph->bitmapLeft = left;
    glyph->bitmapTop = descent;
    glyph->bitmapWidth = width;
    glyph->bitmapHeight = height;
    Atlas* atlas = (Atlas*)cAtlas;
    glyph->atlasNode = atlas->reserve(glyph->bitmapWidth, glyph->bitmapHeight, 1);
    return (jlong)glyph;
}


class OSCanvas {
public:
    OSCanvas(jobject canvas) {
        _canvas = getJNIEnv()->NewGlobalRef(canvas);
    }
    ~OSCanvas() {
        getJNIEnv()->DeleteGlobalRef(_canvas);
    }
    void resize(int width, int height) {
        //jobject directBuffer = getJNIEnv()->NewDirectByteBuffer(pixelBuffer->data, pixelBuffer->cb);
        getJNIEnv()->CallVoidMethod(_canvas, jmidResize, width, height);
        jobject jbitmap = getJNIEnv()->CallObjectMethod(_canvas, jmidGetBitmap);
        _bitmap = new OSBitmap(jbitmap);
    }
    void clearToColour(COLOUR colour) {
        getJNIEnv()->CallVoidMethod(_canvas, jmidClear, (jint)colour);
        _bitmap->_needsUpload = true;
    }
    void setFillColour(COLOUR fillColour) {
        getJNIEnv()->CallVoidMethod(_canvas, jmidSetFillColour, (jint)fillColour);
    }
    void setStrokeColour(COLOUR strokeColour) {
        getJNIEnv()->CallVoidMethod(_canvas, jmidSetStrokeColour, (jint)strokeColour);
    }
    void setStrokeWidth(float strokeWidth) {
        getJNIEnv()->CallVoidMethod(_canvas, jmidSetStrokeWidth, (jfloat)strokeWidth);
    }
    void setAffineTransform(AffineTransform* t) {
        if (t) {
            getJNIEnv()->CallVoidMethod(_canvas, jmidSetTransform,
                                        (jfloat)t->a, (jfloat)t->b,
                                        (jfloat)t->c, (jfloat)t->d,
                                        (jfloat)t->tx, (jfloat)t->ty);
        } else {
            getJNIEnv()->CallVoidMethod(_canvas, jmidClearTransform);
        }
    }
    void drawRect(const RECT& rect) {
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawRect,
                                    (jfloat)rect.origin.x,
                                    (jfloat)rect.origin.y,
                                    (jfloat)rect.size.width,
                                    (jfloat)rect.size.height);
        _bitmap->_needsUpload = true;
    }
    void drawOval(const RECT& rect) {
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawOval,
                                    (jfloat)rect.origin.x,
                                    (jfloat)rect.origin.y,
                                    (jfloat)rect.size.width,
                                    (jfloat)rect.size.height);
        _bitmap->_needsUpload = true;
    }
    void drawPath(void* ospath) {
        jobject path = (jobject)ospath;
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawPath, path);
        _bitmap->_needsUpload = true;
    }




    jobject _canvas;
    ObjPtr<OSBitmap> _bitmap;
};

// API
void* oakCanvasCreate() {
    JNIEnv* env = getJNIEnv();
    jclassCanvas = env->FindClass(PACKAGE "/Canvas");
    jclassCanvas = (jclass)env->NewGlobalRef(jclassCanvas);
    jclassPath = env->FindClass("android/graphics/Path");
    jclassPath = (jclass)env->NewGlobalRef(jclassPath);
    jmidCanvasConstructor = env->GetMethodID(jclassCanvas, "<init>", "()V");
    jmidResize = env->GetMethodID(jclassCanvas, "resize", "(II)V");
    jmidGetBitmap = env->GetMethodID(jclassCanvas, "getBitmap", "()Landroid/graphics/Bitmap;");
    jmidClear = env->GetMethodID(jclassCanvas, "clear", "(I)V");
    jmidSetFillColour = env->GetMethodID(jclassCanvas, "setFillColour", "(I)V");
    jmidSetStrokeColour = env->GetMethodID(jclassCanvas, "setStrokeColour", "(I)V");
    jmidSetStrokeWidth = env->GetMethodID(jclassCanvas, "setStrokeWidth", "(F)V");
    jmidSetTransform = env->GetMethodID(jclassCanvas, "setTransform", "(FFFFFF)V");
    jmidClearTransform = env->GetMethodID(jclassCanvas, "clearTransform", "()V");
    jmidDrawRect = env->GetMethodID(jclassCanvas, "drawRect", "(FFFF)V");
    jmidDrawOval = env->GetMethodID(jclassCanvas, "drawOval", "(FFFF)V");
    jmidDrawPath = env->GetMethodID(jclassCanvas, "drawPath", "(Landroid/graphics/Path;)V");
    //jmidCommitChanges = env->GetMethodID(jclassCanvas, "commitChanges", "()V");

    jmidPathConstructor = env->GetMethodID(jclassPath, "<init>", "()V");
    jmidPathMoveTo = env->GetMethodID(jclassPath, "moveTo", "(FF)V");
    jmidPathLineTo = env->GetMethodID(jclassPath, "lineTo", "(FF)V");
    jmidPathCubicTo = env->GetMethodID(jclassPath, "cubicTo", "(FFFFFF)V");
    
    jobject jcanvas = env->NewObject(jclassCanvas, jmidCanvasConstructor);
    return new OSCanvas(jcanvas);
}
void oakCanvasResize(void* oscanvas, int width, int height) {
    ((OSCanvas*)oscanvas)->resize(width, height);
}
Bitmap* oakCanvasGetBitmap(void* oscanvas) {
    return ((OSCanvas*)oscanvas)->_bitmap;
}
void oakCanvasClear(void* oscanvas, COLOUR colour) {
    ((OSCanvas*)oscanvas)->clearToColour(colour);
}
void oakCanvasSetFillColour(void* oscanvas, COLOUR colour) {
    ((OSCanvas*)oscanvas)->setFillColour(colour);
}
void oakCanvasSetStrokeColour(void* oscanvas, COLOUR colour) {
    ((OSCanvas*)oscanvas)->setStrokeColour(colour);
}
void oakCanvasSetStrokeWidth(void* oscanvas, float strokeWidth) {
    ((OSCanvas*)oscanvas)->setStrokeWidth(strokeWidth);
}
void oakCanvasSetAffineTransform(void* oscanvas, AffineTransform* t) {
    ((OSCanvas*)oscanvas)->setAffineTransform(t);
}
void oakCanvasDrawRect(void* oscanvas, RECT rect) {
    ((OSCanvas*)oscanvas)->drawRect(rect);
}
void oakCanvasDrawOval(void* oscanvas, RECT rect) {
    ((OSCanvas*)oscanvas)->drawOval(rect);
}
void oakCanvasDrawPath(void* oscanvas, void* ospath) {
    ((OSCanvas*)oscanvas)->drawPath(ospath);
}
void oakCanvasRelease(void* oscanvas) {
    delete (OSCanvas*)oscanvas;
}

void* oakCanvasPathCreate() {
    JNIEnv* env = getJNIEnv();
    jobject path = env->NewObject(jclassPath, jmidPathConstructor);
    return (void*)env->NewGlobalRef(path);
}
void oakCanvasPathMoveTo(void* ospath, POINT pt) {
    jobject path = (jobject)ospath;
    getJNIEnv()->CallVoidMethod(path, jmidPathMoveTo, (jfloat)pt.x, (jfloat)pt.y);
}
void oakCanvasPathLineTo(void* ospath, POINT pt) {
    jobject path = (jobject)ospath;
    getJNIEnv()->CallVoidMethod(path, jmidPathLineTo, (jfloat)pt.x, (jfloat)pt.y);
}
void oakCanvasPathCurveTo(void* ospath, POINT ctrl1, POINT ctrl2, POINT pt) {
    jobject path = (jobject)ospath;
    getJNIEnv()->CallVoidMethod(path, jmidPathCubicTo,
                          (jfloat)ctrl1.x, (jfloat)ctrl1.y,
                          (jfloat)ctrl2.x, (jfloat)ctrl2.y,
                          (jfloat)pt.x,    (jfloat)pt.y);
}
void oakCanvasPathRelease(void* ospath) {
    jobject path = (jobject)ospath;
    getJNIEnv()->DeleteGlobalRef(path);
}


