#ifndef STICK_THREAD_POOL_HPP
#define STICK_THREAD_POOL_HPP

namespace stick
{
    namespace detail
        {
            /**
             * @brief A Worker is one thread. Only for internal use.
             */
            template<class PoolType>
            class Worker
            {
            public:
                
                typedef typename PoolType::TaskType TaskType;
                
                Worker(PoolType * _pool);
                
                ~Worker()
                {
                    join();
                }
                
                void run();
                
                void join()
                {
                    m_thread.join();
                }
                
                bool busy() const;
                
                //called from threadpool
                void setBusy();
                
            private:
                
                //the pool that owns this worker
                PoolType * m_pool;
                
                Thread m_thread;
                
                std::atomic<bool> m_bIsProcessingTask;
            };
        }
        
        /**
         * @brief A first in first out scheduler for a ThreadPool
         *
         * A scheduler needs to implement all the functions you can see below:
         * add, next, pop, clear, size and empty.
         * The Scheduler will be locked from the ThreadPool, so you don't have to do any locking.
         *
         * @see ThreadPool
         */
        template<class TaskT>
        class FifoScheduler
        {
        private:
            
            typedef TaskT TaskType;
            
        public:
            
            /**
             * @brief Add a new task to the scheduler.
             */
            void add(const TaskType & _task);
            
            /**
             * @brief Get the next task from the scheduler.
             */
            const TaskType & next() const;
            
            /**
             * @brief Pop/remove the next task.
             */
            void pop();
            
            /**
             * @brief Remove all tasks.
             */
            void clear();
            
            /**
             * @brief Returns the number of tasks in the scheduler.
             */
            Size size() const;
            
            /**
             * @brief Returns true if the scheduler has no tasks left.
             */
            bool empty() const;
            
        private:
            
            std::deque<TaskType> m_tasks;
        };
        
        /**
         * @brief A ThreadPool is a group of threads that execute tasks in parallel.
         * @arg TaskT The type of task. This needs to be a callable type (i.e. functor, function etc.)
         * @arg SchedulingPolicy The policy responsible for scheduling tasks.
         * @see FifoScheduler
         */
        template<class TaskT,
        template<class> class SchedulingPolicy>
        class ThreadPoolT
        {
            friend class detail::Worker<ThreadPoolT>;
            
        private:
            
            typedef TaskT TaskType;
            
            typedef SchedulingPolicy<TaskType> SchedulerType;
            
            typedef detail::Worker<ThreadPoolT> WorkerType;
            
            typedef std::shared_ptr<WorkerType> WorkerPtr;
            
            typedef std::vector<WorkerPtr> WorkerPtrArray;
            
            typedef typename WorkerPtrArray::iterator WorkerIter;
            
            typedef typename WorkerPtrArray::const_iterator WorkerConstIter;
            
        public:
            
            /**
             * @brief Constructs a ThreadPool with a certain number of threads.
             * @param _threadCount The number of threads. Defaults to the number of physical cpu cores.
             */
            ThreadPoolT(Size _threadCount = Thread::hardwareConcurrency());
            
            /**
             * @brief Destructor
             */
            ~ThreadPoolT();
            
            /**
             * @brief Schedule a new task on the pool.
             * @param _task The job to execute.
             */
            void schedule(const TaskType & _task);
            
            /**
             * @brief Wait until a only a certain number of tasks remain in the scheduler.
             * @param _tasksLeft Wait until this number of jobs is left in the scheduler. Defaults to zero.
             */
            void wait(Size _tasksLeft = 0) const;
            
            /**
             * @brief Waits until all currently executing tasks finish and then stops all threads.
             */
            void stop();
            
            /**
             * @brief Resize the internal number of worker threads.
             * @param _numWorkers The new worker count.
             */
            void resize(Size _numWorkers);
            
            /**
             * @brief Returns the number of worker threads.
             */
            Size workerCount() const;
            
            /**
             * @brief Returns the number of workers currently executing a task.
             */
            Size activeWorkerCount() const;
            
            /**
             * @brief Returns the number of idling workers.
             */
            Size availableWorkerCount() const;
            
            /**
             * @brief Returns the number of tasks in the scheduler.
             */
            Size pendingTaskCount() const;
            
            /**
             * @brief Returns the default ThreadPool instance.
             *
             * The default instance is only initialized after the first call to this function.
             */
            static ThreadPoolT & defaultInstance();
            
        private:
            
            SchedulerType m_scheduler;
            
            //if no tasks are available, all worker threads wait on this condition variable
            mutable Condition m_newTaskCondition;
            
            mutable Condition m_taskFinishedCondition;
            
            WorkerPtrArray m_workers;
            
            //array of stopped, but not yet destected workers
            WorkerPtrArray m_stoppedWorkers;
            
            mutable Mutex m_mutex;
            
            std::atomic<bool> m_bStopThreads;
            
            Size m_threadCountToStop; //indicates how many threads should be terminated to reach the target thread number
            
            
            SchedulerType & scheduler();
            
            void finishedTask();
            
            void releaseStoppedWorkers();
            
            //non locking version
            Size activeNonLocking() const;
            
            //gets the next task for the requesting worker, or waits on m_newTaskCondition if none is available
            TaskType nextTask(WorkerType * _requester);
            
            //called from nextTask function, if a thread should be terminated (since there are more threads running than desired, i.e. after a resize call)
            void stopThread(WorkerType * _thread);
            
            //kills the thread and respawns a new one
            void workerDied(WorkerType * _thread);
            
            //the defaul instance of the TheadPool
            static SingletonHolder<ThreadPoolT> s_defaultInstance;
        };
        
        
        //implementations
        //________________________________________________
        
        
        //Worker
        namespace detail
        {
            template<class T>
            Worker<T>::Worker(T * _pool):
            m_pool(_pool),
            m_bIsProcessingTask(false),
            m_thread(std::bind(&Worker::run, this))
            {
                
            }
            
            template<class T>
            void Worker<T>::run()
            {
                try
                {
                    for(;;)
                    {
                        TaskType task = m_pool->nextTask(this);
                        //check if the task is valid, if not this means, that
                        //the worker should return and be terminated
                        if(task)
                        {
                            task();
                            
                            m_bIsProcessingTask = false;
                            
                            m_pool->finishedTask();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                catch(...)
                {
                    //the worker died due to an unhandled exception
                    m_pool->workerDied(this);
                }
                
                //make sure the thread is removed from the pool
                m_pool->stopThread(this);
            }
            
            template<class T>
            bool Worker<T>::busy() const
            {
                return m_bIsProcessingTask;
            }
            
            template<class T>
            void Worker<T>::setBusy()
            {
                m_bIsProcessingTask = true;
            }
        }
        
        
        //FifoScheduler
        
        template<class T>
        void FifoScheduler<T>::add(const T & _task)
        {
            m_tasks.push_back(_task);
        }
        
        template<class T>
        const T & FifoScheduler<T>::next() const
        {
            return m_tasks.front();
        }
        
        template<class T>
        void FifoScheduler<T>::pop()
        {
            m_tasks.pop_front();
        }
        
        template<class T>
        void FifoScheduler<T>::clear()
        {
            m_tasks.clear();
        }
        
        template<class T>
        Size FifoScheduler<T>::size() const
        {
            return m_tasks.size();
        }
        
        template<class T>
        bool FifoScheduler<T>::empty() const
        {
            return m_tasks.empty();
        }
        
        
        //ThreadPoolT
        
        //initialization of the default instance
        template<class TT, template<class> class ST>
        SingletonHolder<ThreadPoolT<TT, ST> > ThreadPoolT<TT, ST>::s_defaultInstance;
        
        template<class TT, template<class> class ST>
        ThreadPoolT<TT, ST>::ThreadPoolT(Size _threadCount):
        m_bStopThreads(false),
        m_threadCountToStop(0)
        {
            resize(_threadCount);
        }
        
        template<class TT, template<class> class ST>
        ThreadPoolT<TT, ST>::~ThreadPoolT()
        {
            stop();
        }
        
        template<class TT, template<class> class ST>
        void ThreadPoolT<TT, ST>::resize(Size _numWorkers)
        {
            ScopedLock lock(m_mutex);
            if(_numWorkers > m_workers.size())
            {
                Size diff = _numWorkers - m_workers.size();
                for(Size i = 0; i < diff; ++i)
                {
                    m_workers.push_back(WorkerPtr(new WorkerType(this)));
                }
            }
            else if(_numWorkers < m_workers.size())
            {
                m_threadCountToStop = m_workers.size() - _numWorkers;
                m_newTaskCondition.notifyAll();
            }
        }
        
        template<class TT, template<class> class ST>
        void ThreadPoolT<TT, ST>::schedule(const TaskType & _task)
        {
            ScopedLock lock(m_mutex);
            m_scheduler.add(_task);
            m_newTaskCondition.notifyOne();
        }
        
        template<class TT, template<class> class ST>
        void ThreadPoolT<TT, ST>::wait(Size _tasksLeft) const
        {
            ScopedLock lock(m_mutex);
            while(m_scheduler.size() + activeNonLocking() > _tasksLeft && !m_bStopThreads)
            {
                m_taskFinishedCondition.wait(lock);
            }
        }
        
        template<class TT, template<class> class ST>
        void ThreadPoolT<TT, ST>::finishedTask()
        {
            m_taskFinishedCondition.notifyAll();
        }
        
        template<class TT, template<class> class ST>
        void ThreadPoolT<TT, ST>::stop()
        {
            m_bStopThreads = true;
            WorkerPtrArray tmp;
            {
                ScopedLock lock(m_mutex);
                std::swap(m_workers, tmp);

                m_newTaskCondition.notifyAll();
                m_taskFinishedCondition.notifyAll();
            }
            
            WorkerConstIter it = tmp.begin();
            for(; it != tmp.end(); ++it)
            {
                (*it)->join();
            }
        }
        
        template<class TT, template<class> class ST>
        Size ThreadPoolT<TT, ST>::workerCount() const
        {
            ScopedLock lock(m_mutex);
            return m_workers.size();
        }
        
        template<class TT, template<class> class ST>
        Size ThreadPoolT<TT, ST>::activeWorkerCount() const
        {
            ScopedLock lock(m_mutex);
            return activeNonLocking();
        }
        
        template<class TT, template<class> class ST>
        Size ThreadPoolT<TT, ST>::activeNonLocking() const
        {
            Size ret = 0;
            WorkerConstIter it = m_workers.begin();
            for(; it != m_workers.end(); ++it)
            {
                if((*it)->busy())
                    ret++;
            }
            return ret;
        }
        
        template<class TT, template<class> class ST>
        Size ThreadPoolT<TT, ST>::availableWorkerCount() const
        {
            ScopedLock lock(m_mutex);
            Size ret = 0;
            WorkerConstIter it = m_workers.begin();
            for(; it != m_workers.end(); ++it)
            {
                if(!(*it)->busy())
                    ret++;
            }
            return ret;
        }
        
        template<class TT, template<class> class ST>
        Size ThreadPoolT<TT, ST>::pendingTaskCount() const
        {
            ScopedLock lock(m_mutex);
            return m_scheduler.size();
        }
        
        template<class TT, template<class> class ST>
        ThreadPoolT<TT, ST> & ThreadPoolT<TT, ST>::defaultInstance()
        {
            return s_defaultInstance.instance();
        }
        
        template<class TT, template<class> class ST>
        void ThreadPoolT<TT, ST>::releaseStoppedWorkers()
        {
            m_stoppedWorkers.clear();
        }
        
        template<class TT, template<class> class ST>
        void ThreadPoolT<TT, ST>::stopThread(WorkerType * _thread)
        {
            //if we are shutting down, we don't need to do anything here
            if(m_bStopThreads)
                return;
            
            ScopedLock lock(m_mutex);
            
            WorkerIter it = std::find_if(m_workers.begin(), m_workers.end(), RawPtrComparator<WorkerType>(_thread));
            if(it != m_workers.end())
            {
                m_stoppedWorkers.push_back(*it);
                m_workers.erase(it);
            }
        }
        
        template<class TT, template<class> class ST>
        void ThreadPoolT<TT, ST>::workerDied(WorkerType * _thread)
        {
            stopThread(_thread);
            resize(m_workers.size() + 1);
        }
        
        template<class TT, template<class> class ST>
        TT ThreadPoolT<TT, ST>::nextTask(WorkerType * _requester)
        {
            ScopedLock lock(m_mutex);
            TaskType ret;
            
            //do the housekeeping
            releaseStoppedWorkers();
            
            while(m_scheduler.empty() && !m_bStopThreads)
            {
                //check if this thread should be stoped
                if(m_threadCountToStop)
                {
                    m_threadCountToStop--;
                    return ret;
                }
                
                //wait until a new task is scheduled
                m_newTaskCondition.wait(lock);
            }
        
            if(m_bStopThreads)
                return ret; //empty task signals thread to exit.
            
            //return the next task
            _requester->setBusy();
            ret = m_scheduler.next();
            m_scheduler.pop();
            
            return ret;
        }
        
        typedef ThreadPoolT<std::function<void()>, FifoScheduler> ThreadPool;
    }
}

#endif //STICK_THREAD_POOL_HPP
