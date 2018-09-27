
class CameraFrameAndroid : public CameraFrame {
public:
    GLuint fb;
    GLuint indexBufferId, vertexBufferId;
    GLuint program;
    GLuint posMvp;
    
    CameraFrameAndroid();
    ~CameraFrameAndroid();
    
    virtual Bitmap* asBitmap() override;
};

