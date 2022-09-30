

#ifndef VOLPAK_C_H
#define VOLPAK_C_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define	NOMEAS	100			/* Max. number of measures  */
#define	NORELTS	(NOMEAS * 2 + 2)	/* No of elements in internal arrays. */


/*  Volpak Prototypes. */

#ifdef __cplusplus
extern "C" {
#endif

    double calcx(double rad, double p, double q, double a);
    void param(double h1, double r1, double h2, double r2, double h3, double r3, double *p, double *q, double *a);
    double radius(double x, double p, double q, double a);
    void nloid(void);
    void vpakinit(double h[], double d[], double tht, int noelts);
    double vhyper(double r1, double r2, double p, double q);
    double vpara(double r1, double r2, double p, double q);
    double calcv(double StumpRadi, double StumpHt);
    double condht(int index, double hl);
    double conhtd(int index, double rl);
    double vtm( void );
    double htd(double d1);
    double vold(double d1);
    double volh(double h1);

#ifdef __cplusplus
}
#endif


#endif
