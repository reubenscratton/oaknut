//
// Copyright © 2020 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

/** @name Logging
 * @{
 */
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_NONE  0

/** Log an informational message */
#ifndef LOG_LEVEL
# if DEBUG
#  define LOG_LEVEL LOG_LEVEL_INFO
# else
#  define LOG_LEVEL LOG_LEVEL_WARN
# endif
#endif
void log_v(const char* prefix, char const* fmt, va_list& v);
inline void log_dbg(char const* fmt, ...) {
#if LOG_LEVEL>=LOG_LEVEL_DEBUG
    va_list args;
    va_start(args, fmt);
    log_v("DEBUG: ", fmt, args);
#endif
}
inline void log_info(char const* fmt, ...) {
#if LOG_LEVEL>=LOG_LEVEL_INFO
    va_list args;
    va_start(args, fmt);
    log_v("INFO: ", fmt, args);
#endif
}
inline void log_warn(char const* fmt, ...) {
#if LOG_LEVEL>=LOG_LEVEL_WARN
    va_list args;
    va_start(args, fmt);
    log_v("WARNING: ", fmt, args);
#endif
}
inline void log_error(char const* fmt, ...) {
#if LOG_LEVEL>=LOG_LEVEL_ERROR
    va_list args;
    va_start(args, fmt);
    log_v("ERROR: ", fmt, args);
#endif
}

/**@}*/

