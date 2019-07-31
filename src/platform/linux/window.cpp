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
    float _scale;
    int _inputFlags;

    WindowLinux() {
        glwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

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
        g_signal_connect(glarea, "realize", G_CALLBACK(glarea_realize), this);
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
    void setRootViewController(ViewController* viewController) override {
        Window::setRootViewController(viewController);
        gtk_window_set_title(GTK_WINDOW(glwindow), viewController->getTitle().data());
    }
    void show() override {
        Window::show();
        gtk_widget_set_size_request(glarea,
                                    app->getStyleFloat("window.default-width"),
                                    app->getStyleFloat("window.default-height"));
    }
    
    void requestRedrawNative() override {
        gtk_gl_area_queue_render(GTK_GL_AREA(glarea));
    }

    void keyboardShow(bool show) override {
        
    }
    void keyboardNotifyTextChanged() override {
        
    }

    
    // Static callbacks
    static void glarea_realize(GtkWidget *widget, gpointer callback_data) {
        WindowLinux* window = (WindowLinux*)callback_data;
        gtk_gl_area_make_current (GTK_GL_AREA(window->glarea));

    }
    static void glarea_size_allocate(GtkWidget *widget, GdkRectangle *allocation, gpointer callback_data) {
        WindowLinux* window = (WindowLinux*)callback_data;
        GdkScreen* screen = gdk_screen_get_default();
        window->_scale = gdk_screen_get_monitor_scale_factor(screen, 0);
        double res = gdk_screen_get_resolution(screen);
        window->resizeSurface(allocation->width*window->_scale, allocation->height*window->_scale, 160.0/res);
    }
    static gboolean render(GtkGLArea *area, GdkGLContext *context, gpointer callback_data)  {
        WindowLinux* window = (WindowLinux*)callback_data;
        //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window->draw();
        //glFlush();
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
                window->_inputFlags |= INPUT_FLAG_LBUTTON_DOWN;
                break;
            case GDK_MOTION_NOTIFY:
                eventType = INPUT_EVENT_MOVE;
                break;
            case GDK_BUTTON_RELEASE:
                eventType = INPUT_EVENT_UP;
                window->_inputFlags &= ~INPUT_FLAG_LBUTTON_DOWN;
                break;
            default:
                app->log("unhandled mouse event type=%d", event->type);
                return FALSE;
        }
        //app->log("t=%d x=%d y=%d", eventType, x, y);
        INPUTEVENT inputEvent;
        inputEvent.deviceType = INPUTEVENT::Mouse;
        inputEvent.deviceIndex = 0;
        inputEvent.type = eventType;
        inputEvent.flags = window->_inputFlags;
        inputEvent.pt.x = x * window->_scale;
        inputEvent.pt.y = y * window->_scale;
        inputEvent.time = app->currentMillis();
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

class LinuxRenderer : public GLRenderer {
public:
    LinuxRenderer(Window* window) : GLRenderer(window) {
    }
    void bindToNativeWindow(long nativeWindowHandle) override {
        // todo: move EAGL setup code here
    }
    void commit() override {
        // todo: move Linux swapBuffer stuff here
    }

};

Renderer* Renderer::create(Window* window) {
    return new LinuxRenderer(window);
}

#endif

