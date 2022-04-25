#include <reload/implementation/sound.h>
namespace reload
{
    sound::sound()
    {
        imp = std::make_unique<sound::implementation>(this);
    }

    sound::~sound()
    {

    }
}
