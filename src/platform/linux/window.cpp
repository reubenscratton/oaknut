//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//
#ifdef PLATFORM_LINUX

#include <oaknut.h>


class WindowLinux : public Window {
public:
    
    GtkWidget* glarea;
    GtkWidget* glwindow;

    WindowLinux() {
        glwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(glwindow), "Scene"); // todo!
        
        glarea = gtk_gl_area_new();
        gtk_gl_area_set_required_version(GTK_GL_AREA(glarea), 3, 2);
        g_signal_connect (glarea, "render", G_CALLBACK (render), this);
        
        gtk_widget_set_events(GTK_WIDGET(glarea),
                              GDK_EXPOSURE_MASK|
                              GDK_BUTTON_PRESS_MASK|
                              GDK_BUTTON_RELEASE_MASK|
                              GDK_KEY_PRESS_MASK|
                              GDK_KEY_RELEASE_MASK|
                              GDK_POINTER_MOTION_MASK|
                              GDK_POINTER_MOTION_HINT_MASK);
        g_signal_connect(glarea, "size_allocate", G_CALLBACK(glarea_size_allocate), this);
        g_signal_connect(glarea, "motion_notify_event", G_CALLBACK(glarea_mouse_event), this);
        g_signal_connect(glarea, "button_press_event", G_CALLBACK(glarea_mouse_event), this);
        g_signal_connect(glarea, "button_release_event", G_CALLBACK(glarea_mouse_event), this);
        g_signal_connect(glarea, "key_press_event", G_CALLBACK(glarea_key_press_event), this);
        g_signal_connect(glarea, "key_release_event", G_CALLBACK(glarea_key_release_event), this);
        g_signal_connect(glarea, "destroy", G_CALLBACK(gtk_main_quit), this);

        // ... Insert glarea into window and show it
        gtk_container_add(GTK_CONTAINER(glwindow),glarea);
        gtk_widget_set_can_focus(glarea, true);
        gtk_widget_show(glarea);
        gtk_widget_show(glwindow);
        
        gtk_widget_grab_focus(GTK_WIDGET(glarea));

    }
    void show() override {
        gtk_widget_set_size_request(glarea,
                                    app.getStyleFloat("window.default-width"),
                                    app.getStyleFloat("window.default-height"));
    }
    
    void requestRedrawNative() override {
        gtk_gl_area_queue_render(GTK_GL_AREA(glarea));
    }

    void keyboardShow(bool show) override {
        
    }
    void keyboardNotifyTextChanged() override {
        
    }

    
    // Static callbacks
    static void glarea_size_allocate(GtkWidget *widget, GdkRectangle *allocation, gpointer callback_data) {
        WindowLinux* window = (WindowLinux*)callback_data;
        window->resizeSurface(allocation->width, allocation->height, 1);
    }
    static gboolean render(GtkGLArea *area, GdkGLContext *context, gpointer callback_data)  {
        WindowLinux* window = (WindowLinux*)callback_data;
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window->draw();
        glFlush();
        return TRUE;
    }
    static gint glarea_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer callback_data) {
        WindowLinux* window = (WindowLinux*)callback_data;
        if (window->_keyboardHandler) {
            char32_t charCode = event->string ? event->string[0] : 0;
            KeyboardInputSpecialKeyCode sk = SpecialKeyNone;
            window->_keyboardHandler->keyInputEvent(KeyDown, sk, event->hardware_keycode, charCode);
            return (TRUE);
        }
        return FALSE;
    }
    static gint glarea_key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer callback_data) {
        WindowLinux* window = (WindowLinux*)callback_data;
        if (window->_keyboardHandler) {
            char32_t charCode = event->string ? event->string[0] : 0;
            KeyboardInputSpecialKeyCode sk = SpecialKeyNone;
            window->_keyboardHandler->keyInputEvent(KeyUp, sk, event->hardware_keycode, charCode);
            return (TRUE);
        }
        return FALSE;
    }
    static gint glarea_mouse_event(GtkWidget *widget, GdkEventMotion *event, gpointer callback_data) {
        WindowLinux* window = (WindowLinux*)callback_data;
        gint            x, y;
        GdkRectangle        area;
        GdkModifierType        state;
        
        if (event->is_hint) {
            gdk_window_get_device_position(event->window, event->device, &x, &y, &state);
        } else {
            x = (gint)event->x;
            y = (gint)event->y;
            state = (GdkModifierType)event->state;
        }
        
        int eventType;
        switch(event->type) {
            case GDK_BUTTON_PRESS:
                eventType = INPUT_EVENT_DOWN;
                break;
            case GDK_MOTION_NOTIFY:
                eventType = INPUT_EVENT_MOVE;
                break;
            case GDK_BUTTON_RELEASE:
                eventType = INPUT_EVENT_UP;
                break;
            default:
                app.log("unhandled mouse event type=%d", event->type);
                return FALSE;
        }
        //app.log("x=%d y=%d", x, y);
        INPUTEVENT inputEvent;
        inputEvent.deviceType = INPUTEVENT::Mouse;
        inputEvent.type = eventType;
        inputEvent.pt.x = x;
        inputEvent.pt.y = y;
        inputEvent.time = app.currentMillis();
        window->dispatchInputEvent(inputEvent);
        
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

};

Window* Window::create() {
    return new WindowLinux();
}



#endif

