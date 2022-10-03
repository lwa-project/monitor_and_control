/* Astrnomical support functions for MCS/Executive */

#ifndef ME_ASTRO_H
#define ME_ASTRO_H

/* me_findlun.c */
extern void me_findlun(long int mjd, long int mpm, double lat, double lng, double elev, float *ra, float *dec, float *dist);

/* me_findjov.c */
extern void me_findjov(long int mjd, long int mpm, float *ra, float *dec, float *dist);

/* me_findsol.c */
extern void me_findsol(long int mjd, long int mpm, float *ra, float *dec, float *dist);

/* me_getaltaz.c */
extern void me_getaltaz(double ra, double dec, double dist, long int mjd, long int mpm, double lat, double lon, double elev, double *LAST, double *alt, double *az);

/* me_getlst.c */
extern void me_getlst(long int mjd, long int mpm, double lat, double lon, double *LAST);

/* me_point_corr.c */
extern void me_point_corr(float fPCAxisTh, float fPCAxisPH, float fPCRot, double *alt, double *az);

/* me_precess.c */
extern void me_precess(long int mjd, long int mpm, float *ra, float *dec);

#endif // #ifndef ME_ASTRO_H
