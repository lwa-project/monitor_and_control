#include "sofa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <curl/curl.h>

int iauDat(int iy, int im, int id, double fd, double *deltat)
/*
**  - - - - - - -
**   i a u D a t
**  - - - - - - -
**
**  For a given UTC date, calculate Delta(AT) = TAI-UTC.
**
**     :------------------------------------------:
**     :                                          :
**     :                 IMPORTANT                :
**     :                                          :
**     :  A new version of this function must be  :
**     :  produced whenever a new leap second is  :
**     :  announced.  There are four items to     :
**     :  change on each such occasion:           :
**     :                                          :
**     :  1) A new line must be added to the set  :
**     :     of statements that initialize the    :
**     :     array "changes".                     :
**     :                                          :
**     :  2) The constant IYV must be set to the  :
**     :     current year.                        :
**     :                                          :
**     :  3) The "Latest leap second" comment     :
**     :     below must be set to the new leap    :
**     :     second date.                         :
**     :                                          :
**     :  4) The "This revision" comment, later,  :
**     :     must be set to the current date.     :
**     :                                          :
**     :  Change (2) must also be carried out     :
**     :  whenever the function is re-issued,     :
**     :  even if no leap seconds have been       :
**     :  added.                                  :
**     :                                          :
**     :  Latest leap second:  2016 December 31   :
**     :                                          :
**     :__________________________________________:
**
**  This function is part of the International Astronomical Union's
**  SOFA (Standards Of Fundamental Astronomy) software collection.
** 
**  This function has been modified to refresh the list of leap 
**  seconds from the internet every seven days.
**
**  Status:  user-replaceable support function.
**
**  Given:
**     iy     int      UTC:  year (Notes 1 and 2)
**     im     int            month (Note 2)
**     id     int            day (Notes 2 and 3)
**     fd     double         fraction of day (Note 4)
**
**  Returned:
**     deltat double   TAI minus UTC, seconds
**
**  Returned (function value):
**            int      status (Note 5):
**                       1 = dubious year (Note 1)
**                       0 = OK
**                      -1 = bad year
**                      -2 = bad month
**                      -3 = bad day (Note 3)
**                      -4 = bad fraction (Note 4)
**                      -5 = internal error (Note 5)
**
**  Notes:
**
**  1) UTC began at 1960 January 1.0 (JD 2436934.5) and it is improper
**     to call the function with an earlier date.  If this is attempted,
**     zero is returned together with a warning status.
**
**     Because leap seconds cannot, in principle, be predicted in
**     advance, a reliable check for dates beyond the valid range is
**     impossible.  To guard against gross errors, a year five or more
**     after the release year of the present function (see the constant
**     IYV) is considered dubious.  In this case a warning status is
**     returned but the result is computed in the normal way.
**
**     For both too-early and too-late years, the warning status is +1.
**     This is distinct from the error status -1, which signifies a year
**     so early that JD could not be computed.
**
**  2) If the specified date is for a day which ends with a leap second,
**     the TAI-UTC value returned is for the period leading up to the
**     leap second.  If the date is for a day which begins as a leap
**     second ends, the TAI-UTC returned is for the period following the
**     leap second.
**
**  3) The day number must be in the normal calendar range, for example
**     1 through 30 for April.  The "almanac" convention of allowing
**     such dates as January 0 and December 32 is not supported in this
**     function, in order to avoid confusion near leap seconds.
**
**  4) The fraction of day is used only for dates before the
**     introduction of leap seconds, the first of which occurred at the
**     end of 1971.  It is tested for validity (0 to 1 is the valid
**     range) even if not used;  if invalid, zero is used and status -4
**     is returned.  For many applications, setting fd to zero is
**     acceptable;  the resulting error is always less than 3 ms (and
**     occurs only pre-1972).
**
**  5) The status value returned in the case where there are multiple
**     errors refers to the first error detected.  For example, if the
**     month and day are 13 and 32 respectively, status -2 (bad month)
**     will be returned.  The "internal error" status refers to a
**     case that is impossible but causes some compilers to issue a
**     warning.
**
**  6) In cases where a valid result is not available, zero is returned.
**
**  References:
**
**  1) For dates from 1961 January 1 onwards, the expressions from the
**     file ftp://maia.usno.navy.mil/ser7/tai-utc.dat are used.
**
**  2) The 5ms timestep at 1961 January 1 is taken from 2.58.1 (p87) of
**     the 1992 Explanatory Supplement.
**
**  Called:
**     iauCal2jd    Gregorian calendar to JD
**
**  This revision:  2022 Oct 26
**
**  SOFA release 2021-05-12
**
**  Copyright (C) 2021 IAU SOFA Board.  See notes at end.
*/
{
/* Release year for this version of iauDat */
/* (to be updated after we download the leapsec.txt file) */
   int IYV = 2021;

/* Reference dates (MJD) and drift rates (s/day), pre leap seconds */
   static const double drift[][2] = {
      { 37300.0, 0.0012960 },
      { 37300.0, 0.0012960 },
      { 37300.0, 0.0012960 },
      { 37665.0, 0.0011232 },
      { 37665.0, 0.0011232 },
      { 38761.0, 0.0012960 },
      { 38761.0, 0.0012960 },
      { 38761.0, 0.0012960 },
      { 38761.0, 0.0012960 },
      { 38761.0, 0.0012960 },
      { 38761.0, 0.0012960 },
      { 38761.0, 0.0012960 },
      { 39126.0, 0.0025920 },
      { 39126.0, 0.0025920 }
   };

/* Number of Delta(AT) expressions before leap seconds were introduced */
   enum { NERA1 = (int) (sizeof drift / sizeof (double) / 2) };

/* Dates and Delta(AT)s */
   typedef struct {
      int iyear, month;
      double delat;
   } dat_change;
   dat_change *changes = (dat_change*) malloc(256*sizeof(dat_change));
   
   int count = 0;
   changes[count].iyear = 1960; changes[count].month =  1; changes[count++].delat = 1.4178180;
   changes[count].iyear = 1961; changes[count].month =  1; changes[count++].delat = 1.4228180;
   changes[count].iyear = 1961; changes[count].month =  8; changes[count++].delat = 1.3728180;
   changes[count].iyear = 1962; changes[count].month =  1; changes[count++].delat = 1.8458580;
   changes[count].iyear = 1963; changes[count].month = 11; changes[count++].delat = 1.9458580;
   changes[count].iyear = 1964; changes[count].month =  1; changes[count++].delat = 3.2401300;
   changes[count].iyear = 1964; changes[count].month =  4; changes[count++].delat = 3.3401300;
   changes[count].iyear = 1964; changes[count].month =  9; changes[count++].delat = 3.4401300;
   changes[count].iyear = 1965; changes[count].month =  1; changes[count++].delat = 3.5401300;
   changes[count].iyear = 1965; changes[count].month =  3; changes[count++].delat = 3.6401300;
   changes[count].iyear = 1965; changes[count].month =  7; changes[count++].delat = 3.7401300;
   changes[count].iyear = 1965; changes[count].month =  9; changes[count++].delat = 3.8401300;
   changes[count].iyear = 1966; changes[count].month =  1; changes[count++].delat = 4.3131700;
   changes[count].iyear = 1968; changes[count].month =  2; changes[count++].delat = 4.2131700;
   
   int ret, j;
   struct stat fstats;
   time_t now = time(NULL);
   FILE *fh;
   char line[257];

/* Refresh the leapsec.txt file */
   memset(&fstats, 0, sizeof(fstats));
   stat("state/leapsec.txt", &fstats);
   if( now - fstats.st_mtime > 7*86400 ) {
     // cURL setup
     CURL *curl_handle;
     curl_global_init(CURL_GLOBAL_ALL);
     curl_handle = curl_easy_init();
     if( curl_handle == NULL ) {
       fprintf(stderr, "Cannot create cURL handle: curl_easy_init() returned NULL");
       curl_global_cleanup();
       return -5;
     }
     curl_easy_setopt(curl_handle, CURLOPT_URL, "https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat");
     curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
     
     // Download the file
     fh = fopen("./state/leapsec.txt.temp", "w");
     if( fh == NULL ) {
       perror("Cannot open leap second temporary file for writing");
       curl_easy_cleanup(curl_handle);
       curl_global_cleanup();
       return -5;
     }
     curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fh);
     CURLcode res = curl_easy_perform(curl_handle);
     if (res == CURLE_OK) {
       // Great, it worked
       fclose(fh);
       rename("./state/leapsec.txt.temp", "./state/leapsec.txt");
     } else {
       // Boo, it didn't
       fclose(fh);
       remove("./state/leapsec.txt");
     }
     
     // cURL cleanup
     curl_easy_cleanup(curl_handle);
     curl_global_cleanup();
   }

/* Update the value of IYV */
   if( stat("state/leapsec.txt", &fstats) != 0 ) {
     perror("Cannot stat leap second file");
     return -5;
   }
   struct tm *now_gm = gmtime(&(fstats.st_mtime));
   IYV = now_gm->tm_year + 1900;
   
/* Load in leadpsec.txt */
   int entry_year, entry_month, entry_day, entry_deltat;
   double entry_mjd;

   fh = fopen("state/leapsec.txt", "r");
   if( fh == NULL ) {
     perror("Cannot open leap second file for reading");
     return -5;
   }
   while( fgets(&line[0], 256, fh) != NULL ) {
     ret = sscanf(&line[0], \
                  "%lf %i %i %i %i", \
                  &entry_mjd, &entry_day, &entry_month, &entry_year, &entry_deltat);
     if( ret == 5 ) {
       changes[count].iyear = entry_year;
       changes[count].month = entry_month;
       changes[count++].delat = (double) entry_deltat;
     }
     if( count == 256 ) {
       j = 1;
       break;
     }
   }
   fclose(fh);

/* Number of Delta(AT) changes */
   int NDAT = count;

/* Miscellaneous local variables */
   int i, m;
   double da, djm0, djm;


/* Initialize the result to zero. */
   *deltat = da = 0.0;

/* If invalid fraction of a day, set error status and give up. */
   if (fd < 0.0 || fd > 1.0) return -4;

/* Convert the date into an MJD. */
   j = iauCal2jd(iy, im, id, &djm0, &djm);

/* If invalid year, month, or day, give up. */
   if (j < 0) return j;

/* If pre-UTC year, set warning status and give up. */
   if (iy < changes[0].iyear) return 1;

/* If suspiciously late year, set warning status but proceed. */
   if (iy > IYV + 5) j = 1;

/* Combine year and month to form a date-ordered integer... */
   m = 12*iy + im;

/* ...and use it to find the preceding table entry. */
   for (i = NDAT-1; i >=0; i--) {
      if (m >= (12 * changes[i].iyear + changes[i].month)) break;
   }

/* Prevent underflow warnings. */
   if (i < 0) return -5;

/* Get the Delta(AT). */
   da = changes[i].delat;

/* If pre-1972, adjust for drift. */
   if (i < NERA1) da += (djm + fd - drift[i][0]) * drift[i][1];

/* Return the Delta(AT) value. */
   *deltat = da;

/* Cleanup. */
   free(changes);

/* Return the status. */
   return j;

/* Finished. */

/*----------------------------------------------------------------------
**
**  Copyright (C) 2018
**  Standards Of Fundamental Astronomy Board
**  of the International Astronomical Union.
**
**  =====================
**  SOFA Software License
**  =====================
**
**  NOTICE TO USER:
**
**  BY USING THIS SOFTWARE YOU ACCEPT THE FOLLOWING SIX TERMS AND
**  CONDITIONS WHICH APPLY TO ITS USE.
**
**  1. The Software is owned by the IAU SOFA Board ("SOFA").
**
**  2. Permission is granted to anyone to use the SOFA software for any
**     purpose, including commercial applications, free of charge and
**     without payment of royalties, subject to the conditions and
**     restrictions listed below.
**
**  3. You (the user) may copy and distribute SOFA source code to others,
**     and use and adapt its code and algorithms in your own software,
**     on a world-wide, royalty-free basis.  That portion of your
**     distribution that does not consist of intact and unchanged copies
**     of SOFA source code files is a "derived work" that must comply
**     with the following requirements:
**
**     a) Your work shall be marked or carry a statement that it
**        (i) uses routines and computations derived by you from
**        software provided by SOFA under license to you; and
**        (ii) does not itself constitute software provided by and/or
**        endorsed by SOFA.
**
**     b) The source code of your derived work must contain descriptions
**        of how the derived work is based upon, contains and/or differs
**        from the original SOFA software.
**
**     c) UNLIKE OTHER SOFA SOFTWARE, WHICH IS STRICTLY "READ ONLY",
**        USERS ARE PERMITTED TO REPLACE THIS FUNCTION WITH ONE USING
**        THE SAME NAME BUT DIFFERENT CODE.  This is to allow use of
**        locally supported mechanisms for keeping track of leap
**        seconds, perhaps file or network based.  It avoids the need
**        for applications to be relinked periodically in order to pick
**        up SOFA updates.
**
**     d) The origin of the SOFA components of your derived work must
**        not be misrepresented;  you must not claim that you wrote the
**        original software, nor file a patent application for SOFA
**        software or algorithms embedded in the SOFA software.
**
**     e) These requirements must be reproduced intact in any source
**        distribution and shall apply to anyone to whom you have
**        granted a further right to modify the source code of your
**        derived work.
**
**     Note that, as originally distributed, the SOFA software is
**     intended to be a definitive implementation of the IAU standards,
**     and consequently third-party modifications are discouraged.  All
**     variations, no matter how minor, must be explicitly marked as
**     such, as explained above.
**
**  4. You shall not cause the SOFA software to be brought into
**     disrepute, either by misuse, or use for inappropriate tasks, or
**     by inappropriate modification.
**
**  5. The SOFA software is provided "as is" and SOFA makes no warranty
**     as to its use or performance.   SOFA does not and cannot warrant
**     the performance or results which the user may obtain by using the
**     SOFA software.  SOFA makes no warranties, express or implied, as
**     to non-infringement of third party rights, merchantability, or
**     fitness for any particular purpose.  In no event will SOFA be
**     liable to the user for any consequential, incidental, or special
**     damages, including any lost profits or lost savings, even if a
**     SOFA representative has been advised of such damages, or for any
**     claim by any third party.
**
**  6. The provision of any version of the SOFA software under the terms
**     and conditions specified herein does not imply that future
**     versions will also be made available under the same terms and
**     conditions.
*
**  In any published work or commercial product which uses the SOFA
**  software directly, acknowledgement (see www.iausofa.org) is
**  appreciated.
**
**  Correspondence concerning SOFA software should be addressed as
**  follows:
**
**      By email:  sofa@ukho.gov.uk
**      By post:   IAU SOFA Center
**                 HM Nautical Almanac Office
**                 UK Hydrographic Office
**                 Admiralty Way, Taunton
**                 Somerset, TA1 2DN
**                 United Kingdom
**
**--------------------------------------------------------------------*/

}
