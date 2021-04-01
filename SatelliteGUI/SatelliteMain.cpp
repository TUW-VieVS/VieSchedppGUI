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
 * File:   satelliteMain.cpp
 * Author: hwolf
 *
 * Created on February 5, 2019
 */

#include "SatelliteMain.h"


using namespace std;

SatelliteMain::SatelliteMain( const VieVS::Network &network ) { network_ = network; }

void SatelliteMain::run() {
    VieVS::ScanTimes::setAlignmentAnchor( VieVS::ScanTimes::AlignmentAnchor::individual );

    // read in satellites
    vector<SatelliteForGUI> satellites = SatelliteForGUI::readSatelliteFile( "../../CATALOGS/satellites.txt" );
    std::cout << "Choose Satellite to observe  (press number in brackets): " << std::endl;
    for ( int k = 0; k < satellites.size(); k++ ) {
        std::cout << "[" << k << "]: " << satellites[k].getHeader() << std::endl;
    }
    int sat;  //=7;
    cin >> sat;


    // generate start Time and End Time
    DateTime start_date( VieVS::TimeSystem::startTime.date().year(), VieVS::TimeSystem::startTime.date().month(),
                         VieVS::TimeSystem::startTime.date().day(), VieVS::TimeSystem::startTime.time_of_day().hours(),
                         VieVS::TimeSystem::startTime.time_of_day().minutes(), VieVS::TimeSystem::startTime.time_of_day().seconds() );
    DateTime end_date( start_date.AddSeconds( VieVS::TimeSystem::duration ) );

    // create Satellite Output
    SatelliteOutput satOut = SatelliteOutput( network_, satellites[sat], start_date, end_date );
    satOut.printStart();
    satOut.printStations();

    CoordGeodetic TEST =  satellites[sat].getPosition(start_date);

    // calculate the passes for each Station
    std::vector<std::vector<SatelliteForGUI::SatPass>> passList =
        satellites[sat].generatePassList( network_, start_date, end_date, 60 );
    satOut.printPassList( passList );

    // PassList to Overlaps
    std::vector<SatelliteObs> overlaps = SatelliteObs::passList2Overlap( passList );
    satOut.printOverlaps( overlaps );

    // Overlaps to PointingVector
    std::vector<std::vector<std::vector<VieVS::PointingVector>>> pvRes =
        SatelliteObs::passList2pv( passList, satellites[sat], this->network_, start_date );
    satOut.printPV( pvRes );

    SatelliteObs::totextfile(pvRes);
     //SatelliteObs::createOutput(passList, overlaps, network_);

    // create Scan List
    vector<VieVS::Scan> scanList =
        SatelliteObs::createScanList( passList, network_, satellites[sat], start_date );

    //for(const auto &scan : scanList){
    //    auto tmp = scan.toPropertyTree();
    //}


    satOut.addScans( scanList );
    satOut.printScan();
}

void SatelliteMain::initialize( const std::string &pathAntenna, const std::string &pathEquip,
                 const std::string &pathPosition, const std::string &pathMask,
                 boost::posix_time::ptime startTime, boost::posix_time::ptime endTime,
                 const std::vector<std::string> &stations){

    // initialize times
    int sec_ = startTime.time_of_day().total_seconds();
    double mjdStart = startTime.date().modjulian_day() + sec_ / 86400.0;

    int sec = VieVS::util::duration( startTime, endTime );
    auto duration = static_cast<unsigned int>( sec );

    VieVS::TimeSystem::mjdStart = mjdStart;
    VieVS::TimeSystem::startTime = startTime;
    VieVS::TimeSystem::endTime = endTime;
    VieVS::TimeSystem::duration = duration;

    startDate_ = DateTime( VieVS::TimeSystem::startTime.date().year(), VieVS::TimeSystem::startTime.date().month(),
                         VieVS::TimeSystem::startTime.date().day(), VieVS::TimeSystem::startTime.time_of_day().hours(),
                         VieVS::TimeSystem::startTime.time_of_day().minutes(), VieVS::TimeSystem::startTime.time_of_day().seconds() );
    endDate_ = DateTime( startDate_.AddSeconds( VieVS::TimeSystem::duration ) );


    // initialize Network
    VieVS::SkdCatalogReader skdCatalogs;
    skdCatalogs.setStationNames(stations);
    skdCatalogs.setCatalogFilePathes(pathAntenna, pathEquip, "", "", "", "", pathMask, "", pathPosition, "", "", "", "");
    skdCatalogs.initializeStationCatalogs();

    string outPath = "satelliteSchedulingLog.txt";
    ofstream of( outPath );

    VieVS::LookupTable::initialize();
    VieVS::Initializer init;

    std::unordered_map<std::string, unsigned int> band2channel{{"X", 10}, {"S", 6}};
    std::unordered_map<std::string, double> band2wavelength{{"L", 0.3},      {"S", 0.131},   {"C", 0.06},
                                                            {"X", 0.0349},   {"Ku", 0.0231}, {"K", 0.0134},
                                                            {"Ka", 0.01000}, {"E", 0.005},   {"W", 0.00375}};
    init.initializeObservingMode( stations.size(), 32, 2, band2channel, band2wavelength );

    init.initializeAstronomicalParameteres();
    init.createStations( skdCatalogs, of );

    network_ = init.getNetwork();

    // set scan alignment anchor
    VieVS::ScanTimes::setAlignmentAnchor( VieVS::ScanTimes::AlignmentAnchor::individual );

}


vector<SatelliteForGUI> SatelliteMain::readSatelliteFile( const std::string &pathToTLE ) const{
    return  SatelliteForGUI::readSatelliteFile( pathToTLE );
}

vector<VieVS::Scan> SatelliteMain::generateScanList ( const vector<SatelliteForGUI> &satellites ) const{

    //[satellite][station][pass]
    std::vector<std::vector<std::vector<SatelliteForGUI::SatPass>>> passLists;
    for(const auto & thisSat : satellites){
        passLists.push_back(thisSat.generatePassList(network_, startDate_, endDate_, 60));
    }


    vector<VieVS::Scan> list;
    for( size_t i=0; i< satellites.size(); ++i){
        auto scanList = SatelliteObs::createScanList( passLists[i], network_, satellites[i], startDate_ );
        list.insert(list.end(), scanList.begin(), scanList.end());
    }

    return list;
}

VieVS::Scan SatelliteMain::createAdjustedScan(SatelliteForGUI sat,std::vector<unsigned long> selectedStationIds, std::vector<unsigned long> startTimes, std::vector<unsigned long> endTimes)
{
    std::vector<VieVS::PointingVector> pointingVectorsStart;
    std::vector<VieVS::PointingVector> pointingVectorsEnd;
    unsigned long nsta = selectedStationIds.size();
    vector<unsigned int> endOfObservingTime( nsta, 0 );
    vector<unsigned int> endOfLastScans( nsta, 0 );
    vector<unsigned int> fieldSystemTimes( nsta, 0 );
    vector<unsigned int> slewTimes( nsta, 0 );
    vector<unsigned int> preobTimes( nsta, 0 );
    vector<unsigned int> pvStartTimes( nsta, 0 );

    for(size_t i=0;i<nsta; i++)
    {
        VieVS::Station station = network_.getStation(selectedStationIds.at(i));
        SatelliteObs::TimePoint tstart;
        tstart.ts = VieVS::Timestamp ::start;
        tstart.time = startDate_.AddSeconds(startTimes.at(i));
        tstart.satelliteID = sat.getId();
        tstart.stationID = station.getId();

        SatelliteObs::TimePoint tend;
        tend.ts = VieVS::Timestamp ::end;
        tend.time = startDate_.AddSeconds(endTimes.at(i));
        tend.satelliteID = sat.getId();
        tend.stationID = station.getId();

        VieVS::PointingVector pvStart = SatelliteObs::createPV(station, sat, startDate_, tstart);
        VieVS::PointingVector pvEnd = SatelliteObs::createPV(station, sat, startDate_, tend);
        pointingVectorsStart.push_back(pvStart);
        pointingVectorsEnd.push_back(pvEnd);


        endOfObservingTime.at( i ) = pointingVectorsEnd.at( i ).getTime();
        fieldSystemTimes.at(i) = network_.getStation(i).getPARA().systemDelay;
        preobTimes.at(i) = network_.getStation(i).getPARA().preob;
        pvStartTimes.at(i) = pointingVectorsStart.at(i).getTime();

        if ( pvStartTimes.at(i) < ( fieldSystemTimes.at(i) + preobTimes.at(i) ) ) {
            endOfLastScans.at(i) = pvStartTimes.at(i);
            fieldSystemTimes.at(i) = 0;
            preobTimes.at(i) = 0;
        } else {
            endOfLastScans.at(i) = pointingVectorsStart.at(i).getTime() -
                                     fieldSystemTimes.at(i) - preobTimes.at(i);
        }

    }

    std::vector<VieVS::PointingVector> pvstartcopy( pointingVectorsStart );
    VieVS::Scan scan( pointingVectorsStart, endOfLastScans, VieVS::Scan::ScanType::standard );
    scan.setPointingVectorsEndtime(pointingVectorsEnd);
    scan.setScanTimes( endOfLastScans, fieldSystemTimes, slewTimes, preobTimes,
                       pvStartTimes, endOfObservingTime);
    SatelliteObs::calcObservations( &scan, network_, sat, pvstartcopy );
    return scan;
}
