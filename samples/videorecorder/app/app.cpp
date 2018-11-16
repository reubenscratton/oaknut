//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>
#include "MaskView.h"

static string BASEURL="https://check.gateway.dev.post-quantum.io";
//static string BASEURL="http://192.168.225.62:8083";
static string ORGANIZATION = "idaas";
static string USERNAME = "idaas";
static string PASSWORD = "password";

class MainViewController : public ViewController {
public:

    Camera* _camera;
    CameraView* _cameraView;
    MaskView* _maskView;
    Label* _salutation;
    Label* _instruction;
    Label* _phrase;
    ImageView* _recordButton;
    FaceDetector* _faceDetector;
    AudioInput* _audioInput;
    Worker* _audioEncoder;
    bytearray _encodedAudio;
    Timer* _frameCaptureTimer;
    bool _frameCaptureNeeded;
    AVIWriter* _aviWriter;
    ByteBufferStream* _aviData;

    MainViewController() {
        inflate("layout/main.res");
        bind(_cameraView, "cameraView");
        bind(_maskView, "maskView");
        bind(_salutation, "salutation");
        bind(_instruction, "instruction");
        bind(_phrase, "phrase");
        bind(_recordButton, "record");
        
        // Configure views
        _maskView->setHoleStrokeColour(app.getStyleColor("colorError"));
        _recordButton->onInputEvent = [=](View* view, INPUTEVENT* event) -> bool {
            if (event->type == INPUT_EVENT_DOWN) {
                //SIZE size =  {(float)_camera->_previewWidth, (float)_camera->_previewHeight};
                startRecording();
            }
            if (event->type == INPUT_EVENT_UP) {
                stopRecording();
            }
            return true;
        };
        
        _faceDetector = new FaceDetector();
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

        AudioFormat audioFormat;
        audioFormat.sampleRate = 22050;
        audioFormat.numChannels = 1;
        audioFormat.sampleType = AudioFormat::Int16;
        _audioInput = AudioInput::create();
        _audioInput->open(audioFormat);
        _audioEncoder = (Worker*)Object::createByName("Mp3Encoder");
        variant config;
        config.set("sampleRate", audioFormat.sampleRate);
        _audioEncoder->start(config);
        _audioInput->onNewAudioSamples = [=](AudioInputSamples* samples) {
            variant indata = samples->getData();
            _audioEncoder->process(indata, [=](const variant& outdata) {
                const bytearray& outbytes = outdata.bytearrayVal();
                if (outbytes.size() > 0) {
                    _encodedAudio.append(outbytes);
                }
            });
        };
        _audioInput->start();

    }
    
    void stopRecording() {
        _frameCaptureTimer->stop();
        _audioInput->stop();
        _audioInput->close();
        _audioEncoder->stop([=]() {

            if (!_aviWriter) {
                return;
            }

            _aviWriter->close();
            
            app.log("total avi : %d", _aviData->_data.cb);
            app.log("total encoded: %d", _encodedAudio.size());
            
            string url = string::format("%s/auth/admin/tokens/%s", BASEURL.data(), ORGANIZATION.data());
            auto req = URLRequest::get(url);
            string userPasswordData = string::format("%s:%s", USERNAME.data(), PASSWORD.data());
            req->setHeader("Authorization", string::format("Basic %s", base64_encode(userPasswordData).data()));
            req->setHeader("Accept", "application/vnd.net.postquantum.idaas.api.access.token.v1+json");
            req->handleJson([=](URLRequest* req, const variant& json) {
                if (req->error()) {
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
                    if (req->error()) {
                        app.log("Error: %d, %s", req->getHttpStatus(), v.stringVal("message").data());
                        return;
                    }
                    const variant* admin_token = v.get("token");
                    string user_token = admin_token->stringVal("access_token");
                    
                    // Post the AVI
                    string body = "--X-INSOMNIA-BOUNDARY\r\n"
                                  "Content-Disposition: form-data; name=\"type\"\r\n"
                                  "Content-Type: text/plain\r\n"
                                  "\r\n"
                                  "face\r\n";
                    body +=       "--X-INSOMNIA-BOUNDARY\r\n"
                                  "Content-Disposition: form-data; name=\"sample\"; filename=\"foo.avi\"\r\n"
                                  //"Content-Type: application/octet-stream\r\n"
                                  "Content-Transfer-Encoding: binary\r\n"
                                  "Content-Type: video/x-msvideo\r\n"
                                  //"Content-Transfer-Encoding: base64\r\n"
                                  "\r\n";
                    bytearray bb = body.toByteArray(false);
                    bb.append(_aviData->_data.data, (int)_aviData->_data.cb);
                    bb.append(string("\r\n--X-INSOMNIA-BOUNDARY--\r\n").toByteArray(false));
                    //body.append(base64_encode(_aviData->_data.data, (int)_aviData->_data.cb));
                    //body.append("--X-INSOMNIA-BOUNDARY--\r\n");
                    //bytearray bb = body.toByteArray(false);
                    
                    string url = string::format("%s/register", BASEURL.data());
                    req = URLRequest::post(url, bb);
                    req->setHeader("Authorization", string::format("Bearer %s", user_token.data()));
                    req->setHeader("Content-Type", "multipart/form-data; boundary=X-INSOMNIA-BOUNDARY");
                    //req->setHeader("Content-Length", string::format("%d", bb.size() -15));
                    req->setHeader("Accept", "*/*");
                    req->handleJson([](URLRequest* req, const variant& v) {
                        app.log("I POSTed a sample! %d", req->getHttpStatus());
                        if (req->error()) {
                            app.log("Error: %d, %s", req->getHttpStatus(), v.stringVal("message").data());
                            return;
                        }
                    });
                });

                
            });
 
/*
#if PLATFORM_WEB
            //auto str = base64_encode(_encodedAudio);
            auto str = base64_encode(_aviData->_data.data, _aviData->offsetWrite);

            EM_ASM({
                var data = Pointer_stringify($0);
                var element = document.createElement('a');
                element.setAttribute('href', 'data:video/avi;base64,' + data);
                element.setAttribute('download', 'foo_yilin.avi');
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
            fwrite(_aviData->_data.data, _aviData->offsetWrite, 1, tmp );
            //FILE* tmp = fopen("foo.aac", "wb");
            //fwrite(_encodedAudio.data(), _encodedAudio.size(), 1, tmp );
            fclose(tmp);
#endif
 */
        });
    }
    


    void handleNewCameraFrame(CameraFrame* frame) {
        // Ask camera to convert frame texture to bitmap
        sp<Bitmap> frameBitmap = frame->asBitmap();
        
        // Update camera preview view
        _cameraView->handleNewCameraFrame(frameBitmap);
        
        if (!_faceDetector->isBusy()) {
            _faceDetector->detectFaces(frameBitmap, [=](int numFaces) {
                app.log("Detected %d faces", numFaces);
            });
        }
        
        // Frame capture
        if (_frameCaptureNeeded) {
            
            // Can only instantiate the AVI writer once the frame size is known
            if (!_aviWriter) {
                _aviData = new ByteBufferStream(256*1024);
                _aviWriter = new AVIWriter(_aviData, frameBitmap->_width, frameBitmap->_height, 1);
                _aviWriter->startWriteAVI(1);
                _aviWriter->writeStreamHeader(AVIWriter::MJPG_CC);
            }

            bytearray jpeg = frameBitmap->toJpeg(0.9);
            app.log("Result of toJpeg() : %d", jpeg.size());
            _aviWriter->writeChunk(jpeg);
            _frameCaptureNeeded = false;
        }
        
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
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTex);
            GLuint fb = 0;
            glGenFramebuffers(1, &fb);
            glBindFramebuffer(GL_FRAMEBUFFER, fb);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   frameBitmap->_textureId, 0);
            glBindTexture(GL_TEXTURE_2D, frameBitmap->_textureId);
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



void App::main() {
    
    _window->setRootViewController(new MainViewController());
}


