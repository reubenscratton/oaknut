package org.oaknut.main;

import android.graphics.SurfaceTexture;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.opengl.GLES11Ext;
import android.os.Handler;
import android.os.SystemClock;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.lang.reflect.Method;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.concurrent.Semaphore;

import static android.opengl.GLES20.*;

/**
 * VideoPlayer : decodes an .mp4 or other video asset into a sequence of video frames and
 * audio packets.
 *
 */
public class VideoPlayer {

    static final Charset UTF_8 = Charset.forName("UTF-8");

    abstract class Track {
        int trackIndex;
        long latencyUs;
        MediaCodec codec;
        abstract boolean handleOutputBuffer(int outputBufferIndex, int bufferOffset, int bufferSize);

        void handleOutputFormatChanged(MediaFormat newOutputFormat) {

        }

        Thread thread = new Thread() {
            @Override
            public void run() {
                MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();

                while (true) {

                    // Wait for next frame to arrive
                    int outputBufferIndex = codec.dequeueOutputBuffer(info, -1);
                    if (outputBufferIndex < 0) {
                        if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                            handleOutputFormatChanged(codec.getOutputFormat());
                        }
                        continue;
                    }

                    // Sleep until frame is due to be presented
                    long dueUs = info.presentationTimeUs;
                    long nowUs = SystemClock.elapsedRealtime() * 1000;
                    long delayUs = (dueUs - nowUs) + latencyUs;
                    if (delayUs > 0) {
                        try {
                            sleep(delayUs / 1000, (int) (delayUs % 1000));
                        } catch (InterruptedException e) {
                            break;
                        }
                    }

                    // Handle the end-of-stream marker
                    if ((info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                        mainHandler.post(new Runnable() {
                            @Override
                            public void run() {
                                nativeOnFinished(nativeObj);
                            }
                        });
                        codec.flush();
                        continue;
                    }

                    boolean render = handleOutputBuffer(outputBufferIndex, info.offset, info.size);

                    codec.releaseOutputBuffer(outputBufferIndex, render);
                }
            }
        };
    }

    private long nativeObj;
    private String path;
    private Handler mainHandler = new Handler();
    private SurfaceTexture surfaceTexture;
    private Surface surface;
    private int textureID;
    private int videoWidth;
    private int videoHeight;
    private DemuxerThread demuxerThread;
    private Semaphore playSemaphore = new Semaphore(0);
    private boolean isOpen;
    private boolean isPaused;
    private boolean seekNeeded;
    private long seekTimeUs;

    VideoPlayer(long nativeObj, byte[] path) throws IOException {
        this.nativeObj = nativeObj;
        this.path = new String(path, UTF_8);
    }

    public void open() {

        // Create and configure a texture
        int[] textures = new int[1];
        glGenTextures(1, textures, 0);
        textureID = textures[0];
        assert(textureID > 0); // erk! there must be a current GL context...
        glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureID);
        glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Wrap the texture in a SurfaceTexture
        surfaceTexture = new SurfaceTexture(textureID);
        surfaceTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
            float transform[] = new float[16];
            @Override
            public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                surfaceTexture.getTransformMatrix(transform);
                surfaceTexture.updateTexImage();
                nativeGotFrame(nativeObj, textureID, videoWidth, videoHeight, transform);
            }
        });
        surface = new Surface(surfaceTexture);

        // Start the demuxer thread
        isOpen = true;
        demuxerThread = new DemuxerThread();
        demuxerThread.start();
    }

    public void play() {
        assert(demuxerThread!=null);
        isPaused = false;
        while (playSemaphore.hasQueuedThreads()) {
            playSemaphore.release();
        }
    }
    public void seek(int offsetMs) {
        seekTimeUs = offsetMs * 1000;
        seekNeeded = true;
    }
    public void pause() {
        assert(demuxerThread!=null);
        isPaused = true;
    }

    public void close() {
        isOpen = false;
        if (demuxerThread != null) {
            try {
                demuxerThread.interrupt();
                demuxerThread.join(500);
            } catch (InterruptedException e) {
            }
            demuxerThread = null;
        }
        if (surfaceTexture != null) {
            surfaceTexture.release();
            surfaceTexture = null;
        }
        if (surface != null) {
            surface.release();
            surface = null;
        }
        if (textureID > 0) {
            int atextures[] = new int[] { textureID };
            glDeleteTextures(1, atextures, 0);
            textureID = 0;
        }
    }


    private class DemuxerThread extends Thread {


        @Override
        public void run() {
            MediaExtractor extractor;

            Track video = new Track() {
                @Override
                boolean handleOutputBuffer(int outputBufferIndex, int bufferOffset, int bufferSize) {
                    // Nothing to do here. When the output buffer is released it will automatically
                    // signal SurfaceTexture.onFrameAvailable on the main thread
                    return true;
                }
            };
            Track audio = new Track() {

                AudioTrack audioTrack;

                @Override
                void handleOutputFormatChanged(MediaFormat newOutputFormat) {
                    int sampleRate = newOutputFormat.getInteger(MediaFormat.KEY_SAMPLE_RATE);
                    int channelCount = newOutputFormat.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
                    int channelConfig = (channelCount==1) ? AudioFormat.CHANNEL_OUT_MONO : AudioFormat.CHANNEL_OUT_STEREO;
                    int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
                    int playbackBufferSize = AudioTrack.getMinBufferSize(sampleRate, channelConfig, audioFormat);
                    audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                            sampleRate,
                            channelConfig,
                            audioFormat,
                            playbackBufferSize,
                            AudioTrack.MODE_STREAM);
                    audioTrack.play();
                }

                @Override
                boolean handleOutputBuffer(int outputBufferIndex, int bufferOffset, int bufferSize) {

                    ByteBuffer buffer = codec.getOutputBuffers()[outputBufferIndex];
                    // Such a clever API, here we have to do a completely pointless copy.
                    // (They did fix it in API 23 though)
                    byte[] bytes = new byte[bufferSize];
                    buffer.get(bytes);
                    audioTrack.write(bytes, bufferOffset, bufferSize);

                    // Update latency
                    long latencyUs = 0;
                    try {
                        Method getLatencyMethod =
                                android.media.AudioTrack.class.getMethod("getLatency", (Class < ? > []) null);
                        latencyUs = (Integer)getLatencyMethod.invoke(audioTrack, (Object[]) null) * 1000L;
                    } catch (Exception e) {
                        // There's no guarantee this method exists. Do nothing.
                    }
                    video.latencyUs = latencyUs + 200000; // the above is always a hopeless underestimate of the true latency

                    return false;
                }
            };

            // Open the media file and find the codecs
            try {
                extractor = new MediaExtractor();
                extractor.setDataSource(path);
                int numTracks = extractor.getTrackCount();
                for (int i = 0; i < numTracks; ++i) {
                    MediaFormat format = extractor.getTrackFormat(i);
                    String mime = format.getString(MediaFormat.KEY_MIME);
                    if (mime.startsWith("video")) {
                        video.codec = MediaCodec.createDecoderByType(mime);
                        video.trackIndex = i;
                        extractor.selectTrack(i);
                        videoWidth = format.getInteger(MediaFormat.KEY_WIDTH);
                        videoHeight = format.getInteger(MediaFormat.KEY_HEIGHT);
                        video.codec.configure(format, surface, null, 0);
                        mainHandler.post(new Runnable() {
                            @Override
                            public void run() {
                                nativeSetReady(nativeObj);
                            }
                        });
                    }
                    else if (mime.startsWith("audio")) {
                        audio.codec = MediaCodec.createDecoderByType(mime);
                        audio.trackIndex = i;
                        extractor.selectTrack(i);
                        audio.codec.configure(format, null, null, 0);

                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
                // todo: report via error api
                return;
            }


            // Start the codecs
            video.codec.start();
            audio.codec.start();

            // Start the media threads
            video.thread.start();
            audio.thread.start();

            long playbackStartTimeUs = SystemClock.elapsedRealtime() * 1000;
            long initialSeekTimeUs = 0;
            boolean endReached = false;

            // The play loop
            while (isOpen) {

                // Block until allowed to play
                if (isPaused) {

                    try {
                        playSemaphore.acquire();
                    } catch (InterruptedException e) {
                        break;
                    }
                }

                // Handle a pending seek
                if (seekNeeded) {
                    extractor.seekTo(seekTimeUs, MediaExtractor.SEEK_TO_PREVIOUS_SYNC);
                    seekNeeded = false;
                    seekTimeUs = 0;
                    if (!endReached) {
                        video.codec.flush();
                        audio.codec.flush();
                    }
                    endReached = false;
                    playbackStartTimeUs = SystemClock.elapsedRealtime() * 1000;
                    initialSeekTimeUs = extractor.getSampleTime();
                }

                // If we've already seen the end of this track sleep for 100ms and loop back
                if (endReached) {
                    try {
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                    }
                    continue;
                }

                // Select the track for the current extractor sample
                int trackIndex = extractor.getSampleTrackIndex();
                Track track = (trackIndex == video.trackIndex) ? video : audio;


                // Dequeue an input buffer for the codec for this track
                int index = track.codec.dequeueInputBuffer(100000);
                if (index>=0) {

                    // Read the data into the codec's input buffer
                    long presentationTimeUs = extractor.getSampleTime();
                    presentationTimeUs = playbackStartTimeUs + (presentationTimeUs - initialSeekTimeUs);
                    int flags = 0;
                    int chunkSize = extractor.readSampleData(track.codec.getInputBuffers()[index], 0);
                    if (chunkSize < 0) { // end-of-stream
                        chunkSize = 0;
                        endReached = true;
                        presentationTimeUs = 0;
                        flags = MediaCodec.BUFFER_FLAG_END_OF_STREAM;
                    }

                    // Send the buffer into the codec and advance the extractor
                    track.codec.queueInputBuffer(index, 0, chunkSize, presentationTimeUs, flags);
                    extractor.advance();
                }

            }


            // Stop the threads
            video.thread.interrupt();
            audio.thread.interrupt();

            // Stop the codecs.
            try {
                video.codec.stop();
                audio.codec.stop();
            }
            catch (Exception e) {
            }

            extractor.release();

        }
    }


    private native void nativeSetReady(long nativeObj);
    private native void nativeGotFrame(long nativeObj, int textureID, int width, int height, float[] transform);
    private native void nativeOnFinished(long nativeObj);

}
