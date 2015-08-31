/* This is stuff from astro.h in the ephem code that is needed for VSOP87. */
/* A lot of unneeded stuff was excluded */

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

#define SPD (24.0*3600.0)	/* seconds per day */