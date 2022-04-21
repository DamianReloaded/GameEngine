#pragma once
#include "squeue.h"
#include "message.h"
namespace reload {

    class messagebus
    {
        public:
            messagebus () {}
            virtual ~messagebus () {}

            void push(imessage* t)
            {
                q.push(t);
            }

            imessage* pop()
            {
                return q.pop();
            }

        private:
            squeue<imessage*> q;
    };
}

