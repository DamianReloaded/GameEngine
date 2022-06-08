#pragma once
#include "../../../../reload/display.h"
#include <windows.h>
#include <vector>
#include <deque>

namespace reload {
    class display::implementation
    {
        public:
                                            implementation       (reload::display* p_display);
            virtual                         ~implementation      ();
            bool                            start                ();
            void                            process_events       ();
            void                            swap_buffers         ();
            void                            make_context_current ();
            void                            stop                 ();

        private:
                                            implementation      (){};
            struct                          video_mode          { int x, y, width, height, bpp, frequency, mode; };

            void                            query_video_modes   ();
            static LRESULT CALLBACK         winproc             (HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
            void                            insert_video_mode   (DEVMODE& dm, int mode);
            video_mode                      get_best_video_mode (const video_mode& v);
            const bool&	                    fullscreen          ();
            void                            fullscreen	        (const bool& _val, const bool& _store=true);
            void                            set_size            (int , int );

            void                            raise               ();
            bool                            init_multisampling  (const int& _num_samples);
            bool                            create              (const bool& _from_multisampling=false);

            std::string                     m_title;
            bool                            m_created;
            bool                            m_vsync;
            reload::display*                m_display;
            int                             xoffset;
            int                             yoffset;
            int                             width;
            int                             height;
            bool                            m_fullscreen;
            uint32_t                        depth;
            std::deque<video_mode>          video_modes;
            size_t                          current_video_mode;
            uint8_t                         key_state[256];
            int                             m_multisampling;

            HINSTANCE	                    m_hinstance;
            HWND		                    m_hwnd;
            HDC			                    m_hdc;
            HGLRC		                    m_hrc;
            static WNDPROC		            m_wndproc;
            DWORD		                    m_style;
            DWORD		                    m_extended_style;
            POINT                           m_mouse_coordinates;
            DEVMODE		                    m_dmScreenSettings;
            int			                    m_pixel_format;
    };
}

