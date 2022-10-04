/* This is stuff from astro.h in the ephem code that is needed for VSOP87. */
/* A lot of unneeded stuff was excluded */

#ifndef EPHEM_ASTRO_H  /* this keeps this header from getting rolled in more than once */
#define EPHEM_ASTRO_H

#include <stdio.h>

#ifndef PI
#define	PI		3.141592653589793
#endif

/* conversions among hours (of ra), degrees and radians. */
#define	degrad(x)	((x)*PI/180.)
#define	raddeg(x)	((x)*180./PI)
#define	hrdeg(x)	((x)*15.)
#define	deghr(x)	((x)/15.)
#define	hrrad(x)	degrad(hrdeg(x))
#define	radhr(x)	deghr(raddeg(x))

/* ratio of from synodic (solar) to sidereal (stellar) rate */
#define	SIDRATE		.9972695677

/* manifest names for planets.
 * N.B. must coincide with usage in pelement.c and plans.c.
 * N.B. only the first 8 are valid for use with plans().
 */
typedef enum {
    MERCURY,
    VENUS,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    NEPTUNE,
    PLUTO,
    SUN,
    MOON,
    NOBJ	/* total number of basic objects */
} PLCode;

#define MJD0  2415020.0
#define J2000 (2451545.0 - MJD0)      /* yes, 2000 January 1 at 12h */

#define SPD  (24.0*3600.0)	/* seconds per day */
#define MAU  (1.4959787e11)	/* m / au */
#define ERAD (6.37816e6)		/* earth equitorial radius, m */

/* ephem_aberration.c */
extern void ab_ecl (double m, double lsn, double *lam, double *bet);
extern void ab_eq (double m, double lsn, double *ra, double *dec);

/* ephem_deflect.c */
extern void elongation (double lam, double bet, double lsn, double *el);
extern void deflect (double mjd1, double lpd, double psi, double rsn, double lsn, double rho, double *ra, double *dec);

/* deltat.c */
extern double deltat (double m);

/* ephem_eq_ecl.c */
extern void eq_ecl (double m, double ra, double dec, double *lt,double *lg);
extern void ecl_eq (double m, double lt, double lg, double *ra,double *dec);

/* ephem_mjd.c */
extern void cal_mjd (int mn, double dy, int yr, double *m);
extern void mjd_cal (double m, int *mn, double *dy, int *yr);
extern void mjd_year (double m, double *yr);
extern double mjd_day (double jd);
extern double mjd_hr (double jd);
extern void ephem_range (double *v, double r);
extern void radecrange (double *ra, double *dec);

/* ephem_moon.c */
void moon (double m, double *lam, double *bet, double *rho, double *msp, double *mdp);

/* ephem_nutation.c */
extern void nutation (double m, double *deps, double *dpsi);
extern void nut_eq (double m, double *ra, double *dec);

/* ephem_obliq.c */
extern void obliquity (double m, double *eps);

/* ephem_parallax.c */
extern void ta_par (double tha, double tdec, double phi, double ht, double *rho, double *aha, double *adec);

/* ephem_precess.c */
extern void precess (double mjd1, double mjd2, double *ra, double *dec);

/* ephem_sphcart.c */
extern void sphcart (double l, double b, double r, double *x, double *y, double *z);
extern void cartsph (double x, double y, double z, double *l, double *b, double *r);

/* ephem_sun.c */
extern void sunpos (double m, double *lsn, double *rsn, double *bsn);

/* ephem_utc_gst.c */
extern void utc_gst (double m, double utc, double *gst);
extern void gst_utc (double m, double gst, double *utc);

/* ephem_vsop87.c */
extern int vsop87 (double m, int obj, double prec, double *ret);

#endif // #ifndef EPHEM_ASTRO_H 
