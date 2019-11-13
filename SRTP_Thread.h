/* 
 * File:   SRTP_Thread.h
 * Author: Varun & Shruthi
 *
 * Created on November 3, 2008, 10:28 PM
 */

#ifndef _SRTP_THREAD_H
#define	_SRTP_THREAD_H

#include "SRTP_CommonTypes.h"
#include <iostream>
#include <pthread.h>

/* 
 *Base Thread class
 *
*/
class SRTP_Thread
{
private: 
    pthread_t m_thread;
    SRTP_ThreadID m_thread_id;
    bool m_thread_started;
    bool m_is_thread_running;

protected:
	bool m_stop_thread;
    
public:
    /* Constructor */
    SRTP_Thread(){
        std::clog<<"ENTER:\t SRTP_Thread::SRTP_Thread\n";
        m_thread_started = false;
        m_is_thread_running = false;
        m_thread_id = 0;
		m_stop_thread = false;
        std::clog<<"EXIT:\t SRTP_Thread::SRTP_Thread\n";
    }
    
    /* Destructor */
    ~SRTP_Thread(){
        std::clog<<"ENTER:\t SRTP_Thread::~SRTP_Thread\n";
        exit_thread();
        std::clog<<"EXIT:\t SRTP_Thread::~SRTP_Thread\n";
    }

    /* Function to start the thread */
    inline bool start_thread(){
        std::clog<<"ENTER:\t SRTP_Thread::start_thread\n";
       
        if(pthread_create(&m_thread, NULL, initiate_thread, this) != 0)
        {
            return false;
        }
        m_thread_started = true;
        m_is_thread_running = true;        
        
        std::clog<<"EXIT:\t SRTP_Thread::start_thread\n";
        return true;
    }
    
    static void* initiate_thread(void* arg){
        std::clog<<"ENTER:\t SRTP_Thread::initiate_thread\n";
        static_cast<SRTP_Thread*>(arg)->run();
        std::clog<<"EXIT:\t SRTP_Thread::initiate_thread\n";
    }
    
    virtual void run(){
        std::clog<<"ENTER:\t SRTP_Thread::run\n";
        std::clog<<"EXIT:\t SRTP_Thread::run\n";
    }
    
    inline bool join_thread(){
        std::clog<<"ENTER:\t SRTP_Thread::join_thread\n";
        if(pthread_join(m_thread, NULL) != 0)
        {
            std::clog<<"EXIT:\t SRTP_Thread::join_thread\n";
            return false;
        }
        std::clog<<"EXIT:\t SRTP_Thread::join_thread\n";
        return true;
    }

    inline bool close_thread(){
        std::clog<<"ENTER:\t SRTP_Thread::close_thread\n";
        if(m_stop_thread == false && m_is_thread_running != false)
        {
            if(pthread_cancel(m_thread) != 0)
            {
                std::clog<<"EXIT:\t SRTP_Thread::close_thread\n";
                return false;
            }

            std::clog<<"EXIT:\t SRTP_Thread::close_thread\n";
            return true;
        }
        else
        {
			std::clog<<"ERROR:\t Already stop initiate for the thread\n";
            std::clog<<"EXIT:\t SRTP_Thread::close_thread\n";
            return false;
        }
   }
    
    inline void exit_thread(){
        std::clog<<"ENTER:\t SRTP_Thread::exit_thread\n";
        int exit_code = -1;
        pthread_exit(&exit_code);
        std::clog<<"EXIT:\t SRTP_Thread::exit_thread\n";
    }
    
    inline pthread_t* get_thread(){
        std::clog<<"ENTER:\t SRTP_Thread::get_thread\n";
        std::clog<<"EXIT:\t SRTP_Thread::get_thread\n";
        return &m_thread;        
    }
    
    inline SRTP_ThreadID get_thread_id(){
        std::clog<<"ENTER:\t SRTP_Thread::get_thread_id\n";
        std::clog<<"EXIT:\t SRTP_Thread::get_thread_id\n";
        return m_thread;
    }
    
    inline bool is_thread_started(){
        std::clog<<"ENTER:\t SRTP_Thread::is_thread_started\n";
        std::clog<<"EXIT:\t SRTP_Thread::is_thread_started\n";
        return m_thread_started;
    }
    
    inline bool is_thread_running(){
        std::clog<<"ENTER:\t SRTP_Thread::is_thread_running\n";
        std::clog<<"EXIT:\t SRTP_Thread::is_thread_running\n";
        return m_is_thread_running;
    }    
};


#endif	/* _SRTP_THREAD_H */

