package org.oaknut.main;

import android.text.TextUtils;
import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.Charset;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.List;
import java.util.Map;


public class URLRequest implements Runnable {


    static final Charset UTF_8 = Charset.forName("UTF-8");

    static LinkedBlockingQueue<Runnable> queue = new LinkedBlockingQueue<>();
    static ThreadPoolExecutor executor = new ThreadPoolExecutor(4, 4, 0, TimeUnit.MILLISECONDS, queue);

    long cobj;
    String url;
    String method;
    Future<?> future;
    byte[] headersBytes;
    byte[] requestEntityBytes;

    public URLRequest(long cobj, String url, String method, byte[] headersBytes, byte[] requestEntityBytes) {
        this.cobj = cobj;
        this.url = url;
        this.method = method;
        this.headersBytes = headersBytes;
        this.requestEntityBytes = requestEntityBytes;
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

            if (headersBytes != null) {
                String[] headersStrs = new String(headersBytes, UTF_8).split("\n");
                for (int i=0 ; i<headersStrs.length ; i++) {
                    String headersStr = headersStrs[i];
                    int p = headersStr.indexOf(':');
                    urlConnection.setRequestProperty(headersStr.substring(0, p), headersStr.substring(p+1));
                }
            }

            // Set request entity if there is one
            if (requestEntityBytes != null) {
                urlConnection.setDoOutput(true);
                urlConnection.getOutputStream().write(requestEntityBytes);
            }

            // The request executes here!
            int status = urlConnection.getResponseCode();

            InputStream inputStream;
            if(status != HttpURLConnection.HTTP_OK)
                inputStream = new BufferedInputStream(urlConnection.getErrorStream());
            else
                inputStream = new BufferedInputStream(urlConnection.getInputStream());


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
            nativeOnGotData(cobj, status,
              headers.getBytes(UTF_8),
              baos.toByteArray());

        } catch (Exception e) {
            e.printStackTrace();
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
