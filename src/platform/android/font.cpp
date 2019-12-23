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
static jmethodID s_jmidRasterizeGlyph;

FontAndroid::FontAndroid(const string& fontAssetPath, float size, float weight) : Font(fontAssetPath, size, weight) {
    JNIEnv *env = getJNIEnv();
    if (!s_jclass) {
        s_jclass = env->FindClass(PACKAGE "/Font");
        s_jclass = (jclass) env->NewGlobalRef(s_jclass);
        s_jmidConstructor = env->GetMethodID(s_jclass, "<init>", "(JLjava/lang/String;FF)V");
        s_jmidCreateGlyph = env->GetMethodID(s_jclass, "createGlyph", "(I)J");
        s_jmidRasterizeGlyph = env->GetMethodID(s_jclass, "rasterizeGlyph", "(ILandroid/graphics/Bitmap;FF)V");
    }
    jstring strAssetPath = env->NewStringUTF(fontAssetPath.c_str());
    _obj = env->NewObject(s_jclass, s_jmidConstructor, (jlong)this, strAssetPath, size, weight);
    _obj = env->NewGlobalRef(_obj);
}

class GlyphAndroid : public Glyph {
public:
    GlyphAndroid(FontAndroid* font, char32_t ch) : Glyph(font, ch) {
    }

};

Glyph* FontAndroid::createGlyph(char32_t ch) {
    JNIEnv *env = getJNIEnv();
    return (Glyph*)env->CallLongMethod(_obj, s_jmidCreateGlyph, (jint)ch);
}
JAVA_FN(jlong, Font, nativeCreateGlyph)(JNIEnv *env, jobject jfont, jlong cfont, jint charcode, jint left, jint descent, jint width, jint height, jfloat advance) {
    FontAndroid* font = (FontAndroid*)cfont;
    Glyph *glyph = new GlyphAndroid(font, charcode);
    glyph->_advance.width = advance;
    glyph->_origin.x = left;
    glyph->_origin.y = descent;
    glyph->_size = {width, height};
    return (jlong)glyph;
}

void FontAndroid::rasterizeGlyph(Glyph *glyph, Atlas *atlas) {

    // Reserve a space in the glyph atlas
    glyph->_atlasNode = atlas->reserve(glyph->_size.width, glyph->_size.height, 1);

    // Get the atlas bitmap context
    auto bitmap = glyph->_atlasNode->page->_bitmap.as<BitmapAndroid>();
    POINT pt = glyph->_atlasNode->rect.origin;

    //  env->CallVoidMethod(_obj, s_jmidDrawGlyph, (jint)ch, bitmap->_androidBitmap, pt.x-glyph->bitmapLeft, pt.y+glyph->bitmapHeight+glyph->bitmapTop);
    JNIEnv *env = getJNIEnv();
    env->CallVoidMethod(_obj, s_jmidRasterizeGlyph, (jint)glyph->_codepoint, bitmap->_androidBitmap, pt.x-glyph->_origin.x, pt.y+glyph->_size.height+glyph->_origin.y);
    bitmap->texInvalidate();
}

Font* Font::create(const string& fontAssetPath, float size, float weight) {
    return new FontAndroid(fontAssetPath, size, weight);
}



JAVA_FN(void, Font, nativeSetMetrics)(JNIEnv *env, jobject jfont, jlong cobj, jfloat ascent, jfloat descent, jfloat leading) {
    Font* font = (Font*)cobj;
    font->_ascent = ascent;
    font->_descent = descent;
    font->_leading = leading;
    font->_height = (ascent-descent) + leading;
}

JAVA_FN(void, Font, nativeRasterizeGlyph)(JNIEnv *env, jobject jfont, jlong cglyph, jlong catlas) {
    Glyph *glyph = (Glyph *) cglyph;
    Atlas *atlas = (Atlas *) catlas;
    glyph->_atlasNode = atlas->reserve(glyph->_size.width, glyph->_size.height, 1);
}

#endif
