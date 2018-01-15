package org.oaknut.main;

import android.graphics.Bitmap;

import java.nio.Buffer;

public class FaceDetector {

    android.media.FaceDetector faceDetector;
    int width, height;
    android.media.FaceDetector.Face faces[] = {null,null};

    public FaceDetector() {
    }
    public int update(int width, int height, int stride, Buffer pixelData) {
        if (this.width != width || this.height!=height) {
            this.width = width;
            this.height = height;
            faceDetector = new android.media.FaceDetector(width, height, faces.length);
        }
        for (int i=0; i<faces.length ; i++) {
            faces[i] = null;
        }
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.copyPixelsFromBuffer(pixelData);
        bitmap = bitmap.copy(Bitmap.Config.RGB_565, false);
        //bitmap.reconfigure(width, height, Bitmap.Config.RGB_565);
        int numFaces = faceDetector.findFaces(bitmap, faces);
        for (int i=0 ; i<numFaces ; i++) {
            android.media.FaceDetector.Face face = faces[i];
            // FaceDetector occasionally generates false positives, I haven't caught it in the
            // debugger but I suspect that the false ones are much smaller than a normal face.
            // In the absence of a face-size API use eyesDistance to filter out titchy faces.
            if (face.eyesDistance() < 50) { //
                numFaces--;
            }
        }
        return numFaces;
    }


}
