
/*class TextureConverter {
public:
    GLuint fb;
    GLuint indexBufferId, vertexBufferId;
    GLuint program;
    GLuint matrixST;

    TextureConverter();
    ~TextureConverter();

    int convert(GLuint texId, int width, int height, float* transform);

};*/



class CameraFrameAndroid : public CameraFrame {
public:
    ~CameraFrameAndroid();
    sp<Bitmap> _bitmap;
    float _transform[16];
    jbyteArray _jbytes;


    virtual Bitmap* asBitmap() override;
};

