//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

// NB: Your CPATH variable should include all the ridiculous number
// of places these headers can be found
#include    <gdk/gdk.h>
#include    <gtk/gtk.h>
#define GL_GLEXT_PROTOTYPES
#include	<GL/gl.h>
#include	<GL/glext.h>
#include	<GL/glu.h>
#include <cairo.h>
#include <curl/curl.h>

#define HAS_VAO
