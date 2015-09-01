#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "ephem_astro.h"

/* given geocentric ecliptic longitude and latitude, lam and bet, of some object
 * and the longitude of the sun, lsn, find the elongation, el. this is the
 * actual angular separation of the object from the sun, not just the difference
 * in the longitude. the sign, however, IS set simply as a test on longitude
 * such that el will be >0 for an evening object <0 for a morning object.
 * to understand the test for el sign, draw a graph with lam going from 0-2*PI
 *   down the vertical axis, lsn going from 0-2*PI across the hor axis. then
 *   define the diagonal regions bounded by the lines lam=lsn+PI, lam=lsn and
 *   lam=lsn-PI. the "morning" regions are any values to the lower left of the
 *   first line and bounded within the second pair of lines.
 * all angles in radians.
 */
void
elongation (double lam, double bet, double lsn, double *el)
{
	*el = acos(cos(bet)*cos(lam-lsn));
	if (lam>lsn+PI || (lam>lsn-PI && lam<lsn)) *el = - *el;
}

/* apply relativistic light bending correction to ra/dec; stern
 *
 * The algorithm is from:
 * Mean and apparent place computations in the new IAU 
 * system. III - Apparent, topocentric, and astrometric 
 * places of planets and stars
 * KAPLAN, G. H.;  HUGHES, J. A.;  SEIDELMANN, P. K.;
 * SMITH, C. A.;  YALLOP, B. D.
 * Astronomical Journal (ISSN 0004-6256), vol. 97, April 1989, p. 1197-1210.
 *
 * This article is a very good collection of formulea for geocentric and
 * topocentric place calculation in general.  The apparent and
 * astrometric place calculation in this file currently does not follow
 * the strict algorithm from this paper and hence is not fully correct.
 * The entire calculation is currently based on the rotating EOD frame and
 * not the "inertial" J2000 frame.
 */

void
deflect (
double mjd1,		/* equinox */
double lpd, double psi,	/* heliocentric ecliptical long / lat */
double rsn, double lsn,	/* distance and longitude of sun */
double rho,		/* geocentric distance */
double *ra, double *dec)/* geocentric equatoreal */
{
	double hra, hdec;	/* object heliocentric equatoreal */
	double el;		/* HELIOCENTRIC elongation object--earth */
	double g1, g2;		/* relativistic weights */
	double u[3];		/* object geocentric cartesian */
	double q[3];		/* object heliocentric cartesian unit vect */
	double e[3];		/* earth heliocentric cartesian unit vect */
	double qe, uq, eu;	/* scalar products */
	int i;			/* counter */

#define G	1.32712438e20	/* heliocentric grav const; in m^3*s^-2 */
#define c	299792458.0	/* speed of light in m/s */

	elongation(lpd, psi, lsn-PI, &el);
	el = fabs(el);
	/* only continue if object is within about 10 deg around the sun,
	 * not obscured by the sun's disc (radius 0.25 deg) and farther away
	 * than the sun.
	 *
	 * precise geocentric deflection is:  g1 * tan(el/2)
	 *	radially outwards from sun;  the vector munching below
	 *	just applys this component-wise
	 *	Note:	el = HELIOCENTRIC elongation.
	 *		g1 is always about 0.004 arc seconds
	 *		g2 varies from 0 (highest contribution) to 2
	 */
	if (el<degrad(170) || el>degrad(179.75) || rho<rsn) return;

	/* get cartesian vectors */
	sphcart(*ra, *dec, rho, u, u+1, u+2);

	ecl_eq(mjd1, psi, lpd, &hra, &hdec);
	sphcart(hra, hdec, 1.0, q, q+1, q+2);

	ecl_eq(mjd1, 0.0, lsn-PI, &hra, &hdec);
	sphcart(hra, hdec, 1.0, e, e+1, e+2);

	/* evaluate scalar products */
	qe = uq = eu = 0.0;
	for(i=0; i<=2; ++i) {
	    qe += q[i]*e[i];
	    uq += u[i]*q[i];
	    eu += e[i]*u[i];
	}

	g1 = 2*G/(c*c*MAU)/rsn;
	g2 = 1 + qe;

	/* now deflect geocentric vector */
	g1 /= g2;
	for(i=0; i<=2; ++i)
	    u[i] += g1*(uq*e[i] - eu*q[i]);
	
	/* back to spherical */
	cartsph(u[0], u[1], u[2], ra, dec, &rho);	/* rho thrown away */
}