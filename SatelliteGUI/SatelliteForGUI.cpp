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
 * File:   satellite.cpp
 * Author: hwolf
 *
 * Created on February 7, 2019
 */

#include "SatelliteForGUI.h"

using namespace std;
unsigned long SatelliteForGUI::nextId = 0;

SatelliteForGUI::SatelliteForGUI()
    : header_( "" ), line1_( "" ), line2_( "" ), VieVS_NamedObject::VieVS_NamedObject( "", "", nextId++ ) {
    pTleData_ = nullptr;
    pSGP4Data_ = nullptr;
}


SatelliteForGUI::SatelliteForGUI( std::string hdr, std::string l1, std::string l2 )
    : header_( hdr ), line1_( l1 ), line2_( l2 ), VieVS_NamedObject::VieVS_NamedObject( hdr, hdr, nextId++ ) {
    pTleData_ = new Tle( hdr, l1, l2 );
    pSGP4Data_ = new SGP4( *pTleData_ );
}


SatelliteForGUI::~SatelliteForGUI() {
    // delete pSGP4Data;
    // delete pTleData;
}

const std::string SatelliteForGUI::getHeader() const noexcept { return this->header_; }

const std::string SatelliteForGUI::getLine1() const noexcept { return this->line1_; }

const std::string SatelliteForGUI::getLine2() const noexcept { return this->line2_; }

SGP4* SatelliteForGUI::getSGP4Data() { return this->pSGP4Data_; }

/* This function calculates the ra and dec and ha for the satellite to the given current time and sets the variables in
 * the pointing vector
 * */
void SatelliteForGUI::calcRaDeHa( DateTime current_time, DateTime start_time, Eci eci, Observer obs, VieVS::PointingVector *pv ) {
    Eci stat( current_time, obs.GetLocation() );

    // difference vector between station and satellite
    Vector x_sat = eci.Position();
    Vector x_stat = stat.Position();
    Vector xd = x_sat - x_stat;
    Vector vd = eci.Velocity() - stat.Velocity();


    // calculation of right ascension and declination for satellite
    double r = sqrt( xd.x * xd.x + xd.y * xd.y + xd.z * xd.z );
    double dec = asin( xd.z / r );
    double ra;
    if(sqrt(xd.x*xd.x + xd.y*xd.y) < 0.00000001) {
        ra = atan2(vd.y,vd.x);
    }
    else {
        ra = atan2(xd.y,xd.x);
    }

    //if ( eci.Position().y / r > 0 ) {
    //    ra = acos( xd.x / r * 1 / cos( dec ) );
    //} else {
    //    ra = 2 * pi - acos( xd.x / r * 1 / cos( dec ) );
    //}

    pv->setTime(
        round( ( ( current_time.ToJ2000() - start_time.ToJ2000() ) * 86400 ) ) );  // seconds since session start

    double mjd = current_time.ToJulian() - 2400000.5;
    double gmst = VieVS::TimeSystem::mjd2gmst( mjd );
    double ha = gmst + obs.GetLocation().longitude - ra;
    while ( ha > pi ) {
        ha = ha - twopi;
    }
    while ( ha < -pi ) {
        ha = ha + twopi;
    }
    pv->setHa( ha );
    pv->setDc( dec );
}

double SatelliteForGUI::getSunDistance(DateTime current_time, VieVS::Station station, std::vector<std::vector<double>> AzElSun, DateTime sessionStartTime) const
{
    double seconds = (current_time - sessionStartTime).TotalSeconds();
    int i =0;
    for(i; i< AzElSun.size();i++) {
        std::vector<double> it = AzElSun.at(i);
        if(seconds == it.at(0))
        {
            break;
        }
        if(seconds<it.at(0)) {
            i = i-1;
            break;
        }
    }
    double factor = (seconds-AzElSun[i][0]) / (AzElSun[i+1][0] - AzElSun[i][0]);

    double az1 = AzElSun[i][1];
    double az2 = AzElSun[i+1][1];
    if(abs(az1-az2)>halfpi) {
               if(az2 > az1) {
                   az1 += twopi;
               }
               if(az1 > az2) {
                   az2 += twopi;
               }
           }
           double az_sun = az1 + factor * (az2-az1);
           if(az_sun > twopi) {
               az_sun -= twopi;
           }

    double el_sun = AzElSun[i][2] + factor * (AzElSun[i+1][2] - AzElSun[i][2]);

    SGP4* sgp4( this->pSGP4Data_ );
    Eci eci = sgp4->FindPosition( current_time );
    CoordGeodetic user_geo( station.getPosition()->getLat(), station.getPosition()->getLon(),
                            station.getPosition()->getAltitude() / 1000, true );
    Observer obs( user_geo );
    CoordTopocentric topo = obs.GetLookAngle( eci );
    double az_sat = topo.azimuth;
    double el_sat = topo.elevation;

    std::vector<double> v_src = {cos(el_sat)*cos(az_sat),cos(el_sat)*sin(az_sat),sin(el_sat)}; // Unity vector station-source
    std::vector<double> v_sun = {cos(el_sun)*cos(az_sun),cos(el_sun)*sin(az_sun),sin(el_sun)}; // Unity vector station-sun
    double sun_dist = acos(v_src[0]*v_sun[0] + v_src[1] * v_sun[1] + v_src[2] * v_sun[2]);
    //std::cout << seconds/60 << " " << sun_dist * 180/pi << " " << az_sun*180/pi << " " << el_sun*180/pi << " " << az_sat*180/pi << " " << el_sat*180/pi << std::endl;
    return sun_dist;
}

/*
 * This function checks the found satellite passes concerning the slew rates of the atennas and the distance between the sun and the to observing satelllite.
 * If the slew rates are exceeded or the satellite is too close too close at the sun the satellite pass will
 * be ended. The checking will be contued for the rest of the satellite scan. So it can happen that one satelite pass will be splitted in two passede because
 * the antenna slew rates are exceeded or the sun distance is too small.
 */
std::vector<SatelliteForGUI::SatPass> SatelliteForGUI::checkSatPass( struct SatPass satPass, DateTime sessionStartTime, SGP4 &sgp4_old, VieVS::Station station, std::vector<std::vector<double>> AzElSun) const
{
    //std::cout << station.getName() << std::endl;
    double minSunDistance = 4 * deg2rad;   ///< minimum sun distance in radians
    bool hasStarted = false;
    DateTime current_time = satPass.start;  
    
    CoordGeodetic user_geo( station.getPosition()->getLat(), station.getPosition()->getLon(),
                            station.getPosition()->getAltitude() / 1000, true );
    Observer obs( user_geo );

    //find Pointing Vector at the start time
    SGP4 *sgp4 = this->pSGP4Data_;
    Eci eci = sgp4->FindPosition( current_time );
    CoordTopocentric topo = obs.GetLookAngle( eci );
    VieVS::PointingVector pv_old = VieVS::PointingVector( 1, 0 );
    pv_old.setAz( topo.azimuth );
    pv_old.setEl( topo.elevation );
    SatelliteForGUI::calcRaDeHa( current_time, sessionStartTime, eci, obs, &pv_old );

    current_time = current_time + TimeSpan( 0, 0, 10 );
    
    struct SatPass satPasschecked;
    std::vector<struct SatPass> checkedSatPasses;

    while(current_time < (satPass.end+ TimeSpan( 0, 0, 10 )))
    {
       double sunDistance = this->getSunDistance(current_time - TimeSpan(0,0,10), station, AzElSun, sessionStartTime);
       //std::cout << current_time<< "  " << sunDistance << "  " << minSunDistance << std::endl;
       Eci eci = sgp4->FindPosition( current_time );
       CoordTopocentric topo = obs.GetLookAngle( eci );
       VieVS::PointingVector pv_new = VieVS::PointingVector( 1, 0 );
       pv_new.setAz( topo.azimuth );
       pv_new.setEl( topo.elevation );
       SatelliteForGUI::calcRaDeHa( current_time, sessionStartTime, eci, obs, &pv_new );

       station.getCableWrap().calcUnwrappedAz(pv_old,pv_new);
       unsigned int Slewtime = station.getAntenna().slewTimeTracking(pv_old, pv_new);
       if(Slewtime < 10 && !hasStarted && sunDistance > minSunDistance) //set the start
       {
            satPasschecked.start = current_time - TimeSpan( 0, 0, 10);
            satPasschecked.stationID = satPass.stationID;
            satPasschecked.satelliteID = satPass.satelliteID;
            hasStarted = true;
       }
       else if(Slewtime>10 && hasStarted ) //set the end and append
       {
           //std::cout << current_time <<" "<< station.getName() << std::endl;
           //std::cout << pv_old.getAz()*180/pi << " "<< pv_old.getEl()*180/pi << std::endl;
           //std::cout << pv_new.getAz()*180/pi << " "<< pv_new.getEl()*180/pi << std::endl;
           //unsigned int Slewtime = station.getAntenna().slewTime(pv_old, pv_new);

           DateTime t = current_time;
           current_time = current_time - TimeSpan(0,0,10);
           while(current_time<t) {
               current_time = current_time + TimeSpan(0,0,1);
               Eci eci = sgp4->FindPosition( current_time );
               CoordTopocentric topo = obs.GetLookAngle( eci );
               VieVS::PointingVector pv_new = VieVS::PointingVector( 1, 0 );
               pv_new.setAz( topo.azimuth );
               pv_new.setEl( topo.elevation );
               SatelliteForGUI::calcRaDeHa( t, sessionStartTime, eci, obs, &pv_new );

               station.getCableWrap().calcUnwrappedAz(pv_old,pv_new);
               unsigned int Slewtime = station.getAntenna().slewTimeTracking(pv_old, pv_new);
               if(Slewtime>1) {
                   //std::cout << current_time - TimeSpan(0,0,1) << " " << pv_old.getAz()*180/pi << " "<< pv_old.getEl()*180/pi << std::endl;
                   //std::cout << current_time << " " << pv_new.getAz()*180/pi << " "<< pv_new.getEl()*180/pi << std::endl;
                   break;
               }
               else {
                   pv_old = pv_new;
               }
           }
           satPasschecked.end = current_time - TimeSpan(0,0,1) ;
           checkedSatPasses.push_back(satPasschecked);
           hasStarted = false;
       }
       else if(sunDistance<minSunDistance && hasStarted){
           DateTime t = current_time - TimeSpan(0,0,10);
           current_time = current_time - TimeSpan(0,0,20);
           while(current_time<t) {
               current_time = current_time + TimeSpan(0,0,1);
               double sunDistance = this->getSunDistance(current_time, station, AzElSun, sessionStartTime);
               if(sunDistance<minSunDistance) {
                   //std::cout << current_time - TimeSpan(0,0,1) << " " << pv_old.getAz()*180/pi << " "<< pv_old.getEl()*180/pi << std::endl;
                   //std::cout << current_time << " " << pv_new.getAz()*180/pi << " "<< pv_new.getEl()*180/pi << std::endl;
                   //std::cout << "SUN DISTANCE " << station.getName() << " " <<  sunDistance << " " << minSunDistance << std::endl;
                   Eci eci = sgp4->FindPosition( current_time- TimeSpan(0,0,1) );
                   CoordTopocentric topo = obs.GetLookAngle( eci );
                   VieVS::PointingVector pv_new = VieVS::PointingVector( 1, 0 );
                   pv_new.setAz( topo.azimuth );
                   pv_new.setEl( topo.elevation );
                   SatelliteForGUI::calcRaDeHa( current_time- TimeSpan(0,0,1), sessionStartTime, eci, obs, &pv_new );
                   break;
               }
           }
           satPasschecked.end = current_time - TimeSpan(0,0,1) ;
           checkedSatPasses.push_back(satPasschecked);
           hasStarted = false;
       }
       pv_old = pv_new; 
       current_time = current_time + TimeSpan( 0, 0, 10 );
    }
    if(current_time > satPass.end && hasStarted)
    {
        satPasschecked.end = satPass.end;
        checkedSatPasses.push_back(satPasschecked);
    }
    return checkedSatPasses;
}

std::tuple<std::vector<unsigned int>, std::vector<double>, std::vector<double>> SatelliteForGUI::interpolateRaDecSun()
{
    vector<unsigned int> reftimeSun = VieVS::AstronomicalParameters::sun_time;
    /*for(int j = 0; j<reftimeSun.size();j++)
    {
        std::cout << AstronomicalParameters::sun_time[j] << " " << AstronomicalParameters::sun_ra[j]*180/pi << " " <<  AstronomicalParameters::sun_dec[j]*180/pi << std::endl;
    }
    std::cout << std::endl;*/
    unsigned int secs = 0;
    std::vector<unsigned int> timeSun;
    std::vector<double> raSun;
    std::vector<double> decSun;
    while(secs <= VieVS::TimeSystem::duration) {
       timeSun.push_back(secs);
       int i =0;
       for(i; i< reftimeSun.size();i++) {
           if(secs == reftimeSun.at(i)) {
               break;
           }
           if(secs< reftimeSun.at(i)) {
               i = i-1;
               break;
           }
       }
       double factor = (static_cast<double>(secs)-static_cast<double>(reftimeSun[i])) / static_cast<double>(reftimeSun[i+1] - reftimeSun[i]);
       double ra1 = VieVS::AstronomicalParameters::sun_ra[i];
       double ra2 = VieVS::AstronomicalParameters::sun_ra[i+1];

       if(abs(ra1-ra2)>halfpi) {
           if(ra2 > ra1) {
               ra1 += twopi;
           }
           if(ra1 > ra2) {
               ra2 += twopi;
           }
       }
       double ra = ra1 + factor * (ra2-ra1);
       if(ra > twopi) {
           ra -= twopi;
       }

       double dec  =  VieVS::AstronomicalParameters::sun_dec[i] + factor * (VieVS::AstronomicalParameters::sun_dec[i+1] - VieVS::AstronomicalParameters::sun_dec[i]);
       raSun.push_back(ra);
       decSun.push_back(dec);
       //std::cout << secs << " " << ra*180/pi << " " <<  dec*180/pi << std::endl;
       secs = secs + 1800;

    }
    std::tuple<std::vector<unsigned int>, std::vector<double>, std::vector<double>> RaDecSun(timeSun,raSun, decSun);
    return RaDecSun;
}


std::vector<std::vector<double>> SatelliteForGUI::azelSun(VieVS::Station station, DateTime startTime, DateTime endTime)
{
    std::vector<std::vector<double>> AzElSun;

    std::tuple<std::vector<unsigned int>, std::vector<double>, std::vector<double>> SunRaDec = SatelliteForGUI::interpolateRaDecSun();
    vector<unsigned int> reftimeSun = get<0>(SunRaDec);
    std::vector<double> sun_ra = get<1>(SunRaDec);
    std::vector<double> sun_dec = get<2>(SunRaDec);

    for(int i =0;i<reftimeSun.size();i++) {
        double omega = 7.2921151467069805e-05;  // 1.00273781191135448*D2PI/86400;
        //Time
        double date1 = 2400000.5;
        double mjd = VieVS::TimeSystem::mjdStart + static_cast<double>( reftimeSun[i] ) / 86400.0;
        // Earth Rotation
        double ERA = iauEra00( date1, mjd );

        // precession nutation
        double C[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

        unsigned int nut_precalc_idx = 0;
        while ( VieVS::AstronomicalParameters::earth_nutTime[nut_precalc_idx + 1] < reftimeSun[i] ) {
            ++nut_precalc_idx;
        }
        int delta = VieVS::AstronomicalParameters::earth_nutTime[1] - VieVS::AstronomicalParameters::earth_nutTime[0];

        unsigned int deltaTime = reftimeSun[i] - VieVS::AstronomicalParameters::earth_nutTime[nut_precalc_idx];

        double x = VieVS::AstronomicalParameters::earth_nutX[nut_precalc_idx] +
                   ( VieVS::AstronomicalParameters::earth_nutX[nut_precalc_idx + 1] -
                     VieVS::AstronomicalParameters::earth_nutX[nut_precalc_idx] ) /
                       delta * deltaTime;
        double y = VieVS::AstronomicalParameters::earth_nutY[nut_precalc_idx] +
                   ( VieVS::AstronomicalParameters::earth_nutY[nut_precalc_idx + 1] -
                     VieVS::AstronomicalParameters::earth_nutY[nut_precalc_idx] ) /
                       delta * deltaTime;
        double s = VieVS::AstronomicalParameters::earth_nutS[nut_precalc_idx] +
                   ( VieVS::AstronomicalParameters::earth_nutS[nut_precalc_idx + 1] -
                     VieVS::AstronomicalParameters::earth_nutS[nut_precalc_idx] ) /
                       delta * deltaTime;

        iauC2ixys( x, y, s, C );

        //  Polar Motion
        double W[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        //  GCRS to ITRS
        double c2t[3][3];
        iauC2tcio( C, ERA, W, c2t );
        double t2c[3][3] = {};
        iauTr( c2t, t2c );

        //  Transformation
        double v1[3] = {-omega * station.getPosition()->getX(), omega * station.getPosition()->getY(), 0};

        double v1R[3] = {};
        iauRxp( t2c, v1, v1R );

        double k1a[3] = {};
        double k1a_t1[3];

        k1a_t1[0] = ( VieVS::AstronomicalParameters::earth_velocity[0] + v1[0] ) / CMPS;
        k1a_t1[1] = ( VieVS::AstronomicalParameters::earth_velocity[1] + v1[1] ) / CMPS;
        k1a_t1[2] = ( VieVS::AstronomicalParameters::earth_velocity[2] + v1[2] ) / CMPS;

        // Source vector in CRF
        double ra = sun_ra.at(i);
        double dec = sun_dec.at(i);

        double cosDe = cos(dec);
        const vector<double> &scrs_ = {cosDe*cos(ra),cosDe*sin(ra),sin(dec)};
        double rqu[3] = {scrs_[0], scrs_[1], scrs_[2]};

        double k1a_t2[3] = {};
        iauSxp( iauPdp( rqu, k1a_t1 ), rqu, k1a_t2 );
        k1a_t2[0] = -k1a_t2[0];
        k1a_t2[1] = -k1a_t2[1];
        k1a_t2[2] = -k1a_t2[2];

        double k1a_temp[3] = {};
        iauPpp( rqu, k1a_t1, k1a_temp );
        iauPpp( k1a_temp, k1a_t2, k1a );

        //  source in TRS
        double rq[3] = {};
        iauRxp( c2t, k1a, rq );

        //  station in local system
        const auto &g2l2 = station.getPosition()->getGeodetic2Local();

        double g2l[3][3] = {{g2l2[0][0], g2l2[0][1], g2l2[0][2]},
                            {g2l2[1][0], g2l2[1][1], g2l2[1][2]},
                            {g2l2[2][0], g2l2[2][1], g2l2[2][2]}};
        double lq[3] = {};
        iauRxp( g2l, rq, lq );

        double zd = acos( lq[2] );
        double el = DPI / 2 - zd;

        double saz = atan2( lq[1], lq[0] );
        if ( lq[1] < 0 ) {
            saz = DPI * 2 + saz;
        }
        double az = fmod( saz + DPI, DPI * 2 );
        std::vector<double> d;
        d.push_back(reftimeSun[i]);
        d.push_back(az);
        d.push_back(el);
        //std::cout << reftimeSun[i] << " " << az*180/pi << " " << el*180/pi << " " << std::endl;
        AzElSun.push_back(d);
    }
    return AzElSun;
}

//[Station][SatellitePasses]
std::vector<std::vector<SatelliteForGUI::SatPass>> SatelliteForGUI::generatePassList( const VieVS::Network& network,
                                                                          const DateTime& start_time,
                                                                          const DateTime& end_time,
                                                                          const int time_step ) const {
#ifdef VIESCHEDPP_LOG
    BOOST_LOG_TRIVIAL( info ) << "start generating passlists for stations";
#else
    cout << "[info] start generating passlists for stations";
#endif
    //[Station][SatellitePasses]
    std::vector<std::vector<SatPass>> passList;
    std::vector<SatPass> satellitePasses;
    std::vector<VieVS::Station> stations = network.getStations();
    for ( const auto& station : stations ) {
        std::vector<std::vector<double>> azelSun = SatelliteForGUI::azelSun(station, start_time, end_time);
        satellitePasses.clear();
        CoordGeodetic user_geo( station.getPosition()->getLat(), station.getPosition()->getLon(),
                                station.getPosition()->getAltitude() / 1000, true );
        Observer obs( user_geo );
        SGP4* sgp4( this->pSGP4Data_ );
        DateTime aos_time;
        DateTime los_time;
        bool found_aos = false;

        DateTime previous_time( start_time );
        DateTime current_time( start_time );
        //std::cout << endl;
        //std::cout << station.getName() << std::endl ;
        while ( current_time < end_time ) {
            bool end_of_pass = false;
            /*
             * calculate satellite position
             */
            Eci eci = sgp4->FindPosition( current_time );
            CoordTopocentric topo = obs.GetLookAngle( eci );
            VieVS::PointingVector pv = VieVS::PointingVector( 1, 0 );
            pv.setAz( topo.azimuth );
            pv.setEl( topo.elevation );

            if ( !found_aos && station.isVisible( pv, 0 )) {
                /*
                 * aos hasnt occured yet, but the satellite is now above horizon
                 * this must have occured within the last time_step
                 */
                if ( start_time == current_time ) {
                    /*
                     * satellite was already above the horizon at the start,
                     * so use the start time
                     */
                    aos_time = start_time;
                } else {
                    /*
                     * find the point at which the satellite crossed the horizon
                     */
                    aos_time = findCrossingPoint( station, *sgp4, previous_time, current_time, true );
                }
                found_aos = true;
            }
            else if ( found_aos && !station.isVisible( pv, 0 ) ) {
                found_aos = false;
                /*
                 * end of pass, so move along more than time_step
                 */
                end_of_pass = true;
                /*
                 * already have the aos, but now the satellite is below the horizon,
                 * so find the los
                 */
                los_time = findCrossingPoint( station, *sgp4, previous_time, current_time, false );

                struct SatPass pd;
                pd.start = aos_time;
                pd.end = los_time;
                pd.stationID = station.getId();
                pd.satelliteID = this->getId();

                //std::cout << "Found Satellite Pass " << station.getName() << " " <<  this->getName() <<  std::endl;
                //std::cout << "start: " << pd.start << std::endl;
                //std::cout << "end: " << pd.end << std::endl << std::endl;

                std::vector<SatelliteForGUI::SatPass> checkedSatPasses = this->checkSatPass(pd,start_time,*sgp4,station, azelSun);
                //std::cout << "Checked Satellite Passes " << station.getName() << " " <<  this->getName() <<  std::endl;
                for(auto const &satPass:checkedSatPasses){
                    //std::cout << "start: " << satPass.start << std::endl;
                    //std::cout << "end: " << satPass.end << std::endl << std::endl;
                    satellitePasses.push_back( satPass );
                }
               // satellitePasses.push_back( pd );
            }

            /*
             * save current time
             */
            previous_time = current_time;

            if ( end_of_pass ) {
                /*
                 * at the end of the pass move the time along by 30mins
                 */
                current_time = current_time + TimeSpan( 0, 30, 0 );
                //current_time = current_time + TimeSpan( 0, 0, time_step );
            }
            else
            {
                /*
                 * move the time along by the time step value
                 */
                current_time = current_time + TimeSpan( 0, 0, time_step );
            }

            if ( current_time > end_time ) {
                /*
                 * dont go past end time
                 */
                current_time = end_time;
            }
        }

        if ( found_aos ) {
            /*
             * satellite still above horizon at end of search period, so use end
             * time as los
             */
            struct SatPass pd;
            pd.start = aos_time;
            pd.end = end_time;
            pd.stationID = station.getId();
            pd.satelliteID = this->getId();
            std::vector<SatelliteForGUI::SatPass> checkedSatPasses = checkSatPass(pd,start_time,*sgp4,station,azelSun);

            for(auto const &satPass:checkedSatPasses) {
                satellitePasses.push_back( satPass );
            }
        }
        passList.push_back( satellitePasses );
    }
#ifdef VIESCHEDPP_LOG
    BOOST_LOG_TRIVIAL( info ) << "finish generating passlists for stations sucessfully";
#else
    cout << "[info] finish generating passlists for stations sucessfully";
#endif
    return passList;
}

DateTime SatelliteForGUI::findCrossingPoint( const VieVS::Station& station, const SGP4& sgp4, const DateTime& initial_time1,
                                       const DateTime& initial_time2, bool finding_aos ) const {
    CoordGeodetic user_geo( station.getPosition()->getLat(), station.getPosition()->getLon(),
                            station.getPosition()->getAltitude() / 1000, true );
    Observer obs( user_geo );
    DateTime time1( initial_time1 );
    DateTime time2( initial_time2 );
    DateTime middle_time;

    bool running = true;
    while ( running ) {
        middle_time = time1.AddSeconds( ( time2 - time1 ).TotalSeconds() / 2.0 );
        /*
         * calculate satellite position
         */
        Eci eci = sgp4.FindPosition( middle_time );
        CoordTopocentric topo = obs.GetLookAngle( eci );
        VieVS::PointingVector pv = VieVS::PointingVector( 1, 0 );
        pv.setAz( topo.azimuth );
        pv.setEl( topo.elevation );
        if ( station.isVisible( pv, 0 ) ) {
            /*
             * satellite above horizon
             */
            if ( finding_aos ) {
                time2 = middle_time;
            } else {
                time1 = middle_time;
            }
        } else {
            if ( finding_aos ) {
                time1 = middle_time;
            } else {
                time2 = middle_time;
            }
        }
        if ( ( time2 - time1 ).TotalSeconds() < 0.0001 )  // if the timespan
        {
            /*
             * two times are within a millisecond, stop
             */
            running = false;  // to quit the while loop
        }
    }
    return middle_time;
}

vector<SatelliteForGUI> SatelliteForGUI::readSatelliteFile( std::string filename ) {
    vector<SatelliteForGUI> satellites;
    try {
        std::ifstream fid( filename );
        std::string line;
        std::string hdr;
        std::string line1;
        std::string line2;
        if( fid.is_open() ) {
            while ( getline( fid, line ) ) {
                if ( isalpha( line[0] ) ) {
                    hdr = boost::trim_copy(VieVS::util::simplify(line));
                    continue;
                }
                else if ( line[0] == '1' ) {
                    line1 = boost::trim_copy(line);
                    continue;
                }
                else if ( line[0] == '2' ) {
                    line2 = boost::trim_copy(line);
                    if ( !hdr.empty() && !line1.empty() && !line2.empty() ) {
                        SatelliteForGUI sat( hdr, line1, line2 );
                        satellites.push_back( sat );
                        hdr = "";
                        line1 = "";
                        line2 = "";
                    }
                    else {
/*#ifdef VIESCHEDPP_LOG
                        BOOST_LOG_TRIVIAL( error ) << "wrong file format for satellite input" << filename;
#else
                        cout << "[error] wrong file format for satellite input" << filename;
#endif*/
                        satellites.clear();
                        throw "Wrong file format for satellite file! Please check the required format!";
                    }
                    continue;
                }
                else if(line.empty())
                {
                    if (!getline( fid, line ))
                    {
                        fid.close();
                        break;
                    }
                    else
                    {
                        throw "Error reading the satellite file - there is an empty line in the file!";
                    }
                }
                else {
                    throw "Wrong file format for satellite file! Please check the required format!";
                }
            }
            fid.close();
        }
        else {
            throw "There was an error opening the file!";
        }
    } catch ( std::exception const& e ) {
#ifdef VIESCHEDPP_LOG
        BOOST_LOG_TRIVIAL( error ) << "unable to open " << filename;
#else
        cout << "[error] unable to open " << filename;
#endif
        throw -1;
        //terminate();
    }
    return satellites;
}

CoordGeodetic SatelliteForGUI::getPosition(DateTime time)
{
    Eci eci(this->getSGP4Data()->FindPosition(time));
    CoordGeodetic pos = eci.ToGeodetic();
    return pos;
}

/*void Satellite::GetRates(const Observer &obs, const Eci &eci , DateTime current_time, DateTime start_time)
{
    // Earth rotaion rate [rad/sec], T.S. Kelso, 1995, Satellite Times,
    // Orbital Coordinate Systems, Part I
    double omeg = 7.29211510e-005;

    //pdate the observers Eci to match the time of the Eci passed inif necessary
    //Update( eci.GetDateTime() );
    Eci Obs_eci = Eci(current_time, obs.GetLocation()) ;

    //calculate differences
    Vector range_rate = eci.Velocity() - Obs_eci.Velocity();
    Vector range = eci.Position() - Obs_eci.Position();

    range.w = range.Magnitude();

    //Calculate Local Mean Sidereal Time for observers longitude

    double theta = eci.GetDateTime().ToLocalMeanSiderealTime( obs.GetLocation().longitude );

    double sin_lat = sin( obs.GetLocation().latitude );
    double cos_lat = cos( obs.GetLocation().latitude );
    double sin_theta = sin( theta );
    double cos_theta = cos( theta );

    //Range vector Topocentric - Horizon Coordinate System (south, east, zenith) [m]
    double top_s = sin_lat * cos_theta * range.x + sin_lat * sin_theta * range.y - cos_lat * range.z;
    double top_e = -sin_theta * range.x + cos_theta * range.y;
    double top_z = cos_lat * cos_theta * range.x + cos_lat * sin_theta * range.y + sin_lat * range.z;
    double el = asin( top_z / range.w );

    //Satellite Velocity
    double sv_x = eci.Velocity().x - (-omeg*eci.Position().y);
    double sv_y = eci.Velocity().y - (-omeg*eci.Position().x);
    double sv_z = eci.Velocity().z ;

    double svTop_s = sin_lat * cos_theta * sv_x + sin_lat * sin_theta * sv_y - cos_lat * sv_z;
    double svTop_e = -sin_theta * sv_x + cos_theta * sv_y;
    double svTop_z = cos_lat * cos_theta * sv_x + cos_lat * sin_theta * sv_y + sin_lat * sv_z;

    double r_rate = (top_s * svTop_s + top_e * svTop_e + top_z * svTop_z)/range.w;
    double az_rate = (svTop_s * top_e - svTop_e * top_s) / (top_s * top_s + top_e * top_e);
    double el_rate = (svTop_z - r_rate * sin(el)) / sqrt(top_s * top_s + top_e * top_e);

    Vector xd = range;

    // calculation of right ascension and declination for satellite
    double r = sqrt( xd.x * xd.x + xd.y * xd.y + xd.z * xd.z );
    double dec = asin( xd.z / r );
    double ra;
    if ( eci.Position().y / r > 0 ) {
        ra = acos( xd.x / r * 1 / cos( dec ) );
    } else {
        ra = 2 * pi - acos( xd.x / r * 1 / cos( dec ) );
    }

    //pv->setTime(
        //round( ( ( current_time.ToJ2000() - start_time.ToJ2000() ) * 86400 ) ) );  // seconds since session start
    double mjd = current_time.ToJulian() - 2400000.5;
    double gmst = VieVS::TimeSystem::mjd2gmst( mjd );
    double ha = gmst + obs.GetLocation().longitude - ra;
    while ( ha > pi ) {
        ha = ha - twopi;
    }
    while ( ha < -pi ) {
        ha = ha + twopi;
    }

    double temp1 = -range.y * range.y - range.x*range.x;
    double small = 0.00000001;
    double ra_rate;
    if(abs(temp1)> small)
    {
        //ra_rate= ( v(1)*range.y - v(2)*range.x ) / temp1;
    }

}*/

/*bool Satellite::checkSlewRates(Station station, Eci sat, DateTime current_time )
{
    std::string mount_type = station.getAntenna().getMount();
    if(mount_type == "ALTAZ")
    {
         std::tuple<double, double, double, double> AzElRates = Satellite::getAzElRates(station,sat, current_time);
         if(station.getAntenna().getRate1() > abs(get<2>(AzElRates)) && station.getAntenna().getRate2() > abs(get<3>(AzElRates)))
         {

             //std::cout << current_time << station.getName() <<" " << station.getAntenna().getRate1()*180/pi <<" " << abs(get<2>(AzElRates))*180/pi <<" " << station.getAntenna().getRate2()*180/pi<<" " << abs(get<3>(AzElRates))*180/pi << std::endl;
             return true;
         }
         else
         {
             std::cout << "Antenna Slew Rates exceeded!" <<std::endl;
             std::cout << current_time << station.getName() <<" " << station.getAntenna().getRate1()*180/pi <<" " << abs(get<2>(AzElRates))*180/pi <<" " << station.getAntenna().getRate2()*180/pi<<" " << abs(get<3>(AzElRates))*180/pi << std::endl;
             return false;
         }
    }
    else if(mount_type == "EQUA")
    {
        std::tuple<double, double, double, double, double, double> RaDecHaRates = Satellite::getRaDecHaRates(station,sat, current_time);
        if(station.getAntenna().getRate1() > abs(get<5>(RaDecHaRates)) &&  station.getAntenna().getRate2() > abs(get<4>(RaDecHaRates)))
        {
            return true;
        }
        else
        {
            std::cout << "Antenna Slew Rates exceeded!" <<std::endl;
            return false;
        }
    }
    else if(mount_type == "XY_E")
    {
        std::tuple<double, double, double,double> XYRates = Satellite::getXYRates(station,sat, current_time);
        if(station.getAntenna().getRate1() > abs(get<2>(XYRates)) &&  station.getAntenna().getRate2() > abs(get<3>(XYRates)))
        {
            return true;
        }
        else
        {
            std::cout << "Antenna Slew Rates exceeded!" <<std::endl;
            return false;
        }
    }
}*/

/*std::tuple<double, double, double,double> Satellite::getAzElRates(Station station, Eci sat, DateTime current_time)
{
    CoordGeodetic user_geo( station.getPosition().getLat(), station.getPosition().getLon(),
                            station.getPosition().getHeight() / 1000, true );
    Eci eci_obs = Eci( current_time, user_geo);


    //calculate differences
    Vector range_rate = sat.Velocity() - eci_obs.Velocity();
    Vector range = sat.Position() - eci_obs.Position();

    range.w = range.Magnitude();

    //Calculate Local Mean Sidereal Time for observers longitude
    double theta = sat.GetDateTime().ToLocalMeanSiderealTime( user_geo.longitude );

    double sin_lat = sin( user_geo.latitude );
    double cos_lat = cos( user_geo.latitude );
    double sin_theta = sin( theta );
    double cos_theta = cos( theta );

    double top_s = sin_lat * cos_theta * range.x + sin_lat * sin_theta * range.y - cos_lat * range.z;
    double top_e = -sin_theta * range.x + cos_theta * range.y;
    double top_z = cos_lat * cos_theta * range.x + cos_lat * sin_theta * range.y + sin_lat * range.z;

    Vector roh_sez = Vector(top_s, top_e, top_z);
    roh_sez.w = roh_sez.Magnitude();

    //Vector t2 = sat.Velocity() - Vector(-omega*sat.Position().y,+omega*sat.Position().x,0);
    Vector t = sat.Velocity() - eci_obs.Velocity();
    double v_top_s = sin_lat * cos_theta * t.x + sin_lat * sin_theta * t.y - cos_lat * t.z;
    double v_top_e = -sin_theta * t.x + cos_theta * t.y;
    double v_top_z = cos_lat * cos_theta * t.x + cos_lat * sin_theta * t.y + sin_lat * t.z;
    Vector v_roh_sez = Vector(v_top_s, v_top_e, v_top_z);

    double el = asin(roh_sez.z / roh_sez.w);

    double az = atan( -top_e / top_s );

    if ( top_s > 0.0 ) {
        az += kPI;
    }

    if ( az < 0.0 ) {
        az += 2.0 * kPI;
    }
   // double az ;
   // if(el != pi/2)
   // {
   //     az = asin(top_e / sqrt(roh_sez.x * roh_sez.x + roh_sez.y * roh_sez.y));
   // }
   // else
   // {
   //     az = asin(v_top_e/sqrt(v_top_s * v_top_s + v_top_e*v_top_e));
   //}
   // if ( top_s > 0.0 ) {
   //    az += kPI;
   //}
   //if ( az < 0.0 ) {
   //   az += 2.0 * kPI;
   //}

    double roh_rate = roh_sez.Dot(v_roh_sez)/range.w ;
    double az_rate = (v_roh_sez.x * roh_sez.y - v_roh_sez.y * roh_sez.x)/(roh_sez.x * roh_sez.x + roh_sez.y * roh_sez.y);
    double el_rate = (v_roh_sez.z - roh_rate * sin(el))/ sqrt(roh_sez.x * roh_sez.x + roh_sez.y * roh_sez.y);
    std::tuple<double, double, double, double> AzElRates(az, el, az_rate, el_rate);


    //std::cout << " Az: " << az*180/pi << " El: " << el*180/pi << " Az Rate: " << get<2>(AzElRates)*180/pi << " El Rate: " << get<3>(AzElRates)*180/pi << std::endl;

    return AzElRates;
}*/

/*std::tuple<double, double, double, double, double, double> Satellite::getRaDecHaRates(Station station, Eci sat, DateTime current_time)
{
    CoordGeodetic user_geo( station.getPosition().getLat(), station.getPosition().getLon(),
                            station.getPosition().getHeight() / 1000, true );
    Eci eci_obs = Eci( current_time, user_geo);

    Vector range_rate = sat.Velocity() - eci_obs.Velocity();
    Vector range = sat.Position() - eci_obs.Position();

    range.w = range.Magnitude();
    double dec = asin( range.z / range.w );
    double ra3;
    if(sqrt(range.x *range.x + range.y * range.y) != 0)
    {
        ra3 = atan2(range.y, range.x);
        //ra3 = asin (range.y / sqrt(range.x * range.x + range.y * range.y));
    }
    else
    {
        ra3 = atan2(range_rate.y, range_rate.x);
        //ra3 = asin(range_rate.y / (sqrt(range_rate.x * range_rate.x + range_rate.y * range_rate.y)));
    }

    double ra;
    if ( sat.Position().y / range.w > 0 ) {
            ra = acos( range.x / range.w * 1 / cos( dec ) );
        } else {
            ra = 2 * pi - acos( range.x / range.w * 1 / cos( dec ) );
        }

    double mjd = current_time.ToJulian() - 2400000.5;
    double gmst = VieVS::TimeSystem::mjd2gmst( mjd );
    double ha = gmst + user_geo.longitude- ra;
    while ( ha > pi ) {
        ha = ha - twopi;
    }
    while ( ha < -pi ) {
        ha = ha + twopi;
    }

    double ra_rate;

    if(-range.y * range.y - range.x * range.x != 0)
    {
        ra_rate = (range_rate.x*range.y - range_rate.y * range.x) / (-range.y * range.y - range.x * range.x);

    }
    else
    {
        ra_rate = 0;
    }
    double dec_rate;
    double roh_rate = range.Dot(range_rate)/range.w ;
    if(sqrt(range.x *range.x + range.y * range.y)!=0)
    {
        dec_rate = (range_rate.z - roh_rate * sin(dec))/sqrt(range.x *range.x + range.y * range.y);
    }
    else
    {
        dec_rate = 0;
    }

    double ha_rate = omega - ra_rate;



    //std::cout << endl;
    //std::cout << " Ra check: " << ra*180/pi/15<< " dec: " << dec*180/pi << " Ha: " << ha*180/pi *1/15 << " ra Rate: " << ra_rate*180/pi/15 << " dec Rate: " << dec_rate*180/pi << "ha rate" << ha_rate*180/pi/15 << std::endl;

    std::tuple<double, double, double, double, double, double> RaDecHaRates(ra, dec, ha, ra_rate, dec_rate, ha_rate);
    return RaDecHaRates;
}*/

/*std::tuple<double, double, double, double> Satellite::getXYRates(Station station, Eci sat, DateTime current_time)
{
    //az, el in rad,  az_rate, el_rate in rad/sec
    std::tuple<double, double, double, double> AzElRates = Satellite::getAzElRates(station, sat, current_time);
    double az = get<0>(AzElRates);
    double el = get<1>(AzElRates);
    double az_rate = get<2>(AzElRates);
    double el_rate = get<3>(AzElRates);

    double cel = cos( el );
    double sel = sin( el );
    double caz = cos( az);
    double saz = sin( az );
    double x = atan2( cel * caz, sel );
    double y = asin( cel * saz );

    double y_rate = 1/sqrt(1- saz*cel) * (caz * cel * az_rate - saz*sel*el_rate);
    double x_rate = 1/(1 + (caz*cel/sel) * (caz*cel/sel) ) * (-1) * (((saz*cel*az_rate + caz*sel*el_rate)/sel) + ((caz*cel * cel *el_rate)/(sel* sel)));
    std::tuple<double, double, double, double> XYRates(x, y, x_rate,y_rate);

    //std::cout << " X: " << x*180/pi << " Y: " << y*180/pi << " X Rate: " <<x_rate*180/pi << " Y Rate: " << y_rate*180/pi << std::endl;
    return XYRates;
}*/
