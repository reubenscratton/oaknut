package oaknut;


import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.Xfermode;

import java.nio.Buffer;

public class Canvas {

    android.graphics.Canvas canvas;
    Paint paint;
    Bitmap bitmap;
    Rect rect;
    RectF rectF;
    int fillColor;
    int strokeColor;
    float strokeWidth;
    Rect rectSrc;
    Rect rectDst;

    public Canvas() {
        canvas = new android.graphics.Canvas();
        paint = new Paint();
        rect = new Rect();
        rectSrc = new Rect();
        rectDst = new Rect();

        paint.setStrokeCap(Paint.Cap.ROUND);
        paint.setStrokeJoin(Paint.Join.ROUND);
        paint.setAntiAlias(true);
        //paint.setStrokeMiter();
    }

    public void resize(int width, int height) {
        if (bitmap != null) {
            canvas.setBitmap(null);
            bitmap.recycle();
        }
        bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        canvas.setBitmap(bitmap);
        rect.set(0,0,width,height);
    }

    public void clear(int clearColor) {
        paint.setColor(clearColor);
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC));
        canvas.drawRect(rect, paint);
        paint.setXfermode(null);
    }

    public void setFillColor(int fillColor) {
        this.fillColor = fillColor;
    }
    public void setStrokeColor(int strokeColor) {
        this.strokeColor = strokeColor;
    }
    public void setStrokeWidth(float strokeWidth) {
        this.strokeWidth = strokeWidth;
        paint.setStrokeWidth(strokeWidth);
    }


    public void setTransform(float a, float b, float c, float d, float tx, float ty) {
        Matrix matrix = new Matrix();
        float vals[] = new float[] {
               a,c,tx,b,d,ty,0,0,1
        };
        matrix.setValues(vals);
        //canvas.setMatrix(matrix);
        this.matrix = matrix;
    }
    Matrix matrix;

    public void clearTransform() {
        canvas.setMatrix(null);
    }
    public void drawRect(float x, float y, float width, float height) {
        if (fillColor != 0) {
            paint.setColor(fillColor);
            paint.setStyle(Paint.Style.FILL);
            canvas.drawRect(x,y,x+width,y+height, paint);
        }
        if (strokeColor != 0) {
            paint.setColor(strokeColor);
            paint.setStyle(Paint.Style.STROKE);
            canvas.drawRect(x,y,x+width,y+height, paint);
        }
    }
    public void drawOval(float x, float y, float width, float height) {
        if (rectF == null) {
            rectF = new RectF();
        }
        rectF.set(x,y,x+width,y+height);
        if (fillColor != 0) {
            paint.setColor(fillColor);
            paint.setStyle(Paint.Style.FILL);
            canvas.drawOval(rectF, paint);
        }
        if (strokeColor != 0) {
            paint.setColor(strokeColor);
            paint.setStyle(Paint.Style.STROKE);
            canvas.drawOval(rectF, paint);
        }
    }
    public void drawPath(Path path) {
        if (matrix != null) {
            Path path2= new Path();
            path.transform(matrix, path2);
            path = path2;
        }
        if (fillColor!=0) {
            paint.setColor(fillColor);
            paint.setStyle(Paint.Style.FILL);
            canvas.drawPath(path, paint);
        }
        if (strokeColor!=0) {
            paint.setColor(strokeColor);
            paint.setStyle(Paint.Style.STROKE);
            canvas.drawPath(path, paint);
        }
    }
    public void drawBitmap(Bitmap bitmap, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) {
        rectSrc.left = (int)sx;
        rectSrc.top = (int)sy;
        rectSrc.right = (int)(sx+sw);
        rectSrc.bottom = (int)(sy+sh);
        rectDst.left = (int)dx;
        rectDst.top = (int)dy;
        rectDst.right = (int)(dx+dw);
        rectDst.bottom = (int)(dy+dh);
        canvas.drawBitmap(bitmap, rectSrc, rectDst, paint);
    }

    public Bitmap getBitmap() {
        return bitmap;
    }
}
