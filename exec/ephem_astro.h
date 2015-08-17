
/* This is stuff from astro.h in the ephem code that is needed for VSOP87. */
/* A lot of unneeded stuff was excluded */

#ifndef PI
#define	PI		3.141592653589793
#endif

/* conversions among hours (of ra), degrees and radians. */
#define	degrad(x)	((x)*PI/180.)

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
