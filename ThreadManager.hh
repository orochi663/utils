#ifndef __THREADMANAGER_HH__
#define __THREADMANAGER_HH__
/*
  Copyrights (c)2020 Arty(arty.quantum)
*/
#include <unordered_map>
#include <string>
#include <queue>
#include <atomic>
#include <functional>
#include <thread>
#include <mutex>

#include "SingletonMacro.hh"

class ThreadManager
{

private:
    __SINGLETON__(ThreadManager)// use your own implemenation of singleton or use SingletonMacro.hh
    std::unordered_map<std::thread::id, std::thread> threads;
    std::queue<std::thread::id> exitingThreads;
    std::mutex map_mutex;
    std::mutex queue_mutex;
    std::thread poolManagerThread;
    std::atomic_bool queueSignal;
    std::atomic_bool quit;
    const int sleep_timer_in_ms = 200;// change as per need

    void threadPoolManagerFunction()
    {
        while (!quit)
        {
            if (queueSignal)
            {
                joinExitingThread();
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_timer_in_ms));
            }
        }
    }
    void joinExitingThread()
    {
        std::thread::id id;
        while (!exitingThreads.empty())
        {
            { // scope for lock quard
                const std::lock_guard<std::mutex> lock(queue_mutex);
                id = exitingThreads.front();
                exitingThreads.pop();
            }
            { // scope for lock quard
                const std::lock_guard<std::mutex> lock(map_mutex);
                if (threads[id].joinable())
                {
                    threads[id].join();
                }
                threads.erase(id);
            }
        }
    }
    void cleanup()
    {
        if (threads.size() > 0)
        {
            for (auto &element : threads)
            {
                if (element.second.joinable())
                {
                    element.second.join();
                }
            }
        }
        threads.clear();
    }

public:
    void init()
    {
        quit = false;
        poolManagerThread = std::thread(&ThreadManager::threadPoolManagerFunction, this);
    }
    ~ThreadManager()
    {
        joinAll();
        joinExitingThread();
        cleanup();
        quit = true;
        if (poolManagerThread.joinable())
        {
            poolManagerThread.join();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_timer_in_ms)); // wait for cleanup
    }

    template <typename _Callable, typename... _Args>
    void spawnThread(_Callable &&__fn, _Args &&... __args)
    {
        std::thread thread = std::thread([=]() {
            auto routine = std::bind(__fn, __args...);
            std::invoke(routine);
            { // scope for lock quard
                const std::lock_guard<std::mutex> lock(queue_mutex);
                exitingThreads.push(std::this_thread::get_id());
            }
            queueSignal = true;
        });
        { // scope for lock guard
            const std::lock_guard<std::mutex> lock(map_mutex);
            threads.insert(std::make_pair<std::thread::id, std::thread>(thread.get_id(), std::move(thread)));
        }
    }

    void joinAll()
    {
        // scope for lock quard
        const std::lock_guard<std::mutex> lock(queue_mutex);
        for (auto &element : threads)
        {
            exitingThreads.push(element.first);
        }
        queueSignal = true;
    }

    void joinAllAndCleanup()
    {
        joinAll();
        joinExitingThread();
        cleanup();
    }
};
/*
Usage: 
ThreadManager* threadManager = ThreadManager::getInstance();
threadManager.spawnThread(any_function, params);
......
threadManager.joinall();
......
threadManager.joinAllAndCleanup();
*/  
  
  
