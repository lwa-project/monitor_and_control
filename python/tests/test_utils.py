import unittest
import time
import os
from datetime import datetime

from lwa_mcs import utils


class utils_tests(unittest.TestCase):
    def setUp(self):
        with open('test.sh', 'w') as fh:
            fh.write("#!/bin/bash\ndate")
            
    def tearDown(self):
        try:
            os.unlink('test.sh')
        except OSError:
            pass
            
    def test_uptime(self):
        """Test reading in the system uptime"""
        
        ut = utils.get_uptime()
        with open('/proc/uptime', 'r') as fh:
            ut2 = fh.read()
            ut2 = float(ut2.split(None, 1)[0]) / 60
            
        self.assertTrue(abs(ut2-ut) < 1)
        
    def test_current_mjd_mpm(self):
        """Test that the get_current_mjdmpm() function works"""
        
        dt = datetime.utcnow()
        mjd, mpm = utils.get_current_mjdmpm()
        dt2 = utils.mjdmpm_to_datetime(mjd, mpm)
        self.assertTrue((dt2-dt).total_seconds() < 1)
        
    def test_mjd_mpm(self):
        """Test datetime -> mjd, mpm -> datetime"""
        
        dt = datetime.utcnow()
        dt = dt.replace(microsecond=0)
        mjd, mpm = utils.datetime_to_mjdmpm(dt)
        dt2 = utils.mjdmpm_to_datetime(mjd, mpm)
        self.assertEqual(dt, dt2)
        
    def test_get_at_queue(self):
        """Test reading the 'at' queue"""
        
        utils.get_at_queue()
        
    def test_schedule_at_command(self):
        """Test scheduling an 'at' command"""
        
        before = utils.get_at_queue()
        id = utils.schedule_at_command(time.time() + 600, './test.sh')
        self.assertTrue(id != -1)
        after = utils.get_at_queue()
        self.assertTrue(len(after) == len(before) + 1)
        self.assertFalse(id in before)
        self.assertTrue(id in after)
        os.system(f"atrm {id}")
        
    def test_get_at_command(self):
        """Test 'at' queue command inspection"""
        
        id = utils.schedule_at_command(time.time() + 600, './test.sh')
        after = utils.get_at_queue()
        
        command = utils.get_at_command(id)
        with open('test.sh', 'r') as fh:
            self.assertEqual(command, fh.read())
            
        os.system(f"atrm {id}")


class utils_test_suite(unittest.TestSuite):
    def __init__(self):
        unittest.TestSuite.__init__(self)
        
        loader = unittest.TestLoader()
        self.addTests(loader.loadTestsFromTestCase(utils_tests)) 


if __name__ == '__main__':
    unittest.main()
