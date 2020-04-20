//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#include <oaknut.h>


static jclass jclassCanvas;
static jmethodID jmidCanvasConstructor;
static jmethodID jmidResize;
static jmethodID jmidGetBitmap;
static jmethodID jmidClear;
static jmethodID jmidSetFillColor;
static jmethodID jmidSetStrokeColor;
static jmethodID jmidSetStrokeWidth;
static jmethodID jmidSetTransform;
static jmethodID jmidClearTransform;
static jmethodID jmidDrawRect;
static jmethodID jmidDrawOval;
static jmethodID jmidDrawPath;
static jmethodID jmidDrawBitmap;
//static jmethodID jmidCommitChanges;

static jclass jclassPath;
static jmethodID jmidPathConstructor;
static jmethodID jmidPathMoveTo;
static jmethodID jmidPathLineTo;
static jmethodID jmidPathCubicTo;

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
    void resize(int width, int height) override {
        //jobject directBuffer = getJNIEnv()->NewDirectByteBuffer(pixelBuffer->data, pixelBuffer->cb);
        getJNIEnv()->CallVoidMethod(_canvas, jmidResize, width, height);
        jobject jbitmap = getJNIEnv()->CallObjectMethod(_canvas, jmidGetBitmap);
        _bitmap = new BitmapAndroid(jbitmap);
        _bitmap->_hasPremultipliedAlpha = true;
    }
    void clear(COLOR color) override {
        getJNIEnv()->CallVoidMethod(_canvas, jmidClear, (jint)color);
        _bitmap->texInvalidate();
    }
    void setFillColor(COLOR fillColor) override {
        getJNIEnv()->CallVoidMethod(_canvas, jmidSetFillColor, (jint)fillColor);
    }
    void setStrokeColor(COLOR strokeColor) override {
        getJNIEnv()->CallVoidMethod(_canvas, jmidSetStrokeColor, (jint)strokeColor);
    }
    void setStrokeWidth(float strokeWidth) override {
        getJNIEnv()->CallVoidMethod(_canvas, jmidSetStrokeWidth, (jfloat)strokeWidth);
    }
    void setTransform(const AFFINE_TRANSFORM& t) override {
        getJNIEnv()->CallVoidMethod(_canvas, jmidSetTransform,
                                    (jfloat) t.a, (jfloat) t.b,
                                    (jfloat) t.c, (jfloat) t.d,
                                    (jfloat) t.tx, (jfloat) t.ty);
    }
    void clearTransform() override {
        getJNIEnv()->CallVoidMethod(_canvas, jmidClearTransform);
    }
    void drawRect(RECT rect) override {
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawRect,
                                    (jfloat)rect.origin.x,
                                    (jfloat)rect.origin.y,
                                    (jfloat)rect.size.width,
                                    (jfloat)rect.size.height);
        _bitmap->texInvalidate();
    }
    void drawOval(RECT rect) override {
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawOval,
                                    (jfloat)rect.origin.x,
                                    (jfloat)rect.origin.y,
                                    (jfloat)rect.size.width,
                                    (jfloat)rect.size.height);
        _bitmap->texInvalidate();
    }
    void drawPath(Path* path) override {
        AndroidPath* androidPath = (AndroidPath*)path;
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawPath, androidPath->_path);
        _bitmap->texInvalidate();
    }
    void drawBitmap(Bitmap* abitmap, const RECT& rectSrc, const RECT& rectDst) override {
        BitmapAndroid* bitmap = (BitmapAndroid*)abitmap;
        getJNIEnv()->CallVoidMethod(_canvas, jmidDrawBitmap, bitmap->_androidBitmap);
        _bitmap->texInvalidate();
    }
    Bitmap* getBitmap() override {
        return _bitmap;
    }
    Path* createPath() override {
        return new AndroidPath();
    }


    jobject _canvas;
    sp<Bitmap> _bitmap;
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
    jmidSetFillColor = env->GetMethodID(jclassCanvas, "setFillColor", "(I)V");
    jmidSetStrokeColor = env->GetMethodID(jclassCanvas, "setStrokeColor", "(I)V");
    jmidSetStrokeWidth = env->GetMethodID(jclassCanvas, "setStrokeWidth", "(F)V");
    jmidSetTransform = env->GetMethodID(jclassCanvas, "setTransform", "(FFFFFF)V");
    jmidClearTransform = env->GetMethodID(jclassCanvas, "clearTransform", "()V");
    jmidDrawRect = env->GetMethodID(jclassCanvas, "drawRect", "(FFFF)V");
    jmidDrawOval = env->GetMethodID(jclassCanvas, "drawOval", "(FFFF)V");
    jmidDrawPath = env->GetMethodID(jclassCanvas, "drawPath", "(Landroid/graphics/Path;)V");
    jmidDrawBitmap = env->GetMethodID(jclassCanvas, "drawBitmap", "(Landroid/graphics/Bitmap;FFFFFFFF)V");
    //jmidCommitChanges = env->GetMethodID(jclassCanvas, "commitChanges", "()V");

    jmidPathConstructor = env->GetMethodID(jclassPath, "<init>", "()V");
    jmidPathMoveTo = env->GetMethodID(jclassPath, "moveTo", "(FF)V");
    jmidPathLineTo = env->GetMethodID(jclassPath, "lineTo", "(FF)V");
    jmidPathCubicTo = env->GetMethodID(jclassPath, "cubicTo", "(FFFFFF)V");
    
    jobject jcanvas = env->NewObject(jclassCanvas, jmidCanvasConstructor);
    return new AndroidCanvas(jcanvas);
}



#endif
