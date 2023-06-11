//
//  interactive_login_sess.h
//  macOS tips - part 1
//
//  Created by dennisbabkin.com on 6/11/23.
//
//  This project is a part of the blog post.
//  For more details, check:
//
//      https://dennisbabkin.com/blog/?i=AAA11400
//
//  Demonstration of interactive login user sessions
//


#ifndef interactive_login_sess_h
#define interactive_login_sess_h


#include <string>
#include <vector>
#include <assert.h>

#include <pwd.h>
#include <utmpx.h>

#include "CFString_conv.h"




struct LOGIN_SESSION_INFO
{
    std::string strUserName;    //User name
    uid_t nUsrID = -1;          //User ID (or -1 if error)
    gid_t nGrpID = -1;          //User's group ID (or -1 if error)
};

#define SIZEOF(f) (sizeof(f) / sizeof(f[0]))    //Helper preprocessor definition to get the number of elements in C array



///Collect currently logged in user sessions
///'arrSessions' = receives info for all currently logged in user sessions
///RETURN:
///     - true if no errors
///     - false if some errors took place (note that 'arrSessions' may still contain some user sessions)
bool GetCurrentLoginSessions(std::vector<LOGIN_SESSION_INFO>& arrSessions)
{
    bool bRes = true;
    
    arrSessions.clear();
    
    LOGIN_SESSION_INFO lsi;
    
    //Reset the database
    setutxent();
    
    for(intptr_t t = 0;; t++)
    {
        //Make sure that we don't create an infinite loop here...
        //INFO: We need to do this because utmpx functions are kinda stupid.
        //      If they fail, they don't return a meaningful error code.
        if(t > 10000)
        {
            //Overflowed - something went really wrong!
            //We can't have over 10,000 user sessions...
            bRes = false;
            assert(false);
            
            break;
        }
        
        struct utmpx* pUtx = getutxent();
        if(!pUtx)
        {
            //Assume, no more ...
            break;
        }

        //Do some heuristic to see if we need this session
        if(pUtx->ut_type == USER_PROCESS &&
            pUtx->ut_id[0] == '/' &&
            pUtx->ut_id[1] == 0)
        {
            //We must have a user name
            if(pUtx->ut_user[0])
            {
                //Use this session
                pUtx->ut_user[SIZEOF(pUtx->ut_user) - 1] = 0;       //Safety null

                //This will be the user name
                lsi.strUserName = pUtx->ut_user;
                
                //Get other user info
                struct passwd *pw = getpwnam(pUtx->ut_user);
                if(pw)
                {
                    lsi.nUsrID = pw->pw_uid;
                    lsi.nGrpID = pw->pw_gid;
                }
                else
                {
                    //Failed to get
                    bRes = false;
                    
                    //Reset (but don't user 0's, as those are valid IDs!)
                    lsi.nUsrID = -1;
                    lsi.nGrpID = -1;
                }

                //Add it to our list to return back
                arrSessions.push_back(lsi);
            }
            else
            {
                //Empty user name - technically should not happen!
                bRes = false;
            }
        }
    }
    
    //Close the database
    endutxent();
    
    return bRes;
}





#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>

bool GetCurrentInteractiveLoginUser(std::string* pstrOutUserName = nullptr,
                                    uid_t* pOutUserID = nullptr,
                                    gid_t* pOutGroupID = nullptr)
{
    bool bResult = false;
    
    std::string strUsrName;
    
    uid_t uid = -1;
    gid_t gid = -1;
    
    CFStringRef refUsrName = SCDynamicStoreCopyConsoleUser(NULL, &uid, &gid);
    if(refUsrName)
    {
        if(GetString_From_CFStringRef(refUsrName, strUsrName))
        {
            bResult = true;
        }
        else
        {
            //Failed to convert
            assert(false);
        }
        
        CFRelease(refUsrName);
    }
    
    if(pstrOutUserName)
        *pstrOutUserName = strUsrName;
    
    if(pOutUserID)
        *pOutUserID = uid;
    
    if(pOutGroupID)
        *pOutGroupID = gid;
    
    return bResult;
}




#endif /* interactive_login_sess_h */
