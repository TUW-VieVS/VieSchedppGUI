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
 * File:   satelliteOutput.h
 * Author: hwolf
 *
 * Created on August 1, 2019
 */



#ifndef VIESCHEDPP_SATELLITEOUTPUT_H
#define VIESCHEDPP_SATELLITEOUTPUT_H
#include "../VieSchedpp/Misc/VieVS_NamedObject.h"
#include "../VieSchedpp/Scan/Scan.h"
#include "SatelliteMain.h"
#include "SatelliteObs.h"

/**
 * @class Satellite Output
 * @brief this is the Satellite Output class which creates all kind of output
 *
 * @author Helene Wolf
 * @date 01.08.2019
 */
class SatelliteOutput : public VieVS::VieVS_NamedObject {
   public:
    /**
     * @brief constructor
     * @author Helene Wolf
     *
     * @param network_ station network
     * @param sat satellite
     * @param start start time of session
     * @param end end time of session
     */
    SatelliteOutput( VieVS::Network &network_, SatelliteForGUI sat, DateTime start, DateTime end );

    /**
     * @brief destructor
     * @author Helene Wolf
     */
    ~SatelliteOutput();
    /**
     * @brief adds the scanlist to the output object
     * @param scanList list of scans
     * @author Helene Wolf
     */
    void addScans( std::vector<VieVS::Scan> scanList );

    /**
     * @brief print the satellite passes
     * @author Helene Wolf
     */
    void printPassList( std::vector<std::vector<SatelliteForGUI::SatPass>> passList );

    /**
     * @brief print the scans
     * @author Helene Wolf
     */
    void printScan();

    /**
     * @brief print participating stations
     * @author Helene Wolf
     */
    void printStations();

    /**
     * @brief print start and end time of session
     * @author Helene Wolf
     */
    void printStart();

    /**
     * @brief print the overlaps
     * @author Helene Wolf
     */
    void printOverlaps( std::vector<SatelliteObs> overlaps );

    /**
     * @brief print pointing vectors
     * @author Helene Wolf
     */
    void printPV( std::vector<std::vector<std::vector<VieVS::PointingVector>>> pvRes );

   private:
    static unsigned long nextId;  ///< next id for this object type
    VieVS::Network network_;             ///< network
    SatelliteForGUI satellite_;          ///< all sources
    std::vector<VieVS::Scan> scans_;     ///< all scans in schedule
    DateTime startDate_;          ///< session start date
    DateTime endDate_;            ///< session end date
};
#endif  // VIESCHEDPP_SATELLITEOUTPUT_H
