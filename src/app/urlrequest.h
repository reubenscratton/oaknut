//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


typedef enum {
    URLDataTypeData,
    URLDataTypeJson,
    URLDataTypeBitmap
} URLDataType;

class URLData : public Object {
public:
    URLDataType _type;
    union value {
        ByteBuffer* data;
        class JsonValue* json;
        class Bitmap* bitmap;
        value(ByteBuffer* data);
        value(JsonValue* json);
        value(Bitmap* bitmap);
    } _value;
    URLData(ByteBuffer* data);
    URLData(JsonValue* json);
    URLData(Bitmap* bitmap);
    ~URLData();
};

class IURLRequestDelegate {
public:
	virtual void onUrlRequestLoad(URLData* data) = 0;
};

#define URL_FLAG_BITMAP 1   // use this when requesting images. Exists so web can use Image rather than XHR

/**
 * @ingroup app_group
 * @brief General purpose async networking support.
 */
class URLRequest : public Object {
public:
    static void request(const string& url, IURLRequestDelegate* delegate, int flags);
    static void unrequest(const string& url, IURLRequestDelegate* delegate);
    
    string _url;
    int _flags;
    void dispatchOnLoad(URLData* data);
protected:
	enum Status {
		IDLE,
		QUEUED,
		RUNNING
	} _status;
	vector<IURLRequestDelegate*> _delegates;
	string _method;
	void* _osobj;


	URLRequest(const string& url, IURLRequestDelegate* firstDelegate, int flags);
	~URLRequest();
	void start();
	void stop();
	void removeDelegate(IURLRequestDelegate* delegate);
	
    static void flushWorkQueue();
    
    
    // Native API
    void nativeStart();
    void nativeStop();
};


string urlEncode(string str);

