#include <string_view>
#pragma once
namespace reload {
    template <class TApp> class tasklist;
    template <class TApp>
    class task
    {
        friend class tasklist<TApp>;
        public:
                                        task            () : index(0), m_running(false), m_paused(false), m_parent(nullptr) {}
            virtual                     ~task           () {}
            virtual void                created         () {}
            virtual void                destroyed       () {}
            virtual void                started         () {}
            virtual void                stopped         () {}
            virtual void                paused          () {}
            virtual void                resumed         () {}
            virtual void                update          ()=0;
            virtual void                before_update   () {};
            virtual void                after_update    () {};
            virtual std::string_view    name            () { return "unnamed"; }
            virtual TApp*               app             () { return m_parent->app(); }
            void                        parent          (reload::tasklist<TApp>* p_parent) {m_parent = p_parent;}
            reload::tasklist<TApp>*     parent          () {return m_parent;}

        protected:
            size_t                      index;
            bool                        m_running;
            bool                        m_paused;
            reload::tasklist<TApp>*     m_parent;
    };
}

