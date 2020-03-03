//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>
#include "MaskView.h"
#include "RecordingView.h"
#include "AudioRecordingVC.h"

static string BASEURL="https://check.gateway.dev.post-quantum.io";
//static string BASEURL="http://192.168.225.62:8083";
static string ORGANIZATION = "idaas";
static string USERNAME = "idaas";
static string PASSWORD = "password";


class RectDetectorWorker : public WorkerImpl {
public:

    variant process_(int msg, const variant& data_in) {
        int width = data_in.intVal("width");
        int height = data_in.intVal("height");
        const bytearray& pixels = data_in.bytearrayRef("data");
        RECT roi;
        roi.origin.x = data_in.intVal("roiLeft");
        roi.origin.y = data_in.intVal("roiTop");
        roi.size.width = data_in.intVal("roiWidth");
        roi.size.height = data_in.intVal("roiHeight");
        vector<RECT> result = detect(pixels, width, height, roi);
        variant retval;
        retval.setType(variant::ARRAY);
        for (int i=0 ; i<result.size() ; i++) {
            RECT& rect = result[i];
            variant vrect;
            vrect.set("x", rect.origin.x);
            vrect.set("y", rect.origin.y);
            vrect.set("width", rect.size.width);
            vrect.set("height", rect.size.height);
            retval.appendVal(vrect);
        }
        return retval;
    }

    
    vector<RECT> detect(const bytearray& pixels, int width, int height, RECT roi) {
        vector<RECT> rects;

        // Convert just the ROI to a new greyscale image
        int roiLeft = (int)roi.origin.x;
        int roiTop = (int)roi.origin.y;
        int roiWidth = (int)roi.size.width;
        int roiHeight = (int)roi.size.height;
        uint8_t* gray = (uint8_t*)malloc(roiWidth * roiHeight);
        uint32_t* pixels32 = (uint32_t*)pixels.data();
        int i=0;
        for (int y=roiTop; y<roiTop+roiHeight ; y++) {
            int yy = y*width;
            for (int x=roiLeft; x<roiLeft+roiWidth ; x++) {
                uint32_t c = pixels32[yy+x];
                uint8_t luma = ((c>>16&0xff) * 13933 + (c>>8&0xff) * 46871 + (c&0xff) * 4732)>>16;
                gray[i++] = luma;
            }
        }
        
        // From this point we disregard anything outside the ROI
        width = roiWidth;
        height = roiHeight;
        
        
        // Create the integral images
        int32_t* integralImage = (int32_t*) malloc(width * height * 4);
        int32_t* integralImageSquare = (int32_t*) malloc(width * height * 4);
        // top row
        int32_t pixAcc = 0;
        int32_t pixAccSq = 0;
        for (int j = 0; j < width; j++) {
            int32_t pixel = gray[j];
            pixAcc += pixel;
            pixAccSq += pixel*pixel;
            integralImage[j] = pixAcc;
            integralImageSquare[j] = pixAccSq;
        }
        // left column
        for (int i = 1; i < height; i++) {
            int w = i * width;
            int32_t pixel = gray[w];
            int32_t pixelSq = pixel*pixel;
            integralImage[w] = pixel + integralImage[w-width];
            integralImageSquare[w] = pixelSq + integralImageSquare[w-width];
        }
        // Remainder of image
        for (int i = 1; i < height; i++) {
            for (int j = 1; j < width; j++) {
                int w = i * width + j;
                int32_t pixel = gray[w];
                int32_t pixelSq = pixel * pixel;
                integralImage[w] = integralImage[w - width]     // 1 pixel above
                                 + integralImage[w - 1]         // 1 pixel to the left
                                 + pixel                        // current pixel
                                 - integralImage[w - width - 1];// minus pixel above-left.
                integralImageSquare[w] = integralImageSquare[w - width]
                                    + integralImageSquare[w - 1]
                                    + pixelSq
                                    - integralImageSquare[w - width - 1];
            }
        }
        
        
        // Apply a sliding rect across the image
        for (float scale=0.9; scale<=1.0; scale+=0.1) {
            // Credit cards are 3.370 ×2.125 in
            const float CARD_ASPECT = 3.370f/2.125f;
            
            int blockWidth = (scale * width);
            int blockHeight = blockWidth / CARD_ASPECT;
         
            float step = (blockWidth/8);
            
            // Scan the image a block at a time top->bottom, left->right. Blocks overlap a lot.
            for (int i = 0; i < (height - blockHeight); i += step) {
                for (int j = 0; j < (width - blockWidth); j += step) {
                    
                    float inverseArea = 1.0 / (blockWidth * blockHeight);
                    int wbA = i * width + j;
                    int wbB = wbA + blockWidth;
                    int wbD = wbA + blockHeight * width;
                    int wbC = wbD + blockWidth;
                    float mean = (integralImage[wbA] - integralImage[wbB] - integralImage[wbD] + integralImage[wbC]) * inverseArea;
                    float variance = (integralImageSquare[wbA] - integralImageSquare[wbB] - integralImageSquare[wbD] + integralImageSquare[wbC]) * inverseArea - mean * mean;
                    
                    float standardDeviation = 1;
                    if (variance > 0) {
                        standardDeviation = sqrt(variance);
                    }

                    // app->log("mean: %f  std: %f", mean, standardDeviation);
                }
            }
        }
        
        free(integralImage);
        free(integralImageSquare);
        free(gray);
        
        return rects;
    }
};

DECLARE_WORKER_IMPL(RectDetectorWorker);


class RectDetector : public Worker {
public:
    RectDetector() : Worker("RectDetectorWorker") {
        _maxQueueSize = 1;
    }

    void detect(class Bitmap* bitmap, const RECT& roiRect, std::function<void(vector<RECT>)> resultCallback) {
        variant data_in;
        data_in.set("width", bitmap->_width);
        data_in.set("height", bitmap->_height);
        data_in.set("roiLeft", roiRect.origin.x);
        data_in.set("roiTop", roiRect.origin.y);
        data_in.set("roiWidth", roiRect.size.width);
        data_in.set("roiHeight", roiRect.size.height);
        PIXELDATA pixelData;
        bitmap->lock(&pixelData, false);
        bytearray bytes((uint8_t*)pixelData.data, pixelData.cb);
        
        data_in.set("data", bytes);
        bitmap->unlock(&pixelData, false);
        enqueue(0, data_in, [=](const variant& data_out) {
            vector<RECT> results;
            auto& vresults = data_out.arrayRef();
            for (auto& vrect : vresults) {
                RECT rect;
                rect.origin.x = vrect.floatVal("x");
                rect.origin.y = vrect.floatVal("y");
                rect.size.width = vrect.floatVal("width");
                rect.size.height = vrect.floatVal("height");
                results.push_back(rect);
            }
            resultCallback(results);
        });
    }
};

class MainViewController : public ViewController {
public:

    Camera* _camera;
    CameraView* _cameraView;
    MaskView* _maskView;
    Label* _salutation;
    Label* _instruction;
    Label* _phrase;
    ImageView* _recordButton;
    //sp<RectDetector> _rectDetector;
    sp<FaceDetector> _faceDetector;
    //View* _facesOverlayView;
    sp<AudioInput> _audioInput;
    //Worker* _audioEncoder;
    bytearray _encodedAudio;
    Timer* _frameCaptureTimer;
    bool _frameCaptureNeeded;
    RIFFWriter* _riffWriter;
    bytestream* _aviData;
    RectRenderOp* _faceRectOp;
    AudioFormat _audioFormat;
    sp<AudioResampler> _audioResampler;
    sp<JpegEncoder> _jpegEncoder;

    MainViewController() {
        inflate("layout/main.res");
        bind(_cameraView, "cameraView");
        bind(_maskView, "maskView");
        bind(_salutation, "salutation");
        bind(_instruction, "instruction");
        bind(_phrase, "phrase");
        bind(_recordButton, "record");
        
        // Configure views
        _maskView->setHoleStrokeColour(app->getStyleColor("colorError"));
        _recordButton->onInputEvent = [=](View* view, INPUTEVENT* event) -> bool {
            if (event->type == INPUT_EVENT_DOWN) {
                startRecording();
            }
            if (event->type == INPUT_EVENT_UP) {
                stopRecording();
            }
            return true;
        };
        
        _jpegEncoder = new JpegEncoder();
        //_rectDetector = new RectDetector();
        _faceDetector = new FaceDetector();
        //_facesOverlayView = new View();
        //_facesOverlayView->setMeasureSpecs(MEASURESPEC::Fill(), MEASURESPEC::Fill());
        //_view->addSubview(_facesOverlayView);
        
        // Expected audio format
        _audioFormat.sampleRate = 16000;
        _audioFormat.numChannels = 1;
        _audioFormat.sampleType = AudioFormat::Int16;

    }
    
    void onWindowAttached() override {
        ViewController::onWindowAttached();
        
        // Do permissions check
        getWindow()->runWithPermissions({PermissionCamera,PermissionMic}, [=](vector<bool> granted) {
            if (granted[0] && granted[1]) {
                
                // Open front-facing camera
                _camera = Camera::create({
                    .frontFacing = true,
                    .frameSizeShort = 480,
                    .frameSizeLong = 0,
                    .frameRate = 30
                });
                _camera->open();
                _camera->onNewCameraFrame = [=](CameraFrame* frame) {
                    handleNewCameraFrame(frame);
                };
                _camera->start();
                _cameraView->show();
            } else {
                // TODO: 'permissions needed' idiot screen
            }
        });
    }
    
    void startRecording() {
        _frameCaptureTimer = Timer::start([=]() {
            _frameCaptureNeeded = true;
        }, 500, true);

        _audioInput = AudioInput::create();
        AudioFormat inputAudioFormat = _audioFormat;
#if PLATFORM_APPLE
        inputAudioFormat.sampleRate = 44100; // force the resampler to be used
#endif
        _audioInput->open(inputAudioFormat);
        
        // Resampling 44100 to 16000 takes a LONG time to set up the multistage
        // FIR filters - several seconds on a desktop. Shame it can't be precalculated
        // To avoid this crazy delay, just switch to 22050 as the output rate if input
        // is 44100. Maybe we need a better resampler lib.
        if (inputAudioFormat.sampleRate == 44100) {
            _audioFormat.sampleRate = 22050;
        }
        
        if (inputAudioFormat.sampleRate != _audioFormat.sampleRate) {
            _audioResampler = new AudioResampler();
            _audioResampler->onNewAudioSamples = [=](AudioSamples* samples) {
                variant indata = samples->getData();
                _encodedAudio.append(indata.bytearrayRef());
            };
            _audioResampler->open(inputAudioFormat, _audioFormat);
        }
        /*_audioEncoder = (Worker*)Object::createByName("Mp3Encoder");
        variant config;
        config.set("sampleRate", _audioFormat.sampleRate);
        _audioEncoder->start(config);*/
        _audioInput->onNewAudioSamples = [=](AudioSamples* samples) {
            if (_audioResampler) {
                _audioResampler->process(samples);
            } else {
                variant indata = samples->getData();
                _encodedAudio.append(indata.bytearrayRef());
            }
            /*_audioEncoder->process(indata, [=](const variant& outdata) {
                const bytearray& outbytes = outdata.bytearrayVal();
                if (outbytes.size() > 0) {
                    _encodedAudio.append(outbytes);
                }
            });*/
        };
        _audioInput->start();

    }
    
    void stopRecording() {
        _frameCaptureTimer->stop();
        _audioInput->stop();
        _audioInput->close();
        if (_audioResampler) {
            /*_audioResampler->stop([=]() {
                uploadRecording();
            });*/
        } else {
            uploadRecording();
        }
        //_audioEncoder->stop([=]() {

    }
    


    void handleNewCameraFrame(CameraFrame* frame) {
        // Ask camera to convert frame texture to bitmap
        sp<Bitmap> frameBitmap = frame->asBitmap();
        
        // Update camera preview view
        _cameraView->handleNewCameraFrame(frameBitmap);
        
        //if (!_rectDetector->isBusy()) {
        if (!_faceDetector->isBusy()) {
            
            // Determine the region of interest (ROI). It's the intersection of the mask hole rect with the
            // image frame (which may exceed the view bounds cos its aspect-fill), then scaled down
            // to the original camera frame size.
            RECT upscaledImageRect = _cameraView->getDisplayedFrameRect(); 
            if (upscaledImageRect.size.width<=0 || upscaledImageRect.size.height<=0) {
                return; // nothing rendered yet so don't know what size things are
            }
            POINT o = upscaledImageRect.origin;
            RECT roiRect = _maskView->getHoleRect();
            roiRect.intersectWith(upscaledImageRect);
            roiRect.origin -= o;
            roiRect.inset(-app->dp(32),-app->dp(32)); // a small amount of inset cos the haar face cascade cant see faces at the very edges.
            float scaleX = upscaledImageRect.size.width / frameBitmap->_width;
            float scaleY = upscaledImageRect.size.height / frameBitmap->_height;
            roiRect.scale(1/scaleX, 1/scaleY);
            
            /*_rectDetector->detect(frameBitmap, roiRect, [=](vector<RECT> rects) {
                app->log("Rects: %d", rects.size());
            });*/
            
            _faceDetector->detectFaces(frameBitmap, roiRect, [=](vector<RECT> faces) {
                //app->log("Faces: %d", faces.size());
                if (faces.size() == 0) {
                    _maskView->setHoleStrokeColour(app->getStyleColor("colorError"));
                    return;
                }
                _maskView->setHoleStrokeColour(app->getStyleColor("colorSuccess"));
            });
            
        }
        
        // Frame capture
        if (_frameCaptureNeeded) {
            
            // Can only instantiate the AVI writer once the frame size is known
            if (!_riffWriter) {
                _aviData = new bytestream(256*1024);
                _riffWriter = new RIFFWriter(*_aviData);
                _riffWriter->startWriteAVI(1, frameBitmap->_width, frameBitmap->_height, 1);
                _riffWriter->writeStreamHeader(RIFFWriter::MJPG_CC);
            }

            _jpegEncoder->encode(frameBitmap, [=](const bytearray& jpeg) {
                app->log("jpeg size : %d", jpeg.size());
                _riffWriter->writeChunk(jpeg);

            });
            
            _frameCaptureNeeded = false;
        }
        
    }
    
    
    
    void uploadRecording() {
        //if (!_riffWriter) {
        //    return;s
        //}
        _riffWriter->close();
        
        app->log("total avi : %d", _aviData->size());
        app->log("total audio: %d", _encodedAudio.size());
        

        
        /*
        string url = string::format("%s/auth/admin/tokens/%s", BASEURL.data(), ORGANIZATION.data());
        auto req = URLRequest::get(url);
        string userPasswordData = string::format("%s:%s", USERNAME.data(), PASSWORD.data());
        req->setHeader("Authorization", string::format("Basic %s", base64_encode(userPasswordData).data()));
        req->setHeader("Accept", "application/vnd.net.postquantum.idaas.api.access.token.v1+json");
        req->handleJson([=](URLRequest* req, const variant& json) {
            if (req->didError()) {
                app.log("Error: %d, %s", req->getHttpStatus(), json.stringVal("message").data());
                return;
            }
            string access_token = json.stringVal("access_token");
            
            // Create user
            variant data;
            data.setType(variant::MAP);
            string url = string::format("%s/users", BASEURL.data());
            req = URLRequest::post(url, data.toJson().toByteArray(false));
            req->setHeader("Authorization", string::format("Bearer %s", access_token.data()));
            req->setHeader("Content-Type", "application/vnd.net.postquantum.check.api.user.token.v0.1+json");
            req->handleJson([=](URLRequest* req, const variant& v) {
                if (req->didError()) {
                    app.log("Error: %d, %s", req->getHttpStatus(), v.stringVal("message").data());
                    return;
                }
                const variant* admin_token = v.get("token");
                string user_token = admin_token->stringVal("access_token");
                app.log("user_token is %s", user_token.data());
                
                // Generate a multipart http boday containing the binary .avi
                string body = "--X-BOUNDARY\r\n"
                              "Content-Disposition: form-data; name=\"type\"\r\n"
                              "Content-Type: text/plain\r\n"
                              "\r\n"
                              "face\r\n"
                              "--X-BOUNDARY\r\n"
                              "Content-Disposition: form-data; name=\"sample\"; filename=\"sample.avi\"\r\n"
                              "Content-Transfer-Encoding: binary\r\n"
                              "Content-Type: video/x-msvideo\r\n"
                              "\r\n";
                bytearray bb = body.toByteArray(false);
                bb.append(_aviData->_data.data, (int)_aviData->_data.cb);
                bb.append(string("\r\n--X-BOUNDARY--\r\n").toByteArray(false));
                
                // Post the AVI
                string url = string::format("%s/register", BASEURL.data());
                req = URLRequest::post(url, bb);
                req->setHeader("Authorization", string::format("Bearer %s", user_token.data()));
                req->setHeader("Content-Type", "multipart/form-data; boundary=X-BOUNDARY");
                req->setHeader("Accept", "*\/*");
                req->handleJson([](URLRequest* req, const variant& v) {
                    app.log("I POSTed a sample! %d", req->getHttpStatus());
                    if (req->didError()) {
                        app.log("Error: %d, %s", req->getHttpStatus(), v.stringVal("message").data());
                        return;
                    }
                });
            });
        });*/
        

#if PLATFORM_WEB
         //auto str = base64_encode(_encodedAudio);
         auto str = base64_encode(_aviData->_data.data, _aviData->offsetWrite);
         
         EM_ASM({
         var data = Pointer_stringify($0);
         var element = document.createElement('a');
         element.setAttribute('href', 'data:video/avi;base64,' + data);
         element.setAttribute('download', 'foo.avi');
         //element.setAttribute('href', 'data:audio/wav;base64,' + data);
         //element.setAttribute('download', 'foo3.wav');
         //element.setAttribute('href', 'data:audio/mp3;base64,' + data);
         //element.setAttribute('download', 'foo.mp3');
         //element.style.display = 'none';
         element.innerHtml = 'CLICK ME!';
         document.body.appendChild(element);
         element.click();
         //document.body.removeChild(element);
         }, str.data());
         
#else
         FILE* tmp = fopen("foo.avi", "wb");
         fwrite(_aviData->data(), _aviData->offsetWrite, 1, tmp );
         //FILE* tmp = fopen("foo.aac", "wb");
         //fwrite(_encodedAudio.data(), _encodedAudio.size(), 1, tmp );
         fclose(tmp);
#endif

        //_aviData->offsetWrite = 0;
        //_riffWriter->writeWavFile(_audioFormat, _encodedAudio);


    }
    
    /*
    void encodeFrame(Bitmap* frameBitmap, PIXELDATA pixdata) {
        
     
        Task::postToMainThread([=]() {
        PIXELDATA pixdata2 = pixdata;
        frameBitmap->unlock(&pixdata2, false);
        frameBitmap->release();
        });

        
        // Pass to face detector, if it's not still processing an earlier frame
        if (!_faceDetectorBusy) {
            _faceDetectorBusy = true;
            
            // Get the region of interest of the camera image into a bitmap
            // This is expensive and it's a shame we have to do this on the main thread
            // TODO: code appears to get whole image, surely should be just ROI?
            Bitmap *bitmap = Bitmap::create(frameBitmap->_width, frameBitmap->_height, BITMAPFORMAT_RGBA32);
            bitmap->retain(); // explicit retain(), release() runs after face detection done
            PIXELDATA pixelData;
            bitmap->lock(&pixelData, false);
            GLint oldFBO, oldTex;
     assert(0); // dont do glGets!
            //glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
            //glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTex);
            GLuint fb = 0;
            glGenFramebuffers(1, &fb);
            glBindFramebuffer(GL_FRAMEBUFFER, fb);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   frameBitmap->_textureId, 0);
            glBindTexture(GL_TEXTURE_2D, frameBitmap->_textureId);
     assert(0); // dont do glReadPixels like this!
            check_gl(glReadPixels, 0, 0, frameBitmap->_width, frameBitmap->_height, GL_RGBA,
                     GL_UNSIGNED_BYTE, pixelData.data);
            glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
            glBindTexture(GL_TEXTURE_2D, oldTex);
            glDeleteFramebuffers(1, &fb);
            bitmap->unlock(&pixelData, false);
            
            _faceDetectorQueue->enqueueTask([=]() {
                int numFaces = _faceDetector->detectFaces(bitmap);
                Task::postToMainThread([=]() {
                    bitmap->release(); // matches retain() some lines up
                    if (numFaces != _numFacesDetected) {
                        _numFacesDetected = numFaces;
                        onNumFacesDetectedChanged();
                    }
                    _faceDetectorBusy = false;
                });
            });
        }
    }*/
    

};



class SampleApp : public App {
    
    void main() override {
    
    //_window->setRootViewController(new AudioRecordingViewController());
    
    _window->setRootViewController(new MainViewController());
    
    }
} the_app;



/*if (!_faceRectOp) {
    _faceRectOp = new RectRenderOp();
    _faceRectOp->setFillColor(0x4000ff00);
    _facesOverlayView->addRenderOp(_faceRectOp);
}
RECT rect = faces[0];
rect.scale(scaleX, scaleY);
rect.origin += o;
_faceRectOp->setRect(rect);
_facesOverlayView->setNeedsFullRedraw();*/
