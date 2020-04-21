package oaknut;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.Surface;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.HashMap;


class VideoRecorder  {

    static final Charset UTF_8 = Charset.forName("UTF-8");

    long nativePtr;
    String outputPath;
    int videoWidth; // size of the recorded video output, not necessarily same as captured frame size
    int videoHeight;
    int desiredFrameRate;
    int desiredKeyframeRate;
    int audioSampleRate;
    int audioBitRate;
    EGLContext sharedContext;
    WorkerThread workerThread;


    VideoRecorder(long nativePtr, byte[] outputPath) {
        this.nativePtr = nativePtr;
        sharedContext = EGL14.eglGetCurrentContext();
        this.outputPath = new String(outputPath, UTF_8);
    }

    void start(int videoWidth, int videoHeight, int desiredFrameRate, int desiredKeyframeRate, int audioSampleRate) {
        assert(desiredFrameRate>=1);
        this.videoWidth = videoWidth;
        this.videoHeight = videoHeight;
        this.desiredFrameRate = desiredFrameRate;
        this.desiredKeyframeRate = desiredKeyframeRate;
        this.audioSampleRate = audioSampleRate;
        if (audioSampleRate > 0) {
            audioBitRate = supportedAACbitRates.get(audioSampleRate); // exception? Maybe you asked for unsupported audio rate
        }

        file = new File(outputPath);
        if (file.exists()) {
            file.delete();
        }

        workerThread = new WorkerThread();
        workerThread.start();
    }
    void stop() {
        if (workerThread != null) {
            Message.obtain(workerThread.handler, WorkerThread.MSG_STOP).sendToTarget();
            try {
                workerThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            workerThread = null;
        }
    }
    void handleNewCameraFrame(int textureId, long timestamp, float[] transform) {
        if (workerThread == null) {
            return;
        }

        // Android cameras tend to disregard low framerates and produce many more frames than were
        // asked for, so a simple timestamp filter is used here to present them to the codec at the desired rate.
        if (timestamp < lastFrameTimestampInNanos + (1000000000/desiredFrameRate)) {
            return;
        }
        lastFrameTimestampInNanos = timestamp;

        // Send the new frame to the worker thread
        FrameInfo frameInfo = new FrameInfo();
        frameInfo.cameraTextureId = textureId;
        frameInfo.timestampInNanos = timestamp;
        frameInfo.transform = transform;
        workerThread.dispatchNewFrame(frameInfo);
    }

    void handleNewAudioSamples(byte[] samples) {
        if (workerThread == null) {
            return;
        }
        workerThread.dispatchNewAudio(samples);

    }

    // Audio bitrates. The key for this map is the supported sample rate (all AAC LC rates are here)
    // See http://wiki.hydrogenaud.io/index.php?title=Fraunhofer_FDK_AAC#Recommended_Sampling_Rate_and_Bitrate_Combinations
    static HashMap<Integer,Integer> supportedAACbitRates = new HashMap<>();
    static {
        supportedAACbitRates.put(11025, 10000);
        supportedAACbitRates.put(12000, 12000);
        supportedAACbitRates.put(16000, 16000);
        supportedAACbitRates.put(22050, 24000);
        supportedAACbitRates.put(24000, 32000);
        supportedAACbitRates.put(32000, 44000);
        supportedAACbitRates.put(44100, 108000);
        supportedAACbitRates.put(48000, 220000);
    }

    private long lastFrameTimestampInNanos;

    class FrameInfo {
        int cameraTextureId;
        long timestampInNanos;
        float[] transform;
    }

    class WorkerThread extends HandlerThread {
        WorkerThread() {
            super("VideoRecorder");
        }


        void dispatchNewFrame(FrameInfo frameInfo) {
            if (handler == null) { // thread not started yet!
                return;
            }
           Message.obtain(handler, MSG_NEW_FRAME, frameInfo).sendToTarget();
        }

        void dispatchNewAudio(byte[] samples) {
            if (handler == null) { // thread not started yet!
                return;
            }
            Message.obtain(handler, MSG_NEW_AUDIO, samples).sendToTarget();
        }


        static final int MSG_NEW_FRAME = 1;
        static final int MSG_NEW_AUDIO = 2;
        static final int MSG_STOP = 3;

        private Handler handler;
        private GLRenderer renderer;
        private Surface encoderSurface;
        private EGLSurface eglEncoderSurface;
        private MediaCodec videoCodec;
        private MediaCodec audioCodec;
        private int videoTrackIndex = -1;
        private int audioTrackIndex = -1;
        private MediaMuxer mediaMuxer;
        private boolean muxerStarted;
        private boolean seenVideoEOS;
        private long started;
        private MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
        private long audioTimestamp = 0;
        private boolean stopped;

        @Override
        protected void onLooperPrepared() {
            handler = new Handler(getLooper()) {
                @Override
                public void handleMessage(Message msg) {
                    switch (msg.what) {
                        case MSG_NEW_FRAME:
                            handleNewFrame((FrameInfo)msg.obj);
                            break;
                        case MSG_NEW_AUDIO:
                            handleNewAudio((byte[])msg.obj);
                            break;
                        case MSG_STOP:
                            handleStop();
                            break;
                    }
                }
            };
            handleInit();
        }
        private void handleInit() {
            try {

                // Create video codec
                //int videoWidth = cameraView.previewSize.width;
                //int videoHeight = cameraView.previewSize.height;
                videoCodec = MediaCodec.createEncoderByType("video/avc");
                MediaFormat format = MediaFormat.createVideoFormat("video/avc", videoWidth, videoHeight);
                format.setInteger(MediaFormat.KEY_COLOR_FORMAT,
                        MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
                // Set a sensible bit rate. Our aim is for high-quality low-framerate vids,
                // we don't actually care about bitrate but unfortunately some AVC encoder
                // implementations do care so we have to be sensible. The given calculation provides
                // videos at an acceptable size (c.1.5MB for a 5 second clip) with no noticeable
                // drop in quality w.r.t. a crazy high bitrate. Some phones native crash if this isn't sensible.
                format.setInteger(MediaFormat.KEY_BIT_RATE, videoWidth * videoHeight * 2 * desiredFrameRate);
                format.setInteger(MediaFormat.KEY_FRAME_RATE, desiredKeyframeRate);
                format.setInteger("capture-rate", desiredFrameRate);
                format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, desiredFrameRate);
                videoCodec.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);

                // Get video encoding surface. We will render camera frames to it.
                encoderSurface = videoCodec.createInputSurface();

                // Create audio codec
                if (audioSampleRate > 0) {
                    audioCodec = MediaCodec.createEncoderByType("audio/mp4a-latm");
                    MediaFormat audioFormat = MediaFormat.createAudioFormat("audio/mp4a-latm", audioSampleRate, 1);
                    audioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
                    audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, audioBitRate);
                    audioCodec.configure(audioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
                }

                // Create muxer and start the codecs
                mediaMuxer = new MediaMuxer(file.getAbsolutePath(), MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
                videoCodec.start();
                if (audioCodec != null) {
                    audioCodec.start();
                }

                started = System.currentTimeMillis();
            }
            catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
        private void handleNewFrame(FrameInfo frameInfo) {
            if (stopped) {
                return;
            }
            drainCodecs();

            // One-time GL setup for rendering camera texture to encoder surface
            if (renderer == null) {
                renderer = new GLRenderer(sharedContext, false);
                eglEncoderSurface = renderer.createSurface(encoderSurface);
                renderer.makeCurrent(eglEncoderSurface);
                GLES20.glViewport(0, 0, videoWidth, videoHeight);
            }

            // Draw to the encoder surface
            renderer.prepareToDraw(frameInfo.cameraTextureId, frameInfo.transform);
            renderer.draw(eglEncoderSurface, frameInfo.timestampInNanos);

            drainCodecs();
        }
        private void handleNewAudio(byte[] capturedBytes) {
            if (stopped) {
                return;
            }
            int offset = 0;
            int count = capturedBytes.length;
            while (count > 0) {
                drainCodecs();
                int inputBufferIndex = audioCodec.dequeueInputBuffer(-1);
                ByteBuffer[] inputBuffers = audioCodec.getInputBuffers();
                ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
                int cbThis = Math.min(count, inputBuffer.capacity());
                inputBuffer.clear();
                inputBuffer.put(capturedBytes, offset, cbThis);
                audioCodec.queueInputBuffer(inputBufferIndex, 0, cbThis, audioTimestamp, 0);
                audioTimestamp += ((cbThis / 2) * 1000000) / audioSampleRate;
                offset += cbThis;
                count -= cbThis;
            }

        }

        private void handleStop() {
            stopped = true;
            drainCodecs();
            videoCodec.signalEndOfInputStream();
            while (!seenVideoEOS) {
                drainCodecs();
                if (!seenVideoEOS) {
                    try {
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0);
            renderer.makeCurrent(null);
            renderer.destroySurface(eglEncoderSurface);
            encoderSurface.release();
            renderer.destroy();
            renderer = null;

            videoCodec.stop();
            videoCodec.release();

            if (audioCodec != null) {
                audioCodec.stop();
                audioCodec.release();
            }

            try {
                mediaMuxer.stop();
            } catch (java.lang.IllegalStateException e) {
                // Start and stop enough checks and eventually this exception could be thrown
                // by MediaMuxer's native code (see https://android.googlesource.com/platform/frameworks/base/+/master/media/jni/android_media_MediaMuxer.cpp#233)
                // I am reasonably sure it's harmless and can be ignored
            }
            mediaMuxer.release();

            quitSafely();
        }



        private void drainCodecs() {

            // Drain video codec
            int outputBufferId;
            while (MediaCodec.INFO_TRY_AGAIN_LATER != (outputBufferId= videoCodec.dequeueOutputBuffer(bufferInfo, 0))) {
                if (outputBufferId >= 0) {
                    ByteBuffer buffers[] = videoCodec.getOutputBuffers();
                    ByteBuffer buffer = buffers[outputBufferId];
                    bufferInfo.presentationTimeUs = (System.currentTimeMillis() - started) * 1000;
                    if (muxerStarted) {
                        mediaMuxer.writeSampleData(videoTrackIndex, buffer, bufferInfo);
                    }
                    videoCodec.releaseOutputBuffer(outputBufferId, false);
                    if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                        seenVideoEOS = true;
                    }
                } else if (outputBufferId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    videoTrackIndex = mediaMuxer.addTrack(videoCodec.getOutputFormat());
                    //if (audioTrackIndex != -1) {
                        mediaMuxer.start();
                        muxerStarted = true;
                    //}
                }
            }

            // Drain audio codec output
            if (audioCodec != null) {
                while (MediaCodec.INFO_TRY_AGAIN_LATER != (outputBufferId = audioCodec.dequeueOutputBuffer(bufferInfo, 0))) {
                    if (outputBufferId >= 0) {
                        ByteBuffer buffers[] = audioCodec.getOutputBuffers();
                        ByteBuffer buffer = buffers[outputBufferId];
                        if (muxerStarted) {
                            mediaMuxer.writeSampleData(audioTrackIndex, buffer, bufferInfo);
                        }
                        audioCodec.releaseOutputBuffer(outputBufferId, false);
                    } else if (outputBufferId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                        audioTrackIndex = mediaMuxer.addTrack(audioCodec.getOutputFormat());
                        if (videoTrackIndex != -1) {
                            mediaMuxer.start();
                            muxerStarted = true;
                        }
                    }
                }
            }
        }

    };

    private File file;



}
