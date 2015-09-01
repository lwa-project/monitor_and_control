#include "ephem_astro.h"

static double gmst0 (double mj);

/* given a modified julian date, mj, and a universally coordinated time, utc,
 * return greenwich mean siderial time, *gst.
 * N.B. mj must be at the beginning of the day.
 */
void
utc_gst (double mj, double utc, double *gst)
{
	static double lastmj = -18981;
	static double t0;

	if (mj != lastmj) {
	    t0 = gmst0(mj);
	    lastmj = mj;
	}
	*gst = (1.0/SIDRATE)*utc + t0;
	ephem_range (gst, 24.0);
}

/* given a modified julian date, mj, and a greenwich mean siderial time, gst,
 * return universally coordinated time, *utc.
 * N.B. mj must be at the beginning of the day.
 */
void
gst_utc (double mj, double gst, double *utc)
{
	static double lastmj = -10000;
	static double t0;

	if (mj != lastmj) {
	    t0 = gmst0 (mj);
	    lastmj = mj;
	}
	*utc = gst - t0;
	ephem_range (utc, 24.0);
	*utc *= SIDRATE;
}

/* gmst0() - return Greenwich Mean Sidereal Time at 0h UT; stern
 */
static double
gmst0 (
double mj)	/* date at 0h UT in julian days since MJD0 */
{
	double T, x;

	T = ((int)(mj - 0.5) + 0.5 - J2000)/36525.0;
	x = 24110.54841 +
		(8640184.812866 + (0.093104 - 6.2e-6 * T) * T) * T;
	x /= 3600.0;
	ephem_range(&x, 24.0);
	return (x);
}
