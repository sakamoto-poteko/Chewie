#ifndef DataClass_h
#define DataClass_h

#include <cassert>
#include "MessageTools.h"

class TTree;

class Data
{
public:
    Data  (void );
    ~Data (void );

    void         branch                             (TTree*        tree    );
    void         setBranchAddress                   (TTree*        tree    );

    void         setEventNumber	                    (int           v       ){eventNumber_                         = v;}
    void         setRunNumber	                    (int           v       ){runNumber_                           = v;}
    void         setNumberOfTracks	                (int           v       ){numberOfTracks_                      = v;}
    void         setTrackNumber	                    (int           v       ){trackNumber_	                      = v;}
    void         setNumberOfTelescopeHits           (int           v       ){numberOfTelescopeHits_	              = v;}
    void         setNumberOfTelescopeClustersSizeLE2(int           v       ){numberOfTelescopeClustersSizeLE2_	  = v;}
    void         setNdof 	                        (int           v       ){ndof_	                              = v;}
    void         setChi2 	                        (float         v       ){chi2_	                              = v;}
    void         setXIntercept	                    (double        v       ){xIntercept_	                      = v;}
    void         setXSigmaIntercept                 (double        v       ){xSigmaIntercept_                     = v;}
    void         setYIntercept	                    (double        v       ){yIntercept_	                      = v;}
    void         setYSigmaIntercept                 (double        v       ){ySigmaIntercept_                     = v;}
    void         setXSlope	                        (double        v       ){xSlope_	                          = v;}
    void         setXSigmaSlope                     (double        v       ){xSigmaSlope_                         = v;}
    void         setYSlope	                        (double        v       ){ySlope_                              = v;}
    void         setYSigmaSlope                     (double        v       ){ySigmaSlope_                         = v;}
    void         setHasHit	                        (bool          v, int p){hasHit_  	                      [p] = v;}
    void         setBelongsToTrack                  (bool          v, int p){belongsToTrack_                  [p] = v;}
    void         setIsInDetector                    (bool          v, int p){isInDetector_                    [p] = v;}
    void         setIsInDetectorUnconstrained       (bool          v, int p){isInDetectorUnconstrained_       [p] = v;}
    void         setNdofUnconstrained               (int           v, int p){ndofUnconstrained_	              [p] = v;}
    void         setChi2Unconstrained               (float         v, int p){chi2Unconstrained_	              [p] = v;}
    void         setXInterceptUnconstrained         (double        v, int p){xInterceptUnconstrained_	      [p] = v;}
    void         setXSigmaInterceptUnconstrained    (double        v, int p){xSigmaInterceptUnconstrained_    [p] = v;}
    void         setYInterceptUnconstrained         (double        v, int p){yInterceptUnconstrained_	      [p] = v;}
    void         setYSigmaInterceptUnconstrained    (double        v, int p){ySigmaInterceptUnconstrained_    [p] = v;}
    void         setXSlopeUnconstrained             (double        v, int p){xSlopeUnconstrained_	          [p] = v;}
    void         setXSigmaSlopeUnconstrained        (double        v, int p){xSigmaSlopeUnconstrained_        [p] = v;}
    void         setYSlopeUnconstrained             (double        v, int p){ySlopeUnconstrained_             [p] = v;}
    void         setYSigmaSlopeUnconstrained        (double        v, int p){ySigmaSlopeUnconstrained_        [p] = v;}
    void         setClusterSize 	                (int           v, int p){clusterSize_    	              [p] = v;}
    void         setNumberOfCols 	                (unsigned int  v, int p){numberOfCols_    	              [p] = v;}
    void         setNumberOfRows 	                (unsigned int  v, int p){numberOfRows_    	              [p] = v;}
    void         setClusterCharge	                (int           v, int p){clusterCharge_  	              [p] = v;}
    void         setMeanCol		                    (float         v, int p){meanCol_     	                  [p] = v;}
    void         setMeanRow		                    (float         v, int p){meanRow_     	                  [p] = v;}
    void         setColPredicted                    (int           v, int p){colPredicted_                    [p] = v;}
    void         setRowPredicted                    (int           v, int p){rowPredicted_                    [p] = v;}
    void         setXPitchLocal                     (float         v, int p){xPitchLocal_                     [p] = v;}
    void         setYPitchLocal                     (float         v, int p){yPitchLocal_                     [p] = v;}
    void         setXPitchGlobal                    (float         v, int p){xPitchGlobal_                    [p] = v;}
    void         setYPitchGlobal                    (float         v, int p){yPitchGlobal_                    [p] = v;}
    void         setXMeasuredLocal	                (float         v, int p){xMeasuredLocal_                  [p] = v;}
    void         setYMeasuredLocal	                (float         v, int p){yMeasuredLocal_                  [p] = v;}
    void         setXMeasuredGlobal	                (float         v, int p){xMeasuredGlobal_                 [p] = v;}
    void         setYMeasuredGlobal    	            (float         v, int p){yMeasuredGlobal_                 [p] = v;}
    void         setXPredictedLocal                 (float         v, int p){xPredictedLocal_                 [p] = v;}
    void         setYPredictedLocal                 (float         v, int p){yPredictedLocal_                 [p] = v;}
    void         setXPredictedGlobal                (float         v, int p){xPredictedGlobal_                [p] = v;}
    void         setYPredictedGlobal                (float         v, int p){yPredictedGlobal_                [p] = v;}
    void         setXErrorMeasuredLocal	            (float         v, int p){xErrorMeasuredLocal_             [p] = v;}
    void         setYErrorMeasuredLocal	            (float         v, int p){yErrorMeasuredLocal_             [p] = v;}
    void         setXErrorMeasuredGlobal            (float         v, int p){xErrorMeasuredGlobal_            [p] = v;}
    void         setYErrorMeasuredGlobal            (float         v, int p){yErrorMeasuredGlobal_            [p] = v;}
    void         setXErrorPredictedLocal            (float         v, int p){xErrorPredictedLocal_            [p] = v;}
    void         setYErrorPredictedLocal            (float         v, int p){yErrorPredictedLocal_            [p] = v;}
    void         setXErrorPredictedGlobal           (float         v, int p){xErrorPredictedGlobal_           [p] = v;}
    void         setYErrorPredictedGlobal           (float         v, int p){yErrorPredictedGlobal_           [p] = v;}
    void         setXTrackResidualLocal	            (float         v, int p){xTrackResidualLocal_             [p] = v;}
    void         setYTrackResidualLocal	            (float         v, int p){yTrackResidualLocal_             [p] = v;}
    void         setXTrackResidualGlobal            (float         v, int p){xTrackResidualGlobal_            [p] = v;}
    void         setYTrackResidualGlobal            (float         v, int p){yTrackResidualGlobal_            [p] = v;}
    void         setXPixelResidualLocal             (float         v, int p){xPixelResidualLocal_             [p] = v;}
    void         setYPixelResidualLocal             (float         v, int p){yPixelResidualLocal_             [p] = v;}
    void         setXPixelResidualGlobal            (float         v, int p){xPixelResidualGlobal_            [p] = v;}
    void         setYPixelResidualGlobal            (float         v, int p){yPixelResidualGlobal_            [p] = v;}
    void         setXPixelResidualLocalUnconstrained(float         v, int p){xPixelResidualLocalUnconstrained_[p] = v;}
    void         setYPixelResidualLocalUnconstrained(float         v, int p){yPixelResidualLocalUnconstrained_[p] = v;}
    void         setXPixelPitchLocalUnconstrained   (float         v, int p){xPixelPitchLocalUnconstrained_   [p] = v;}
    void         setYPixelPitchLocalUnconstrained   (float         v, int p){yPixelPitchLocalUnconstrained_   [p] = v;}
    void         setClusterPixelRow                 (int   v,  int h, int p){clusterPixelRow_              [h][p] = v;}
    void         setClusterPixelCol                 (int   v,  int h, int p){clusterPixelCol_              [h][p] = v;}
    void         setClusterPixelCharge              (int   v,  int h, int p){clusterPixelCharge_           [h][p] = v;}
    void         setXClusterPixelCenterLocal        (float v,  int h, int p){xClusterPixelCenterLocal_     [h][p] = v;}
    void         setYClusterPixelCenterLocal        (float v,  int h, int p){yClusterPixelCenterLocal_     [h][p] = v;}
    void         setXClusterPixelCenterGlobal       (float v,  int h, int p){xClusterPixelCenterGlobal_    [h][p] = v;}
    void         setYClusterPixelCenterGlobal       (float v,  int h, int p){yClusterPixelCenterGlobal_    [h][p] = v;}
    void         setIsPixelCalibrated               (bool  v,  int h, int p){isPixelCalibrated_            [h][p] = v;}

    int          getEventNumber   	                (void            )const {return eventNumber_                        ;}
    int          getRunNumber   	                (void            )const {return runNumber_                          ;}
    int          getNumberOfTracks  	            (void            )const {return numberOfTracks_                     ;}
    int          getTrackNumber  	                (void            )const {return trackNumber_                        ;}
    int          getNumberOfTelescopeHits           (void            )const {return numberOfTelescopeHits_	            ;}
    int          getNumberOfTelescopeClustersSizeLE2(void            )const {return numberOfTelescopeClustersSizeLE2_   ;}
    int          getNdof    	                    (void            )const {return ndof_	                            ;}
    float        getChi2    	                    (void            )const {return chi2_	                            ;}
    double       getXIntercept  	                (void            )const {return xIntercept_                         ;}
    double       getXSigmaIntercept                 (void            )const {return xSigmaIntercept_                    ;}
    double       getYIntercept  	                (void            )const {return yIntercept_                         ;}
    double       getYSigmaIntercept                 (void            )const {return ySigmaIntercept_                    ;}
    double       getXSlope  	                    (void            )const {return xSlope_                             ;}
    double       getXSigmaSlope                     (void            )const {return xSigmaSlope_                        ;}
    double       getYSlope  	                    (void            )const {return ySlope_                             ;}
    double       getYSigmaSlope                     (void            )const {return ySigmaSlope_                        ;}
    bool         getHasHit  	                    (int  p          )const {return hasHit_                          [p];}
    bool         getBelongsToTrack                  (int  p          )const {return belongsToTrack_                  [p];}
    bool         getIsInDetector                    (int  p          )const {return isInDetector_                    [p];}
    bool         getIsInDetectorUnconstrained       (int  p          )const {return isInDetectorUnconstrained_       [p];}
    double       getNdofUnconstrained               (int  p          )const {return ndofUnconstrained_	             [p];}
    double       getChi2Unconstrained               (int  p          )const {return chi2Unconstrained_	             [p];}
    double       getXInterceptUnconstrained         (int  p          )const {return xInterceptUnconstrained_	     [p];}
    double       getXSigmaInterceptUnconstrained    (int  p          )const {return xSigmaInterceptUnconstrained_    [p];}
    double       getYInterceptUnconstrained         (int  p          )const {return yInterceptUnconstrained_	     [p];}
    double       getYSigmaInterceptUnconstrained    (int  p          )const {return ySigmaInterceptUnconstrained_    [p];}
    double       getXSlopeUnconstrained             (int  p          )const {return xSlopeUnconstrained_	         [p];}
    double       getXSigmaSlopeUnconstrained        (int  p          )const {return xSigmaSlopeUnconstrained_        [p];}
    double       getYSlopeUnconstrained             (int  p          )const {return ySlopeUnconstrained_             [p];}
    double       getYSigmaSlopeUnconstrained        (int  p          )const {return ySigmaSlopeUnconstrained_        [p];}
    int	         getClusterSize    	                (int  p          )const {return clusterSize_                     [p];}
    unsigned int getNumberOfCols    	            (int  p          )const {return numberOfCols_                    [p];}
    unsigned int getNumberOfRows    	            (int  p          )const {return numberOfRows_                    [p];}
    int          getClusterCharge  	                (int  p          )const {return clusterCharge_                   [p];}
    float        getMeanCol     	                (int  p          )const {return meanCol_                         [p];}
    float        getMeanRow     	                (int  p          )const {return meanRow_                         [p];}
    int          getColPredicted                    (int  p          )const {return colPredicted_                    [p];}
    int          getRowPredicted                    (int  p          )const {return rowPredicted_                    [p];}
    float        getXPitchLocal                     (int  p          )const {return xPitchLocal_                     [p];}
    float        getYPitchLocal                     (int  p          )const {return yPitchLocal_                     [p];}
    float        getXPitchGlobal                    (int  p          )const {return xPitchGlobal_                    [p];}
    float        getYPitchGlobal                    (int  p          )const {return yPitchGlobal_                    [p];}
    float        getXMeasuredLocal	                (int  p          )const {return xMeasuredLocal_                  [p];}
    float        getYMeasuredLocal	                (int  p          )const {return yMeasuredLocal_                  [p];}
    float        getXMeasuredGlobal	                (int  p          )const {return xMeasuredGlobal_                 [p];}
    float        getYMeasuredGlobal	                (int  p          )const {return yMeasuredGlobal_                 [p];}
    float        getXPredictedLocal                 (int  p          )const {return xPredictedLocal_                 [p];}
    float        getYPredictedLocal                 (int  p          )const {return yPredictedLocal_                 [p];}
    float        getXPredictedGlobal                (int  p          )const {return xPredictedGlobal_                [p];}
    float        getYPredictedGlobal                (int  p          )const {return yPredictedGlobal_                [p];}
    float        getXErrorMeasuredLocal             (int  p          )const {return xErrorMeasuredLocal_             [p];}
    float        getYErrorMeasuredLocal             (int  p          )const {return yErrorMeasuredLocal_             [p];}
    float        getXErrorMeasuredGlobal            (int  p          )const {return xErrorMeasuredGlobal_            [p];}
    float        getYErrorMeasuredGlobal            (int  p          )const {return yErrorMeasuredGlobal_            [p];}
    float        getXErrorPredictedLocal            (int  p          )const {return xErrorPredictedLocal_            [p];}
    float        getYErrorPredictedLocal            (int  p          )const {return yErrorPredictedLocal_            [p];}
    float        getXErrorPredictedGlobal           (int  p          )const {return xErrorPredictedGlobal_           [p];}
    float        getYErrorPredictedGlobal           (int  p          )const {return yErrorPredictedGlobal_           [p];}
    float        getXTrackResidualLocal             (int  p          )const {return xTrackResidualLocal_             [p];}
    float        getYTrackResidualLocal             (int  p          )const {return yTrackResidualLocal_             [p];}
    float        getXTrackResidualGlobal            (int  p          )const {return xTrackResidualGlobal_            [p];}
    float        getYTrackResidualGlobal            (int  p          )const {return yTrackResidualGlobal_            [p];}
    float        getXPixelResidualLocal             (int  p          )const {return xPixelResidualLocal_             [p];}
    float        getYPixelResidualLocal             (int  p          )const {return yPixelResidualLocal_             [p];}
    float        getXPixelResidualGlobal            (int  p          )const {return xPixelResidualGlobal_            [p];}
    float        getYPixelResidualGlobal            (int  p          )const {return yPixelResidualGlobal_            [p];}
    float        getXPixelResidualLocalUnconstrained(int  p          )const {return xPixelResidualLocalUnconstrained_[p];}
    float        getYPixelResidualLocalUnconstrained(int  p          )const {return yPixelResidualLocalUnconstrained_[p];}
    float        getXPixelPitchLocalUnconstrained   (int  p          )const {return xPixelPitchLocalUnconstrained_   [p];}
    float        getYPixelPitchLocalUnconstrained   (int  p          )const {return yPixelPitchLocalUnconstrained_   [p];}
    int          getClusterPixelRow                 (int  h, int p   )const {if (h < maxHits) return clusterPixelRow_          [h][p]; else {STDLINE("There's a problem with the cluster size: max = 4, required = " + h, ACRed); assert(0);}}
    int          getClusterPixelCol                 (int  h, int p   )const {if (h < maxHits) return clusterPixelCol_          [h][p]; else {STDLINE("There's a problem with the cluster size: max = 4, required = " + h, ACRed); assert(0);}}
    int          getClusterPixelCharge              (int  h, int p   )const {if (h < maxHits) return clusterPixelCharge_       [h][p]; else {STDLINE("There's a problem with the cluster size: max = 4, required = " + h, ACRed); assert(0);}}
    float        getXClusterPixelCenterLocal        (int  h, int p   )const {if (h < maxHits) return xClusterPixelCenterLocal_ [h][p]; else {STDLINE("There's a problem with the cluster size: max = 4, required = " + h, ACRed); assert(0);}}
    float        getYClusterPixelCenterLocal        (int  h, int p   )const {if (h < maxHits) return yClusterPixelCenterLocal_ [h][p]; else {STDLINE("There's a problem with the cluster size: max = 4, required = " + h, ACRed); assert(0);}}
    float        getXClusterPixelCenterGlobal       (int  h, int p   )const {if (h < maxHits) return xClusterPixelCenterGlobal_[h][p]; else {STDLINE("There's a problem with the cluster size: max = 4, required = " + h, ACRed); assert(0);}}
    float        getYClusterPixelCenterGlobal       (int  h, int p   )const {if (h < maxHits) return yClusterPixelCenterGlobal_[h][p]; else {STDLINE("There's a problem with the cluster size: max = 4, required = " + h, ACRed); assert(0);}}
    bool         getIsPixelCalibrated               (int  h, int p   )const {if (h < maxHits) return isPixelCalibrated_        [h][p]; else {STDLINE("There's a problem with the cluster size: max = 4, required = " + h, ACRed); assert(0);}}

private:
    enum {size = 10, maxHits = 4};
    int          eventNumber_                             ;//Event number
    int          runNumber_                               ;//Run number
    int          numberOfTracks_                          ;//Number of tracks in the event
    int          trackNumber_                             ;//Track number in the event
    int          numberOfTelescopeHits_                   ;//Number of hits on the telescope
    int          numberOfTelescopeClustersSizeLE2_        ;//Number of clusters on the telescope with size less or equal 2
    int          ndof_                                    ;//Track degrees of fredom
    float        chi2_                                    ;//Track chisqr
    double       xIntercept_                              ;//Track projection x at z=0
    double       xSigmaIntercept_                         ;//Error on x Intercept
    double       yIntercept_                              ;//Track projection y at z=0
    double       ySigmaIntercept_                         ;//Error on y Intercept
    double       xSlope_                                  ;//Track x Slope
    double       xSigmaSlope_                             ;//Error on x Slope
    double       ySlope_                                  ;//Track y Slope
    double       ySigmaSlope_                             ;//Error on y Slope
    bool         hasHit_                            [size];//Detector has a hit (cluster)
    bool         belongsToTrack_                    [size];//Hit belongs to track
    bool         isInDetector_                      [size];//Predicted impact point is inside detector area
    bool         isInDetectorUnconstrained_         [size];//Predicted impact point of unconstrained track is inside detector area
    int          ndofUnconstrained_                 [size];//Track degrees of fredom without plane hit
    float        chi2Unconstrained_                 [size];//Track chisqr without plane hit
    double       xInterceptUnconstrained_           [size];//Track projection x at z=0 without plane hit
    double       xSigmaInterceptUnconstrained_      [size];//Error on x Intercept without plane hit
    double       yInterceptUnconstrained_           [size];//Track projection y at z=0 without plane hit
    double       ySigmaInterceptUnconstrained_      [size];//Error on y Intercept without plane hit
    double       xSlopeUnconstrained_               [size];//Track x Slope without plane hit
    double       xSigmaSlopeUnconstrained_          [size];//Error on x Slope without plane hit
    double       ySlopeUnconstrained_               [size];//Track y Slope without plane hit
    double       ySigmaSlopeUnconstrained_          [size];//Error on y Slope without plane hit
    int          clusterSize_                       [size];//Number of pixels in cluster
    unsigned int numberOfCols_                      [size];//Number of columns of the cluster
    unsigned int numberOfRows_                      [size];//Number of rows of the cluster
    int          clusterCharge_                     [size];//Cluster charge
    float        meanCol_	                        [size];//Mean value of cluster columns
    float        meanRow_	                        [size];//Mean value of cluster rows
    int          colPredicted_                      [size];//Predicted impact column of the track
    int          rowPredicted_                      [size];//Predicted impact row of the track
    float        xPitchLocal_                       [size];//X pitch in local reference frame (150um)
    float        yPitchLocal_                       [size];//Y pitch in local reference frame (100um)
    float        xPitchGlobal_                      [size];//X pitch in global reference frame (depends by the z rotation)
    float        yPitchGlobal_                      [size];//Y pitch in global reference frame (depends by the z rotation)
    float        xMeasuredLocal_	                [size];//X measured point in local reference frame
    float        yMeasuredLocal_	                [size];//Y measured point in local reference frame
    float        xMeasuredGlobal_	                [size];//X measured point in global reference frame
    float        yMeasuredGlobal_	                [size];//Y measured point in global reference frame
    float        xPredictedLocal_                   [size];//X predicted impact point in local reference frame
    float        yPredictedLocal_                   [size];//Y predicted impact point in local reference frame
    float        xPredictedGlobal_                  [size];//X predicted impact point in global reference frame
    float        yPredictedGlobal_                  [size];//Y predicted impact point in global reference frame
    float        xErrorMeasuredLocal_               [size];//X error on measured point in local reference frame
    float        yErrorMeasuredLocal_               [size];//Y error on measured point in local reference frame
    float        xErrorMeasuredGlobal_              [size];//X error on measured point in global reference frame
    float        yErrorMeasuredGlobal_              [size];//Y error on measured point in global reference frame
    float        xErrorPredictedLocal_              [size];//X error on predicted impact point in local reference frame
    float        yErrorPredictedLocal_              [size];//Y error on predicted impact point in local reference frame
    float        xErrorPredictedGlobal_             [size];//X error on predicted impact point in global reference frame
    float        yErrorPredictedGlobal_             [size];//Y error on predicted impact point in global reference frame
    float        xTrackResidualLocal_               [size];//X residual in local reference frame
    float        yTrackResidualLocal_               [size];//Y residual in local reference frame
    float        xTrackResidualGlobal_              [size];//X residual in global reference frame
    float        yTrackResidualGlobal_              [size];//Y residual in global reference frame
    float        xPixelResidualLocal_               [size];//X residual in the predicted pixel in local reference frame
    float        yPixelResidualLocal_               [size];//Y residual in the predicted pixel in local reference frame
    float        xPixelResidualGlobal_              [size];//X residual in the predicted pixel in global reference frame
    float        yPixelResidualGlobal_              [size];//Y residual in the predicted pixel in global reference frame
    float        xPixelResidualLocalUnconstrained_  [size];//X unconstrained residual in the predicted pixel in local reference frame
    float        yPixelResidualLocalUnconstrained_  [size];//Y unconstrained residual in the predicted pixel in local reference frame
    float        xPixelPitchLocalUnconstrained_     [size];//X pitch of the predicted pixel in local reference frame
    float        yPixelPitchLocalUnconstrained_     [size];//Y pitch of the predicted pixel in local reference frame
    int          clusterPixelRow_          [maxHits][size];//Rows of the pixels in the cluster
    int          clusterPixelCol_          [maxHits][size];//Cols of the pixels in the cluster
    int          clusterPixelCharge_       [maxHits][size];//Charge of the pixels in the cluster
    float        xClusterPixelCenterLocal_ [maxHits][size];//X center of the pixels in the cluster in local reference frame
    float        yClusterPixelCenterLocal_ [maxHits][size];//Y center of the pixels in the cluster in local reference frame
    float        xClusterPixelCenterGlobal_[maxHits][size];//X center of the pixels in the cluster in global reference frame
    float        yClusterPixelCenterGlobal_[maxHits][size];//Y center of the pixels in the cluster in global reference frame
    bool         isPixelCalibrated_        [maxHits][size];//Pixel is calibrated
};

#endif

