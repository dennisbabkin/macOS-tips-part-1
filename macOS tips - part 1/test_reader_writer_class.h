//
//  test_reader_writer_class.h
//  macOS tips - part 1
//
//  Created by dennisbabkin.com on 6/11/23.
//
//  This project is a part of the blog post.
//  For more details, check:
//
//      https://dennisbabkin.com/blog/?i=AAA11400
//
//  Test class that demonstrates the use of a reader/writer lock
//


#ifndef test_reader_writer_class_h
#define test_reader_writer_class_h

#include "rdr_wrtr.h"



struct test_reader_writer_class
{
    
    
    int getValue(void)
    {
        //Acquire shared lock
        READER_LOCK rdl(_lock);

        return _g_v;
    }

    void updateValue(int add)
    {
        //Acquire exclusive lock
        WRITER_LOCK wtl(_lock);

        _g_v += add;

        if(_g_v > 100)
        {
            _g_v = 100;
        }
    }

    
private:

    RDR_WRTR _lock;       //Reader/writer lock for accessing data
    
    int _g_v = 0;
    
};


#endif /* test_reader_writer_class_h */
