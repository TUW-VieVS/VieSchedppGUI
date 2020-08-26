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
 * File:   SatelliteForGUI.h
 * Author: hwolf
 *
 * Created on February 7, 2019
 */


#ifndef VIESCHEDPP_SATELLITEFORGUI_H
#define VIESCHEDPP_SATELLITEFORGUI_H
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "../VieSchedpp/SGP4/CoordTopocentric.h"
#include "../VieSchedpp/SGP4/Observer.h"
#include "../VieSchedpp/SGP4/SGP4.h"
#include "../VieSchedpp/SGP4/Tle.h"
#include "../VieSchedpp/Scan/PointingVector.h"
#include "../VieSchedpp/Station/Network.h"
#include "../VieSchedpp/Station/Station.h"


class SatelliteForGUI : public VieVS::VieVS_NamedObject {
   public:
    /**
     * @brief constructor
     * @author Helene Wolf
     */
    SatelliteForGUI();

    /**
     * @brief constructor
     * @author Helene Wolf
     *
     * @param hdr header line of TLE data
     * @param l1 first line of TLE data
     * @param l2 second line of TLE data
     */
    SatelliteForGUI( std::string hdr, std::string l1, std::string l2 );

    /**
     * @brief destructor
     * @author Helene Wolf
     */
    ~SatelliteForGUI();

    /**
     * @brief getter for header of TLE data
     * @author Helene Wolf
     *
     * @return header line of TLE data
     */
    const std::string getHeader() const noexcept;

    /**
     * @brief getter for first line of TLE data
     * @author Helene Wolf
     *
     * @return first line of TLE data
     */
    const std::string getLine1() const noexcept;

    /**
     * @brief getter for second line of TLE data
     * @author Helene Wolf
     *
     * @return second line of TLE data
     */
    const std::string getLine2() const noexcept;

    /**
     * @brief getter SGP4 data
     * @author Helene Wolf
     *
     * @return SGP4 data of satellite
     */
    SGP4 *getSGP4Data();

    /**
     * @brief getter SGP4 data
     * @author Helene Wolf
     *
     * @return SGP4 data of satellite
     */
    Tle* getTleData() {return this->pTleData_; };

    /**
     * @brief satellite pass for a station
     * @author Helene Wolf
     */
    struct SatPass {
        DateTime start;             ///< start time of satellite pass
        DateTime end;               ///< end time of satellite pass
        unsigned long stationID;    ///< id of observing station
        unsigned long satelliteID;  ///< id of observed satellite
    };

    /**
     * @brief generates the list of satellite passes for one station
     * @author Helene Wolf
     *
     * @param network network which is observing
     * @param start_time start time of session
     * @param end_time end time of session
     * @param time_step time step
     *
     * @return list of satellite passes for the stations - [Station][SatellitePasses]
     */
    std::vector<std::vector<SatPass>> generatePassList( const VieVS::Network &network, const DateTime &start_time,
                                                        const DateTime &end_time, const int time_step ) const;

    /**
     * @brief find exact time when the satellite crosses the horizon of station
     * @author Helene Wolf
     *
     * @param station station for which the pass list is created
     * @param sgp4 sgp4 data of satellite
     * @param initial_time1 first time point of interval to find crossing point
     * @param initial_time2 second time point of interval to find crossing point
     * @param finding_aos boolean if signal is found
     *
     * @return Date and time of the satellite crossing horizon
     */
    DateTime findCrossingPoint( const VieVS::Station &station, const SGP4 &sgp4, const DateTime &initial_time1,
                                const DateTime &initial_time2, bool finding_aos ) const;

    /**
     * @brief reads the satellite file
     * @author Helene Wolf
     *
     * @param filename_ name of file
     *
     * @return list of satellites - [satellite]
     */
    static std::vector<SatelliteForGUI> readSatelliteFile( std::string filename );

    /**
     * @brief calculates the Position of the satellite
     * @author Helene Wolf
     *
     * @param time at which the position should be calulated
     *
     * @return Position of satellite at the specific time in geodetic coordinates (lat, lon, alt)
     */
    CoordGeodetic getPosition(DateTime time);

    /**
     * @brief checks the satellitePass concerning slew rates and sun distance
     * @author Helene Wolf
     *
     * @param satPass satllite pass which will be checked
     * @param sessionStartTime time when the session is starting
     * @param sgp4 satellite SGP4 data
     * @param station for which this satellite pass is
     * @param azelSun vector with azimuth and elevation to the sun in 30min intervals
     *
     *
     * @return vector with satellite checked passes
     */
    std::vector<SatelliteForGUI::SatPass> checkSatPass( struct SatPass satPass, DateTime sessionStartTime, SGP4 &sgp4, VieVS::Station station, std::vector<std::vector<double>> azelSun) const;

    /**
     * @brief calculates the distance between satellite and sun
     * @author Helene Wolf
     *
     * @param current_time time for which the distance whould be calculated
     * @param station station for which the distance between sun and satellite is calculated
     * @param azelSun vector with azimuth and elevation from this station to the sun in a 30min interval
     * @param sessionStartTime time of session start
     *
     * @return distance between sun and satellite at current_time
     */
    double getSunDistance(DateTime current_time, VieVS::Station station, std::vector<std::vector<double>> azelSun, DateTime sessionStartTime) const;

    /**
     * @brief calculates the azimuth and elevation from a station to the sun for the whole session in 30min interval
     * @author Helene Wolf
     *
     * @param station station for which the distance between sun and satellite is calculated
     * @param sessionStartTime time of session start
     * @param sessionEndTime time of session end
     *
     * @return azelSun vector with azimuth and elevation from this station to the sun in a 30min interval
     */
    std::vector<std::vector<double>> static azelSun(VieVS::Station station, DateTime sessionStartTime, DateTime sessionEndTime );

    /**
     * @brief interpolates the ra and dec of sun over the whole session time in a 30 min interval
     * @author Helene Wolf
     *
     * @return tuple of vectors with time(seconds since session start), ra, dec of sun
     */
    std::tuple<std::vector<unsigned int>, std::vector<double>, std::vector<double>> static interpolateRaDecSun();

    /**
     * @brief calculation of right ascension, declination and local hour angle of satellite at specific time
     * This function calculates the ra and dec and ha for the satellite to the given current time and sets the variables
     * in the pointing vector
     * @author Helene Wolf
     *
     * @param current_time time at which ra, dec and lha are calculated
     * @param start_time start time of session
     * @param eci earth center inertial system coordinates of satellite at current time
     * @param obs station position as object observer
     * @param pv pointing vector which is filled
     */
    static void calcRaDeHa( DateTime current_time, DateTime start_time, Eci eci, Observer obs, VieVS::PointingVector *pv );

    //void GetRates(const Observer &obs, const Eci &eci , DateTime current_time, DateTime start_time);
    //bool static checkSlewRates(Station station, Eci sat, DateTime current_time );
    //std::tuple<double, double, double,double> static getAzElRates(Station station, Eci sat, DateTime current_time);
    //std::tuple<double, double, double, double, double, double> static getRaDecHaRates(Station station, Eci sat, DateTime current_time);
    //std::tuple<double, double, double, double> static getXYRates(Station station, Eci sat, DateTime current_time);

private:
    static unsigned long nextId;  ///< next id for this object type
    std::string header_;           ///< header line of TLE Data
    std::string line1_;            ///< first line of TLE Data
    std::string line2_;            ///< second line of TLE Data
    Tle *pTleData_;                ///< pointer to TLE Data
    SGP4 *pSGP4Data_;              ///< pointer to SGP4 Data
};
#endif  // VIESCHEDPP_SATELLITEFORGUI_H
