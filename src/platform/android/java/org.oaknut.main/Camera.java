package org.oaknut.main;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.view.Surface;
import android.view.WindowManager;

import java.io.IOException;
import java.util.List;

import static android.opengl.GLES20.*;
import static android.hardware.Camera.CameraInfo.*;



public class Camera extends Object implements SurfaceTexture.OnFrameAvailableListener, android.hardware.Camera.PreviewCallback {


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




    public void open(long cppCamera, boolean frontFacing, int frameSizeShort, int frameSizeLong, int frameRate) {
        this.cppCamera = cppCamera;

        // Find the right camera
        android.hardware.Camera.CameraInfo cameraInfo = new android.hardware.Camera.CameraInfo();
        int desiredFacing = frontFacing ? CAMERA_FACING_FRONT : CAMERA_FACING_BACK;
        for (int i = 0; i < android.hardware.Camera.getNumberOfCameras(); i++) {
            android.hardware.Camera.getCameraInfo(i, cameraInfo);
            if (cameraInfo.facing == desiredFacing) {
                cameraId = i;
                cameraOrientation = cameraInfo.orientation;
                camera = android.hardware.Camera.open(cameraId);
                break;
            }
        }

        // Find the closest frame size
        List<android.hardware.Camera.Size> sizes = camera.getParameters().getSupportedPreviewSizes();
        int smallestDiff = Integer.MAX_VALUE;
        android.hardware.Camera.Size nearestSize = sizes.get(0);
        for (android.hardware.Camera.Size size : sizes) {
            int diffShort=0, diffLong=0;
            if (frameSizeShort>0) {
                int shortMeasure = Math.min(size.width, size.height);
                diffShort = Math.abs(shortMeasure - frameSizeShort);
            }
            if (frameSizeLong>0) {
                int longMeasure = Math.max(size.width, size.height);
                diffLong = Math.abs(longMeasure - frameSizeLong);
            }
            int diff;
            if (diffLong>0 && diffShort>0) {
                diff=Math.min(diffLong,diffShort);
            } else if (diffLong==0) {
                diff = diffShort;
            } else {
                diff = diffLong;
            }
            if (diff < smallestDiff) {
                nearestSize = size;
                smallestDiff = diff;
            } /*else if (diff == smallestDiff) {
                if (size.width * size.height > nearestSize.width * nearestSize.height) {
                    nearestSize = size;
                }
            }*/
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
        if (cameraInfo.facing == CAMERA_FACING_FRONT) {
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
            camera.setPreviewCallback(this);
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
                camera.setPreviewCallback(null);
            } catch (IOException ioe) {
            }
        }
        if (surfaceTexture != null) {
            surfaceTexture.setOnFrameAvailableListener(null);
            surfaceTexture.release();
            surfaceTexture = null;
        }

    }


    /**

     Android camera insanity, part 94...

     When onFrameAvailable() fires you can only use the surfaceTexture texture before
     the function returns and the underlying buffer is recycled.

     But onPreviewFrame() fires afterwards,

     */


    private byte[] prevFrameBytes; // this is completely nuts

    private native void nativeOnFrameAvailable(long cppCamera, int textureId, long timestamp, int width, int height, float[] transform, byte[] data);

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if (this.surfaceTexture != null) {
            surfaceTexture.updateTexImage();
            surfaceTexture.getTransformMatrix(cameraTextureMatrix);
            long timestamp = surfaceTexture.getTimestamp();
            nativeOnFrameAvailable(cppCamera, textureId, timestamp, previewWidth, previewHeight, cameraTextureMatrix, prevFrameBytes);
        }
    }

    @Override
    public void onPreviewFrame(byte[] data, android.hardware.Camera camera) {
        prevFrameBytes = data;
    }
}
