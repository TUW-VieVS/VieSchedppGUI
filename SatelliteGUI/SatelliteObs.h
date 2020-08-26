/*
 *  VieSched++ Very Long Baseline Interferometry (VLBI) Scheduling Software
 *  Copyright (C) 2018  Matthias Schartner
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * File:   satelliteObs.h
 * Author: hwolf
 *
 * Created on February 13, 2019
 */



#ifndef VIESCHEDPP_SATELLITEOBS_H
#define VIESCHEDPP_SATELLITEOBS_H

#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <functional>  // std::minus
#include <iostream>
#include <numeric>
#include <vector>
#include "../VieSchedpp/Misc/TimeSystem.h"
#include "../VieSchedpp/Scan/PointingVector.h"
#include "../VieSchedpp/Scan/Scan.h"
#include "../VieSchedpp/Station/Network.h"
#include "./SatelliteForGUI.h"
#include "../VieSchedpp/SGP4/CoordGeodetic.h"

class SatelliteObs : public VieVS::VieVS_NamedObject {
   public:
    /**
     * @brief constructor
     * @author Helene Wolf
     */
    SatelliteObs();

    /**
     * @brief constructor
     * @author Helene Wolf
     *
     * @param startTime start time of satellite observation
     * @param endTime end time of satellite observation
     */
    SatelliteObs( DateTime startTime, DateTime endTime );

    /**
     * @brief destructor
     * @author Helene Wolf
     */
    ~SatelliteObs();

    /**
     * @brief Time Point of satellite observation start or end
     * @author Helene Wolf
     */
    struct TimePoint {
        DateTime time;              ///< time of observation start or end
        VieVS::Timestamp ts;               ///< type of time point (start, end)
        unsigned long stationID;    ///< id of observing station
        unsigned long satelliteID;  ///< id of observed satellite
    };

    /**
     * @brief getter for start time
     * @author Helene Wolf
     *
     * @return start time of stallite observation
     */
    const DateTime getStart() const noexcept;

    /**
     * @brief getter for end time
     * @author Helene Wolf
     *
     * @return end time of stallite observation
     */
    const DateTime getEnd() const noexcept;

    /**
     * @brief getter for List of IDs of observing Stations
     * @author Helene Wolf
     *
     * @return List of Station IDs
     */
    const std::vector<unsigned long> getStationIDList() const noexcept;

    /**
     * @brief getter for number of observing stations
     * @author Helene Wolf
     *
     * @return number of observing stations
     */
    const unsigned long getNumberofStations() const noexcept;

    /**
     * @brief converts the pass list to overlaps
     * @author Helene Wolf
     *
     * @param PassList 2D vector containing the possible observing times for the stations
     * @return vector with overlapping the SatellitePasses -> [SatellitePasses] (with list of Stations)
     */
    static std::vector<SatelliteObs> passList2Overlap( std::vector<std::vector<SatelliteForGUI::SatPass>> passList );

    /**
     * @brief compares timepoints
     * @author Helene Wolf
     *
     * @param i1 time of TimePoint2
     * @param i2 time of TimePoint1
     * @return boolean i1 < i2
     */
    static bool compareTimePoint( TimePoint i1, TimePoint i2 );

    /**
     * @brief removes Station ID
     * @author Helene Wolf
     *
     * @param StationIDs list of stations
     * @param reStation station to be removed
     */
    static void removeStationID( std::vector<unsigned long> &stationIDs, unsigned long reStation );


    /**
     * @brief calculation of observations
     * @author Helene Wolf
     *
     * @param scan scan for which observations should be created
     * @param network station network
     * @param sat satellite
     * @param PV_start vector of pointing vectors for each observing station at start time
     */
    static void calcObservations( VieVS::Scan *scan, VieVS::Network &network, SatelliteForGUI sat, std::vector<VieVS::PointingVector> PV_start );

    /**
     * @brief create Pointing Vectors
     * @author Helene Wolf
     *
     * @param PassList_
     * @param sat
     * @param network
     * @param SessionStartTime
     *
     * @return list of pointing Vectors for each Station -> [Station][SatellitePasses][PointingVector]
     */
    static std::vector<std::vector<std::vector<VieVS::PointingVector>>> passList2pv(
        std::vector<std::vector<SatelliteForGUI::SatPass>> passList, SatelliteForGUI sat, VieVS::Network network,
        DateTime sessionStartTime );
    /**
     * @brief create scan list, convert PassList to scans
     * @author Helene Wolf
     *
     * @param PassList list of satellite passes
     * @param network station network
     * @param sat satellite
     * @param sessionStartTime start time of session
     *
     * @return List of scans ->  [Scan]
     */
    std::vector<VieVS::Scan> static createScanList( std::vector<std::vector<SatelliteForGUI::SatPass>> passList, VieVS::Network network,
                                             SatelliteForGUI sat, DateTime sessionStartTime );

    /**
     * @brief create pointing vector
     * @author Helene Wolf
     *
     * @param station station
     * @param sat satellite
     * @param StartTime session start time
     * @param TimePoint start or end time of station
     *
     * @return pointing Vector
     */
    VieVS::PointingVector static createPV( VieVS::Station station, SatelliteForGUI sat, DateTime sessionStartTime,
                                           TimePoint timePoint );

    /**
     * @brief converts the PassList to a sorted list of Timepoints
     * @author Helene Wolf
     *
     * @param PassList list of stallite observations
     *
     * @return vector of sorted timepoints ->  [TimePoint]
     */
    static std::vector<SatelliteObs::TimePoint> createSortedTimePoints(
        std::vector<std::vector<SatelliteForGUI::SatPass>> &passList );


    /**
     * @brief compares two pointing Vectors by time
     * @author Helene Wolf
     *
     * @param pv1 Pointing Vector
     * @param pv2 PointingVector
     *
     * @return bool
     */
    static bool comparePV( VieVS::PointingVector pv1, VieVS::PointingVector pv2 );


    /**
     * @brief creates Output of observation windows and the time
     * @author Helene Wolf
     *
     * @param pv1 PassList
     * @param overlap
     * @param network
     *
     */
    void static createOutput( std::vector<std::vector<SatelliteForGUI::SatPass>> passList, std::vector<SatelliteObs> overlap,
                              VieVS::Network network );


    void static totextfile( std::vector<std::vector<std::vector<VieVS::PointingVector>>> pvRes);

   private:
    static unsigned long nextId;               ///< next id for this object type
    DateTime start_;                            ///< start time of satellite observation
    DateTime end_;                              ///< end time of satellite observation
    std::vector<unsigned long> stationIDList_;  ///< list of ids of observing stations
    unsigned long satelliteID_;                 //< id of observed satellite
};

#endif  // VIESCHEDPP_SATELLITEOBS_H
