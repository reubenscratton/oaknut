//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

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
        _obj = env->NewObject(s_jclass, s_jmidConstructor, (jlong)this, strAssetPath, size);
        _obj = env->NewGlobalRef(_obj);
    }

    Glyph* createGlyph(char32_t ch, Atlas* atlas) {
        JNIEnv *env = getJNIEnv();
        Glyph* glyph = (Glyph*)env->CallLongMethod(_obj, s_jmidCreateGlyph, (jlong)atlas, (jint)ch);
        if (glyph) {
            glyph->charCode = ch;
            POINT pt = glyph->atlasNode->rect.origin;
            Bitmap* bitmap = (Bitmap*)glyph->atlasNode->page->_bitmap._obj;
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

class AndroidPath : public Path {
public:
    jobject _path;

    AndroidPath() {
        JNIEnv* env = getJNIEnv();
        jobject path = env->NewObject(jclassPath, jmidPathConstructor);
        _path = env->NewGlobalRef(path);
    }
    ~AndroidPath() {
        getJNIEnv()->DeleteGlobalRef(_path);
    }

    void moveTo(POINT pt) {
        getJNIEnv()->CallVoidMethod(_path, jmidPathMoveTo, (jfloat)pt.x, (jfloat)pt.y);
    }
    void lineTo(POINT pt) {
        getJNIEnv()->CallVoidMethod(_path, jmidPathLineTo, (jfloat)pt.x, (jfloat)pt.y);
    }
    void curveTo(POINT ctrl1, POINT ctrl2, POINT pt) {
        getJNIEnv()->CallVoidMethod(_path, jmidPathCubicTo,
                              (jfloat)ctrl1.x, (jfloat)ctrl1.y,
                              (jfloat)ctrl2.x, (jfloat)ctrl2.y,
                              (jfloat)pt.x,    (jfloat)pt.y);
    }

};

class AndroidCanvas : public Canvas {
public:
    AndroidCanvas(jobject canvas) {
        _canvas = getJNIEnv()->NewGlobalRef(canvas);
    }
    ~AndroidCanvas() {
        getJNIEnv()->DeleteGlobalRef(_canvas);
    }
    void resize(int width, int height) {
        //jobject directBuffer = getJNIEnv()->NewDirectByteBuffer(pixelBuffer->data, pixelBuffer->cb);
        getJNIEnv()->CallVoidMethod(_canvas, jmidResize, width, height);
        jobject jbitmap = getJNIEnv()->CallObjectMethod(_canvas, jmidGetBitmap);
        _bitmap = new Bitmap(jbitmap);
    }
    void clear(COLOUR colour) {
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
    void drawRect(RECT rect) {
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawRect,
                                    (jfloat)rect.origin.x,
                                    (jfloat)rect.origin.y,
                                    (jfloat)rect.size.width,
                                    (jfloat)rect.size.height);
        _bitmap->_needsUpload = true;
    }
    void drawOval(RECT rect) {
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawOval,
                                    (jfloat)rect.origin.x,
                                    (jfloat)rect.origin.y,
                                    (jfloat)rect.size.width,
                                    (jfloat)rect.size.height);
        _bitmap->_needsUpload = true;
    }
    void drawPath(Path* path) {
        AndroidPath* androidPath = (AndroidPath*)path;
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawPath, androidPath->_path);
        _bitmap->_needsUpload = true;
    }
    Bitmap* getBitmap() {
        return _bitmap;
    }
    Path* createPath() {
        return new AndroidPath();
    }


    jobject _canvas;
    ObjPtr<Bitmap> _bitmap;
};

// API
Canvas* Canvas::create() {
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
    return new AndroidCanvas(jcanvas);
}



#endif
