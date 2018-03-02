#ifdef PLATFORM_LINUX

#include	<oaknut.h>
#include	<gdk/gdk.h>
#include	<gtk/gtk.h>
#define GL_GLEXT_PROTOTYPES
#include	<GL/gl.h>
#include	<GL/glext.h>
#include	<GL/glu.h>



static GtkWidget* glarea=NULL;

void oakRequestRedraw() {
    gtk_gl_area_queue_render(GTK_GL_AREA(glarea));
}

void oakKeyboardShow(bool show) {

}
void oakKeyboardNotifyTextChanged() {

}

static bool called_main;

static gboolean render(GtkGLArea *area, GdkGLContext *context)  {
    if (!called_main) {
        int maj = 0;
        int min = 0;
        gdk_gl_context_get_version(context, &maj, &min);
        called_main = true;
        oakMain();
    }
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mainWindow->draw();
    glFlush();
    return TRUE;
}



void glarea_size_allocate(GtkWidget *widget, GdkRectangle *allocation, gpointer  user_data) {
    mainWindow->resizeSurface(allocation->width, allocation->height, 1);
}




gint glarea_mouse_event(GtkWidget *widget, GdkEventMotion *event) {
    gint			x, y;
    GdkRectangle		area;
    GdkModifierType		state;

    if (event->is_hint) {
        gdk_window_get_device_position(event->window, event->device, &x, &y, &state);
    } else {
        x = (gint)event->x;
        y = (gint)event->y;
        state = (GdkModifierType)event->state;
    }

    int oakEvent;
    switch(event->type) {
        case GDK_BUTTON_PRESS:
            oakEvent = INPUT_EVENT_DOWN;
            break;
        case GDK_MOTION_NOTIFY:
            oakEvent = INPUT_EVENT_MOVE;
            break;
        case GDK_BUTTON_RELEASE:
            oakEvent = INPUT_EVENT_UP;
            break;
        default:
            oakLog("unhandled mouse event type=%d", event->type);
            return FALSE;
    }
            //oakLog("x=%d y=%d", x, y);
    mainWindow->dispatchInputEvent(oakEvent, MAKE_SOURCE(INPUT_SOURCE_TYPE_MOUSE,0), event->time, x, y);

    /*if(state & GDK_BUTTON1_MASK) {
        g_print("Mouse motion button 1 at coordinates (%d,%d)\n",x,y);
    }
    if(state & GDK_BUTTON2_MASK) {
        g_print("Mouse motion button 2 at coordinates (%d,%d)\n",x,y);
    }
    if(state & GDK_BUTTON3_MASK) {
        g_print("Mouse motion button 3 at coordinates (%d,%d)\n",x,y);
    }*/

    return TRUE;
}




gint glarea_key_press_event(GtkWidget *widget, GdkEventKey *event) {
    g_print("Key pressed! todo: handle it\n");
    return (TRUE);
}


int main(int argc, char **argv) {
    GtkWidget* glwindow;
    gtk_init(&argc, &argv);


    mainWindow = new Window();

    glwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if(!glwindow) {
        return(0);
    }
    gtk_window_set_title(GTK_WINDOW(glwindow), "Scene");

    glarea = gtk_gl_area_new ();
    gtk_gl_area_set_required_version(GTK_GL_AREA(glarea), 3, 2);
    g_signal_connect (glarea, "render", G_CALLBACK (render), NULL);

    gtk_widget_set_events(GTK_WIDGET(glarea),
                          GDK_EXPOSURE_MASK|
                          GDK_BUTTON_PRESS_MASK|
                          GDK_BUTTON_RELEASE_MASK|
                          GDK_KEY_PRESS_MASK|
                          GDK_KEY_RELEASE_MASK|
                          GDK_POINTER_MOTION_MASK|
                          GDK_POINTER_MOTION_HINT_MASK);
    g_signal_connect(glarea, "size_allocate", G_CALLBACK(glarea_size_allocate), NULL);
    g_signal_connect(glarea, "motion_notify_event", G_CALLBACK(glarea_mouse_event), NULL);
    g_signal_connect(glarea, "button_press_event", G_CALLBACK(glarea_mouse_event), NULL);
    g_signal_connect(glarea, "button_release_event", G_CALLBACK(glarea_mouse_event), NULL);
    g_signal_connect(glarea, "key_press_event", G_CALLBACK(glarea_key_press_event), NULL);
    g_signal_connect(glarea, "delete_event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_set_size_request(glarea, 640, 480); /* minimum size */
    //gtk_quit_add_destroy(1, GTK_OBJECT(glarea));

    /* ... Insert glarea into window and show it all ... */
    gtk_container_add(GTK_CONTAINER(glwindow),glarea);
    gtk_widget_show(glarea);
    gtk_widget_show(glwindow);

    //GTK_WIDGET_SET_FLAGS(glarea,GTK_CAN_FOCUS);
    gtk_widget_grab_focus(GTK_WIDGET(glarea));



    gtk_main();

    return(0);
}


#endif
