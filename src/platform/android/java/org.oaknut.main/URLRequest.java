package org.oaknut.main;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.text.TextUtils;
import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.List;
import java.util.Map;
import static java.nio.charset.StandardCharsets.UTF_8;


public class URLRequest implements Runnable {


    static LinkedBlockingQueue<Runnable> queue = new LinkedBlockingQueue<>();
    static ThreadPoolExecutor executor = new ThreadPoolExecutor(4, 4, 0, TimeUnit.MILLISECONDS, queue);

    long cobj;
    String url;
    String method;
    Future<?> future;

    public URLRequest(long cobj, String url, String method) {
        this.cobj = cobj;
        this.url = url;
        this.method = method;
        future = executor.submit(this);
    }

    public void cancel() {
        future.cancel(true);
    }

    @Override
    public void run() {
        HttpURLConnection urlConnection = null;
        try {
            URL url = new URL(this.url);
            urlConnection = (HttpURLConnection) url.openConnection();
            urlConnection.setRequestMethod(method);
            // todo: upload request body
            InputStream inputStream = new BufferedInputStream(urlConnection.getInputStream());

            // Put the response headers into a single String
            String headers = "";
            for(Map.Entry<String, List<String>> entry : urlConnection.getHeaderFields().entrySet()) {
                if (entry.getKey() != null) {
                  headers += entry.getKey() + ":";
                  headers += TextUtils.join(";", entry.getValue());
                  headers += "\n";
                }
            }

            // Read whole response into memory.
            final ByteArrayOutputStream baos = new ByteArrayOutputStream();
            byte[] byteChunk = new byte[4096];
            int n;
            while ((n = inputStream.read(byteChunk)) > 0 ) {
                baos.write(byteChunk, 0, n);
            }

            // Pass to C++ for dispatch
            nativeOnGotData(cobj, urlConnection.getResponseCode(),
              headers.getBytes(UTF_8),
              baos.toByteArray());

        } catch (IOException e) {
            android.util.Log.e("Oaknut",  e.toString());
            nativeOnGotData(cobj, 999, null, null);
        } finally {
            if (urlConnection != null) {
                urlConnection.disconnect();
            }
        }
    }

    native void nativeOnGotData(long cobj, int httpStatus, byte[] headersUtf8, byte[] data);


}
