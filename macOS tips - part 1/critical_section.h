//
//  critical_section.h
//  macOS tips - part 1
//
//  Created by dennisbabkin.com on 6/11/23.
//
//  This project is a part of the blog post.
//  For more details, check:
//
//      https://dennisbabkin.com/blog/?i=AAA11400
//
//  Demonstration of critical section classes
//


#ifndef critical_section_h
#define critical_section_h


#include <pthread.h>
#include <assert.h>



struct CRITICAL_SECTION
{
    CRITICAL_SECTION()
    {
        if(_init_CS(_cs) != 0)
        {
            //Failed to initialize - abort!
            assert(false);
            abort();
        }
    }
    
    ~CRITICAL_SECTION()
    {
        pthread_mutex_destroy(&_cs);
    }
    
    ///Acquire a critical section lock
    ///INFO: This function does not return until the lock is available.
    ///      This function supports reentrancy, or calling it repeatedly
    ///      from the same thread without blocking.
    void EnterCriticalSection()
    {
        if(pthread_mutex_lock(&_cs) != 0)
        {
            //Failed to enter - abort!
            assert(false);
            abort();
        }
    }
    
    ///Leave a previously acquired critical section lock
    ///IMPORTANT: You must have called EnterCriticalSection() previously
    ///           the same number of times that you call this function!
    void LeaveCriticalSection()
    {
        if(pthread_mutex_unlock(&_cs) != 0)
        {
            //Failed to leave - abort!
            //There's either a failure in logic in the code that calls this function, or
            //there's some memory corruption...
            assert(false);
            abort();
        }
    }
    
private:
    
    ///Initialize multi-entrant critical section
    ///RETURN:
    ///     - 0 if success, otherwise error number
    int _init_CS(pthread_mutex_t& cs)
    {
        pthread_mutexattr_t attr;
        int r;

        r = pthread_mutexattr_init(&attr);
        if(r != 0)
        {
            //Error
            return r;
        }
        
        //Set mutex to be recursive
        r = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

        if(r == 0)
        {
            //Init mutex
            r = pthread_mutex_init(&cs, &attr);
            if(r != 0)
            {
                //Error
                assert(false);
            }
        }
        else
        {
            //Error
            assert(false);
        }

        //Free attributes
        pthread_mutexattr_destroy(&attr);

        return r;
    }
    
    
private:
    ///Copy constructor and assignments are NOT available!
    CRITICAL_SECTION(const CRITICAL_SECTION& s) = delete;
    CRITICAL_SECTION& operator = (const CRITICAL_SECTION& s) = delete;
    
private:
    
    pthread_mutex_t _cs = {};
};







struct ENTER_CRITICAL_SECTION
{
    ENTER_CRITICAL_SECTION(CRITICAL_SECTION& cs)
    : _cs(cs)
    {
        _cs.EnterCriticalSection();
    }
    
    ~ENTER_CRITICAL_SECTION()
    {
        _cs.LeaveCriticalSection();
    }
    
private:
    ///Copy constructor and assignments are NOT available!
    ENTER_CRITICAL_SECTION(const ENTER_CRITICAL_SECTION& s) = delete;
    ENTER_CRITICAL_SECTION& operator = (const ENTER_CRITICAL_SECTION& s) = delete;
    
private:
    CRITICAL_SECTION& _cs;
};







#endif /* critical_section_h */
