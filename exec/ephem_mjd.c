/* functions to manipulate the modified-julian-date used throughout xephem. */

#include <stdio.h>
#include <math.h>

#include "ephem_astro.h"

/* given a date in months, mn, days, dy, years, yr,
 * return the modified Julian date (number of days elapsed since 1900 jan 0.5),
 * *mjd.
 */
void
cal_mjd (int mn, double dy, int yr, double *mjp)
{
	static double last_mjd, last_dy;
	static int last_mn, last_yr;
	int b, d, m, y;
	long c;

	if (mn == last_mn && yr == last_yr && dy == last_dy) {
	    *mjp = last_mjd;
	    return;
	}

	m = mn;
	y = (yr < 0) ? yr + 1 : yr;
	if (mn < 3) {
	    m += 12;
	    y -= 1;
	}

	if (yr < 1582 || (yr == 1582 && (mn < 10 || (mn == 10 && dy < 15))))
	    b = 0;
	else {
	    int a;
	    a = y/100;
	    b = 2 - a + a/4;
	}

	if (y < 0)
	    c = (long)((365.25*y) - 0.75) - 694025L;
	else
	    c = (long)(365.25*y) - 694025L;

	d = (int)(30.6001*(m+1));

	*mjp = b + c + d + dy - 0.5;

	last_mn = mn;
	last_dy = dy;
	last_yr = yr;
	last_mjd = *mjp;
}

/* given the modified Julian date (number of days elapsed since 1900 jan 0.5,),
 * mj, return the calendar date in months, *mn, days, *dy, and years, *yr.
 */
void
mjd_cal (double mj, int *mn, double *dy, int *yr)
{
	static double last_mj, last_dy;
	static int last_mn, last_yr;
	double d, f;
	double i, a, b, ce, g;

	/* we get called with 0 quite a bit from unused epoch fields.
	 * 0 is noon the last day of 1899.
	 */
	if (mj == 0.0) {
	    *mn = 12;
	    *dy = 31.5;
	    *yr = 1899;
	    return;
	}

	if (mj == last_mj) {
	    *mn = last_mn;
	    *yr = last_yr;
	    *dy = last_dy;
	    return;
	}

	d = mj + 0.5;
	i = floor(d);
	f = d-i;
	if (f == 1) {
	    f = 0;
	    i += 1;
	}

	if (i > -115860.0) {
	    a = floor((i/36524.25)+.99835726)+14;
	    i += 1 + a - floor(a/4.0);
	}

	b = floor((i/365.25)+.802601);
	ce = i - floor((365.25*b)+.750001)+416;
	g = floor(ce/30.6001);
	*mn = (int)(g - 1);
	*dy = ce - floor(30.6001*g)+f;
	*yr = (int)(b + 1899);

	if (g > 13.5)
	    *mn = (int)(g - 13);
	if (*mn < 2.5)
	    *yr = (int)(b + 1900);
	if (*yr < 1)
	    *yr -= 1;

	last_mn = *mn;
	last_dy = *dy;
	last_yr = *yr;
	last_mj = mj;
}

/* given a mjd, return the year as a double. */
void
mjd_year (double mj, double *yr)
{
	static double last_mj, last_yr;
	int m, y;
	double d;
	double e0, e1;	/* mjd of start of this year, start of next year */

	if (mj == last_mj) {
	    *yr = last_yr;
	    return;
	}

	mjd_cal (mj, &m, &d, &y);
	if (y == -1) y = -2;
	cal_mjd (1, 1.0, y, &e0);
	cal_mjd (1, 1.0, y+1, &e1);
	*yr = y + (mj - e0)/(e1 - e0);

	last_mj = mj;
	last_yr = *yr;
}

/* given an mjd, truncate it to the beginning of the whole day */
double
mjd_day(double mj)
{
	return (floor(mj-0.5)+0.5);
}

/* given an mjd, return the number of hours past midnight of the whole day */
double
mjd_hr(double mj)
{
	return ((mj-mjd_day(mj))*24.0);
}

/* insure 0 <= *v < r.
 */
void
ephem_range (double *v, double r)
{
	*v -= r*floor(*v/r);
}

/* insure 0 <= ra < 2PI and -PI/2 <= dec <= PI/2. if dec needs 
 * complimenting, reflect ra too
 */
void
radecrange (double *ra, double *dec)
{
	if (*dec < -PI/2) {
	    *dec = -PI - *dec;
	    *ra += PI;
	} else if (*dec > PI/2) {
	    *dec = PI - *dec;
	    *ra += PI;
	}
	ephem_range (ra, 2*PI);
}
