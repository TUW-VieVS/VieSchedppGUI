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
 * File:   satelliteMain.h
 * Author: hwolf
 *
 * Created on February 5, 2019
 */

#ifndef VIESCHEDPP_SATELLITEMAIN_H
#define VIESCHEDPP_SATELLITEMAIN_H

#include <algorithm>
#include "../VieSchedpp/Scan/Scan.h"
#include "../VieSchedpp/Station/Baseline.h"
#include "../VieSchedpp/Station/Network.h"
#include "../VieSchedpp/Initializer.h"
#include "SatelliteForGUI.h"
#include "SatelliteObs.h"
#include "SatelliteOutput.h"
#include "../VieSchedpp/SGP4/CoordGeodetic.h"
#include "../VieSchedpp/SGP4/CoordTopocentric.h"
#include "../VieSchedpp/SGP4/Eci.h"
#include "../VieSchedpp/SGP4/Observer.h"
#include "../VieSchedpp/SGP4/SGP4.h"
#include "../VieSchedpp/SGP4/Tle.h"
// #include <stdfix.h>


class SatelliteMain {
   public:

    SatelliteMain() = default;

    explicit SatelliteMain( const VieVS::Network &network );

    void run();

    void initialize( const std::string &pathAntenna, const std::string &pathEquip,
        const std::string &pathPosition, const std::string &pathMask,
        boost::posix_time::ptime startTime, boost::posix_time::ptime endTime,
        const std::vector<std::string> &stations);

    std::vector<SatelliteForGUI> readSatelliteFile( const std::string &pathToTLE ) const;

    std::vector<VieVS::Scan> generateScanList( const std::vector<SatelliteForGUI> &satellites ) const;

    VieVS::Network &refNetwork() { return network_;    }

    VieVS::Scan createAdjustedScan(SatelliteForGUI sat,std::vector<unsigned long> selectedStationIds, std::vector<unsigned long> startTimes, std::vector<unsigned long> endTimes);

   private:
    VieVS::Network network_;
    DateTime startDate_;
    DateTime endDate_;
};

#endif  // VIESCHEDPP_SATELLITEMAIN_H
