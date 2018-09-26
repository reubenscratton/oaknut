package org.oaknut.main;

import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.opengl.Matrix;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import static android.opengl.EGL14.EGL_ALPHA_SIZE;
import static android.opengl.EGL14.EGL_BLUE_SIZE;
import static android.opengl.EGL14.EGL_CONTEXT_CLIENT_VERSION;
import static android.opengl.EGL14.EGL_DEFAULT_DISPLAY;
import static android.opengl.EGL14.EGL_DEPTH_SIZE;
import static android.opengl.EGL14.EGL_GREEN_SIZE;
import static android.opengl.EGL14.EGL_NONE;
import static android.opengl.EGL14.EGL_NO_CONTEXT;
import static android.opengl.EGL14.EGL_NO_DISPLAY;
import static android.opengl.EGL14.EGL_NO_SURFACE;
import static android.opengl.EGL14.EGL_OPENGL_ES2_BIT;
import static android.opengl.EGL14.EGL_RED_SIZE;
import static android.opengl.EGL14.EGL_RENDERABLE_TYPE;
import static android.opengl.EGL14.EGL_STENCIL_SIZE;
import static android.opengl.EGL14.EGL_SUCCESS;
import static android.opengl.EGL14.eglChooseConfig;
import static android.opengl.EGL14.eglCreateContext;
import static android.opengl.EGL14.eglCreateWindowSurface;
import static android.opengl.EGL14.eglDestroyContext;
import static android.opengl.EGL14.eglDestroySurface;
import static android.opengl.EGL14.eglGetDisplay;
import static android.opengl.EGL14.eglGetError;
import static android.opengl.EGL14.eglInitialize;
import static android.opengl.EGL14.eglMakeCurrent;
import static android.opengl.EGL14.eglReleaseThread;
import static android.opengl.EGL14.eglSwapBuffers;
import static android.opengl.EGL14.eglTerminate;
import static android.opengl.EGLExt.eglPresentationTimeANDROID;
import static android.opengl.GLES20.GL_COMPILE_STATUS;
import static android.opengl.GLES20.GL_FLOAT;
import static android.opengl.GLES20.GL_FRAGMENT_SHADER;
import static android.opengl.GLES20.GL_LINK_STATUS;
import static android.opengl.GLES20.GL_NO_ERROR;
import static android.opengl.GLES20.GL_TEXTURE0;
import static android.opengl.GLES20.GL_TRIANGLE_STRIP;
import static android.opengl.GLES20.GL_TRUE;
import static android.opengl.GLES20.GL_VERTEX_SHADER;
import static android.opengl.GLES20.glActiveTexture;
import static android.opengl.GLES20.glAttachShader;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glCompileShader;
import static android.opengl.GLES20.glCreateProgram;
import static android.opengl.GLES20.glCreateShader;
import static android.opengl.GLES20.glDeleteProgram;
import static android.opengl.GLES20.glDrawArrays;
import static android.opengl.GLES20.glEnableVertexAttribArray;
import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glGetError;
import static android.opengl.GLES20.glGetProgramInfoLog;
import static android.opengl.GLES20.glGetProgramiv;
import static android.opengl.GLES20.glGetShaderInfoLog;
import static android.opengl.GLES20.glGetShaderiv;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glLinkProgram;
import static android.opengl.GLES20.glShaderSource;
import static android.opengl.GLES20.glUniformMatrix4fv;
import static android.opengl.GLES20.glUseProgram;
import static android.opengl.GLES20.glVertexAttribPointer;

/**
 * A basic GL renderer that takes care of setting up EGL + vertex buffer + index buffer + shaders
 * for drawing a simple quad.
 *
 * Instantiate one of these for each thread that renders: in practice this means we have one
 * instance on our main thread which renders the camera preview texture to the screen (and keeps
 * the face detector thread fed), and then we have a second instance on the encoder thread that
 * renders the same camera texture onto the video MediaCodec's input surface.
 *
 * For this to work the encoder renderer has to share state with the main renderer so it can
 * get at the OES texture (see sharedRenderer constructor parameter).
 */

class GLRenderer {

    private static final int EGL_RECORDABLE_ANDROID = 0x3142;
    private EGLDisplay eglDisplay = EGL_NO_DISPLAY;
    private EGLContext eglContext = EGL_NO_CONTEXT;
    private EGLConfig eglConfig;
    private int programHandle;
    private int locMvpMatrix;
    private int locTexMatrix;
    private int locPosition;
    private int locTextureCoord;


    // Tegra devices sometimes enter deadlock between main and encoder threads. We use
    // a global lock around some GL API usage to prevent it. See https://bugs.chromium.org/p/webrtc/issues/detail?id=5702
    private static final Object driverLock = new Object();

    private static final float[] IDENTITY_MATRIX = new float[16];

    static {
        Matrix.setIdentityM(IDENTITY_MATRIX, 0);
    }


    private static final int SIZEOF_FLOAT = 4;

    private static FloatBuffer createFloatBuffer(float[] coords) {
        ByteBuffer bb = ByteBuffer.allocateDirect(coords.length * SIZEOF_FLOAT);
        bb.order(ByteOrder.nativeOrder());
        FloatBuffer fb = bb.asFloatBuffer();
        fb.put(coords);
        fb.position(0);
        return fb;
    }

    private static final FloatBuffer VERTEX_BUF = createFloatBuffer(new float[]{
            -1.0f, -1.0f,   // 0 bottom left
            1.0f, -1.0f,    // 1 bottom right
            -1.0f, 1.0f,   // 2 top left
            1.0f, 1.0f,    // 3 top right
    });
    private static final FloatBuffer TEXCOORD_BUF = createFloatBuffer(new float[]{
            0.0f, 0.0f,     // 0 bottom left
            1.0f, 0.0f,     // 1 bottom right
            0.0f, 1.0f,     // 2 top left
            1.0f, 1.0f      // 3 top right
    });


    GLRenderer(EGLContext sharedRenderContext, boolean want565) {

        if (sharedRenderContext == null) {
            sharedRenderContext = EGL_NO_CONTEXT;
        }

        eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (eglDisplay == EGL_NO_DISPLAY) {
            throw new RuntimeException("eglGetDisplay() failed");
        }
        int[] version = new int[2];
        if (!eglInitialize(eglDisplay, version, 0, version, 1)) {
            eglDisplay = null;
            throw new RuntimeException("eglInitialize() failed");
        }


        int[] attribList = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_RED_SIZE, want565 ? 5 : 8,
                EGL_GREEN_SIZE, want565 ? 6 : 8,
                EGL_BLUE_SIZE, want565 ? 5 : 8,
                EGL_ALPHA_SIZE, want565 ? 0 : 8,
                EGL_DEPTH_SIZE, 0,
                EGL_STENCIL_SIZE, 0,
                EGL_RECORDABLE_ANDROID, 1,
                EGL_NONE
        };

        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfigs = new int[1];
        if (!eglChooseConfig(eglDisplay, attribList, 0, configs, 0, configs.length,
                numConfigs, 0)) {
            throw new RuntimeException("unable to find an EGLConfig");
        }
        eglConfig = configs[0]; // NB: dangerous cos first not necessarily best. The code commented out below may be better

         /*
        EGLConfig[] configs = new EGLConfig[256];
        int numConfigs[] = new int[] {0};
        eglGetConfigs(eglDisplay,configs,0,256, numConfigs, 0);
        int value[] = new int[] {0};
        for (int i=0 ; i<numConfigs[0] ; i++) {
            eglGetConfigAttrib(eglDisplay, configs[i], EGL_SURFACE_TYPE, value, 0);
            if (EGL_WINDOW_BIT != (value[0] & EGL_WINDOW_BIT)) {
                continue;
            }
            boolean matched = true;
            for (int j = 0; j < attribList.length; j += 2) {
                eglGetConfigAttrib(eglDisplay, configs[i], attribList[j], value, 0);
                if (value[0] != attribList[j + 1]) {
                    matched = false;
                    break;
                }
            }
            if (matched) {
                eglConfig = configs[i];
                break;
            }
        }
        */

        if (eglConfig == null) {
            throw new RuntimeException("Unable to find a suitable EGLConfig");
        }

        eglContext = eglCreateContext(eglDisplay, eglConfig, sharedRenderContext,
                new int[] {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE}, 0);
        checkEglError("eglCreateContext");
    }

    EGLSurface createSurface(Object object) {
        return eglCreateWindowSurface(eglDisplay, eglConfig, object,
                new int[] {EGL_NONE},0);
    }

    void destroySurface(EGLSurface surface) {
        if (eglDisplay!=null && surface != null) { // guard against obscure crash, see issue CHECK-235
            eglDestroySurface(eglDisplay, surface);
        }
    }

    void destroy() {
        glDeleteProgram(programHandle);
        programHandle = -1;
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(eglDisplay, eglContext);
        eglReleaseThread();
        eglTerminate(eglDisplay);
        eglDisplay = EGL_NO_DISPLAY;
        eglContext = EGL_NO_CONTEXT;
        eglConfig = null;
    }

    void makeCurrent(EGLSurface surface) {
        synchronized (driverLock) {
            if (surface == null) {
                EGL14.eglMakeCurrent(eglDisplay, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_CONTEXT);
            } else {
                eglMakeCurrent(eglDisplay, surface, surface, eglContext);
            }
        }
        checkEglError("eglMakeCurrent");
    }

    void prepareToDraw(int textureId, float[] matrix) {
        if (programHandle == 0) {
            programHandle = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
            if (programHandle == 0) {
                throw new RuntimeException("Unable to create program");
            }
            locPosition = glGetAttribLocation(programHandle, "aPosition");
            locTextureCoord = glGetAttribLocation(programHandle, "aTextureCoord");
            locMvpMatrix = glGetUniformLocation(programHandle, "uMVPMatrix");
            locTexMatrix = glGetUniformLocation(programHandle, "uTexMatrix");
        }
        glUseProgram(programHandle);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureId);
        glUniformMatrix4fv(locMvpMatrix, 1, false, IDENTITY_MATRIX, 0);
        glUniformMatrix4fv(locTexMatrix, 1, false, matrix, 0);
        glEnableVertexAttribArray(locPosition);
        glVertexAttribPointer(locPosition, 2, GL_FLOAT, false, 2 * SIZEOF_FLOAT, VERTEX_BUF);
        glEnableVertexAttribArray(locTextureCoord);
        glVertexAttribPointer(locTextureCoord, 2, GL_FLOAT, false, 2 * SIZEOF_FLOAT, TEXCOORD_BUF);
    }

    void draw(EGLSurface surface, long timestamp) {
        synchronized (driverLock) {
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        if (timestamp != 0) {
            eglPresentationTimeANDROID(eglDisplay, surface, timestamp);
        }
        eglSwapBuffers(eglDisplay, surface);
    }


    private static void checkEglError(String msg) {
        int error;
        if ((error = eglGetError()) != EGL_SUCCESS) {
            throw new RuntimeException(msg + ": EGL error: 0x" + Integer.toHexString(error));
        }
    }

    private static void checkGlError(String op) {
        int error = glGetError();
        if (error != GL_NO_ERROR) {
            String msg = op + ": glError 0x" + Integer.toHexString(error);
            throw new RuntimeException(msg);
        }
    }

    private static int createProgram(String vertexSource, String fragmentSource) {
        int vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
        if (vertexShader == 0) {
            return 0;
        }
        int pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
        if (pixelShader == 0) {
            return 0;
        }

        int program = glCreateProgram();
        checkGlError("glCreateProgram");
        if (program == 0) {
            throw new RuntimeException("Could not create program");
        }
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        int[] linkStatus = new int[1];
        glGetProgramiv(program, GL_LINK_STATUS, linkStatus, 0);
        if (linkStatus[0] != GL_TRUE) {
            throw new RuntimeException("Could not link program: " + glGetProgramInfoLog(program));
        }
        return program;
    }

    private static int loadShader(int shaderType, String source) {
        int shader = glCreateShader(shaderType);
        glShaderSource(shader, source);
        glCompileShader(shader);
        int[] compiled = new int[1];
        glGetShaderiv(shader, GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            throw new RuntimeException("Could not compile shader " + shaderType + ": " + glGetShaderInfoLog(shader));
        }
        return shader;
    }


    private static final String VERTEX_SHADER =
            "uniform mat4 uMVPMatrix;\n" +
                    "uniform mat4 uTexMatrix;\n" +
                    "attribute vec4 aPosition;\n" +
                    "attribute vec4 aTextureCoord;\n" +
                    "varying vec2 vTextureCoord;\n" +
                    "void main() {\n" +
                    "    gl_Position = uMVPMatrix * aPosition;\n" +
                    "    vTextureCoord = (uTexMatrix * aTextureCoord).xy;\n" +
                    "}\n";

    private static final String FRAGMENT_SHADER =
            "#extension GL_OES_EGL_image_external : require\n" +
                    "precision mediump float;\n" +
                    "varying vec2 vTextureCoord;\n" +
                    "uniform samplerExternalOES sTexture;\n" +
                    "void main() {\n" +
                    "    gl_FragColor = texture2D(sTexture, vTextureCoord);\n" +
                    "}\n";


}
