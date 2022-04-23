#pragma once
#include "identifiable.h"
namespace reload {
    struct service : public identifiable
    {
        virtual void                update          () { on_update_start(); on_update(); on_update_end(); }
        virtual void                on_update_start () {}
        virtual void                on_update_end   () {}
        virtual void                on_update       () {}
        virtual void                on_create       () {}
        virtual void                on_destroy      () {}
        virtual void                on_start        () =0;
        virtual void                on_stop         () =0;
        virtual void                on_pause        () {}
        virtual void                on_resume       () {}
        virtual void                start           () { if (m_running)  return; m_running = true;  on_start(); }
        virtual void                stop            () { if (!m_running) return; m_running = false; on_stop(); }
        virtual void                pause           () { if (m_paused)   return; m_paused = true;   on_pause(); }
        virtual void                resume          () { if (!m_paused)  return; m_paused = false;  on_resume(); }
        bool                        running         () { return m_running; }

        int run ()
        {
            on_create();
            start();
            m_running = true;
            while (m_running)
            {
                if (!m_paused)
                {
                    update();
                }
            }
            stop();
            on_destroy();
            return 0;
        }

    protected:
        bool                        m_running = false;
        bool                        m_paused = false;
    };
}

