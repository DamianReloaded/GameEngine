#pragma once
#include "service.h"
#include "tasklist.h"
#include "messagebus.h"
#include "assert.h"

namespace reload {

    class application : public service
    {
        public:
            application()
            {

            }

            virtual ~application()
            {
            }


        protected:
    };
}

