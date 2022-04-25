#pragma once
#include "../../../../reload/sound.h"
namespace reload {
    class sound::implementation
    {
        public:
                                            implementation(reload::sound* p_sound);
            virtual                         ~implementation();

            bool                            start           ();
            void                            stop            ();
            void                            pause           ();
            void                            resume          ();
            void                            update          ();

        protected:
            reload::sound*                  m_sound;
        private:
    };
}

