#include "display.h"
#include <cstring>
#include <dependencies/glew/glew.h>
#include <dependencies/glew/wglew.h>

namespace reload
{
    WNDPROC display::implementation::m_wndproc = nullptr;

    display::implementation::implementation(reload::display* p_display)
    {
        m_display = p_display;
        xoffset = 1920*(0);
        yoffset = 0;
        width = 1024;
        height = 768;
        m_vsync = false;
        m_fullscreen = false;
        current_video_mode = 0;
        m_multisampling = 0;
        m_created         = false;
        m_hinstance       = GetModuleHandle(nullptr);
        m_hwnd            = nullptr;
        m_hdc             = nullptr;
        m_hrc             = nullptr;
        m_style           = WS_OVERLAPPEDWINDOW;
        m_extended_style  = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        m_pixel_format    = 0;
        m_wndproc         = &display::implementation::winproc;

    }

    display::implementation::~implementation()
    {
        stop();
    }

    bool display::implementation::start()
    {
        return create();
    }

    void display::implementation::process_events()
    {
        MSG		msg;
        while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                //owl::log << "close pressed" <<owl::endl;
                //break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void display::implementation::stop()
    {
        if (m_hwnd==nullptr) return;

		if (m_hrc)												// Do We Have A Rendering Context?
		{
			if (!wglMakeCurrent(NULL,NULL))						// Are We Able To Release The DC And RC Contexts?
			{
			}

            if (!wglDeleteContext(m_hrc))						// Are We Able To Delete The RC?
            {
            }
            m_hrc=NULL;
		}

		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG)0);

		if (m_hdc)						// Are We Able To Release The DC
		{
            ReleaseDC(m_hwnd,m_hdc);
			m_hdc=NULL;
		}

		if (m_hwnd)						// Are We Able To Destroy The Window?
		{
            DestroyWindow(m_hwnd);
			m_hwnd=NULL;
		}
    }

bool display::implementation::create (const bool& _from_multisampling)
{
	if (m_created && !_from_multisampling) return m_created;

	WNDCLASS	wc;
	RECT		WindowRect;
	uint32_t	PixelFormat;

	WindowRect.left	    = (long)0;
	WindowRect.right    = (long)0+(long)width;
	WindowRect.top      = (long)0;
	WindowRect.bottom   = (long)0+(long)height;
	auto classname      = "owl::win::window";

    //if (!_from_multisampling) owl::log << "Initializing " << classname << owl::endl;

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) m_wndproc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hinstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classname;

	if (!RegisterClass(&wc))
	{
		if (GetLastError()!=1410) // Don't log 'already registered' error.
        {
            //owl::log << "Cannot Register Class: " << std::string(wc.lpszClassName) << " - Error: " << to_number<int>((int)GetLastError()) << owl::endl;
        }
	}

	AdjustWindowRectEx(&WindowRect, m_style, false, m_extended_style);

	HWND parent = NULL;

	/*if (m_window->m_parent!=NULL)
    {
        m_style = WS_VISIBLE | WS_CHILD;
        parent = m_window->m_parent->imp->m_hwnd;
    }*/

	// Create The Window
	if (!(m_hwnd=CreateWindowEx(m_extended_style,
								classname,
								m_title.c_str(),
								m_style |
								WS_CLIPSIBLINGS |					// Required Window m_style
								WS_CLIPCHILDREN,					// Required Window m_style
								0, 0, //x,y
								WindowRect.right-WindowRect.left,   // Calculate Adjusted Window Width
                                WindowRect.bottom-WindowRect.top,
								parent,
								NULL,								// No Menu
								m_hinstance,
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		//owl::log << "'CreateWindowEx' Error: " << GetLastError() << owl::endl;
		return m_created;
	}

    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);

	static	PIXELFORMATDESCRIPTOR pfd=							// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),							// Size Of This Pixel Format Descriptor
		1,														// Version Number
		PFD_DRAW_TO_WINDOW |									// Format Must Support Window
		PFD_SUPPORT_OPENGL |									// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,										// Must Support Double Buffering
		PFD_TYPE_RGBA,											// Request An RGBA Format
		(uint8_t)32,											// Select Our Color Depth (bpp)
		0, 0, 0, 0, 0, 0,										// Color Bits Ignored
		8,														// Alpha Buffer 8bits
		0,														// Shift Bit Ignored
		0,														// No Accumulation Buffer
		0, 0, 0, 0,												// Accumulation Bits Ignored
		24,														// 16Bit Z-Buffer (Depth Buffer)
		0,														// No Stencil Buffer
		0,														// No Auxiliary Buffer
		PFD_MAIN_PLANE,											// Main Drawing Layer
		0,														// Reserved
		0, 0, 0													// Layer Masks Ignored
	};

	if (!(m_hdc=GetDC(m_hwnd)))
	{
		//owl::log << "Can't get DC. Error: " << GetLastError() << owl::endl;
		return m_created;
	}

	if (!(PixelFormat=ChoosePixelFormat(m_hdc,&pfd)))
	{
		//owl::log << "ChoosePixelFormat Error: " << GetLastError() << owl::endl;
		return m_created;
	}

	if(!SetPixelFormat(m_hdc,(m_pixel_format==0)?PixelFormat:m_pixel_format,&pfd))
	{
		//owl::log << "SetPixelFormat Error: " << GetLastError() << owl::endl;
		return m_created;
	}

	if (m_hrc==NULL)
    {
        if (!(m_hrc=wglCreateContext(m_hdc)))
        {
            //owl::log << "wglCreateContext Error: " << GetLastError() << owl::endl;
            return m_created;
        }
    }

	if(!wglMakeCurrent(m_hdc,m_hrc))
	{
		//owl::log << "wglMakeCurrent Error: " << GetLastError() << owl::endl;
		return m_created;
	}

    // Enable VSync
    PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");
    if (strstr(_wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != NULL)
    {
        auto wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
        wglSwapIntervalEXT((m_vsync)?1:0);
    }

	if (m_fullscreen)
	{
		fullscreen(true);
	}
	else
	{
		//if (m_window->m_parent==NULL) center();
	}

	m_created = (glewInit()==GLEW_OK);

	swap_buffers();

	if (!_from_multisampling) init_multisampling (m_multisampling);

	raise();
	UpdateWindow(m_hwnd);

	return m_created;
}

const bool&	display::implementation::fullscreen ()
{
	return m_fullscreen;
}

void display::implementation::fullscreen (const bool& _val, const bool& _store)
{
	if (_store) m_fullscreen = _val;

	if (!m_hwnd)
	{
		return;
	}

	//owl::log << "Initializing fullscreen." << owl::endl;

	if (_val)
	{
		memset(&m_dmScreenSettings,0,sizeof(m_dmScreenSettings));	// Makes Sure Memory's Cleared
		m_dmScreenSettings.dmSize=sizeof(m_dmScreenSettings);		// Size Of The Devmode Structure

		query_video_modes();
		video_mode m_video_mode;
		m_video_mode.width = width;
		m_video_mode.height = height;
		m_video_mode = get_best_video_mode( m_video_mode );
		width = m_video_mode.width;
		height = m_video_mode.height;

		// Get the parameters for the best matching display mode
		(void) EnumDisplaySettings( NULL, m_video_mode.mode, &m_dmScreenSettings );

		m_dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;

		m_extended_style=WS_EX_APPWINDOW | WS_EX_TOPMOST;					// Window Extended m_style
		m_style=WS_POPUP;										// Windows m_style

		SetWindowLongPtr(m_hwnd, GWL_STYLE, m_style);
		SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, m_extended_style);
		SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, width, height, SWP_HIDEWINDOW);

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&m_dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","Message",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				m_fullscreen=false;								// Windowed Mode Selected.  Fullscreen = false
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return;
			}
		}

		//raise();

		//owl::log << "Video mode changed to " << m_video_mode.width << "x" << m_video_mode.height << "x" <<m_video_mode.bpp << " at " <<m_video_mode.frequency <<"hz" <<owl::endl;
	}
	else //windowed
	{
		ChangeDisplaySettings(NULL,0);						// If So Switch Back To The Desktop

		if (_store)
		{
			m_extended_style |=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended m_style
			//if (!m_window->m_parent) m_style |=WS_OVERLAPPEDWINDOW;							// Windows m_style

			SetWindowLongPtr(m_hwnd, GWL_STYLE, m_style);
			SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, 0);

			set_size(width, height);
		}
	}
}

void display::implementation::set_size (int p_width, int p_height)
{
    width=p_width;
    height=p_height;

    if (!m_hwnd) return;

    RECT rc = {0, 0, width, height};

    AdjustWindowRectEx(&rc,                // pointer to the RECT structure to use
                       m_style,     // window styles
                       false,    // TRUE if the window has a menu, FALSE if not
                       0);  // extended window styles

    SetWindowPos (m_hwnd, HWND_NOTOPMOST, 0, 0, rc.right-rc.left, rc.bottom-rc.top, SWP_NOMOVE);
    MoveWindow(m_hwnd, 0, 0, rc.right-rc.left, rc.bottom-rc.top, 1);
}

void display::implementation::raise ()
{
	if (m_fullscreen)
		ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
	else
		ShowWindow(m_hwnd, SW_NORMAL);
}

bool display::implementation::init_multisampling (const int& _num_samples)
{
	if (_num_samples<1) return false;

	m_multisampling = _num_samples;

	if(m_pixel_format == 0)
	{
		if(GLEW_ARB_multisample)
		{
			while(m_multisampling > 0)
			{
				UINT NumFormats = 0;

				int PFAttribs[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, 32,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
					WGL_SAMPLES_ARB, m_multisampling,
					0
				};

				if(wglChoosePixelFormatARB(m_hdc, PFAttribs, NULL, 1, &m_pixel_format, &NumFormats) == TRUE && NumFormats > 0) break;

				m_multisampling--;
			}

			stop();

            wglDeleteContext(m_hrc);
            m_hrc=NULL;

            //owl::log << "Initializing multisampling to " << m_multisampling << owl::endl;
			return create (true);
		}
		else
		{
			m_multisampling = 0;
			return true;
		}
	}
	else
	{
		m_multisampling = 0;
		return true;
	}

	//owl::log << "Can't Find A Suitable multisampling Pixel Format of value " << _num_samples << owl::endl;
	return false;
}

    void display::implementation::swap_buffers()
    {
        SwapBuffers(m_hdc);
    }

    void display::implementation::make_context_current ()
    {
        wglMakeCurrent(m_hdc, m_hrc);
    }

    display::implementation::video_mode display::implementation::get_best_video_mode( const display::implementation::video_mode& v )
    {
        size_t i = 0;
        video_mode default_choice;
        video_mode* selected = &default_choice;
        auto& vms = video_modes;

        for (i=0; i<vms.size(); i++)
        {
            if ( (vms[i].width<=v.width && vms[i].width>=selected->width) && (vms[i].height<=v.height && vms[i].height>=selected->height) && vms[i].bpp >= selected->bpp)
            {
                selected = &vms[i];
            }

            if ( vms[i].width==selected->width && vms[i].height==selected->height && vms[i].frequency>=selected->frequency)
            {
                selected = &vms[i];
            }
        }

        return *selected;
    }

    void display::implementation::insert_video_mode (DEVMODE& dm, int mode)
    {
        video_mode vm;
        vm.width		= dm.dmPelsWidth;
        vm.height		= dm.dmPelsHeight;
        vm.bpp			= dm.dmBitsPerPel;
        vm.frequency	= dm.dmDisplayFrequency;
        vm.mode			= mode;
        video_modes.push_back(vm);
    }

    void display::implementation::query_video_modes()
    {
        video_modes.clear();
        DEVMODE dm;
        int mode = 0;
        bool success = true;
        while( success )
        {
            dm.dmSize = sizeof( DEVMODE );
            success = EnumDisplaySettings( NULL, mode, &dm )>0;
            if (success)
            {
                if ((dm.dmBitsPerPel==16 || dm.dmBitsPerPel==32) && dm.dmDisplayFrequency>=60)
                {
                    insert_video_mode(dm, mode);
                }
            }
            mode++;
        }
        if (current_video_mode>video_modes.size()-1) current_video_mode = 0;
    }

LRESULT CALLBACK display::implementation::winproc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	auto * imp= (display::implementation*)GetWindowLongPtr(hwnd, GWLP_USERDATA );

	if (imp != NULL)
	{
		switch (umsg)
		{
			case WM_CLOSE:
			{
			    if (imp->m_display->event.window.close!=nullptr) imp->m_display->event.window.close();
				PostQuitMessage(0);
				return false;
			}
			break;

			case WM_MOVE:
            {

                // this gets called when control is moved programatically so beware of recursive calls
            }
            break;

			case WM_SIZE:											// Resize The OpenGL Window
			{
                //imp->m_area.size.set(LOWORD(lparam), HIWORD(lparam));
			    //if (imp->m_display->event.resize!=nullptr) imp->m_display->event.resize(imp->m_display);
			}
			break;

			case WM_PAINT:
            {
//                if (imp->m_display->event.paint!=nullptr)
//                {
//                    imp->m_display->event.paint(imp->m_display);
//                }
            }
            break;

            case WM_SETFOCUS:
            {
//                imp->m_focused = true;
//                imp->m_display->on_focus_in();
            }
            break;

            case WM_KILLFOCUS:
            {
//                imp->m_focused = false;
//                imp->m_display->on_focus_out();
            }
            break;

			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
            {
//                imp->m_focused = true;
//                imp->m_display->on_focus_in();
//                input::button::id button = (umsg==WM_LBUTTONDOWN)?input::button::mouse1:((umsg==WM_MBUTTONDOWN)?input::button::mouse2:input::button::mouse3);
//                imp->m_display->mouse.x = GET_X_LPARAM(lparam);
//                imp->m_display->mouse.y = GET_Y_LPARAM(lparam);
//                imp->m_display->on_pointer_press(imp->m_display->mouse,button);
            }
            break;

			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
            {
//                input::button::id button = (umsg==WM_LBUTTONUP)?input::button::mouse1:((umsg==WM_MBUTTONUP)?input::button::mouse2:input::button::mouse3);
//                imp->m_display->mouse.x = GET_X_LPARAM(lparam);
//                imp->m_display->mouse.y = GET_Y_LPARAM(lparam);
//                imp->m_display->on_pointer_release(imp->m_display->mouse,button);
            }
            break;

            case WM_MOUSEWHEEL:
            {
//                imp->m_display->mouse.wheel = static_cast<int16_t>(HIWORD(wparam)) / 120;
//                imp->m_display->on_pointer_scroll (imp->m_display->mouse.wheel);
            }
            break;

            case WM_MOUSEMOVE:
            {
//                if (!imp->m_mouse_inside)
//                {
//                    imp->track_mouse(hwnd);
//                    imp->m_mouse_inside = true;
//                    imp->m_display->on_pointer_enter();
//                }
//
//                imp->m_display->mouse.x = GET_X_LPARAM(lparam);
//                imp->m_display->mouse.y = GET_Y_LPARAM(lparam);
//
//                imp->m_display->on_pointer_move(imp->m_display->mouse);
            }
            break;

            case WM_MOUSEHOVER:
            {

            }
            break;

            case WM_MOUSELEAVE:
            {
//                imp->m_mouse_inside = false;
//                imp->m_display->on_pointer_leave();
            }
            break;

            case WM_KEYDOWN:
            {
//                imp->m_display->m_input->keyboard.implementation()->process(umsg, wparam, lparam, imp->m_inputmsg);
//                imp->m_display->on_key_press(imp->m_inputmsg);
            }
            break;
            case WM_KEYUP:
            {
//                imp->m_display->m_input->keyboard.implementation()->process(umsg, wparam, lparam, imp->m_inputmsg);
//                imp->m_display->on_key_release(imp->m_inputmsg);
            }
            break;
            case WM_UNICHAR:
            case WM_CHAR:
            {
//                imp->m_display->m_input->keyboard.implementation()->process(umsg, wparam, lparam, imp->m_inputmsg);
//                if (imp->m_inputmsg.unicode!=0) imp->m_display->on_key_text(imp->m_inputmsg);
            }
            break;

            case WM_ACTIVATE:
            {
//                imp->m_active = LOWORD(wparam)!=0;
                return 0;
            }
            break;

            case WM_INPUT:
            {
//                PRAWINPUT pRawInput;
//                UINT      bufferSize;
//                HANDLE    hHeap;
//                auto      joy = imp->m_display->m_input->joystick.implementation();
//                GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
//
//                hHeap     = GetProcessHeap();
//                pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, bufferSize);
//                if(!pRawInput) return 0;
//
//                GetRawInputData((HRAWINPUT)lparam, RID_INPUT, pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));
//                joy->process(pRawInput);
//
//                HeapFree(hHeap, 0, pRawInput);
//
////                std::cout << i << std::endl;
////                std::cout << "axis_x: " << joy->axis_x << std::endl;
////                std::cout << "axis_y: " << joy->axis_y << std::endl;
////                std::cout << "axis_z: " << joy->axis_z << std::endl;
////                std::cout << "axis_rz: " << joy->axis_rz << std::endl;
////                std::cout << "hat: " << joy->hat << std::endl;
//
//                for (size_t i=0; i<joy->max_buttons; i++)
//                {
//                    if (!joy->previous[i] && joy->button[i]) imp->m_display->on_joystick_press (5, (owl::input::button::id)(input::button::button1+i));
//                    if (joy->previous[i] && !joy->button[i]) imp->m_display->on_joystick_release (5, (owl::input::button::id)(input::button::button1+i));
//
//                    // X-Axis
//                    if (joy->axis_x_prev!=joy->axis_x)
//                    {
//                        if (joy->axis_x>0)
//                        {
//                            imp->m_display->on_joystick_press (5, (owl::input::button::id)(input::button::axis_x_pos));
//                        }
//                        else if (joy->axis_x<0)
//                        {
//                            imp->m_display->on_joystick_press (5, (owl::input::button::id)(input::button::axis_x_neg));
//                        }
//                        else
//                        {
//                            if (joy->axis_x_prev>0)
//                            {
//                                imp->m_display->on_joystick_release (5, (owl::input::button::id)(input::button::axis_x_pos));
//                            }
//                            else if (joy->axis_x_prev<0)
//                            {
//                                imp->m_display->on_joystick_release (5, (owl::input::button::id)(input::button::axis_x_neg));
//                            }
//
//                        }
//                    }
//
//                    // Y-Axis
//                    if (joy->axis_y_prev!=joy->axis_y)
//                    {
//                        if (joy->axis_y>0)
//                        {
//                            imp->m_display->on_joystick_press (5, (owl::input::button::id)(input::button::axis_y_pos));
//                        }
//                        else if (joy->axis_y<0)
//                        {
//                            imp->m_display->on_joystick_press (5, (owl::input::button::id)(input::button::axis_y_neg));
//                        }
//                        else
//                        {
//                            if (joy->axis_y_prev>0)
//                            {
//                                imp->m_display->on_joystick_release (5, (owl::input::button::id)(input::button::axis_y_pos));
//                            }
//                            else if (joy->axis_y_prev<0)
//                            {
//                                imp->m_display->on_joystick_release (5, (owl::input::button::id)(input::button::axis_y_neg));
//                            }
//
//                        }
//                    }
//
//                    // Z-Axis
//                    if (joy->axis_z_prev!=joy->axis_z)
//                    {
//                        if (joy->axis_z>0)
//                        {
//                            imp->m_display->on_joystick_press (5, (owl::input::button::id)(input::button::axis_z_pos));
//                        }
//                        else if (joy->axis_z<0)
//                        {
//                            imp->m_display->on_joystick_press (5, (owl::input::button::id)(input::button::axis_z_neg));
//                        }
//                        else
//                        {
//                            if (joy->axis_z_prev>0)
//                            {
//                                imp->m_display->on_joystick_release (5, (owl::input::button::id)(input::button::axis_z_pos));
//                            }
//                            else if (joy->axis_z_prev<0)
//                            {
//                                imp->m_display->on_joystick_release (5, (owl::input::button::id)(input::button::axis_z_neg));
//                            }
//                        }
//                    }
//
//                }
            }
            return 0;

		}
	}

	return DefWindowProc(hwnd,umsg,wparam,lparam);				// Pass All Unhandled Messages To DefWindowProc
}
}
