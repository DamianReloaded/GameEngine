#pragma once
#include "service.h"
#include <functional>
namespace reload {
    class display : public service
    {
        public:
            RELOAD_GAME_DECLARE_ID(20);

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
                typedef std::function<void()> ev_default;

                struct
                {
                    typedef std::function<void(uint32_t,bool)> ev_key;
                    typedef std::function<void(const std::string&)> ev_text;
                    ev_key     key;
                    ev_text    text;
                } keyboard;
                struct
                {
                    typedef std::function<void(uint32_t,bool)> ev_button;
                    typedef std::function<void(int32_t,int32_t)> ev_move;
                    ev_button   button;
                    ev_move     move;
                } pointer;

                struct
                {
                    ev_default          close;
                } window;

            } event;
        protected:
            class           implementation;
            std::unique_ptr<implementation> imp;
        private:
    };
}

