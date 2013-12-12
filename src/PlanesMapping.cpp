#include "PlanesMapping.h"
#include "MessageTools.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////////
PlanesMapping::PlanesMapping()
{
    planeToPosition_["Telescope_Downstream0"] = 0;
    planeToPosition_["Telescope_Downstream1"] = 1;
    planeToPosition_["Telescope_Downstream2"] = 2;
    planeToPosition_["Telescope_Downstream3"] = 3;
    planeToPosition_["Telescope_Upstream0"]   = 4;
    planeToPosition_["Telescope_Upstream1"]   = 5;
    planeToPosition_["Telescope_Upstream2"]   = 6;
    planeToPosition_["Telescope_Upstream3"]   = 7;
    planeToPosition_["Dut1"]                  = 8;
    planeToPosition_["Dut0"]                  = 9;

    positionToPlane_[0] = "Telescope_Downstream0";
    positionToPlane_[1] = "Telescope_Downstream1";
    positionToPlane_[2] = "Telescope_Downstream2";
    positionToPlane_[3] = "Telescope_Downstream3";
    positionToPlane_[4] = "Telescope_Upstream0"  ;
    positionToPlane_[5] = "Telescope_Upstream1"  ;
    positionToPlane_[6] = "Telescope_Upstream2"  ;
    positionToPlane_[7] = "Telescope_Upstream3"  ;
    positionToPlane_[8] = "Dut1"                 ;
    positionToPlane_[9] = "Dut0"                 ;

    positionToMonicelliPlane_[0] = "Station: 0 - Plaq: 0";
    positionToMonicelliPlane_[1] = "Station: 0 - Plaq: 1";
    positionToMonicelliPlane_[2] = "Station: 0 - Plaq: 2";
    positionToMonicelliPlane_[3] = "Station: 0 - Plaq: 3";
    positionToMonicelliPlane_[4] = "Station: 2 - Plaq: 0";
    positionToMonicelliPlane_[5] = "Station: 2 - Plaq: 1";
    positionToMonicelliPlane_[6] = "Station: 2 - Plaq: 2";
    positionToMonicelliPlane_[7] = "Station: 2 - Plaq: 3";
    positionToMonicelliPlane_[8] = "Station: 4 - Plaq: 1";
    positionToMonicelliPlane_[9] = "Station: 4 - Plaq: 0";
}

//////////////////////////////////////////////////////////////////////////
PlanesMapping::~PlanesMapping()
{
}

//////////////////////////////////////////////////////////////////////////
std::string PlanesMapping::getMonicelliPlaneName(int position)
{
    if(positionToMonicelliPlane_.find(position) != positionToMonicelliPlane_.end())
        return positionToMonicelliPlane_[position];
    else
    {
        std::stringstream ss;
        ss << "ERROR: Plane number " << position << " doesn't exist!";
        STDLINE(ss.str(), ACRed);
    }
    return "";
}

//////////////////////////////////////////////////////////////////////////
std::string PlanesMapping::getPlaneName(int position)
{
    if(positionToPlane_.find(position) != positionToPlane_.end())
        return positionToPlane_[position];
    else
    {
        std::stringstream ss;
        ss << "ERROR: Plane number " << position << " doesn't exist!";
        STDLINE(ss.str(), ACRed);
    }
    return "";
}

//////////////////////////////////////////////////////////////////////////
int PlanesMapping::getStation(int position)
{
    if(position>=0 && position<=3)
        return 0;
    else if(position>=4 && position<=7)
        return 2;
    else if(position>=8 && position<=9)
        return 4;
    else
    {
        std::stringstream ss;
        ss << "ERROR: Plane number " << position << " doesn't exist!";
        STDLINE(ss.str(), ACRed);
        return -1;
    }
}

//////////////////////////////////////////////////////////////////////////
int PlanesMapping::getPlaquette(int position)
{
    if(position<8)
        return position%4;
    else if(position>=8)
        return 1-(position%4);
    else
    {
        std::stringstream ss;
        ss << "ERROR: Plane number " << position << " doesn't exist!";
        STDLINE(ss.str(), ACRed);
        return -1;
    }
}

//////////////////////////////////////////////////////////////////////////
int PlanesMapping::getPlanePosition(std::string plane)
{
    if(planeToPosition_.find(plane) != planeToPosition_.end())
        return planeToPosition_[plane];
    else
        STDLINE("ERROR: Plane name " + plane + " doesn't exist!", ACRed);
    return -1;
}
