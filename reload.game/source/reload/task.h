#include <string_view>
#include "service.h"
#pragma once
namespace reload {
    template <class TApp> class tasklist;
    template <class TApp>
    class task : service
    {
        friend class tasklist<TApp>;
        public:
            virtual                     ~task           ()                                  { }
            virtual std::string_view    name            ()                                  { return "unnamed"; }
            virtual TApp*               app             ()                                  { return m_parent->app(); }
            void                        parent          (reload::tasklist<TApp>* p_parent)  { m_parent = p_parent; }
            reload::tasklist<TApp>*     parent          ()                                  { return m_parent; }
            virtual void                start           () override                         { parent()->start(this); }
            virtual void                stop            () override                         { parent()->stop(this); }
            virtual void                pause           () override                         { parent()->pause(this); }
            virtual void                resume          () override                         { parent()->resume(this); }

            static constexpr size_t     invalid_index   = (size_t)-1;
        protected:
                                        task            () : index(invalid_index), m_parent(nullptr) {}

            virtual void                on_start         () {}
            virtual void                on_stop         () {}


            size_t                      index;
            reload::tasklist<TApp>*     m_parent;
    };
}

