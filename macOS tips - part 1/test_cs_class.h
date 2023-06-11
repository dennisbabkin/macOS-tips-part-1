//
//  test_cs_class.h
//  macOS tips - part 1
//
//  Created by dennisbabkin.com on 6/11/23.
//
//  This project is a part of the blog post.
//  For more details, check:
//
//      https://dennisbabkin.com/blog/?i=AAA11400
//
//  Test class that demonstrates the use of a critical section
//


#ifndef test_cs_class_h
#define test_cs_class_h

#include "critical_section.h"





struct test_cs_class
{
    int getValue(void)
    {
        //Lock critical section
        ENTER_CRITICAL_SECTION lock(_cs);

        return _g_v;
    }
    
    int updateAndGetPrevious(int add)
    {
        int previous;

        //Lock critical section
        ENTER_CRITICAL_SECTION lock(_cs);

        previous = _g_v;
        _g_v += add;

        if(_g_v > 100)
        {
            _g_v = 100;
        }

        return previous;
    }


private:
    
    CRITICAL_SECTION _cs;       //Critical section for accessing data
    
    int _g_v = 0;
    
};



#endif /* test_cs_class_h */
