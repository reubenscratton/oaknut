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

    public Font(long cobj, String name, float size) {
        this.cobj = cobj;
        if (TextUtils.isEmpty(name)) {
            typeface = Typeface.DEFAULT;
        } else {
            typeface = Typeface.createFromAsset(App.app.getAssets(), name);
        }
        this.size = size;
        textPaint = new TextPaint();
        textPaint.setTypeface(typeface);
        textPaint.setTextSize(size);
        textPaint.setColor(0xFFFFFFFF);
        textPaint.setAntiAlias(true);

        float ascent = textPaint.ascent();   // -ve in android
        float descent = textPaint.descent(); // +ve in android

        nativeSetMetrics(cobj, -ascent, -descent, 4);
    }

    private Rect rect2 = new Rect();

    public long createGlyph(long atlas, int charcode) {
        char[] ach = Character.toChars(charcode);
        float advance = rect.width();
        if (charcode != ' ') {
            textPaint.getTextBounds(ach, 0, 1, rect);
            textPaint.getTextBounds("L", 0, 1, rect2);
            int pipeWidth = rect2.width();
            textPaint.getTextBounds(new String(ach) + "L", 0, 2, rect2);
            advance = (rect2.width() - pipeWidth);
        }
        long foo = nativeCreateGlyph(cobj, atlas, charcode, rect.left, -rect.bottom, rect.width(), rect.height(), advance);
        return foo;
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
