//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#if PLATFORM_ANDROID

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
        "uniform highp mat4 matrixST;\n"
        "void main() {\n"
        "  gl_Position = vec4(vPosition,0,1);\n"
        "  v_texcoord = vec4(matrixST * vec4(texcoord,0,1)).xy;\n"
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


TextureConverter::TextureConverter() {
    fb = 0;
}
TextureConverter::~TextureConverter() {
    if (fb != 0) {
        check_gl(glDeleteFramebuffers, 1, &fb);
        check_gl(glDeleteBuffers, 1, &indexBufferId);
        check_gl(glDeleteBuffers, 1, &vertexBufferId);
        fb = 0;
    }
}

int TextureConverter::convert(GLuint texId, int width, int height, float* transform) {
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
    app._window->_currentVertexConfig = -1; // todo: THIS IS HIDEOUS! Find a way we dont have to do this

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
        QUAD quad = QUAD(RECT(-1,1,2,-2),0); // the default GL coordinate space, upside down
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

        matrixST = check_gl(glGetUniformLocation, program, "matrixST");

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

    // Bind to the input texture
    check_gl(glBindTexture, GL_TEXTURE_EXTERNAL_OES, texId);

    // Render to texture
    check_gl(glUseProgram, program);
    check_gl(glUniformMatrix4fv, matrixST, 1, 0, transform);
    check_gl(glViewport, 0,0,width,height);
    //glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
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

    return texId2;

}

Bitmap* CameraFrameAndroid::asBitmap() {
    return _bitmap;
};


class CameraAndroid : public Camera {
public:
    jobject camera;
    TextureConverter _converter;

    CameraAndroid(const Options& options) : Camera(options) {
        JNIEnv *env = getJNIEnv();
        jclassCamera = env->FindClass(PACKAGE "/Camera");
        jclassCamera = (jclass) env->NewGlobalRef(jclassCamera);
        jmidConstructor = env->GetMethodID(jclassCamera, "<init>", "()V");
        jmidOpen = env->GetMethodID(jclassCamera, "open", "(JI)V");
        jmidStartPreview = env->GetMethodID(jclassCamera, "startPreview", "()V");
        jmidStopPreview = env->GetMethodID(jclassCamera, "stopPreview", "()V");
        jmidClose = env->GetMethodID(jclassCamera, "close", "()V");
    }


    void open() override {
        JNIEnv *env = getJNIEnv();
        camera = env->NewObject(jclassCamera, jmidConstructor);
        camera = env->NewGlobalRef(camera);
        env->CallVoidMethod(camera, jmidOpen, (jlong)this, (jint)0);

        _previewWidth = env->GetIntField(camera, env->GetFieldID(jclassCamera, "previewWidth", "I"));
        _previewHeight = env->GetIntField(camera, env->GetFieldID(jclassCamera, "previewHeight", "I"));
    }

    void start() override {
        getJNIEnv()->CallVoidMethod(camera, jmidStartPreview);
    }
    void stop() override {
        if (camera) {
            getJNIEnv()->CallVoidMethod(camera, jmidStopPreview);
        }
    }

    void close() override {
        if (camera) {
            JNIEnv *env = getJNIEnv();
            env->CallVoidMethod(camera, jmidClose);
            env->DeleteGlobalRef(camera);
            camera = NULL;
        }
    }


};

Camera* Camera::create(const Options& options) {
    return new CameraAndroid(options);
}


JAVA_FN(void, Camera, nativeOnFrameAvailable)(JNIEnv *env, jobject obj, jlong cameraPtr, jint textureId, jlong timestamp, jint width, jint height, jfloatArray jtransform) {
    CameraAndroid* camera = (CameraAndroid*)cameraPtr;
    sp<CameraFrameAndroid> frame = new CameraFrameAndroid();
    env->GetFloatArrayRegion(jtransform, 0, 16, frame->_transform);
    auto texId2 = camera->_converter.convert(textureId, width, height, frame->_transform);
    frame->_bitmap = new BitmapAndroid(texId2);
    frame->_bitmap->_width = width;
    frame->_bitmap->_height = height;
    frame->_textureId = textureId;
    frame->_timestamp = timestamp;
    frame->_width = width;
    frame->_height = height;
    camera->onNewCameraFrame(frame);
}


#endif
