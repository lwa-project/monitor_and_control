
import unittest

import os
import time
import shutil
import subprocess


class SchedulerTests(unittest.TestCase):
    _sch_path = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
    
    def tearDown(self):
        # Force a shutdown just in case
        try:
            subprocess.check_call(['/bin/bash', 'ms_shutdown.sh'],
                                  cwd=self._sch_path,
                                  stdout=subprocess.DEVNULL,
                                  stderr=subprocess.DEVNULL)
        except subprocess.CalledProcessError:
            pass
            
        # Cleanup the reference ID state file
        try:
            os.unlink(os.path.join(self._sch_path, '.mcs_reference_id'))
        except OSError:
            pass
            
        # Cleanup the old mselog.txt file
        try:
            os.unlink(os.path.join(self._sch_path, 'mselog.txt'))
        except OSError:
            pass
            
        # Give the sockets a bit to sleep
        time.sleep(60)
        
    def _run_test(self, scriptname, expect_unsolicited=False):
        # Run the provided test script
        cmd_success = False
        cmd_error = ''
        output = ''
        try:
            output = subprocess.check_output(['/bin/bash', scriptname],
                                             timeout=120,
                                             cwd=self._sch_path,
                                             stderr=subprocess.DEVNULL,
                                             text=True)
            cmd_success = True
        except (subprocess.CalledProcessError, subprocess.TimeoutExpired) as e:
            cmd_error = str(e)
            
        if not cmd_success:
            print("Full command output:")
            for line in output.split('\n'):
                print(f"> {line}")
            print(f"Command error: {cmd_error}")
        self.assertTrue(cmd_success)
        
        # Count RPTs and unsolicited RPTs
        found_rpt = 0
        found_png = 0
        found_uns = 0
        with open(os.path.join(self._sch_path, 'mselog.txt'), 'r') as fh:
            for line in fh:
                if len(line) < 3:
                    continue
                    
                ## Parse the line
                try:
                    ymd, hms, mjd, mpm, _, ref, status, sid, cid, data = line.split(None, 9)
                except ValueError:
                    continue
                    
                if cid == 'PNG' and status == '3':
                    ## Ping
                    found_png += 1
                if cid == 'RPT' and status == '3':
                    ## Regular report
                    found_rpt += 1
                if cid == 'RPT' and status == '10':
                    ## unsolicited report
                    found_uns += 1
                    
        if (found_png == 0 and found_rpt == 0) or (expect_unsolicited and found_uns == 0):
            print("Full mselog.txt")
            with open(os.path.join(self._sch_path, 'mselog.txt'), 'r') as fh:
                for line in fh:
                    print(f"> {line}")
        self.assertTrue(found_png + found_rpt > 0)
        if expect_unsolicited:
            self.assertTrue(found_uns > 0)
            
    def test_test1(self):
        """
        Running test1.sh
        """
        
        self._run_test('test1.sh', expect_unsolicited=True)
        
    def test_test2(self):
        """
        Running test2.sh
        """
        
        self._run_test('test2.sh', expect_unsolicited=False)
        
    def test_test3(self):
        """
        Running test3.sh
        """
        
        self._run_test('test3.sh', expect_unsolicited=True)
        
    def test_test4(self):
        """
        Running test4.sh
        """
        
        self._run_test('test4.sh', expect_unsolicited=False)


class scheduler_test_suite(unittest.TestSuite):
    """A unittest.TestSuite class which contains all of the MCS scheduler
    tests."""
    
    def __init__(self):
        unittest.TestSuite.__init__(self)
        
        loader = unittest.TestLoader()
        self.addTests(loader.loadTestsFromTestCase(SchedulerTests))


if __name__ == '__main__':
    unittest.main()
