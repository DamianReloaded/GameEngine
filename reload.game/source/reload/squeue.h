#pragma once
#include <assert.h>
#include <queue>
#include <mutex>
#include <condition_variable>
namespace reload {
    template <class T>
    class squeue
    {
        public:
            squeue () : queue(), mutex(){}
            virtual ~squeue () {}

            void push(T t)
            {
                std::lock_guard<std::mutex> lock(mutex);
                queue.push(t);
            }

            T pop(void)
            {
                assert(queue.empty());
                std::unique_lock<std::mutex> lock(mutex);
                T val = queue.front();
                queue.pop();
                return val;
            }


        private:
            std::queue<T> queue;
            mutable std::mutex mutex;
    };
}

