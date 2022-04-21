#pragma once
#include "identifiable.h"
namespace reload {
    struct service : public identifiable
    {
        virtual void                start           () =0;
        virtual void                stop            () =0;
        virtual void                pause           () =0;
        virtual void                resume          () =0;
        virtual void                update          () =0;
        virtual void                update_begin    () =0;
        virtual void                update_end      () =0;
        virtual void                created         () =0;
        virtual void                destroyed       () =0;
        virtual void                started         () =0;
        virtual void                stopped         () =0;
        virtual void                paused          () =0;
        virtual void                resumed         () =0;

        protected:
        bool                        m_running = false;
        bool                        m_paused = false;

    };
}

