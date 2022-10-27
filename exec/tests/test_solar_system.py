
import unittest

import os
import numpy
import shutil
import subprocess
from datetime import datetime, timedelta

from lsl.astro import DJD_OFFSET, MJD_OFFSET

try:
    import ephem
    use_ephem = True
except ImportError:
    use_ephem = False

try:
    from astropy.coordinates import solar_system, EarthLocation, AltAz
    from astropy.time import Time, TimeDelta
    use_astropy = True
except ImportError:
    use_astropy = False

try:
    import json
    import requests
    use_horizons = True
except ImportError:
    use_horizons = False


_test_date_start = datetime.utcnow()
_test_date_end = _test_date_start + timedelta(days=60)
TEST_DATE_START = (_test_date_start.year, _test_date_start.month, _test_date_start.day)
TEST_DATE_END = (_test_date_end.year, _test_date_end.month, _test_date_end.day)
TEST_DATE_STEP_DAY = 1

TEST_TOLERANCE_ARCSEC = 1.0


def _call_mcs(mjd, mpm, lat, lng, hgt, type='sol'):
    find = subprocess.check_output(['./me_find%s_x' % type, str(mjd), str(mpm)],
                                   cwd=os.path.dirname(os.path.dirname(__file__)))
    find = find.decode()
    for line in find.split('\n'):
        if line.startswith('RA'):
            _, _, ra = line.split('=', 2)
            h, _, m, _, s, _ = ra.split(None, 5)
            ra = float(h) + float(m)/60 + float(s)/3600
        elif line.startswith('dec'):
            _, _, dec = line.split('=', 2)
            d, _, m, _, s, _ = dec.split(None, 5)
            g = 1
            if d[0] == '-':
                g = -1
            dec = abs(float(d)) + float(m)/60 + float(s)/3600
            dec *= g
        elif line.startswith('dist'):
            _, dist = line.split('=', 1)
            dist, _ = dist.split(None, 1)
            dist = float(dist)
            
    get = subprocess.check_output(['./me_getaltaz_x', str(ra), str(dec), str(dist), str(mjd), str(mpm), str(lat), str(lng), str(hgt)],
                                  cwd=os.path.dirname(os.path.dirname(__file__)))
    get = get.decode()
    for line in get.split('\n'):
        if line.startswith('ALT'):
            _, _, alt = line.split('=', 2)
            d, _, m, _, s, _ = alt.split(None, 5)
            g = 1
            if d[0] == '-':
                g = -1
            alt = abs(float(d)) + float(m)/60 + float(s)/3600
            alt *= g
        elif line.startswith('AZ'):
            _, _, az = line.split('=', 2)
            d, _, m, _, s, _ = az.split(None, 5)
            g = 1
            if d[0] == '-':
                g = -1
            az = abs(float(d)) + float(m)/60 + float(s)/3600
            az *= g
            
    return (alt, az)


def _call_mcs_sun(mjd, mpm, lat, lng, hgt):
    return _call_mcs(mjd, mpm, lat, lng, hgt, type='sol')


def _call_mcs_jupiter(mjd, mpm, lat, lng, hgt):
    return _call_mcs(mjd, mpm, lat, lng, hgt, type='jov')


def _call_mcs_moon(mjd, mpm, lat, lng, hgt):
    return _call_mcs(mjd, mpm, lat, lng, hgt, type='lun')


def _separation(az_alt1, az_alt2):
    az1, alt1 = az_alt1
    az1 = az1 * numpy.pi/180
    alt1 = alt1 * numpy.pi/180
    
    az2, alt2 = az_alt2
    az2 = az2 * numpy.pi/180
    alt2 = alt2 * numpy.pi/180
    
    sdlon = numpy.sin(az2 - az1)
    cdlon = numpy.cos(az2 - az1)
    salt1 = numpy.sin(alt1)
    salt2 = numpy.sin(alt2)
    calt1 = numpy.cos(alt1)
    calt2 = numpy.cos(alt2)
    
    num1 = calt2 * sdlon
    num2 = calt1 * salt2 - salt1 * calt2 * cdlon
    denominator = salt1 * salt2 + calt1 * calt2 * cdlon
    
    sep = numpy.arctan2(numpy.hypot(num1, num2), denominator)
    return sep * 180/numpy.pi * 3600


class SolarSystemTests(unittest.TestCase):
    _lat = 35.0
    _lng = -108.0
    _hgt = 2000.0
    
    _ras = (0, 2, 4, 8, 16)
    _decs = (-60, -30, 0, 30, 60, 89)
    
    def run_ephem_values(self, planet_base, lat=35.0, lng=-180.0, hgt=2000.0):
        observer = ephem.Observer()
        observer.lat = str(lat)
        observer.lon = str(lng)
        observer.elevation = hgt
        observer.pressure = 0
        observer.temp = 0
        tStart = ephem.Date(TEST_DATE_START)
        tStop = ephem.Date(TEST_DATE_END)
        tStep = TEST_DATE_STEP_DAY
        
        body = planet_base()
        
        t = tStart*1.0
        values = []
        while t < tStop:
            observer.date = t
            mjd = t+DJD_OFFSET-MJD_OFFSET
            mpm = int((int(mjd)- mjd)*86400*1000)
            mjd = int(mjd)
            
            body.compute(observer)
            if planet_base is ephem.Sun:
                mcs_alt, mcs_az = _call_mcs_sun(mjd, mpm, self._lat, self._lng, self._hgt)
            elif planet_base is ephem.Jupiter:
                mcs_alt, mcs_az = _call_mcs_jupiter(mjd, mpm, self._lat, self._lng, self._hgt)
            else:
                mcs_alt, mcs_az = _call_mcs_moon(mjd, mpm, self._lat, self._lng, self._hgt)
                
            values.append({'mjd': t+DJD_OFFSET-MJD_OFFSET,
                           'alt': body.alt*180/numpy.pi,
                           'az':  body.az*180/numpy.pi,
                           'mcs_alt': mcs_alt,
                           'mcs_az':  mcs_az})
            t += tStep
            
        return values
        
    @unittest.skipUnless(use_ephem, "requires the 'ephem' module")
    def test_ephem_sun(self):
        """compare mcs to pyephem for the Sun"""
        
        values = self.run_ephem_values(ephem.Sun,
                                       self._lat, self._lng, self._hgt)
        
        for value in values:
            with self.subTest(mjd=value['mjd']):
                sep = _separation((value['az'], value['alt']),
                                  (value['mcs_az'], value['mcs_alt']))
                self.assertLess(sep, TEST_TOLERANCE_ARCSEC)
                
    @unittest.expectedFailure
    @unittest.skipUnless(use_ephem, "requires the 'ephem' module")
    def test_ephem_jupiter(self):
        """compare mcs to pyephem for Jupiter"""
        
        values = self.run_ephem_values(ephem.Jupiter,
                                       self._lat, self._lng, self._hgt)
        
        for value in values:
            with self.subTest(mjd=value['mjd']):
                sep = _separation((value['az'], value['alt']),
                                  (value['mcs_az'], value['mcs_alt']))
                self.assertLess(sep, TEST_TOLERANCE_ARCSEC)
                
    @unittest.expectedFailure
    @unittest.skipUnless(use_ephem, "requires the 'ephem' module")
    def test_ephem_moon(self):
        """compare mcs to pyephem for the Moon"""
        
        values = self.run_ephem_values(ephem.Moon,
                                       self._lat, self._lng, self._hgt)
        
        for value in values:
            with self.subTest(mjd=value['mjd']):
                sep = _separation((value['az'], value['alt']),
                                  (value['mcs_az'], value['mcs_alt']))
                self.assertLess(sep, TEST_TOLERANCE_ARCSEC)
                
    def run_astropy_values(self, planet_name, lat=35.0, lng=-180.0, hgt=2000.0):
        observer = EarthLocation.from_geodetic(lng, lat, height=hgt)
        tStart = Time('%04i-%02i-%02i 00:00:00' % TEST_DATE_START, format='iso', scale='utc')
        tStop = Time('%04i-%02i-%02i 00:00:00' % TEST_DATE_END, format='iso', scale='utc')
        tStep = TimeDelta(TEST_DATE_STEP_DAY*86400, format='sec')
        
        t = tStart.copy()
        values = []
        while t < tStop:
            body = solar_system.get_body(planet_name, t)
            frame = AltAz(location=observer, obstime=t,
                          pressure=0, temperature=0, obswl=0)
            mjd = t.mjd
            mpm = int((int(mjd)- mjd)*86400*1000)
            mjd = int(mjd)
            
            obj = body.transform_to(frame)
            if planet_name == 'sun':
                mcs_alt, mcs_az = _call_mcs_sun(mjd, mpm, self._lat, self._lng, self._hgt)
            elif planet_name == 'jupiter':
                mcs_alt, mcs_az = _call_mcs_jupiter(mjd, mpm, self._lat, self._lng, self._hgt)
            else:
                mcs_alt, mcs_az = _call_mcs_moon(mjd, mpm, self._lat, self._lng, self._hgt)
                
            values.append({'mjd': t.mjd,
                           'alt': obj.alt.to('deg').value,
                           'az':  obj.az.to('deg').value,
                           'mcs_alt': mcs_alt,
                           'mcs_az':  mcs_az})
            t += tStep
            
        return values
        
    @unittest.skipUnless(use_astropy, "requires the 'astropy' module")
    def test_astropy_sun(self):
        """compare mcs to astropy for the Sun"""
        
        values = self.run_astropy_values('sun',
                                         self._lat, self._lng, self._hgt)
        
        for value in values:
            with self.subTest(mjd=value['mjd']):
                sep = _separation((value['az'], value['alt']),
                                  (value['mcs_az'], value['mcs_alt']))
                self.assertLess(sep, TEST_TOLERANCE_ARCSEC)
                
    @unittest.skipUnless(use_astropy, "requires the 'astropy' module")
    def test_astropy_jupiter(self):
        """compare mcs to astropy for Jupiter"""
        
        values = self.run_astropy_values('jupiter',
                                         self._lat, self._lng, self._hgt)
        
        for value in values:
            with self.subTest(mjd=value['mjd']):
                sep = _separation((value['az'], value['alt']),
                                  (value['mcs_az'], value['mcs_alt']))
                self.assertLess(sep, TEST_TOLERANCE_ARCSEC)
                
    @unittest.skipUnless(use_astropy, "requires the 'astropy' module")
    def test_astropy_moon(self):
        """compare mcs to astropy for the Moon"""
        
        values = self.run_astropy_values('moon',
                                         self._lat, self._lng, self._hgt)
        
        for value in values:
            with self.subTest(mjd=value['mjd']):
                sep = _separation((value['az'], value['alt']),
                                  (value['mcs_az'], value['mcs_alt']))
                self.assertLess(sep, TEST_TOLERANCE_ARCSEC)
                
    def run_horizons_values(self, planet_code, lat=35.0, lng=-180.0, hgt=2000.0):
        r = requests.get("https://ssd.jpl.nasa.gov/api/horizons.api",
                         params={'COMMAND': str(planet_code),
                                 'OBJ_DATA':  'NO',
                                 'MAKE_EPHEM': 'YES',
                                 'EPHEM_TYPE': 'OBSERVER',
                                 'CENTER': "'coord'",
                                 'SITE_COORD': f"'{lng},{lat},{hgt}'",
                                 'COORD_TYPE': 'GEODETIC',
                                 'START_TIME': "'%04i-%02i-%02i'" % TEST_DATE_START,
                                 'STOP_TIME': "'%04i-%02i-%02i'" % TEST_DATE_END,
                                 'STEP_SIZE': "'%i d'" % TEST_DATE_STEP_DAY,
                                 'CAL_FORMAT': "'JD'",
                                 'ANG_FORMAT': "'DEG'",
                                 'APPARENT': "'AIRLESS'",
                                 'QUANTITIES': "'1,4,20'"})
        j = json.loads(r.content)
        
        in_block = False
        values = []
        for line in j['result'].split('\n'):
            if line.startswith('$$SOE'):
                in_block = True
            elif line.startswith('$$EOE'):
                in_block = False
            elif in_block:
                try:
                    try:
                        jd, _, ra, dec, az, alt, dist, _ = line.split(None, 7)
                    except ValueError:
                        jd, ra, dec, az, alt, dist, _ = line.split(None, 6)
                    mjd = float(jd) - MJD_OFFSET
                    mpm = int((int(mjd)- mjd)*86400*1000)
                    mjd = int(mjd)
                    
                    if planet_code == 10:
                        mcs_alt, mcs_az = _call_mcs_sun(mjd, mpm, self._lat, self._lng, self._hgt)
                    elif planet_code == 599:
                        mcs_alt, mcs_az = _call_mcs_jupiter(mjd, mpm, self._lat, self._lng, self._hgt)
                    else:
                        mcs_alt, mcs_az = _call_mcs_moon(mjd, mpm, self._lat, self._lng, self._hgt)
                        
                    values.append({'mjd':  float(jd) - MJD_OFFSET,
                                   'az':   float(az),
                                   'alt':  float(alt),
                                   'mcs_alt': mcs_alt,
                                   'mcs_az':  mcs_az})
                except ValueError:
                    print(f"Bad line: {line}")
                    
        return values
        
    @unittest.expectedFailure
    @unittest.skipUnless(use_horizons, "requires the 'requests' module")
    def test_horizons_sun(self):
        """compare mcs to JPL Horizons for the Sun"""
        
        values = self.run_horizons_values(10,
                                          self._lat, self._lng, self._hgt)
        
        for value in values:
            with self.subTest(mjd=value['mjd']):
                sep = _separation((value['az'], value['alt']),
                                  (value['mcs_az'], value['mcs_alt']))
                self.assertLess(sep, TEST_TOLERANCE_ARCSEC)
                
    @unittest.expectedFailure
    @unittest.skipUnless(use_horizons, "requires the 'requests' module")
    def test_horizons_jupiter(self):
        """compare mcs to JPL Horizons for Jupiter"""
        
        values = self.run_horizons_values(599,
                                          self._lat, self._lng, self._hgt)
        
        for value in values:
            with self.subTest(mjd=value['mjd']):
                sep = _separation((value['az'], value['alt']),
                                  (value['mcs_az'], value['mcs_alt']))
                self.assertLess(sep, TEST_TOLERANCE_ARCSEC)
                
    @unittest.expectedFailure
    @unittest.skipUnless(use_horizons, "requires the 'requests' module")
    def test_horizons_moon(self):
        """compare mcs to JPL Horizons for the Moon"""
        
        values = self.run_horizons_values(301,
                                          self._lat, self._lng, self._hgt)
        
        for value in values:
            with self.subTest(mjd=value['mjd']):
                sep = _separation((value['az'], value['alt']),
                                  (value['mcs_az'], value['mcs_alt']))
                self.assertLess(sep, TEST_TOLERANCE_ARCSEC)


class solar_system_test_suite(unittest.TestSuite):
    """A unittest.TestSuite class which contains all of the MCS astrometry tests
    for solar system objects."""
    
    def __init__(self):
        unittest.TestSuite.__init__(self)
        
        loader = unittest.TestLoader()
        self.addTests(loader.loadTestsFromTestCase(SolarSystemTests))


if __name__ == '__main__':
    unittest.main()
