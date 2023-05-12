#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include <vector>
#include <array>
#include "locker.h"
#include <memory>
#include <string_view>
using namespace std;
// 线程池类，将它定义为模板类是为了代码复用，模板参数T是任务类
template<typename T>
class threadpool {
public:
    threadpool(const threadpool&)=delete;
    threadpool& operator=(const threadpool&)=delete;
    bool append(T& request);
    static threadpool* getPoolInst();
    static void delPoolInst();
private:
    /*thread_number是线程池中线程的数量，max_requests是请求队列中最多允许的、等待处理的请求的数量*/
    threadpool(int thread_number = 16, int max_requests = 10000);
    ~threadpool();
    static threadpool* m_threadpool;

private:
    /*工作线程运行的函数，它不断从工作队列中取出任务并执行之*/
    static void* worker(void* arg);
    void run();

private:
    // 线程的数量
    int m_thread_number;  
    
    // 描述线程池的数组，大小为m_thread_number    
    vector<pthread_t> m_threads;

    // 请求队列中最多允许的、等待处理的请求的数量  
    int m_max_requests; 
    
    // 请求队列
    list< T* > m_workqueue;  

    // 保护请求队列的互斥锁
    pthread_mutex_t mu;  

    // 是否有任务需要处理
    sem m_queuestat;

    // 是否结束线程          
    bool m_stop;                    
};
template<typename T>
threadpool<T>* threadpool<T>::m_threadpool=new threadpool();

template< typename T >
threadpool< T >::threadpool(int thread_number, int max_requests) : 
        m_thread_number(thread_number), m_max_requests(max_requests), 
        m_stop(false) {

    if((thread_number <= 0) || (max_requests <= 0) ) {
        throw std::exception();
    }
    m_threads.resize(thread_number);
    // 创建thread_number 个线程，并将他们设置为脱离线程。
    for ( int i = 0; i < thread_number; ++i ) {
        printf( "create the %dth thread\n", i);
        if(pthread_create(&m_threads[i], NULL, worker, this ) != 0) 
            throw std::exception();
        
        if( pthread_detach( m_threads[i] ) ) 
            throw std::exception();
    }
}

template< typename T >
threadpool< T >::~threadpool() {
    delete m_threadpool;
    m_stop = true;
}

template<typename T>
threadpool<T>* threadpool<T>::getPoolInst()
{
    return m_threadpool;
}

template<typename T>
void threadpool<T>::delPoolInst()
{
    if (m_threadpool)
    {
        delete m_threadpool;
        m_threadpool=nullptr;
    }
}
template< typename T >
bool threadpool< T >::append(T& request )
{
    // 操作工作队列时一定要加锁，因为它被所有线程共享。
    if (1)
    {
        lock_guarder lg(mu);
        if ( m_workqueue.size() > m_max_requests ) 
            return false;
        m_workqueue.emplace_back(&request);
    }
    m_queuestat.post();
    return true;
}

template< typename T >
void* threadpool< T >::worker( void* arg )
{
    threadpool* pool = ( threadpool* )arg;
    pool->run();
    return pool;
}

template< typename T >
void threadpool< T >::run() {

    while (!m_stop) {
        m_queuestat.wait();
        T* request=nullptr;
        if(1)
        {
            lock_guarder lg(mu);
            if (!m_workqueue.empty() ) {  
            request=m_workqueue.front();
            m_workqueue.pop_front();
        }
        }        
        if (request)
            request->process();
    }

}

#endif
