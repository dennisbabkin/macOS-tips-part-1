//
//  test_synched_data.h
//  macOS tips - part 1
//
//  Created by dennisbabkin.com on 6/11/23.
//
//  This project is a part of the blog post.
//  For more details, check:
//
//      https://dennisbabkin.com/blog/?i=AAA11400
//
//  Test class that demonstrates the use of a synchronized data template class
//


#ifndef test_synched_data_h
#define test_synched_data_h

#include "synched_data.h"

#include <string>



struct TENANT_INFO
{
    int floor;
    std::string name;
};



struct Globals
{
    SYNCHED_DATA<TENANT_INFO> g_Tenant;     //Class member that uses synchronized access to it
    
    
    Globals()
        : g_Tenant({})
    {
    }
 
    
    void vacateTenant(int aboveFloor)
    {
        g_Tenant.callFunc_ToSet(_callback, &aboveFloor, nullptr);
    }

    
private:

    ///This function is invoked from within the writer lock!
    static void _callback(TENANT_INFO* pTenant, const void* pParam1, const void* pParam2)
    {
        int* pFloor = (int*)pParam1;
        assert(pFloor);

        if(pTenant->floor > *pFloor)
        {
            pTenant->name.clear();
        }
    }
    
};



#endif /* test_synched_data_h */
