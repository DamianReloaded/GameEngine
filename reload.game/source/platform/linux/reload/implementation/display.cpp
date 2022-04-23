#include "display.h"
#include <cstring>
namespace reload
{
    Atom display::implementation::wmdelete;

    display::implementation::implementation(reload::display* p_display)
    {
        m_display = p_display;
        xoffset = 1920*(0);
        yoffset = 0;
        width = 1024;
        height = 768;
        fullscreen = false;
        resource = 0;
        inputmask = FocusChangeMask | ButtonPressMask | ButtonReleaseMask   | ButtonMotionMask | PointerMotionMask |
                    KeyPressMask    | KeyReleaseMask  | StructureNotifyMask | EnterWindowMask  | LeaveWindowMask;

        attributelist = { GLX_RGBA,
                          GLX_DEPTH_SIZE,       24,
                          GLX_STENCIL_SIZE,     8,
                          GLX_DOUBLEBUFFER,     True,
                          GLX_SAMPLE_BUFFERS,   1,
                          GLX_SAMPLES,          8
                        };
    }

    display::implementation::~implementation()
    {

    }

    bool display::implementation::start()
    {
        int         glx_major_version;
        int         glx_minor_version;

        display    = XOpenDisplay(0);
        screen     = DefaultScreen(display);
        visualinfo = glXChooseVisual(display, screen, &attributelist[0]);
        if (visualinfo==NULL)
        {
            attributelist[19] = False;
            visualinfo = glXChooseVisual(display, screen, &attributelist[0]);
        }

        query_resources();

        glXQueryVersion(display, &glx_major_version, &glx_minor_version);

        context                         = glXCreateContext(display, visualinfo, 0, GL_TRUE);
        attributes.colormap             = XCreateColormap(display, RootWindow(display, visualinfo->screen), visualinfo->visual, AllocNone);
        attributes.background_pixmap    = None;
        attributes.border_pixel         = 0;
        attributes.event_mask           = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;

        if (fullscreen)
        {
            attributes.override_redirect = True;
            window = XCreateWindow(display, RootWindow(display, visualinfo->screen),
                                   resources[resource].x, resources[resource].y, resources[resource].width, resources[resource].height,
                                   0, visualinfo->depth, InputOutput, visualinfo->visual,
                                   CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &attributes);
            XWarpPointer(display, None, window, 0,0,0,0,0,0);
            XMapRaised(display, window);
            XGrabKeyboard(display, window, True, GrabModeAsync, GrabModeAsync, CurrentTime);
            XGrabPointer(display, window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);
        }
        else
        {
            window = XCreateWindow(display, RootWindow(display, visualinfo->screen),
                                   0, 0, resources[resource].width, resources[resource].height,
                                   0, visualinfo->depth, InputOutput, visualinfo->visual,
                                   CWBorderPixel | CWColormap | CWEventMask, &attributes);
            XSetStandardProperties(display, window, "title", "title2", None, NULL, 0, NULL);
            XMapRaised(display, window);
        }

        XFree(visualinfo);

        wmdelete = XInternAtom(display, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(display, window, &wmdelete, 1);
        glXMakeCurrent(display, window, context);

        Window      dummywindow;
        uint32_t    dummyborder;
        int         x=0, y=0;
        XGetGeometry(display, window, &dummywindow, &x, &y, (unsigned int*)&width, (unsigned int*)&height, &dummyborder, &depth);

        if (glXIsDirect(display, context)) {} //do something
        XSelectInput(display, window, inputmask);
        glewInit();

        return true;
    }

    void display::implementation::process_events()
    {
        XEvent xevent;
        while (XPending(display))
        {
            XNextEvent(display, &xevent);

            switch(xevent.type)
            {
                case ClientMessage:
                {
                    if ((Atom)xevent.xclient.data.l[0] == wmdelete)
                    {
                        if (m_display->event.closed!=nullptr) m_display->event.closed();
                    }
                }
                break;

                case DestroyNotify:
                {

                }
                break;

                case Expose:
                {
                    swap_buffers();
                }
                break;

                case ButtonPress:
                {
                    switch(xevent.xbutton.button)
                    {
                        case 1: m_display->event.closed(); break;
                    }
                }
                break;
            }
        }
    }

    void display::implementation::stop()
    {

    }

    void display::implementation::swap_buffers()
    {
        if (attributelist[19])
        {
            glXSwapBuffers(display, window);
        }
    }


    void display::implementation::query_resources()
    {
        resources.clear();
        XRRScreenResources* res = XRRGetScreenResources(display, RootWindow(display,visualinfo->screen));
        for (int i=0; i<res->noutput; i++)
        {
            auto info = XRRGetOutputInfo(display, res, res->outputs[i]);
            for (int c=0; c<info->ncrtc; c++)
            {
                auto crtcinfo = XRRGetCrtcInfo(display, res, res->crtcs[c]);
                store_resource(crtcinfo->x, crtcinfo->y, crtcinfo->width, crtcinfo->height, crtcinfo->rotation);
                XRRFreeCrtcInfo(crtcinfo);
            }
            XRRFreeOutputInfo(info);
        }
        if (resource>resources.size()-1) resource = 0;
    }

    void display::implementation::store_resource(int p_x, int p_y, int p_w, int p_h, int p_rotation)
    {
        if (p_w<1 || p_h<1) return;
        for (size_t i=0; i<resources.size(); i++)
        {
            if (resources[i].x == p_x && resources[i].y == p_y && resources[i].width == p_w && resources[i].height == p_h && resources[i].rotation == p_rotation)
            {
                return;
            }
        }
        resources.resize(resources.size()+1);
        resources.back().x        = p_x;
        resources.back().y        = p_y;
        resources.back().width    = p_w;
        resources.back().height   = p_h;
        resources.back().rotation = p_rotation;

        if ( resources.back().x == xoffset && resources.back().y == yoffset && resources.back().width == width && resources.back().height == height ) resource = resources.size()-1;
    }
}
