#pragma once
#include "../../../../reload/display.h"
#include <X11/Xlib.h>
#include <glew/glew.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/extensions/Xrandr.h>
#include <X11/keysym.h>
#include <vector>
#include <deque>

namespace reload {
    class display::implementation
    {
        public:
                                            implementation  (reload::display* p_display);
            virtual                         ~implementation ();
            bool                            start           ();
            void                            process_events  ();
            void                            swap_buffers    ();
            void                            stop            ();

        private:
            reload::display*        m_display;
            struct                  video_mode      { int x, y, width, height, rotation; };
            void                    query_video_modes ();
            void                    store_video_mode  (int p_x, int p_y, int p_w, int p_h, int p_rotation);
            Display*                display;
            int                     screen;
            Window                  window;
            GLXContext              context;
            XSetWindowAttributes    attributes;
            XVisualInfo*            visualinfo;
            XIC                     xic;
            XIM                     xim;
            int                     xoffset;
            int                     yoffset;
            int                     width;
            int                     height;
            bool                    fullscreen;
            uint32_t                inputmask;
            uint32_t                depth;
            static Atom             wmdelete;
            std::vector<int>        attributelist;
            std::deque<video_mode>  video_modes;
            size_t                  current_video_mode;
            uint8_t                 key_state[256];
            bool                    disable_control_key_modifier;
            bool                    disable_autorepeat_key_release;
            bool                    disable_autorepeat_key_press;
    };
}

