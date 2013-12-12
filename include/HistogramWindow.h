#ifndef HistogramWindow_h
#define HistogramWindow_h

#include "Window.h"
#include "Data.h"

class TTree;
class TH2F;

class HistogramWindow : public Window
{
public:
    HistogramWindow (std::string name, int nBinsX, int nBinsY);
    ~HistogramWindow(void                                    );

    bool checkWindow       (float col, float row) const;
    void calculateWindow   (int   planeID , const Data& aData,
                            int   lowerCol, int   higherCol  ,
                            int   lowerRow, int   higherRow    );
    void removePixel       (int   col     , int   row          );
    void sumThreaded       (void                               ){;}

    std::pair<int,int> getNbins(void)const {return std::make_pair(nBinsX_,nBinsY_);}

private:
    TH2F* theHWindow_;
}; 

#endif
