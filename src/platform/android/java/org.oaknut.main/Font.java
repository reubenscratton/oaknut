package org.oaknut.main;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.text.TextPaint;
import android.text.TextUtils;

public class Font {

    long cobj;
    Typeface typeface;
    float size;
    TextPaint textPaint;
    Rect rect = new Rect();
    android.graphics.Canvas canvas = new android.graphics.Canvas();

    public Font(long cobj, String name, float size, float weight) {
        this.cobj = cobj;
        if (TextUtils.isEmpty(name)) {

            // Before API 28 we can get at 8 "weights", bold and non-bold renders of 4 underlying font variations
            //   0 -  199 : thin
            // 200 -  399 : light
            // 400 -  699 : normal
            // 700 - 1000 : medium
            String suffix = "";
            boolean fakeBold = false;
            if (weight < 200) {suffix = "-thin"; if (weight>=100) fakeBold=true;}
            else if (weight < 400) {suffix = "-light"; if (weight>=300) fakeBold=true;}
            else if (weight < 700) {suffix = ""; if (weight>500) fakeBold=true;}
            else {suffix = "-medium"; if (weight>800) fakeBold=true;}
            typeface = Typeface.create("sans-serif" + suffix, fakeBold?Typeface.BOLD:Typeface.NORMAL);
        } else {
            typeface = Typeface.createFromAsset(App.app->getAssets(), name);
        }
        this.size = size;
        textPaint = new TextPaint();
        textPaint.setTypeface(typeface);
        textPaint.setTextSize(size);
        textPaint.setColor(0xFFFFFFFF);
        textPaint.setAntiAlias(true);

        float ascent = textPaint.ascent();   // -ve in android
        float descent = textPaint.descent(); // +ve in android

        nativeSetMetrics(cobj, -ascent, -descent, 0);
    }


    public long createGlyph(long atlas, int charcode) {
        char[] ach = Character.toChars(charcode);
        float advance = textPaint.measureText(ach, 0, 1);
        textPaint.getTextBounds(ach, 0, 1, rect);
        rect.top--; // textBounds often rounds down the true glyph height, add a pixel in height to compensate
        return nativeCreateGlyph(cobj, atlas, charcode, rect.left, -rect.bottom, rect.width(), rect.height(), advance);
    }

    public void drawGlyph(int charcode, Bitmap atlasBitmap, float x, float y) {
        char[] ach = Character.toChars(charcode);
        canvas.setBitmap(atlasBitmap);
        canvas.drawText(ach, 0, 1, x, y, textPaint);
        canvas.setBitmap(null);
    }


    native void nativeSetMetrics(long cobj, float ascent, float descent, float leading);
    native long nativeCreateGlyph(long cobj, long cAtlas, int charcode, int left, int descent, int width, int height, float advance);
}
