package org.oaknut.main;


import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

import java.nio.ByteBuffer;

public class AudioInput {

    long nativeObj;
    int sampleRate;
    boolean started;
    Thread thread;

    public AudioInput(long nativeObj, int sampleRate) {
        this.nativeObj = nativeObj;
        this.sampleRate = sampleRate;
    }

    public void start() {
        if (!started) {
            started = true;
            thread = new Thread(threadFunc);
            thread.start();
        }
    }

    public void stop() {
        if (started) {
            started = false;
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private Runnable threadFunc = new Runnable() {
        @Override
        public void run() {
            ByteBuffer buff = ByteBuffer.allocateDirect(16384);
            AudioRecord audioRecord;
            audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRate, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT, buff.capacity());
            audioRecord.startRecording();
            while (started) {
                int cbRead = audioRecord.read(buff, buff.capacity());
                if (cbRead == 0) {
                    try {
                        Thread.sleep(250);
                    } catch (InterruptedException e) {
                        break;
                    }
                } else if (cbRead > 0) {
                    nativeOnGotData(nativeObj, buff, cbRead);
                }
            }
            audioRecord.stop();
            audioRecord.release();
        }
    };

    private native void nativeOnGotData(long nativeObj, ByteBuffer buff, int cb);
}
