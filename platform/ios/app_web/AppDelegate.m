//
// Copyright © 2017 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#ifdef IOS_WEB

#import "AppDelegate.h"
#import <WebKit/WebKit.h>
#include <sys/socket.h>
#include <netinet/in.h>
#import <JavaScriptCore/JavaScriptCore.h>


@interface HTTPServer : NSObject {
@public 
    dispatch_semaphore_t _sem;
    NSMutableArray* _connections;
}
@end

@interface HTTPConnection : NSObject <NSStreamDelegate> {
    HTTPServer* _server;
    NSInputStream* _input;
    NSOutputStream* _output;
    NSMutableString* _request;
    NSMutableData* _responseData;
    BOOL _processedHeaders;
}
- (id)initWithServer:(HTTPServer*)server input:(NSInputStream*)input output:(NSOutputStream*)output;
- (void)start;
@end


@implementation HTTPServer

void handleConnect(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void *data, void *info) {
    HTTPServer* server = (__bridge HTTPServer*)info;
    [server handleConnect:type data:data];
}

- (void)handleConnect:(CFSocketCallBackType)type data:(const void *)data {
    if (type == kCFSocketAcceptCallBack) {
        CFSocketNativeHandle nativeSocketHandle = *(CFSocketNativeHandle *)data;
        uint8_t name[SOCK_MAXADDRLEN];
        socklen_t namelen = sizeof(name);
        NSData *peer = nil;
        if (0 == getpeername(nativeSocketHandle, (struct sockaddr *)name, &namelen)) {
            peer = [NSData dataWithBytes:name length:namelen];
        }
        CFReadStreamRef readStream = NULL;
        CFWriteStreamRef writeStream = NULL;
        CFStreamCreatePairWithSocket(kCFAllocatorDefault, nativeSocketHandle, &readStream, &writeStream);
        if (readStream && writeStream) {
            CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            //dispatch_async(_queue, ^() {
            dispatch_semaphore_wait(_sem, DISPATCH_TIME_FOREVER);
            HTTPConnection* conn = [[HTTPConnection alloc] initWithServer:self
                            input:(NSInputStream*) CFBridgingRelease(readStream)
                            output:(NSOutputStream*)CFBridgingRelease(writeStream)];
            @synchronized (self) {
                [_connections addObject:conn];
            }
            [conn start];
            //});
            
        } else {
            close(nativeSocketHandle);
        }
    }
}

- (void)removeConn:(HTTPConnection*)conn {
    @synchronized (self) {
        [_connections removeObject:conn];
    }
    dispatch_semaphore_signal(_sem);
}

- (id)initWithPort:(int)port {
    if (self = [super init]) {
        _connections = [NSMutableArray new];
        _sem = dispatch_semaphore_create(100);
        CFSocketContext context;
        memset(&context, 0, sizeof(context));
        context.info = (__bridge_retained void*)(self);
        CFSocketRef socket = CFSocketCreate(kCFAllocatorDefault, PF_INET, SOCK_STREAM, IPPROTO_TCP,
                                                  kCFSocketAcceptCallBack, handleConnect, &context);
        
        int t= 1;
        setsockopt(CFSocketGetNative(socket), SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t));

        
        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_len = sizeof(sin);
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.s_addr= htonl(INADDR_LOOPBACK);
        CFDataRef sincfd = CFDataCreate(kCFAllocatorDefault, (UInt8 *)&sin, sizeof(sin));

        CFSocketSetAddress(socket, sincfd);
        CFRelease(sincfd);
        
        CFRunLoopSourceRef socketsource = CFSocketCreateRunLoopSource(kCFAllocatorDefault, socket, 0);
        CFRunLoopAddSource(CFRunLoopGetCurrent(), socketsource, kCFRunLoopDefaultMode);

    }
    return self;
}


@end



@implementation HTTPConnection

- (id)initWithServer:(HTTPServer*)server input:(NSInputStream*)input output:(NSOutputStream*)output {
    if (self = [super init]) {
        _server = server;
        _input = input;
        _output = output;
        _request = [NSMutableString new];
    }
    return self;
}
- (void)start {
    [_input setDelegate:self];
    [_output setDelegate:self];
    [_input scheduleInRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    [_output scheduleInRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    [_input open];
}

- (void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)eventCode {
    if (aStream == _input) {
        switch (eventCode) {
            case NSStreamEventHasBytesAvailable:
                while (_input.hasBytesAvailable) {
                    uint8_t buf[1024];
                    NSUInteger cb = [_input read:buf maxLength:sizeof(buf)];
                    if (cb > 0) {
                        [_request appendString:[[NSString alloc] initWithBytes:buf length:cb encoding:NSUTF8StringEncoding]];
                    }
                }
                if (!_processedHeaders) {
                    _processedHeaders = true;
                    [self processRequest];
                    [_output open];
                }
                break;
            case NSStreamEventEndEncountered:
                [_input close];
                [_input setDelegate:nil];
                [_input removeFromRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
                _input = nil;
                break;
            default:
                //NSStreamEventErrorOccurred
                break;
        }
    }
    else if (aStream == _output) {
        if (eventCode == NSStreamEventHasSpaceAvailable) {
            while (_output.hasSpaceAvailable && _responseData.length > 0) {
                NSUInteger cb = [_output write:_responseData.bytes maxLength:_responseData.length];
                if (cb > 0) {
                    [_responseData replaceBytesInRange:NSMakeRange(0,cb) withBytes:nil length:0];
                }
            }
            if (_responseData.length ==0) {
                [_input close];
                [_input setDelegate:nil];
                [_input removeFromRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
                _input = nil;
                [_output close];
                [_output setDelegate:nil];
                [_output removeFromRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
                _output = nil;
                [_server removeConn:self];
            }
        }
        
    }
}

- (void)processRequest {
    NSData* responseBody = nil;
    NSString* contentType = nil;
    NSString* status = @"200 OK";
    
    // Get the request in string form and handle log requests early
    NSArray* lines = [_request componentsSeparatedByString:@"\r\n"];
    
    // Extract filename
    NSString* firstLine = lines[0];
    assert([firstLine hasPrefix:@"GET"]);
    NSString* filename = [firstLine substringFromIndex:4];
    filename = [filename substringToIndex:[filename rangeOfString:@" "].location];
    
    if ([filename isEqualToString:@"/print"]) {
        for (NSString* header in lines) {
            if ([header hasPrefix:@"xprint"]) {
                NSString* logstr = [header substringFromIndex:8];
                printf("%s\n", logstr.cString);
                break;
            }
        }
    } else {
        
        
        //NSLog(request);
        //     NSInteger spacePos = .location;
        //     [_request replaceCharactersInRange:NSMakeRange(spacePos, _request.length-spacePos) withString:@""];
        
        // Determine content type from file extension
        contentType = @"application/octet-stream";
        if ([filename hasSuffix:@".htm"] || [filename hasSuffix:@".html"]) {
            contentType = @"text/html";
        } else if ([filename hasSuffix:@".js"]) {
            contentType = @"text/javascript";
        }
        
        // Slurp file into RAM
        NSString* docRoot = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"webroot"];
        filename = [docRoot stringByAppendingString:filename];
        responseBody = [NSData dataWithContentsOfFile:filename];
        if (!responseBody) {
            status = @"404 Not Found";
        }
    }
    
    // Prepare the response
    _responseData = [NSMutableData new];
    NSMutableString* response = [NSMutableString new];
    [response appendFormat:@"HTTP/1.1 %@\r\n", status];
    if (responseBody) {
        [response appendFormat:@"Content-Length: %d\r\n", (int)responseBody.length];
    }
    if (contentType) {
        [response appendFormat:@"Content-Type: %@\r\n", contentType];
    }
    [response appendString:@"Connection: closed\r\n"];
    [response appendString:@"\r\n"];
    [_responseData appendData: [response dataUsingEncoding:NSUTF8StringEncoding]];
    if (responseBody) {
        [_responseData appendData:responseBody];
    }
    
}

@end


@interface AppDelegate () <WKScriptMessageHandler>

@property (nonatomic) HTTPServer* httpServer;
//@property (nonatomic) UIWebView *webView;
@property (nonatomic) WKWebView *webView;

@end

@implementation AppDelegate

- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message          {
    NSString* body = @"";
    if ([message.body isKindOfClass:[NSString class]]) {
        body = message.body;
    } else if ([message.body isKindOfClass:[NSNumber class]]) {
        body = ((NSNumber*)message.body).stringValue;
    } else assert(0);
    printf("%s\n", body.cString);
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    UIViewController* vc = [[UIViewController alloc] init];
	
	vc.view = [[UIView alloc] initWithFrame:self.window.bounds];
    
	WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
	config.preferences.javaScriptEnabled = YES;
    //WKUserScript *s = [[WKUserScript alloc] initWithSource:@"alert('Hello, World!')" injectionTime:WKUserScriptInjectionTimeAtDocumentStart forMainFrameOnly:YES];
    WKUserContentController *c = [[WKUserContentController alloc] init];
    //[c addUserScript:s];
    [c addScriptMessageHandler:self name:@"buttonClicked"];
    config.userContentController = c;
    
	self.webView = [[WKWebView alloc] initWithFrame:vc.view.bounds configuration:config];
	//self.webView = [[UIWebView alloc] initWithFrame:vc.view.bounds];
	//self.webView.navigationDelegate = self;
    
	[vc.view addSubview:self.webView];
    self.window.rootViewController = vc;
	[self.window makeKeyAndVisible];
	
	// Create server using our custom MyHTTPServer class
    self.httpServer = [[HTTPServer alloc] initWithPort:12345];
	
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	[self.webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"http://127.0.0.1:12345/xx.html"]]];
}

- (void)applicationWillTerminate:(UIApplication *)application {
}


@end

#endif

