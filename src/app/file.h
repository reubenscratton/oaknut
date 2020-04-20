
/** @name Files
 * @{
 * A collection of file-related helpers intended to supplement rather than replace
 * the traditional C/C++ file APIs.
 *
 * Standard C/C++ support for file IO presents the modern cross-platform app
 * developer with some issues:
 *
 * - The APIs are synchronous and most cannot be used from the main thread without blocking rendering.
 * - The APIs are very old and do not support closures.
 * - The commonest file tasks - e.g. load a file into memory - are not the single line of code they ought to be.
 * - There are platform-specific quirks that need hiding. For example, files in Android app
 *  bundles (.apks)  are only accessible through Android-specific APIs, while Windows uses
 *  a different path separator, and so on.
 *
 * With that in mind the Oaknut approach to files is to offer helpers for common tasks,
 * but to also allow POSIX for anything else.
 *
 * Paths: In Oaknut the filesystem is navigated with standard Unix path notation with the addition
 * of some special double-slash roots that refer to platform-specific, app-specific, and user-
 * specific locations:
 *
 *                      MacOS
 *                      =====
 *     //assets         <app bundle>/assets
 *     //data           ~/Library/Application Support/AppName
 *     //cache          ~/Library/Caches/AppName
 *     //tmp
 *     //userdocs       ~/Documents
 *
 * Loading: `File::load()` loads some or all of a file asynchronously and your callback processes as many of the loaded bytes
 * as required and returns the number processed. The callback will continue to be called until the callback has read
 * all of the file or returned 0.
 *
 * Note that Oaknut has a separate API for structured, database-like storage. See LocalStorage.
 */

/**@}*/

class File {
public:
    

    // Asynchronous or non-IO APIs. These can be used on any thread.
    static Task* load(const string& path, std::function<size_t(variant&)> callback, size_t cb=0);
    static bool resolve(string& path);
    static Task* exists(const string& path, std::function<void(bool)> callback);
    static Task* dir(string& path, std::function<void(vector<string>)> callback);

    

    // Synchronous APIs. These should generally only be used from an IO thread, since they will block
    // the calling thread from doing other work while the slow IO completes.
    
    // Same as POSIX fopen() but resolves the file path if its //assets, //cache etc
    // and handles Android assets transparently
    static FILE* fopen_sync(string& path, const char* mode); // NB: use on an IO thread
    static vector<string> dir_sync(string& path);
    static bool exists_sync(const string& path);
    static variant load_sync(const string& path);
    static variant load_sync(int fd);
    static bool mkdirs_sync(const string& path, bool pathIsFilePath=false);
    static error save_sync(const string& path, const bytearray& data);
  
private:
    File() {} // don't instantiate this class, it's really a namespace
};
