#ifndef Window_h
#define Window_h

#include <string>
#include <sstream>
#include "MessageTools.h"


class Data;

class Window
{
public:
    Window          (std::string name, int nBinsX, int nBinsY);
    virtual ~Window (void                                    );

    virtual bool               checkWindow     (float col     , float row         ) const = 0;
    virtual void               calculateWindow (int   planeID , const Data& aData,
                                                int   lowerCol, int   higherCol  ,
                                                int   lowerRow, int   higherRow   ) = 0;
    virtual void               removePixel     (int   col     , int   row         ) = 0;
    virtual void               sumThreaded     (void                              ){;}
    virtual std::pair<int,int> getNbins        (void                              ) const = 0;
    void                       setDoNotDelete  (bool  flag                        ){fDoNotDelete_ = flag;}

    void                       print           (void                              )const {STDLINE("Window name = " + name_, ACYellow);}

protected:
    std::string  name_        ;
    int          nBinsX_      ;
    int          nBinsY_      ;
    bool         fDoNotDelete_;
}; 

#endif
