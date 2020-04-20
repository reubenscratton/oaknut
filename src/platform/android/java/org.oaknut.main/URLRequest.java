package org.oaknut.main;

import android.text.TextUtils;
import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.SocketTimeoutException;
import java.net.URL;
import java.nio.charset.Charset;
import java.util.List;
import java.util.Map;


public class URLRequest {


    static final Charset UTF_8 = Charset.forName("UTF-8");


    long cobj;
    String url;
    String method;
    byte[] headersBytes;
    byte[] requestEntityBytes;
    HttpURLConnection urlConnection;
    InputStream inputStream;

    public URLRequest(long cobj, String url, String method, byte[] headersBytes, byte[] requestEntityBytes) {
        this.cobj = cobj;
        this.url = url;
        this.method = method;
        this.headersBytes = headersBytes;
        this.requestEntityBytes = requestEntityBytes;
    }

    public void cancel() {
        close();
    }

    private void close() {
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (IOException e) {}
            inputStream = null;
        }
        if (urlConnection != null) {
            urlConnection.disconnect();
        }
    }

    public void run(long cobjResponse) {
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

            // Set the read timeout cos we have to poll for data
            urlConnection.setReadTimeout(100);

            // The request executes here!
            int status = urlConnection.getResponseCode();

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
            for (;;) {
                try {
                    int n = inputStream.read(byteChunk);
                    if (n > 0 ) {
                        baos.write(byteChunk, 0, n);
                    }
                    else if (n<0 || nativeCheckIfCancelled(cobj)) {
                        break;
                    }
                }
                catch (SocketTimeoutException e) {
                    android.util.Log.i("http", "timeout");
                }
            }

            // Pass to C++ for dispatch
            nativeOnGotData(cobj, cobjResponse, status,
              headers.getBytes(UTF_8),
              baos.toByteArray());

        } catch (Exception e) {
            e.printStackTrace();
            android.util.Log.e("Oaknut",  e.toString());
            nativeOnGotData(cobj, cobjResponse, 999, null, null);
        } finally {
            close();
        }
    }

    native void nativeOnGotData(long cobj, long cobjResponse, int httpStatus, byte[] headersUtf8, byte[] data);
    native boolean nativeCheckIfCancelled(long cobj);

}
