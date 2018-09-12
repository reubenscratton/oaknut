//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

#include <oaknut.h>

static jclass s_jclass;
static jmethodID s_jmidConstructor;
static jmethodID s_jmidCreateGlyph;
static jmethodID s_jmidDrawGlyph;

Font::Font(const string& fontAssetPath, float size, float weight) : FontBase(fontAssetPath, size, weight) {
    JNIEnv *env = getJNIEnv();
    if (!s_jclass) {
        s_jclass = env->FindClass(PACKAGE "/Font");
        s_jclass = (jclass) env->NewGlobalRef(s_jclass);
        s_jmidConstructor = env->GetMethodID(s_jclass, "<init>", "(JLjava/lang/String;FF)V");
        s_jmidCreateGlyph = env->GetMethodID(s_jclass, "createGlyph", "(JI)J");
        s_jmidDrawGlyph = env->GetMethodID(s_jclass, "drawGlyph", "(ILandroid/graphics/Bitmap;FF)V");
    }
    jstring strAssetPath = env->NewStringUTF(fontAssetPath.data());
    _obj = env->NewObject(s_jclass, s_jmidConstructor, (jlong)this, strAssetPath, size);
    _obj = env->NewGlobalRef(_obj);
}
    
Glyph* Font::createGlyph(char32_t ch, Atlas* atlas) {
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



JAVA_FN(void, Font, nativeSetMetrics)(JNIEnv *env, jobject jfont, jlong cobj, jfloat ascent, jfloat descent, jfloat leading) {
    Font* font = (Font*)cobj;
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


#endif
