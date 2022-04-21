#pragma once
#include <deque>
#include "task.h"
namespace reload {
    template <class TApp>
    class tasklist
    {
        public:
            tasklist ()
            {
            }

            virtual ~tasklist ()
            {
                terminate();
            }

            void terminate()
            {
                stop_all();
                delete_all_tasks();
            }

            virtual void update_task(task<TApp>* p_task)
            {
                if (!p_task->m_running) return;
                if (p_task->m_paused) return;
                p_task->update_begin();
                p_task->update();
                p_task->update_end();
            }

            void stop(task<TApp>* p_task)
            {
                if (p_task->m_running)
                {
                    p_task->m_running = false;
                    p_task->stopped();
                }
            }

            void start(task<TApp>* p_task)
            {
                if (p_task->index==task<TApp>::invalid_index) return;
                if (!p_task->m_running)
                {
                    p_task->m_running = true;
                    p_task->started();
                }
            }

            void stop_all()
            {
                for (auto t : tasks) stop(t);
            }

            void start_all()
            {
                for (auto t : tasks) stop(t);
            }

            virtual void update ()
            {
                for (auto t : tasks) update_task(t);
                if (tasks_to_remove.size()>0) process_tasks_to_remove();
                if (tasks_to_add.size()>0) process_tasks_to_add();
            }

            template <class T, typename... Args>
            T* add (Args&&... args)
            {
                auto t = new T(std::forward<Args>(args)...);
                t->parent(this);
                tasks_to_add.push_back(t);
                return t;
            }

            void remove(task<TApp>* p_task)
            {
                tasks_to_remove.push_back(p_task);
            }

            void pause(task<TApp>* p_task)
            {
                p_task->m_paused=true;
                p_task->paused();
            }

            void resume(task<TApp>* p_task)
            {
                p_task->m_paused=false;
                p_task->resumed();
            }

            void app (TApp* p_app)
            {
                m_app = p_app;
            }

            TApp* app()
            {
                return m_app;
            }

            std::deque<task<TApp>*> tasks;

        protected:
            TApp*                   m_app;
            std::deque<task<TApp>*> tasks_to_add;
            std::deque<task<TApp>*> tasks_to_remove;

            void delete_tasks(std::deque<task<TApp>*>& p_container)
            {
                for (auto i=p_container.end(); i!=p_container.begin();)
                {
                    --i;
                    auto t = *i;
                    remove_and_delete_task(t, p_container);
                }
                p_container.clear();
            }

            void delete_all_tasks()
            {
                delete_tasks(tasks_to_add);
                delete_tasks(tasks);
                tasks_to_remove.clear();
            }

            void process_tasks_to_add ()
            {
                for (auto t : tasks_to_add)
                {
                    tasks.push_back(t);
                    t->index = tasks.size()-1;
                    t->created();
                    start(t);
                    update_task(t);
                }
                tasks_to_add.clear();
            }

            void remove_and_delete_task(task<TApp>* p_task, std::deque<task<TApp>*>& p_container)
            {
                stop(p_task);
                p_container.erase(p_container.begin()+p_task->index);
                p_task->destroyed();
                delete p_task;
            }

            void process_tasks_to_remove ()
            {
                for (auto t : tasks_to_remove)
                {
                    remove_and_delete_task(t, tasks);
                }
                tasks_to_remove.clear();
            }
    };
}

