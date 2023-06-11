//
//  main.cpp
//  macOS tips - part 1
//
//  Created by dennisbabkin.com on 6/11/23.
//
//  This project is a part of the blog post.
//  For more details, check:
//
//      https://dennisbabkin.com/blog/?i=AAA11400
//


#include <iostream>

#include "test_cs_class.h"
#include "test_reader_writer_class.h"
#include "test_synched_data.h"
#include "interactive_login_sess.h"

#include <pthread.h>
#include <assert.h>
#include <string>
#include <vector>



//Forward function declarations
void* testThread(void *arg);
bool IsRunningAsRoot(void);



//Global variables
test_cs_class g_testCS;                 //Class that uses critical section for synchronization
test_reader_writer_class g_testRW;      //Class that uses reader/writer lock for synchronization
Globals g_Globals;                      //Class that uses a synchronized template class




//Entry point for this executable
//
int main(int argc, const char * argv[])
{
    int nErr = 0;
    
    const int knNumThreads = 3;
    pthread_t threads[knNumThreads] = {};
    
    //Create a few threads for our tests
    for(int t = 0; t < knNumThreads; t++)
    {
        nErr = pthread_create(&threads[t], nullptr, testThread, (void*)(intptr_t)(t));
        assert(nErr == 0);
    }
    
    //Wait for all of our threads to exit
    for(int t = 0; t < knNumThreads; t++)
    {
        pthread_join(threads[t], nullptr);
    }

    printf("Result of CS test: %d\n", g_testCS.getValue());
    printf("Result of RW test: %d\n", g_testRW.getValue());
    
    
    
    //Collect all interactive login user sessions
    std::vector<LOGIN_SESSION_INFO> arrLSIs;
    if(!GetCurrentLoginSessions(arrLSIs))
    {
        //Something went wrong
        assert(false);
    }
    
    printf("Found %lu interactive login user session(s):\n", arrLSIs.size());
    
    //Print all interactive login user sessions
    for(const LOGIN_SESSION_INFO& lsi : arrLSIs)
    {
        printf(" - User=\"%s\", UserID=%d, GroupID=%d\n",
               lsi.strUserName.c_str(),
               lsi.nUsrID,
               lsi.nGrpID);
    }

    
    
    //Get Interactive Login Session That This Process Runs In
    std::string strUsrName;
    uid_t userID;
    gid_t grpID;
    if(GetCurrentInteractiveLoginUser(&strUsrName, &userID, &grpID))
    {
        printf("Current interactive user: \"%s\", UserID=%d, GroupID=%d\n",
               strUsrName.c_str(),
               userID,
               grpID);
    }
    else
    {
        //Failed
        assert(false);
    }
    
    
    //Are we running as a root user?
    if(IsRunningAsRoot())
    {
        printf("The process is running as root!\n");
    }
    else
    {
        printf("The process is not running as root.\n");
    }
    
    
    return nErr;
}




void* testThread(void *arg)
{
    int v = (int)(intptr_t)arg;
    
    v++;
    
    //Example of use of a critical section
    g_testCS.updateAndGetPrevious(v);
    
    //Example of use of a reader/writer lock
    g_testRW.updateValue(v);

    
    //To read from a synchronized data variable
    TENANT_INFO tenant;
    g_Globals.g_Tenant.get(&tenant);

    printf("Tenant floor: %d, name=\"%s\"\n", tenant.floor, tenant.name.c_str());
    
    g_Globals.vacateTenant(v);

    //Or, to set it
    tenant.name = "John Doe";
    g_Globals.g_Tenant.set(&tenant);
    
    
    return nullptr;
}




///RETURN: - true if this process is running as root
bool IsRunningAsRoot(void)
{
    return geteuid() == 0;
}




