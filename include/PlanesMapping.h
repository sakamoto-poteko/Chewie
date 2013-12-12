#ifndef PLANESMAPPING_H
#define PLANESMAPPING_H

#include <map>
#include <string>

class PlanesMapping
{
public:
    PlanesMapping  (void);
    ~PlanesMapping (void);

    std::string   getPlaneName          (int         position);
    std::string   getMonicelliPlaneName (int         position);
    int           getStation            (int         position);
    int           getPlaquette          (int         position);
    int           getPlanePosition      (std::string plane   );
    unsigned int  getNumberOfPlanes     (void                ) {return planeToPosition_.size();}

private:
    std::map<std::string, int> planeToPosition_         ;
    std::map<int, std::string> positionToPlane_         ;
    std::map<int, std::string> positionToMonicelliPlane_;
};

#endif // PLANESMAPPING_H
