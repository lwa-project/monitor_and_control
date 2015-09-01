/* Astrnomical support functions for MCS/Executive */

#ifndef ME_ASTRO_H
#define ME_ASTRO_H

/* me_findjov.c */
extern void me_findjov(long int mjd, long int mpm, float *ra, float *dec);

/* me_findsol.c */
extern void me_findsol(long int mjd, long int mpm, float *ra, float *dec);

/* me_getaltaz.c */
extern void me_getaltaz(double ra, double dec, long int mjd, long int mpm, double lat, double lon, double *LAST, double *alt, double *az);

/* me_point_corr.c */
extern void me_point_corr(float fPCAxisTh, float fPCAxisPH, float fPCRot, double *alt, double *az);

/* me_precess.c */
extern void me_precess(long int mjd, long int mpm, float *ra, float *dec);

#endif // #ifndef ME_ASTRO_H