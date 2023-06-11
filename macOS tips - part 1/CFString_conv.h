//
//  CFString_conv.h
//  macOS tips - part 1
//
//  Created by dennisbabkin.com on 6/11/23.
//
//  This project is a part of the blog post.
//  For more details, check:
//
//      https://dennisbabkin.com/blog/?i=AAA11400
//
//  Demonstration of string conversion from CFString to std::string
//


#ifndef CFString_conv_h
#define CFString_conv_h

#include <CoreFoundation/CoreFoundation.h>




///Convert 'ref' into std::string
///'strOut' = receives converted string, or an empty string if conversion fails
///RETURN:
///     = true if converted successfully
bool GetString_From_CFStringRef(CFStringRef ref, std::string& strOut)
{
    bool bRes = false;
    
    if(ref)
    {
        const char* pStr = CFStringGetCStringPtr(ref, kCFStringEncodingUTF8);
        if(pStr)
        {
            //Simple example
            strOut = pStr;
            
            bRes = true;
        }
        else
        {
            //Need to do more work
            CFIndex nchSize = CFStringGetLength(ref);     //This is the size in characters
            if(nchSize > 0)
            {
                CFIndex ncbSz = CFStringGetMaximumSizeForEncoding(nchSize,
                                                                  kCFStringEncodingUTF8);
                if(ncbSz != kCFNotFound)
                {
                    UInt8* pBuff = new (std::nothrow) UInt8[ncbSz + 1];
                    if(pBuff)
                    {
                        if(CFStringGetCString(ref,
                                              (char*)pBuff,
                                              ncbSz + 1,
                                              kCFStringEncodingUTF8))
                        {
                            //Note that 'ncbSz' will most certainly include trailing 0's,
                            //thus we cannot assume that that is the length of our string!
                            
                            //Let's place a safety null there first
                            pBuff[ncbSz] = 0;
                            
                            strOut = (const char*)pBuff;
                            
                            bRes = true;
                        }
                        else
                            assert(false);
                        
                        //Free mem
                        delete[] pBuff;
                        pBuff = nullptr;
                    }
                    else
                        assert(false);
                }
                else
                    assert(false);
            }
        }
    }
    
    if(!bRes)
    {
        //Clear resulting string on error
        strOut.clear();
    }
    
    return bRes;
}



#endif /* CFString_conv_h */
