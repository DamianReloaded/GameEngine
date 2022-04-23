#pragma once
#include "service.h"
#include <functional>
namespace reload {
    class display : public service
    {
        public:
            RELOAD_GAME_ID(20);

                        display();
            virtual     ~display();

            virtual void on_start() override;
            virtual void on_stop() override;
            virtual void on_pause() override;
            virtual void on_resume() override;
            virtual void on_create() override;
            virtual void on_destroy() override;
            virtual void on_update() override;
            void         swap_buffers();

            struct
            {
                typedef std::function<void()>   void_function;
                void_function                   closed;
            } event;
        protected:
            class           implementation;
            std::unique_ptr<implementation> imp;
        private:
    };
}

