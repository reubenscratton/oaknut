
class TextureConverter {
public:
    GLuint fb;
    GLuint indexBufferId, vertexBufferId;
    GLuint program;
    GLuint matrixST;

    TextureConverter();
    ~TextureConverter();

    int convert(GLuint texId, int width, int height, float* transform);

};



class CameraFrameAndroid : public CameraFrame {
public:
    ObjPtr<Bitmap> _bitmap;
    float _transform[16];


    virtual Bitmap* asBitmap() override;
};

