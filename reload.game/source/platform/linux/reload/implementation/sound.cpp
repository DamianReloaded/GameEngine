#include "sound.h"

namespace reload
{

    sound::implementation::implementation(reload::sound* p_sound) : m_sound(p_sound)
    {

    }

    sound::implementation::~implementation()
    {

    }

    bool sound::implementation::start ()
    {

        return true;
    }

    void sound::implementation::stop ()
    {

    }

    void sound::implementation::pause ()
    {

    }

    void sound::implementation::resume ()
    {

    }

    void sound::implementation::update ()
    {

    }

}
