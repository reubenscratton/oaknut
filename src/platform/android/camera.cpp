//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID && OAKNUT_WANT_CAMERA

#include <oaknut.h>


static jclass jclassCamera;
static jmethodID jmidConstructor;
static jmethodID jmidOpen;
static jmethodID jmidStartPreview;
static jmethodID jmidStopPreview;
static jmethodID jmidClose;


static const char* VERTEX_SHADER =
        "precision mediump float;\n"
        "attribute highp vec2 vPosition;\n"
        "attribute vec2 texcoord;\n"
        "varying vec2 v_texcoord;\n"
        "uniform highp mat4 mvp;\n"
        "void main() {\n"
        "  gl_Position = mvp * vec4(vPosition,0,1);\n"
        "  v_texcoord = texcoord;\n"
        "}\n";

static const char* FRAGMENT_SHADER =
        "#extension GL_OES_EGL_image_external : require\n"
        "varying vec2 v_texcoord;\n"
        "uniform samplerExternalOES sampler;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(sampler, v_texcoord);\n"
        "}\n";


static GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = check_gl(glCreateShader, shaderType);
    if (shader) {
        check_gl(glShaderSource, shader, 1, &pSource, NULL);
        check_gl(glCompileShader, shader);
        GLint compiled = 0;
        check_gl(glGetShaderiv, shader, GL_COMPILE_STATUS, &compiled);
        assert (compiled);
    }
    return shader;
}


class Camera {
public:
    jobject camera;
    CameraPreviewDelegate delegate;
    GLuint fb;
    GLuint indexBufferId, vertexBufferId;
    GLuint program;
    GLuint posMvp;

    Camera() {
        JNIEnv* env = getJNIEnv();
        camera = env->NewObject(jclassCamera, jmidConstructor);
        camera = env->NewGlobalRef(camera);
        env->CallVoidMethod(camera, jmidOpen, (jlong)this, (jint)0);
        fb = 0;
    }
    ~Camera() {
        if (fb != 0) {
            check_gl(glDeleteFramebuffers, 1, &fb);
            check_gl(glDeleteBuffers, 1, &indexBufferId);
            check_gl(glDeleteBuffers, 1, &vertexBufferId);
        }

        JNIEnv* env = getJNIEnv();
        env->CallVoidMethod(camera, jmidClose);
        env->DeleteGlobalRef(camera);
    }
    void startPreview(CameraPreviewDelegate delegate) {
        this->delegate = delegate;
        getJNIEnv()->CallVoidMethod(camera, jmidStartPreview);
    }
    void stopPreview() {
        getJNIEnv()->CallVoidMethod(camera, jmidStopPreview);
    }
    
    void nativeOnGotFrame(int textureId, int width, int height, jfloat* transform) {

        GLint oldFBO, oldTex;
        GLint viewport[4], oldProg;
        GLint oldVertexBuffer, oldIndexBuffer;
        check_gl(glGetIntegerv, GL_FRAMEBUFFER_BINDING, &oldFBO);
        check_gl(glGetIntegerv, GL_TEXTURE_BINDING_2D, &oldTex);
        check_gl(glGetIntegerv, GL_VIEWPORT, viewport);
        check_gl(glGetIntegerv, GL_CURRENT_PROGRAM, &oldProg);
        check_gl(glGetIntegerv, GL_ARRAY_BUFFER_BINDING, &oldVertexBuffer);
        check_gl(glGetIntegerv, GL_ELEMENT_ARRAY_BUFFER_BINDING, &oldIndexBuffer);

        // Reset the canvas' vertex config since our binding to another VBO trashed existing attrib config
        app._window->_canvas->_currentVertexConfig = -1;

        // Create a framebuffer and a normal 2D texture to render to
        GLuint texId2 = 0;
        if (fb == 0) {
            check_gl(glGenFramebuffers, 1, &fb);
            check_gl(glBindFramebuffer, GL_FRAMEBUFFER, fb);
            // Create VBO
            check_gl(glGenBuffers, 1, &indexBufferId);
            check_gl(glGenBuffers, 1, &vertexBufferId);
            check_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
            check_gl(glBindBuffer, GL_ARRAY_BUFFER, vertexBufferId);
            GLshort indexes[] = {0,1,2,2,1,3};
            QUAD quad = QUADFromRECT(RECT(-1,0,2,2),0); // I don't know why y be 0 rather than -1 but it do...
            check_gl(glBufferData, GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * 6, indexes, GL_STATIC_DRAW);
            check_gl(glBufferData, GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_DYNAMIC_DRAW);

            // Create the shader
            GLuint vertexShader = loadShader(GL_VERTEX_SHADER, VERTEX_SHADER);
            GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
            program = check_gl(glCreateProgram);
            check_gl(glBindAttribLocation, program, VERTEXATTRIB_POSITION, "vPosition");
            check_gl(glBindAttribLocation, program, VERTEXATTRIB_TEXCOORD, "texcoord");
            check_gl(glAttachShader, program, vertexShader);
            check_gl(glAttachShader, program, pixelShader);
            check_gl(glLinkProgram, program);
            GLint linkStatus = GL_FALSE;
            check_gl(glGetProgramiv, program, GL_LINK_STATUS, &linkStatus);
            assert(linkStatus);

            posMvp = check_gl(glGetUniformLocation, program, "mvp");

        } else {
            check_gl(glBindFramebuffer, GL_FRAMEBUFFER, fb);
            check_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
            check_gl(glBindBuffer, GL_ARRAY_BUFFER, vertexBufferId);
        }

        check_gl(glGenTextures, 1, &texId2);
        check_gl(glBindTexture, GL_TEXTURE_2D, texId2);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        check_gl(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        check_gl(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        check_gl(glBindTexture, GL_TEXTURE_2D, 0);
        check_gl(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId2,0);
        //GLuint e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        //assert(e == GL_FRAMEBUFFER_COMPLETE);

        // Bind to the camera texture
        check_gl(glBindTexture, GL_TEXTURE_EXTERNAL_OES, textureId);

        // Render to texture
        check_gl(glUseProgram, program);
        check_gl(glUniformMatrix4fv, posMvp, 1, 0, transform);
        check_gl(glViewport, 0,0,width,height);
        check_gl(glVertexAttribPointer, VERTEXATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), 0);
        check_gl(glVertexAttribPointer, VERTEXATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)8);
        check_gl(glEnableVertexAttribArray, VERTEXATTRIB_POSITION);
        check_gl(glEnableVertexAttribArray, VERTEXATTRIB_TEXCOORD);
        check_gl(glDrawElements, GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

        // Finished
        check_gl(glBindTexture, GL_TEXTURE_2D, oldTex);
        check_gl(glBindFramebuffer, GL_FRAMEBUFFER, oldFBO);
        check_gl(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, oldIndexBuffer);
        check_gl(glBindBuffer, GL_ARRAY_BUFFER, oldVertexBuffer);
        check_gl(glViewport, viewport[0], viewport[1], viewport[2], viewport[3]);
        check_gl(glUseProgram, oldProg);


        ObjPtr<Bitmap> bitmap = new Bitmap(texId2);
        bitmap->_width = width;
        bitmap->_height = height;


        delegate(bitmap, 5.0);
    }

};

void* oakCameraOpen(int cameraId) {
    JNIEnv* env = getJNIEnv();
    jclassCamera = env->FindClass(PACKAGE "/Camera");
    jclassCamera = (jclass)env->NewGlobalRef(jclassCamera);
    jmidConstructor = env->GetMethodID(jclassCamera, "<init>", "()V");
    jmidOpen = env->GetMethodID(jclassCamera, "open", "(JI)V");
    jmidStartPreview = env->GetMethodID(jclassCamera, "startPreview", "()V");
    jmidStopPreview = env->GetMethodID(jclassCamera, "stopPreview", "()V");
    jmidClose = env->GetMethodID(jclassCamera, "close", "()V");
    return new Camera();
}
void oakCameraPreviewStart(void* oscamera, CameraPreviewDelegate delegate) {
    ((Camera*)oscamera)->startPreview(delegate);
}
void oakCameraPreviewStop(void* oscamera) {
    ((Camera*)oscamera)->stopPreview();
}
void oakCameraClose(void* oscamera) {
    delete (Camera*)oscamera;
}


JAVA_FN(void, Camera, nativeOnFrameAvailable)(JNIEnv *env, jobject oscamera, jlong camera, jint textureId, jint width, jint height, jfloatArray transform) {
    jfloat* transformVals = env->GetFloatArrayElements(transform, NULL);
    ((Camera*)camera)->nativeOnGotFrame(textureId, width, height, transformVals);
    env->ReleaseFloatArrayElements(transform, transformVals, 0);
}


#endif
