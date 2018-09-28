package org.oaknut.main;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.media.ImageReader;
import android.opengl.GLES11Ext;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.AttributeSet;
import android.view.Gravity;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.FrameLayout;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.List;

import static android.opengl.GLES20.*;




public class Camera extends Object implements SurfaceTexture.OnFrameAvailableListener {


    android.hardware.Camera camera;
    long cppCamera;
    int cameraId;
    int cameraOrientation;
    int previewWidth;
    int previewHeight;
    int previewWidthActual;
    int previewHeightActual;
    int textureId;
    SurfaceTexture surfaceTexture;
    float cameraTextureMatrix[] = new float[16];




    public void open(long cppCamera, int flags) {
        this.cppCamera = cppCamera;

        // Find the front-facing camera
        android.hardware.Camera.CameraInfo cameraInfo = new android.hardware.Camera.CameraInfo();
        for (int i = 0; i < android.hardware.Camera.getNumberOfCameras(); i++) {
            android.hardware.Camera.getCameraInfo(i, cameraInfo);
            if (cameraInfo.facing == android.hardware.Camera.CameraInfo.CAMERA_FACING_FRONT) {
                cameraId = i;
                cameraOrientation = cameraInfo.orientation;
                camera = android.hardware.Camera.open(cameraId);
                break;
            }
        }

        // Find the closest to CamcorderProfile 480P (which some devices don't support)
        List<android.hardware.Camera.Size> sizes = camera.getParameters().getSupportedPreviewSizes();
        int smallestWidthDiff = Integer.MAX_VALUE;
        android.hardware.Camera.Size nearestSize = sizes.get(0);
        for (android.hardware.Camera.Size size : sizes) {
            int width = (cameraOrientation == 0 || cameraOrientation == 180) ?
                    size.width : size.height;
            int diff = Math.abs(width - 480);
            if (diff < smallestWidthDiff) {
                nearestSize = size;
                smallestWidthDiff = diff;
            } else if (diff == smallestWidthDiff) {
                if (size.width * size.height > nearestSize.width * nearestSize.height) {
                    nearestSize = size;
                }
            }
        }

        previewWidthActual = nearestSize.width;
        previewHeightActual = nearestSize.height;
        if (cameraOrientation == 0 || cameraOrientation == 180) {
            previewWidth = previewWidthActual;
            previewHeight = previewHeightActual;
        } else {
            previewWidth = previewHeightActual;
            previewHeight = previewWidthActual;
        }

        // Configure camera with desired preview size and other settings
        android.hardware.Camera.Parameters parameters = camera.getParameters();
        parameters.setPreviewSize(previewWidthActual, previewHeightActual);
        //parameters.setPreviewFrameRate(30);
        List<String> focusModes = parameters.getSupportedFocusModes();
        if (focusModes.contains(android.hardware.Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
            parameters.setFocusMode(android.hardware.Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        }
        camera.setParameters(parameters);
        //camera.setDisplayOrientation(360 - cameraOrientation);

        WindowManager windowManager = (WindowManager)App.app.getSystemService(Context.WINDOW_SERVICE);
        int rotation = windowManager.getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0: degrees = 0; break;
            case Surface.ROTATION_90: degrees = 90; break;
            case Surface.ROTATION_180: degrees = 180; break;
            case Surface.ROTATION_270: degrees = 270; break;
        }

        int result;
        if (cameraInfo.facing == android.hardware.Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (cameraInfo.orientation + degrees) % 360;
            result = (360 - result) % 360;  // compensate the mirror
        } else {  // back-facing
            result = (cameraInfo.orientation - degrees + 360) % 360;
        }
        camera.setDisplayOrientation(result);
    }




    public void close() {
        if (camera != null) {
            camera.release();
            camera = null;
        }
    }


    public void startPreview() {

        int[] textures = new int[1];
        glGenTextures(1, textures, 0);
        textureId = textures[0];
        glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureId);
        glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        surfaceTexture = new SurfaceTexture(textureId);
        surfaceTexture.setOnFrameAvailableListener(this);

        try {
            camera.setPreviewTexture(surfaceTexture);
        } catch (IOException ioe) {
            throw new RuntimeException(ioe);
        }
        camera.startPreview();
    }

    public void stopPreview() {
        if (camera != null) {
            camera.stopPreview();
            try {
                camera.setPreviewTexture(null);
            } catch (IOException ioe) {
            }
        }
        if (surfaceTexture != null) {
            surfaceTexture.setOnFrameAvailableListener(null);
            surfaceTexture.release();
            surfaceTexture = null;
        }

    }




    private native void nativeOnFrameAvailable(long cppCamera, int textureId, long timestamp, int width, int height, float[] transform);

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if (this.surfaceTexture != null) {
            surfaceTexture.updateTexImage();
            surfaceTexture.getTransformMatrix(cameraTextureMatrix);
            long timestamp = surfaceTexture.getTimestamp();
            nativeOnFrameAvailable(cppCamera, textureId, timestamp, previewWidth, previewHeight, cameraTextureMatrix);

        }
    }
}
