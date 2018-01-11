package org.oaknut.main;


import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

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
            short buff[] = new short[8192];
            AudioRecord audioRecord;
            int mbs = AudioRecord.getMinBufferSize(sampleRate, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
            audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRate, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT, buff.length);
            audioRecord.startRecording();
            while (started) {
                int numRead = audioRecord.read(buff, 0, buff.length);
                if (numRead == 0) {
                    try {
                        Thread.sleep(250);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } else if (numRead > 0) {
                    nativeOnGotData(nativeObj, buff, 0, numRead);
                }
            }
            audioRecord.stop();
            audioRecord.release();
        }
    };

    private native void nativeOnGotData(long nativeObj, short[] buff, int offset, int length);
}
