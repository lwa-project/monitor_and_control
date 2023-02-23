#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define ME_LOAD_MAX_DAYS 60

int me_geteop(
               long int mjd,  /* (input) mean julian date */
               long int mpm,  /* (input) milliseconds past UTC midnight */
               double *xp,    /* (output) [arcsec] polar motion - x */
               double *yp,    /* (output) [arcsec] polar motion - y */
               double *dut    /* (output) [sec] UT1 - UTC */
              ) {

  int year, month, day, count = 0;
  double xp_error, yp_error, dut_error;
  long int list_mjd[ME_LOAD_MAX_DAYS];
  double list_xp[ME_LOAD_MAX_DAYS];
  double list_yp[ME_LOAD_MAX_DAYS];
  double list_dut[ME_LOAD_MAX_DAYS];
  
  int ret;
  struct stat fstats;
  time_t now = time(NULL);
  FILE *fh;
  char line[257];
  
  // See if we need to refresh the Earth orientation parameters file.  We refresh
  // if (a) there is no "eop.txt" file or (b) if the file is more than seven days
  // old.
  memset(&fstats, 0, sizeof(fstats));
  stat("state/eop.txt", &fstats);
  if( now - fstats.st_mtime > 7*86400 ) {
    ret = system("wget --quiet --no-use-server-timestamps -FO ./state/eop.txt https://datacenter.iers.org/data/latestVersion/6_BULLETIN_A_V2013_016.txt");
  }
  
  // Load in the contents of the file and save lists of MJD, x, y, and DUT
  fh = fopen("state/eop.txt", "r");
  while( fgets(&line[0], 256, fh) != NULL ) {
    ret = sscanf(&line[0], \
                 "%i %i %i %li %lf %lf %lf %lf %lf %lf", \
                 &year, &month, &day, &list_mjd[count], &list_xp[count], &xp_error, &list_yp[count], &yp_error, &list_dut[count], &dut_error);
    if( ret == 10 ) {
      // We got it all!
      count++;
    } else if( ret == 7 ) {
      // Predictions, rearrange
      list_dut[count] = list_yp[count];
      list_yp[count] = xp_error;
      count++;
    } else {
      // Nothing
      continue;
    }
    if( count == ME_LOAD_MAX_DAYS) {
      break;
    }
  }
  fclose(fh);
  
  // Set to the 50-year mean values in case we don't find a match in the file
  ret = 1;
  *xp = 0.035;
  *yp = 0.29;
  *dut = 0.0;
  
  // Loop throught the list of MJD to file two values that bracket the requested
  // MJD.  Once found, use the MPM value to interpolate.
  int i;
  double wgt;
  for(i=0; i<count-2; i++) {
    if( mjd >= list_mjd[i] && mjd < list_mjd[i+1] ) {
      wgt = mpm/1000.0/86400;
      *xp  = list_xp[i] *(1-wgt) + list_xp[i+1] *wgt;
      *yp  = list_yp[i] *(1-wgt) + list_yp[i+1] *wgt;
      *dut = list_dut[i]*(1-wgt) + list_dut[i+1]*wgt;
      ret = 0;
    }
  }
  
  return ret;
}
