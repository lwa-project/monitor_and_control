
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
    from astropy.coordinates import SkyCoord, EarthLocation, AltAz
    from astropy.time import Time, TimeDelta
    use_astropy = True
except ImportError:
    use_astropy = False


_test_date_start = datetime.utcnow()
_test_date_end = _test_date_start + timedelta(days=60)
TEST_DATE_START = (_test_date_start.year, _test_date_start.month, _test_date_start.day)
TEST_DATE_END = (_test_date_end.year, _test_date_end.month, _test_date_end.day)
TEST_DATE_STEP_DAY = 1

TEST_TOLERANCE_ARCSEC = 1.5


def _call_mcs(mjd, mpm, ra, dec, lat, lng, hgt):
    find = subprocess.check_output(['./me_precess_x', str(mjd), str(mpm), str(ra), str(dec)],
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
            
    get = subprocess.check_output(['./me_getaltaz_x', str(ra), str(dec), str(1e9), str(mjd), str(mpm), str(lat), str(lng), str(hgt)],
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


class CelestialTests(unittest.TestCase):
    _lat = 35.0
    _lng = -108.0
    _hgt = 2000.0
    
    _ras = (0, 2, 4, 8, 16)
    _decs = (-60, -30, 0, 30, 60, 89)
    
    def run_ephem_values(self, ra, dec, lat=35.0, lng=-108.0, hgt=2000.0):
        observer = ephem.Observer()
        observer.lat = str(lat)
        observer.lon = str(lng)
        observer.elevation = hgt
        observer.pressure = 0
        observer.temp = 0
        tStart = ephem.Date(TEST_DATE_START)
        tStop = ephem.Date(TEST_DATE_END)
        tStep = TEST_DATE_STEP_DAY
        
        body = ephem.FixedBody()
        body._ra = ra*15 * numpy.pi/180
        body._dec = dec * numpy.pi/180
        body._epoch = ephem.J2000
        
        t = tStart*1.0
        values = []
        while t < tStop:
            observer.date = t
            mjd = t+DJD_OFFSET-MJD_OFFSET
            mpm = int((int(mjd)- mjd)*86400*1000)
            mjd = int(mjd)
            
            body.compute(observer)
            mcs_alt, mcs_az = _call_mcs(mjd, mpm, ra, dec, self._lat, self._lng, self._hgt)
            
            values.append({'mjd': t+DJD_OFFSET-MJD_OFFSET,
                           'ra': ra,
                           'dec': dec,
                           'alt': body.alt*180/numpy.pi,
                           'az':  body.az*180/numpy.pi,
                           'mcs_alt': mcs_alt,
                           'mcs_az':  mcs_az})
            t += tStep
            
        return values
        
    @unittest.skipUnless(use_ephem, "requires the 'ephem' module")
    def test_ephem(self):
        """compare mcs to pyephem for celestial objects"""
        
        for ra in self._ras:
            for dec in self._decs:
                values = self.run_ephem_values(ra, dec,
                                               self._lat, self._lng, self._hgt)
                
                for value in values:
                    with self.subTest(mjd=value['mjd'], ra=ra, dec=dec):
                        sep = _separation((value['az'], value['alt']),
                                          (value['mcs_az'], value['mcs_alt']))
                        self.assertLess(sep, TEST_TOLERANCE_ARCSEC)
                        
    def run_astropy_values(self, ra, dec, lat=35.0, lng=-108.0, hgt=2000.0):
        observer = EarthLocation.from_geodetic(lng, lat, height=hgt)
        tStart = Time('%04i-%02i-%02i 00:00:00' % TEST_DATE_START, format='iso', scale='utc')
        tStop = Time('%04i-%02i-%02i 00:00:00' % TEST_DATE_END, format='iso', scale='utc')
        tStep = TimeDelta(TEST_DATE_STEP_DAY*86400, format='sec')
        
        body = SkyCoord(ra*15, dec, unit='deg', frame='fk5')
        
        t = tStart.copy()
        values = []
        while t < tStop:
            frame = AltAz(location=observer, obstime=t,
                          pressure=0, temperature=0, relative_humidity=0, obswl=0)
            mjd = t.mjd
            mpm = int((int(mjd)- mjd)*86400*1000)
            mjd = int(mjd)
            
            obj = body.transform_to(frame)
            mcs_alt, mcs_az = _call_mcs(mjd, mpm, ra, dec, self._lat, self._lng, self._hgt)
            # print(obj.alt.to('deg').value, obj.az.to('deg').value, mjd, mpm, ra, dec, mcs_alt, mcs_az)
            
            values.append({'mjd': t.mjd,
                           'ra': ra,
                           'dec': dec,
                           'alt': obj.alt.to('deg').value,
                           'az':  obj.az.to('deg').value,
                           'mcs_alt': mcs_alt,
                           'mcs_az':  mcs_az})
            t += tStep
            
        return values
        
    @unittest.skipUnless(use_astropy, "requires the 'astropy' module")
    def test_astropy(self):
        """compare mcs with astropy for celestial objects"""
        for ra in self._ras:
            for dec in self._decs:
                values = self.run_astropy_values(ra, dec,
                                                 self._lat, self._lng, self._hgt)
                
                for value in values:
                    with self.subTest(mjd=value['mjd'], ra=ra, dec=dec):
                        sep = _separation((value['az'], value['alt']),
                                          (value['mcs_az'], value['mcs_alt']))
                        self.assertLess(sep, TEST_TOLERANCE_ARCSEC)


class celestial_test_suite(unittest.TestSuite):
    """A unittest.TestSuite class which contains all of the MCS astrometry tests
    for celestial objects."""
    
    def __init__(self):
        unittest.TestSuite.__init__(self)
        
        loader = unittest.TestLoader()
        self.addTests(loader.loadTestsFromTestCase(CelestialTests))


if __name__ == '__main__':
    unittest.main()
