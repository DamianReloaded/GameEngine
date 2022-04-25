#pragma once
#include <set>
#include "assert.h"
namespace reload {

    class idregistry
    {
        public:
            size_t add (const size_t& p_id)
            {
                reload_assert (ids.find(p_id)==ids.end(), reload::format("Identifiable id already registered '{i}'", p_id));
                ids.insert(p_id);
                return p_id;
            }

            size_t operator ()(const size_t& p_id) { return add(p_id); }

        protected:
            std::set<size_t> ids;
    };


    struct identifiable
    {
        virtual size_t          id      () const =0;
        template <class T> T*   cast    () // Returns nullptr if can't be casted to the requested type
        {
            static const T ref {};
            return (ref.id()==id())?static_cast<T*>(this):nullptr;
        }
    };

    #define RELOAD_GAME_DECLARE_ID(idvalue) static constexpr size_t sid = idvalue; size_t id() const { return sid; }

}

