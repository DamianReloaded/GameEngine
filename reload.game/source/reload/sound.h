#pragma once
#include "service.h"
#include <functional>
namespace reload {
    class sound : public service
    {
        public:
                                    sound     ();
            virtual                 ~sound    ();

            virtual void on_start() override;
            virtual void on_stop() override;
            virtual void on_pause() override;
            virtual void on_resume() override;
            virtual void on_create() override;
            virtual void on_destroy() override;
            virtual void on_update() override;

            std::string             message;

        protected:
            class           implementation;
            std::unique_ptr<implementation> imp;
    };
}

