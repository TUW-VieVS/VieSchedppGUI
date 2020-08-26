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
 * File:   satelliteObs.cpp
 * Author: hwolf
 *
 * Created on February 13, 2019
 */

#include "SatelliteObs.h"

using namespace std;
unsigned long SatelliteObs::nextId = 0;

SatelliteObs::SatelliteObs() : start_( 0 ), end_( 0 ), VieVS_NamedObject::VieVS_NamedObject( "SatObs", "", nextId++ ) {}

SatelliteObs::SatelliteObs( DateTime startTime, DateTime endTime )
    : start_( startTime ), end_( endTime ), VieVS_NamedObject::VieVS_NamedObject( "SatObs", "", nextId++ ) {}

SatelliteObs::~SatelliteObs() {}

const unsigned long SatelliteObs::getNumberofStations() const noexcept { return this->stationIDList_.size(); }

const std::vector<unsigned long> SatelliteObs::getStationIDList() const noexcept { return this->stationIDList_; }

const DateTime SatelliteObs::getStart() const noexcept { return this->start_; }

const DateTime SatelliteObs::getEnd() const noexcept { return this->end_; }

std::vector<SatelliteObs> SatelliteObs::passList2Overlap( std::vector<std::vector<SatelliteForGUI::SatPass>> passList ) {
#ifdef VIESCHEDPP_LOG
    BOOST_LOG_TRIVIAL( info ) << "start creating Overlaps";
#else
    cout << "[info] start creating Overlaps";
#endif

    std::vector<TimePoint> timePoints;
    timePoints = createSortedTimePoints( passList );

    std::vector<SatelliteObs> overlap_list;
    SatelliteObs overlap = SatelliteObs();
    bool signal = false;

    // find Overlaps (compare time -> check if timepoints are overlapping each other and check timestamp (if it is a
    // start or end of observation)
    // if it is a start the station will be added to the list, if it is an end the station will be removed from the list

    for ( const auto &any : timePoints ) {
        if ( !signal ) {
            overlap.start_ = any.time;
            overlap.stationIDList_.push_back( any.stationID );
            overlap.satelliteID_ = any.satelliteID;
            signal = true;
            continue;
        } else  // if there is already signal (at least one station is already observing)
        {
            switch ( any.ts ) {
                case VieVS::Timestamp::start:  // another station starts to observe
                {
                    if ( overlap.start_ == any.time )  // the satellite start to be visible for 2 stations at the same
                    {
                        overlap.stationIDList_.push_back( any.stationID );
                        continue;
                    } else  // another stations starts to observe -> end last overlap and start a new overlap (set
                            // start, add station to  IDList)
                    {
                        overlap.end_ = any.time;
                        overlap_list.push_back( overlap );
                        overlap.start_ = any.time;
                        overlap.stationIDList_.push_back( any.stationID );
                        continue;
                    }
                }
                case VieVS::Timestamp::end:  // a station stops to observe
                {
                    if ( overlap.end_ == any.time )  // two stations stop to observe at the same time
                    {
                        removeStationID( overlap.stationIDList_, any.stationID );
                        continue;
                    } else {
                        overlap.end_ = any.time;
                        overlap_list.push_back( overlap );
                        removeStationID( overlap.stationIDList_, any.stationID );
                        if ( overlap.stationIDList_.empty() ) {
                            signal = false;  // if any station is no longer observing
                        } else {
                            overlap.start_ = any.time;
                        }
                        continue;
                    }
                }
            }
        }
    }
#ifdef VIESCHEDPP_LOG
    BOOST_LOG_TRIVIAL( info ) << "finish creating Overlaps";
#else
    cout << "[info] finish creating Overlaps";
#endif
    return overlap_list;
}


bool SatelliteObs::compareTimePoint( TimePoint i1, TimePoint i2 ) { return ( i1.time < i2.time ); }

void SatelliteObs::removeStationID( std::vector<unsigned long> &stationIDs, unsigned long reStation ) {
    auto position = std::find( stationIDs.begin(), stationIDs.end(), reStation );
    if ( position != stationIDs.end() )  // == StationIDs.end() means the element was not found
        stationIDs.erase( position );
}

//[Station][SatellitePasses][PointingVector]
std::vector<std::vector<std::vector<VieVS::PointingVector>>> SatelliteObs::passList2pv(
    std::vector<std::vector<SatelliteForGUI::SatPass>> passList, SatelliteForGUI sat, VieVS::Network network,
    DateTime sessionStartTime ) {
#ifdef VIESCHEDPP_LOG
    BOOST_LOG_TRIVIAL( info ) << "start creating PointingVectors";
#else
    cout << "[info] start creating PointingVectors";
#endif
    std::vector<VieVS::PointingVector> pvVec;
    std::vector<std::vector<VieVS::PointingVector>> pvList;
    std::vector<std::vector<std::vector<VieVS::PointingVector>>> pvRes;
    std::vector<VieVS::Station> stations = network.getStations();
    SGP4 *sgp4 = sat.getSGP4Data();
    for ( unsigned long i = 0; i < passList.size(); i++ )  // i Station
    {
        pvList.clear();
        for ( int j = 0; j < passList.at( i ).size(); j++ ) {
            pvVec.clear();
            VieVS::PointingVector pv = VieVS::PointingVector( stations.at( i ).getId(), sat.getId() );
            Observer obs( stations.at( i ).getPosition()->getLat() * 180 / pi,
                          stations.at( i ).getPosition()->getLon() * 180 / pi,
                          stations.at( i ).getPosition()->getAltitude() / 1000 );

            DateTime start_time = ( ceil( ( passList.at( i ).at( j ).start.Ticks() ) / pow( 10, 6 ) ) * pow( 10, 6 ) );
            DateTime end_time = ( floor( ( passList.at( i ).at( j ).end.Ticks() ) / pow( 10, 6 ) ) * pow( 10, 6 ) );
            DateTime current_time( start_time );

            while ( current_time < end_time + TimeSpan( 0, 1, 0 ) ) {
                if ( current_time > end_time ) {
                    current_time = end_time;
                }
                Eci eci = sgp4->FindPosition( current_time );
                CoordTopocentric topo = obs.GetLookAngle( eci );
                pv.setAz( topo.azimuth );
                pv.setEl( topo.elevation );

                SatelliteForGUI::calcRaDeHa( current_time, sessionStartTime, eci, obs, &pv );

                pvVec.push_back( pv );
                current_time = current_time + TimeSpan( 0, 10, 0 );
            }
            pvList.push_back( pvVec );
        }
        pvRes.push_back( pvList );
    }
#ifdef VIESCHEDPP_LOG
    BOOST_LOG_TRIVIAL( info ) << "finish creating PointingVectors";
#else
    cout << "[info] finish creating PointingVectors";
#endif
    return pvRes;
}


void SatelliteObs::createOutput( std::vector<std::vector<SatelliteForGUI::SatPass>> passList,
                                 std::vector<SatelliteObs> overlap, VieVS::Network network ) {
    vector<VieVS::Station> all_stations = network.getStations();
    unsigned long nSta = all_stations.size();
    vector<vector<char>> stationBoolList(
        overlap.size(),
        vector<char>( all_stations.size(),
                      false ) );  // create 2D boolean vector with size of overlaps and number of stations
    vector<long> stationCount( overlap.size(), 0 );

    vector<TimePoint> timePoints;
    timePoints = createSortedTimePoints( passList );

    for ( unsigned long i = 0; i < overlap.size(); i++ )  // add value true at position of id of station
    {
        for ( const unsigned long &j : overlap.at( i ).stationIDList_ ) {
            stationBoolList.at( i ).at( j ) = true;
        }
        stationCount[i] = std::count( stationBoolList.at( i ).begin(), stationBoolList.at( i ).end(), true );
    }

    DateTime last;
    for ( unsigned long i = 0; i < timePoints.size(); i++ ) {
        if ( i == 0 ) {
            std::cout << boost::format( "  %2f:%2f:%2f " ) % timePoints.at( i ).time.Hour() %
                    timePoints.at( i ).time.Minute() % timePoints.at( i ).time.Second();
            last = timePoints.at( i ).time;
            continue;
        } else if ( timePoints.at( i ).time == last ) {
            last = timePoints.at( i ).time;
            continue;
        } else {
            std::cout << boost::format( "  %2f:%2f:%2f " ) % timePoints.at( i ).time.Hour() %
                    timePoints.at( i ).time.Minute() % timePoints.at( i ).time.Second();
            last = timePoints.at( i ).time;
            continue;
        }
    }
    std::cout << std::endl;
    for ( unsigned long j = 0; j < nSta; j++ ) {
        for ( const auto &stationBool : stationBoolList ) {
            if ( stationBool.at( j ) != 0 ) {
                std::cout << "     " << 1 << "     ";
            } else {
                std::cout << "     " << 0 << "     ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

void SatelliteObs::calcObservations( VieVS::Scan *scan, VieVS::Network &network, SatelliteForGUI sat, vector<VieVS::PointingVector> PV_start ) {
    vector<VieVS::Observation> obs = scan->getObservations();
    obs.clear();
    // bool valid = false;
    unsigned long srcid = sat.getId();

    // loop over all pointingVectors
    for ( unsigned long i = 0; i < PV_start.size(); ++i ) {
        for ( unsigned long j = i + 1; j < PV_start.size(); ++j ) {
            unsigned long staid1 = PV_start[i].getStaid();
            unsigned long staid2 = PV_start[j].getStaid();
            const VieVS::Baseline &bl = network.getBaseline( staid1, staid2 );
            unsigned long blid = bl.getId();

            // add new baseline
            unsigned int startTime = max( {scan->getTimes().getObservingTime( i, VieVS::Timestamp::start ),
                                           scan->getTimes().getObservingTime( j, VieVS::Timestamp::start )} );

            obs.emplace_back( blid, staid1, staid2, srcid, startTime );

            unsigned int observingTime = scan->referenceTime().getObservingDuration( i, j );
            obs[obs.size() - 1].setObservingTime( observingTime );
            // valid = true;
        }
    }
    scan->setObservations( obs );
}

VieVS::PointingVector SatelliteObs::createPV( VieVS::Station station, SatelliteForGUI sat, DateTime sessionStartTime,
                                              TimePoint timePoint ) {
    Observer obs( station.getPosition()->getLat() * 180 / pi, station.getPosition()->getLon() * 180 / pi,
                  station.getPosition()->getAltitude() / 1000 );
    VieVS::PointingVector pv( station.getId(), sat.getId() );
    DateTime time;
    switch ( timePoint.ts ) {
        case VieVS::Timestamp::start: {
            time = ( ceil( ( timePoint.time.Ticks() ) / pow( 10, 6 ) ) * pow( 10, 6 ) );
            break;
        }
        case VieVS::Timestamp::end: {
            time = ( floor( ( timePoint.time.Ticks() ) / pow( 10, 6 ) ) * pow( 10, 6 ) );
            break;
        }
    }
    Eci eci = sat.getSGP4Data()->FindPosition( time );
    CoordTopocentric topo = obs.GetLookAngle( eci );
    pv.setAz( topo.azimuth );
    pv.setEl( topo.elevation );

    SatelliteForGUI::calcRaDeHa( time, sessionStartTime, eci, obs, &pv );
    return pv;
}


/*
void SatelliteObs::calcTopoRaDec(Eci obs, Eci sat)
{
    Vector x = sat.Position() - obs.Position();
    Vector x_v = sat.Velocity() - obs.Velocity();
    double x_norm = x.Magnitude();

    double dec = asin(x.z/x_norm);
    double ra;
    if(sqrt(x.x * x.x + x.y +x.y) != 0)
    {
        ra = asin(x.y/sqrt(x.x * x.x + x.y * x.y));
        double check = acos(x.x/sqrt(x.x * x.x + x.y * x.y));
    }
    else
    {
        ra = asin(x_v.y/sqrt(x_v.x * x_v.x + x_v.y * x_v.y));
        double check = acos(x_v.x/sqrt(x_v.x * x_v.x + x_v.y * x_v.y));
    }
    double range_rate = x.Dot(x_v)/x_norm;
    double ra_rate = (x_v.x * x.y - x_v.y * x.x)/(-x.y *x.y - x.x * x.x);
    double dec_rate = (x_v.z - range_rate * sin(dec))/(sqrt(x.x * x.x + x.y +x.y));
}*/


bool SatelliteObs::comparePV( VieVS::PointingVector pv1, VieVS::PointingVector pv2 ) { return ( pv1.getStaid() < pv2.getStaid() ); }

std::vector<SatelliteObs::TimePoint> SatelliteObs::createSortedTimePoints(
    std::vector<std::vector<SatelliteForGUI::SatPass>> &passList ) {
    std::vector<TimePoint> timePoints;

    for ( const auto &pass : passList ) {
        for ( const auto &any : pass ) {
            struct TimePoint tStart;
            tStart.time = any.start;
            tStart.ts = VieVS::Timestamp::start;
            tStart.stationID = any.stationID;
            tStart.satelliteID = any.satelliteID;
            timePoints.push_back( tStart );

            struct TimePoint tEnd;
            tEnd.time = any.end;
            tEnd.ts = VieVS::Timestamp::end;
            tEnd.stationID = any.stationID;
            tEnd.satelliteID = any.satelliteID;
            timePoints.push_back( tEnd );
        }
    }
    // sort time points by time (start and end times)
    std::sort( timePoints.begin(), timePoints.end(), compareTimePoint );
    return timePoints;
}


/*This function creates the List of possible scans. it converts the passlist array into an array of the type timepoint.
 * Then it is checked if the observation windows of  the stations are overlapping.
 * there is an vector ignoreNow which has a size of number of the stations and includes for each stations how many
 * observations windows should be ignored by running through the timepoints. if a station stops observing and would
 * start again during a scan, a 1 will be written in the vector ignoreNext, in the end (if ignoreNext isn't empty)
 * ignoreNext and ignoreNow will be added, so the number of observation windows to be ignored will increase.*/

std::vector<VieVS::Scan> SatelliteObs::createScanList( std::vector<std::vector<SatelliteForGUI::SatPass>> passList, VieVS::Network network,
                                                SatelliteForGUI sat, DateTime sessionStartTime ) {
#ifdef VIESCHEDPP_LOG
    BOOST_LOG_TRIVIAL( info ) << "start creating Scanlist";
#else
    cout << "[info] start creating Scanlist";
#endif
    std::vector<VieVS::Scan> scanList;
    std::vector<TimePoint> timePoints;
    unsigned long nSta = network.getNSta();
    std::vector<VieVS::Station> stations = network.getStations();
    timePoints = createSortedTimePoints( passList );

    std::vector<bool> isobs( nSta, false );
    std::vector<int> ignoreNow( nSta, 0 );
    std::vector<int> ignoreNext( nSta, 0 );
    std::vector<int> isDone( nSta, 0 );
    std::vector<int> ignoreNowCOPY( nSta, 0 );
    vector<VieVS::PointingVector> pointingVectorsStart;
    vector<VieVS::PointingVector> pointingVectorsEnd;
    bool isScan = false;

    unsigned long i = 0;
    while ( i < timePoints.size() ) {
        if ( ignoreNow.at( timePoints.at( i ).stationID ) == 0 ) {
            if ( isDone.at( timePoints.at( i ).stationID ) == 0 ) {
                if ( !isScan ) {
                    switch ( timePoints.at( i ).ts ) {
                        case VieVS::Timestamp::start: {
                            auto itr = std::find( isobs.begin(), isobs.end(), true );
                            long idx = std::distance( isobs.begin(), itr );
                            isobs[timePoints.at( i ).stationID] = true;
                            if ( std::count( isobs.begin(), isobs.end(), true ) > 1 )  // start scan
                            {
                                VieVS::PointingVector pvStart( createPV( stations.at( timePoints.at( i ).stationID ), sat,
                                                                  sessionStartTime, timePoints.at( i ) ) );
                                VieVS::PointingVector pvStartPast(
                                    createPV( stations.at( idx ), sat, sessionStartTime, timePoints.at( i ) ) );
                                pointingVectorsStart.push_back( pvStart );
                                pointingVectorsStart.push_back( pvStartPast );
                                isScan = true;
                                i++;
                                continue;
                            } else {
                                i++;
                                continue;
                            }
                        }
                        case VieVS::Timestamp::end: {  // stations stops again to observe
                            isobs[timePoints.at( i ).stationID] = false;
                            ignoreNowCOPY[timePoints.at( i ).stationID] = ignoreNowCOPY[timePoints.at( i ).stationID]+1;
                            i++;
                            continue;
                        }
                    }
                } else {
                    switch ( timePoints.at( i ).ts ) {
                        case VieVS::Timestamp::start: {
                            isobs[timePoints.at( i ).stationID] = true;
                            VieVS::PointingVector pvStart( createPV( stations.at( timePoints.at( i ).stationID ), sat,
                                                              sessionStartTime, timePoints.at( i ) ) );
                            pointingVectorsStart.push_back( pvStart );
                            i++;
                            continue;
                        }
                        case VieVS::Timestamp::end: {
                            isobs[timePoints.at( i ).stationID] = false;
                            isDone.at( timePoints.at( i ).stationID ) = 1;
                            VieVS::PointingVector pvEnd( createPV( stations.at( timePoints.at( i ).stationID ), sat,
                                                            sessionStartTime, timePoints.at( i ) ) );
                            pointingVectorsEnd.push_back( pvEnd );
                            if ( std::count( isobs.begin(), isobs.end(), true ) < 2 )  // finish this scan
                            {
                                auto itr = std::find( isobs.begin(), isobs.end(), true );
                                long idx = std::distance( isobs.begin(), itr );
                                VieVS::PointingVector pvEnd(
                                    createPV( stations[idx], sat, sessionStartTime, timePoints.at( i ) ) );
                                pointingVectorsEnd.push_back( pvEnd );
                                std::sort( pointingVectorsStart.begin(), pointingVectorsStart.end(), comparePV );
                                std::sort( pointingVectorsEnd.begin(), pointingVectorsEnd.end(), comparePV );
                                unsigned long nsta = pointingVectorsStart.size();
                                vector<unsigned int> endOfObservingTime( nsta, 0 );
                                vector<unsigned int> endOfLastScans( nsta, 0 );
                                vector<unsigned int> fieldSystemTimes( nsta, 0 );
                                vector<unsigned int> slewTimes( nsta, 0 );
                                vector<unsigned int> preobTimes( nsta, 0 );
                                vector<unsigned int> pvStartTimes( nsta, 0 );
                                vector<VieVS::PointingVector> pvstartcopy( pointingVectorsStart );

                                for ( unsigned long k = 0; k < nsta; k++ ) {
                                    endOfObservingTime.at( k ) = pointingVectorsEnd.at( k ).getTime();
                                    pvStartTimes.at( k ) = pointingVectorsStart.at( k ).getTime();
                                    fieldSystemTimes.at( k ) = network.getStation( k ).getPARA().systemDelay;
                                    preobTimes.at( k ) = network.getStation( k ).getPARA().preob;
                                    if ( pvStartTimes.at( k ) < ( fieldSystemTimes.at( k ) + preobTimes.at( k ) ) ) {
                                        endOfLastScans.at( k ) = pvStartTimes.at( k );
                                        fieldSystemTimes.at( k ) = 0;
                                        preobTimes.at( k ) = 0;
                                    } else {
                                        endOfLastScans.at( k ) = pointingVectorsStart.at( k ).getTime() -
                                                                 fieldSystemTimes.at( k ) - preobTimes.at( k );
                                    }
                                }
                                VieVS::Scan scan( pointingVectorsStart, endOfLastScans, VieVS::Scan::ScanType::standard );
                                scan.setScanTimes( endOfLastScans, fieldSystemTimes, slewTimes, preobTimes,
                                                   pvStartTimes, endOfObservingTime);
                                scan.setPointingVectorsEndtime( pointingVectorsEnd );
                                calcObservations( &scan, network, sat, pvstartcopy );
                                scanList.push_back( scan );
                                isScan = false;
                                std::vector<int> addThis;
                                if ( std::count( ignoreNext.begin(), ignoreNext.end(), true ) == 0 ) {
                                    if ( i == timePoints.size() - 2 ) {
                                        break;
                                    } else {
                                        addThis = isDone;
                                    }
                                } else {
                                    addThis = ignoreNext;
                                }
                                for ( unsigned long j = 0; j < ignoreNow.size(); j++ ) {
                                    ignoreNow.at( j ) = ignoreNowCOPY.at( j ) + addThis.at( j );
                                }
                                i = 0;
                                isobs.assign( nSta, false );
                                isDone.assign( nSta, 0 );
                                ignoreNext.assign( nSta, 0 );
                                ignoreNowCOPY = ignoreNow;
                                pointingVectorsStart.clear();
                                pointingVectorsEnd.clear();
                                i++;
                                continue;
                            } else {
                                i++;
                                continue;
                            }
                        }
                    }
                }
            } else {
                ignoreNext.at( timePoints.at( i ).stationID ) =
                    1;  // one more observation window from this station will be ignored in the next run
                i++;
            }
        } else {
            switch ( timePoints.at( i ).ts ) {
                case VieVS::Timestamp::start: {
                    i++;
                    continue;
                }
                case VieVS::Timestamp::end: {
                    ignoreNow.at(
                            timePoints.at( i ).stationID )--;  // one observation window was ignored, so reduce number by 1
                    i++;
                }
            }
        }
    }
#ifdef VIESCHEDPP_LOG
    BOOST_LOG_TRIVIAL( info ) << "finish creating Scanlist";
#else
    cout << "[info] finish creating Scanlist";
#endif
    return scanList;
}


void SatelliteObs::totextfile( std::vector<std::vector<std::vector<VieVS::PointingVector>>> pvRes)
{
    ofstream skyPlot;
    skyPlot.open("../example.txt");
    for(const auto &station : pvRes) {
        for(const auto &passes : station ) {
            skyPlot << passes.at(0).getStaid() << std::endl;
            for(const auto &pv : passes) {
                skyPlot << pv.getTime() << ";" << pv.getAz() << ";" << pv.getEl() << std::endl;
            }
        }
    }
}
