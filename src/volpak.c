/*

Original FORTRAN source by Vanclay

Converted by Peter Gordon to C with strigent consistence checking done
See copy volpak.c-peter for his original conversion.

Modified:  Eric  18/02/00  math.h's M_PI, M_PI_4, etc used instead of 
				pi = 3.14159... hard code

Modified:  Eric  18/02/00  vhyper() routine test for slight negitive
				volume due to rounding error.
				Returns zero rather than bomb out.
				Note: does not occur with Linux gcc

Modified:  Eric  15/07/04  Added includes <stdlib.h> and <string.h>

Modified:  Eric  04/09/06  Increase max. hts to 100.
				Warn about too many hag:dia values.
				Improve error messages.
*/


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>

#define	NOMEAS	100			/* Max. number of measures  */
#define	NORELTS	(NOMEAS * 2 + 2)	/* No of elements in internal arrays. */

/*  Volpak Prototypes. */
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


//#define DEBUG

#ifdef DEBUG
#define MAIN
#endif

static int	numpts;	/* Highest element number + 1 in hr & r arrays. */
static double	sh = 0.15;	/* Default stump height. */
static double	sr;		/* Stump Radius. */
static double	p1, q1, a1;	/* Constants from function param. */
static double	hr[NORELTS];
static double	r[NORELTS];
static double	vpaktht;

/*  Calculate the height of the frustrum above the passed radius.
    This height + the height of the radius = theoretical height
    of the tree.
*/
double
calcx(double rad, double p, double q, double a)
/*
double	rad;	The radius of the frustrum.
double	p, q;	Descriptors of the shape of the frustrum.
double	a;	Test value.
*/
{
	double	x;
	if( a == 0.0)	/* ** ERROR STOP. ** */
	{
		fprintf(stderr,"volpak: calcx: a = 0\n");
		exit(1);
	}
	if(a < 0.0)	/* Concave Parabolic frustrum. */
		x = (p - rad) * rad / q;
	else		/* Convex hyperbolic frustrum. */
		x = p * rad / (q - rad);
	if(x >= 0.0 )
		return(x);
	else
	{
		fprintf(stderr,
			"volpak: calcx error: rad = %lf\np = %lf\nq = %lf\na = %lf\n",
				rad, p, q, a);
		exit(1);
	}
}
/*	**********************************************************************	*/
/*
    Calculates the parameters p & q, of a convex hyperbola or a
    concave parabola fitted around three points. "a" is a taper
    measurment.
*/
void
param(double h1, double r1, double h2, double r2, double h3, double r3,
		double *p, double *q, double *a)
{
	double	rdif;	/* Radius differences between measures. */
	double	t1, t2;		/* Tapers between heights. */

	*p = *q = *a = 0.0;
	if( (rdif = r1 - r2) <= 0.001) return;
	if((t1 = (h2 - h1) / rdif) <= 0.0 || t1 > 500.0) return;
	if((rdif = r2 - r3) <= 0.001) return;
	if((t2 = (h3 - h2) / rdif) <= 0.0 || t2 > 500.0) return;
	*a = t1 - t2;
	if( fabs(*a) >= 1.0 && *a > -300.0 )
	{	/* Convex Hyperbola. */
		if( *a >= 0.0 )
		{
			*q = (t1*r1 - t2*r3) / (*a);
			*p = (h3 - h1) / (r1 - r3) * (*q - r1) * (*q - r3) / (*q);
		}
		else	/* Concave Hyperbola. */
		{
			*q = fabs( (r1 - r3) / (*a) );
			*p = (h3 - h1) / (r1 - r3) * (*q) + (r1 + r3);
		}
	}
	else
		*a = 0.0;
}
/*	**********************************************************************	*/
/*
   Function to calculate the radius x metres from the top of the tree.
*/
double
radius(double x, double p, double q, double a)
{
	double	z;
	double	rad;
	if(a == 0.0 )
	{
		fprintf(stderr,"volpak: In radius: a equals 0.0. Aborting.\n");
		exit(1);
	}
	if( a < 0.0 )	/* Concave parabolic. */
	{
		if( (z = p * p - 4.0 * q * x) < 0.0)
		{
			fprintf(stderr,"volpak: In radius sqrt arg = %16.6e\n", z);
			exit(1);
		}
		rad = (p - sqrt(z)) / 2.0;
	}
	else	/* Convex parabolic. */
		rad = q * x / (x + p);
	if(rad >= 0.0 )
		return(rad);
	else
	{
		fprintf(stderr,"volpak: In radius: calcuated radius < 0.0\n");
		fprintf(stderr,"volpak: Height = %16.6e\np = %16.6e\nq = %16.6e\na = %16.6e\n",
				x, p, q, a);
		exit(1);
	}
}
/*	**********************************************************************	*/

/*  Calculates the stump radius and radius at ground level
    when the stump is neiloid in shape.
    If the stump height is > the height of the second measure
    on the stem, linear interpolation between the appropriate
    measusers is used.
*/
void
nloid()
{
/*
   Zero hyperbola & parabola constants to show that the neiloid model is
   being used.
*/

	double	expon = 2.0 /3.0;
	double	rtrans, dif;
	p1 = q1 = a1 = 0.0;
	rtrans = pow(r[1], expon);
	dif = (rtrans - pow(r[2], expon)) / (hr[2] - hr[1]);
	if( dif < 0.0 ) dif = 0.0;
/* Back transform the radii & calculate the radius at gl. */
	r[0] = pow( (rtrans + dif * hr[1]) , 1.5);
	if( sh == 0.0 )	/* NOTE: Test not in FORTRAN version. */
		sr = r[0];
	else
		sr = pow( (rtrans + dif * (hr[1] - sh)), 1.5);
}
/*	**********************************************************************	*/


/*
    Initialises the set of functions for a tree.
    It sets
    Mid point heights for midpoints between the second measure & the total
    height.
    Radius in metres for all measure points plus:
      the radius at ground level   hr[0].
      the stump radius             sr
      the radii for the midpoints between measures.
   p1, q1 & a1 are set as descriptors of the stump neloid.
*/
void
vpakinit( double h[], double d[], double tht, int noelts)
{
	double	temp;
	double	x;
	double	p, q, a;
	int	i;
	int	n;

/* Clear Arrays & tht for a new tree. */
	for(i = 0; i < NORELTS; ++i)
		hr[i] = r[i] = 0.0;
	vpaktht = 0.0;


#ifdef DEBUG
	fprintf(stderr, "vpakinit: Inputs\n");
	fprintf(stderr, "\t(i, hag, radius)\n");
	for(i = 0; i < noelts; i++){
		fprintf(stderr, "\t(%i, %.2f, %.6f)\n", i, h[i], d[i]/200);
	}
	fprintf(stderr, "\n");
#endif

	/* check if too many ht:dia pairs */

	if (noelts > NOMEAS) {
		fprintf(stderr,
			"volpak: vpakinit: too many ht:dia pairs: %d - max: %d\n",
			noelts, NOMEAS);
		exit(1);
	}

/*  Load the measure data into the arrays hr & r.
    Convert the diameters into radii in metres before loading.
*/
	for(i = 0, n = 1;  i < noelts && n < NORELTS; ++i, n +=2)
	{
		hr[n] = h[i];
		r[n] = d[i] / 200.0;
	}

	if( n > NORELTS )
		numpts = NORELTS;
	else
		numpts = n - 1;

	vpaktht = tht;

	if(vpaktht <= 0.0)	/* If no total height, decrease the No of points. */
		numpts = numpts - 2;
	else		/* Put the total height in arrays.  */
	{
		hr[numpts -1] = vpaktht;
		r[numpts -1] = 0.0;
		

		/* check total height to last HAG */

		if (vpaktht < cos(20.0) * hr[numpts-3]) {
			fprintf(stderr,
				"volpak: vpakinit: warn: Tot.Ht: %.1f < highest HAG: %.1f by 20` lean\n",
				vpaktht, hr[numpts-3]);
		}
	}


/*
	Insert the midpoint heights into the hr array.  Set first element to G.L.
	and second element to first measured radius.
*/
/*  Set the mid point heights between measures.  */
	for(i = 2; i < numpts; i +=2)
		hr[i] =  ( hr[i-1] + hr[i+1] ) / 2.0;
/*  If numpts < 6, the tree has two measures & no total height.
    If numpts is equal to 5, the tree has two measures & a total height.
*/

#ifdef DEBUG
	fprintf(stderr, "vpakinit: loaded volpak arrays\n");
	fprintf(stderr, "\t(i, hag, radius)\n");
	for(i = 0; i < numpts; i++){
		fprintf(stderr, "\t(%i, %.2f, %.6f)\n", i, hr[i], r[i]);
	}
	fprintf(stderr, "\n");
#endif


	if(numpts < 6 )
	{
		r[2] = ( r[1] + r[3] ) / 2.0;
		nloid();
		return;
	}

/*  Find the first mid point radius.  ( r[2])
    Also find the first estimate of the second mid point radius.   (temp).
*/
	param(hr[1],r[1], hr[3],r[3], hr[5],r[5], &p1, &q1, &a1);

#ifdef DEBUG
	fprintf(stderr, "vpakinit: (a1, p1, q1): (%.6f, %.6f, %.6f)\n\n", a1, p1, q1);
#endif

	if( a1 != 0.0)	/* Conic model. */
	{		/* Hyperbolic or parabolic model. */
		x = calcx(r[1], p1, q1, a1) + hr[1];
		r[2] = radius( x - hr[2], p1, q1, a1);
		temp = radius( x - hr[4], p1, q1, a1);
/* Calc the radius at G.L & the stump radius. */
		if( sh == 0.0 )	/* *** NOTE: NOT IN FORTRAN CODE. */
			sr = r[0] = radius(x, p1, q1, a1);
		else
		{
			if( p1 * p1 - 4.0 * q1 * x >= 0.0 && a1 <= 0.0)
			{
				r[0] = radius(x, p1, q1, a1);
				if( r[0] >= r[1] && r[0] < M_PI_2 )
					sr = radius(x - sh, p1, q1, a1);
				else
					nloid();	/* Calcs r[0] & stump rad by neloid method. */
			}
			else
				nloid();	/* Calcs r[0] & stump radius by neloid method. */
		}
	}
	else
	{
		temp = ( r[3] + r[5] ) / 2.0;
		r[2] = ( r[1] + r[3]) / 2.0;
		nloid();
	}
	if( numpts == 6 )
	{
		r[numpts-2] = temp;
		return;
	}
/*  Calculate the mid point radii for the remainder of the tree. */
	for( i = 3; i < numpts-4; i +=2)
	{
		param(hr[i],r[i], hr[i+2],r[i+2], hr[i+4],r[i+4], &p,&q,&a);
		if(a != 0.0)		/* Hyperbolic or parabolic model. */
		{
			x = calcx(r[i], p,q,a) + hr[i];
			r[i+1] = ( radius(x - hr[i+1], p, q, a) + temp) / 2.0;
			temp = radius(x - hr[i+3], p, q ,a);
		}
		else	/*  Conic model. */
		{
			r[i+1] = ( (r[i] + r[i+2]) / 2.0 + temp)/2.0;
			temp = ( r[i+2] + r[i+4]) / 2.0;
		}
	}
	r[numpts-2] = temp;	/* Mid point radius between last measure & tht. */

#ifdef DEBUG
	fprintf(stderr, "vpakinit: Final array\n");
	fprintf(stderr, "\t(i, hag, radius)\n");
	for(i = 0; i < numpts; i++){
		fprintf(stderr, "\t(%i, %.2f, %.6f)\n", i, hr[i], r[i]);
	}
	fprintf(stderr, "\n");
#endif


}
/*	**********************************************************************	*/
double
vpgetstph(void)
{
	return(sh);
}
/*	**********************************************************************	*/
double
vhyper(double r1, double r2, double p, double q)
{
	double	s, t, x, v = 0.0;

	s = q - r2;
	t = q - r1;
	x = s/t;
	if(x > 0.0)
		v = M_PI * p * ( pow(r1,3.0)/t - pow(r2,3.0)/s + (r1 - r2)
			* (r1 + r2 + 2.0 * q ) - 2.0 * q * q * log(s/t) );
	else
		v = -HUGE;

	if( v >= 0.0 )
		return(v);
	if( v > -0.0000000000001 )
		return(0.0);

	fprintf(stderr,
		"volpak: vhyper: Error.\nr1 = %.20f\nr2 = %.20f\np = %.20f\nq = %.20f\nv = %.20f\n",
		r1, r2, p, q, v);
	exit(1);
}
/*	**********************************************************************	*/
/*
	Calculate the volume of a concave parabolic frustrum.
*/
double
vpara(double r1, double r2, double p, double q)
{
	double v;

	v = M_PI / q * (p * (pow(r1,3.0) - pow(r2,3.0)) / 3.0
		- (pow(r1,4.0) - pow(r2,4.0)) / 2.0);

	if(v >= 0.0 )
		return(v);
	if( v > -0.0000000000001 )
		return(0.0);

	fprintf(stderr,
		"volpak: vpara: Error.\nr1 = %.20f\nr2 = %.20f\np = %.20f\nq = %.20f\nv = %.20f\n",
		r1, r2, p, q, v);
	exit(1);
}
/*	**********************************************************************	*/
/*
	Calculate the volume from ground level to either the radius limit radi,
	or the height limit stumph.  This height limit must be at or below the
	first measure point.
	NOTE: The FORTRAN code can change the values of radi and stumph.
	This code does not.
	The logic or the setout has been changed.
	The case of both a bad radi and a bad stumph being passed has been added.
*/
double
calcv( double radi, double stumph)
{
	if( radi == 0.0 && stumph == 0.0 )
		return(-HUGE);
/* If required, estimate a radi for use with vhyper or vpara. */
	if(radi == 0.0 && a1 != 0.0 )
	{
		double	x;
		x = calcx(r[0], p1, q1, a1) - stumph;
		radi = radius(x, p1, q1, a1);
	}
	if( a1 > 0.0 || (a1 < 0.0 && r[0] < p1 / 2.0) )	/* vhyper or vpara cond. */
	{
		if( a1 > 0.0 )
			return(vhyper(r[0], radi, p1, q1) );	/* Hyperbolic RETURN. */
		else
			return( vpara(r[0], radi, p1, q1) );	/* Parabolic RETURN. */
	}
	else	/* Neloid Calcs required. */
	{	/* Now either, a1 is == 0 or (a1 < 0 && r[0] >= p1 / 2.) */
		double	x, r0Lin, radiLin, expon, v;
		expon = 2.0 / 3.0;
		r0Lin = pow(r[0], expon);
		if( radi > 0.0 )
			radiLin = pow(radi, expon);
		else
			radiLin = r0Lin - stumph / hr[1] * (r0Lin - pow(r[1], expon));
		if( stumph == 0.0 )
			stumph = hr[1] * (r0Lin - radiLin) / (r0Lin - pow(r[1], expon));
		v = M_PI_4 * stumph * (r0Lin * r0Lin + radiLin * radiLin)
				* (r0Lin + radiLin);
		return(v);
	}
}
/*	**********************************************************************	*/
/*
	Sets a stump height.  The default is 0.15m.
*/
void
stph(double stp)
{
	sh = stp;
}
/*	**********************************************************************	*/
/*
	Calculate the total volume of the tree.
	Total volume includes the volume of the stump.
*/
double
volt( void )
{
	double	vt;
	int	i;
	vt = calcv( r[1], hr[1] );
	for(i = 3; i < numpts; i += 2)
		vt = vt + (M_PI / 6.0)
			* (r[i-2] * r[i-2] + 4.0 * r[i-1] * r[i-1] + r[i] * r[i])
			* (hr[i] - hr[i-2]);
	return(vt);
}
/*	**********************************************************************	*/
/*
	This subroutine added 18/3/93.
	Calculate Total Volume Above Ground - less the volume of a 15 cm stump.
	The method used is consistent with that used for vold and volh, but
	in NOT consistent with volt.
*/
double
vtm( void )
{
	double	vt, stumpv;
	int	i;
	stumpv = calcv(sr, sh);		/* Stump volume. */
/*
	This assumes that the stump height is lower than the first measure point.
	Calc the volume to the first measure point by calcv and subtract the stump.
*/
	vt = calcv(r[1], hr[1]) - stumpv;	/* Vol to first measure - stump vol. */
/*
	Calculate the volumes for each log, from the first measure point on the
	tree to the total height using Newtons method which is:
	V = 6.0/(SAbot + 4SAmid + SAtop)*Length
*/
	for(i = 3; i < numpts; i += 2)
		vt = vt + (M_PI / 6.0)
			* (r[i-2] * r[i-2] + 4.0 * r[i-1] * r[i-1] + r[i] * r[i])
			* (hr[i] - hr[i-2]);
	return(vt);
}
/*	**********************************************************************	*/
/*
	Function for conic interpolation in dht.
*/
double
condht( int index, double hl)
{
	int	i;
	double	dia, ratio;
	i = index;
	ratio = (r[i-2] - r[i]) / (hr[i] - hr[i-2]) * (hl - hr[i-2]);
	dia  = (r[i-2] - ratio) * 200.0;
	return(dia);
}
/*	**********************************************************************	*/
/*
	 Function to do conic interpolation for htd.
*/
double
conhtd( int index, double rl)
{
	int	i;
	double	height, ratio;
	i = index;
	ratio = (rl - r[i]) * (hr[i] - hr[i-2]) / (r[i-2] - r[i]);
	height = hr[i] - ratio;
	return( height );
}
/*	**********************************************************************	*/
/*
	For a given diameter in centimetres, the estimated height in metres
	is returned.  If the supplied diameter is inappropriate, i.e,
	it is > DGL or < the top measured diameter, -HUGE is returned.
*/
double
htd( double dl)
{
	int	i;
	double	height;	/* The height returned. */
	double	expon, r1lin, r2lin, rLlin, phtd, qhtd, ahtd;
	double rl;
	rl = dl / 200.0;
/* Check if the supplied diameter is within range. */
	if( rl < r[numpts-1] || rl > r[0] )
		return(-HUGE);
/*
	SPECIAL CASES
*/
	if ( rl == r[0] )	/* Supplied Diameter == DGL. */
		return(0.0);
	if( rl == r[numpts-1] )	/* Supplied dia == last meas dia. */
		return( hr[numpts-1]);
	if( rl == r[1] )	/* Passed dia == lowest measured diameter. */
		return( hr[1] );
	if(rl > r[1] )	/* Diameter is below the lowest measured point. */
	{
		if( a1 == 0.0 )
		{
/*	Neiloid Interpolation. */
			expon = 2.0 / 3.0;
			r1lin = pow(r[0], expon);
			rLlin = pow(rl, expon);
			r2lin = pow(r[1],  expon);
			height = hr[1] * (r1lin - rLlin) / (r1lin - r2lin);
		}
		else
/*	Hyperbolic or parabolic model used. */
			 height = calcx( r[0], p1, q1, a1) - calcx(rl, p1, q1, a1);
		return(height);
	}
/*
	 END OF SPECIAL CASES.
*/
	for(i = 3; i < numpts; i += 2)	/* Find the dia above the supplied. */
		if(r[i] <= rl)
			break;
/* r(i) is now <= rl. */
	if( rl == r[i] )
		return(hr[i]);
/* r[i] is the first measured diameter above the supplied diameter. */
	param(hr[i-2], r[i-2], hr[i-1], r[i-1],	hr[i], r[i], &phtd, &qhtd, &ahtd);
	if( ahtd == 0.0 || (ahtd < 0.0 && r[i-2] >= phtd/2.0 ) )
		height = conhtd( i, rl);	/* Conic Interpolation. */
	else
	{
/*	Hyperbolic or parabolic interpolation. */
		height = hr[i] + calcx(r[i], phtd, qhtd, ahtd)
			- calcx(rl, phtd, qhtd, ahtd);
		if( height < hr[i-2])	/* Apply Conic is answer is screwy. */
			height = conhtd(i, rl);
	}
	return( height);
}
/*	**********************************************************************	*/
/*
	For a passed height above ground, estimate the diameter at
	that height.  If the passed height is negative or greater than
	the maximum height in the height array, return -HUGE, otherwise
	return the diameter.
*/
double
dht( double hl)
{
	int	i;
	double dia;	/* Returned diameter. */
	double x, pdht, qdht, adht, ptst, expon, r1lin, r2lin, rLlin, rl;

/*	Check if the height is within the range of the tree. */
	if( hl < 0.0 || hl > hr[numpts-1] )
		return(-HUGE);
/*
	SPECIAL CASES.
*/
/*
	1. Ground level,  2. Supplied height equal to last measured height.
	(Usually the total height.)
	3. Height of first measure or  4. height below first measure.
*/
	if( hl == 0.0 )	/* Passed height is GL. */
		return(r[0] * 200.0);
	if( hl == hr[numpts-1] )	/* Passed height == last height. */
		return(r[numpts-1] * 200.0);
	if( hl == hr[1] )	/* Passed height == lowest measured height. */
		return( r[1] * 200.0);
	if( hl < hr[1] )
	{
/*	Height is below first measure point. */
		if( a1 != 0.0 )
		{
/*	Hyperbolic or parabolic model */
			x = calcx(r[0], p1, q1, a1) - hl;
			dia = radius(x, p1, q1, a1);
		}
		else
		{
/*	Neiloid Model. */
			expon = 2.0 / 3.0;
			r1lin = pow(r[0], expon);
			r2lin = pow(r[1], expon);
			rLlin = r1lin - hl / hr[1] * (r1lin - r2lin);
			dia = pow(rLlin, 1.5);
		}
		return( dia * 200.0);
	}
/*
	END OF SPECIAL CASES.
*/
/* Find first point above the passed height. */
	for(i = 3; i < numpts - 1; i += 2)
		if(hr[i]  > hl)
			break;
		if(hl == hr[i] )	/* Passed height == measured height. */
			return(r[i] * 200.0);
		param(hr[i-2], r[i-2], hr[i-1], r[i-1], hr[i], r[i],
			&pdht, &qdht, &adht);
		if( adht == 0.0 )
			return( condht(i, hl));	/* Return Conic Interpolation. */
/*	Hyperbolic or Parabolic model. */
		x = calcx(r[i], pdht, qdht, adht) - hl + hr[i];
		ptst = pdht * pdht - 4.0 * qdht * x;
		if( ptst < 0.0 && adht < 0.0 )
			return( condht(i, hl) );	/* Return Conic Interpolation. */
		rl = radius(x, pdht, qdht, adht);
		if( rl > r[i-2] || rl >= pdht/2.0 && i == 4 && adht < 0.0 )
			return( condht(i, hl));	/* Return Conic Interpolation. */
		dia = rl * 200;
		return(dia);
}
/*	**********************************************************************	*/
/*
	For a passed height above ground in metres, the volume in cubic metres
	is calculated from above the stump height to the height supplied.
	Returns:
		The volume in cubic metres
		or
		-HUGE if an error occurs.
*/
double
volh( double htl)
{
	double	x = 0.0, sumv, pvlh, qvlh, avlh, rb, hb, rl;
	int	i;
	if(htl < 0.0 || htl > hr[numpts-1])
		return(-HUGE);
	if( sh == 0.0 )	/* NOTE this test not in FORTRAN version. */
		sumv = 0.0;
	else
		sumv = -(calcv(sr, sh));
	if( htl == 0.0 )
		return( sumv);
	if(htl ==  hr[numpts-1])
		return( sumv + volt() );
	if( htl < hr[1] )
		return( sumv + calcv(x, htl) );
	if(htl == hr[1] )
		return(sumv + calcv(r[1], hr[1]) );
/* Supplied height is above first measure height. */
	sumv += calcv( r[1], hr[1]);

/* #ifdef DEBUG
	fprintf(stderr, "volh:  i: 0  vol = %.6f\n", sumv);
#endif */

	for(i = 3; i < numpts && htl >= hr[i]; i += 2)
	{
		sumv += (M_PI / 6.0) * (r[i-2] * r[i-2] + 4.0 * r[i-1] * r[i-1]
				+ r[i] * r[i]) * (hr[i] - hr[i-2]);
				
/* #ifdef DEBUG
	fprintf(stderr, "volh:  i: %i  vol = %.6f\n", (i-1)/2, sumv);
#endif */
		if(htl == hr[i])
			return(sumv);
	}
	param(hr[i-2], r[i-2], hr[i-1], r[i-1], hr[i], r[i], &pvlh, &qvlh, &avlh);
	if( avlh == 0.0 )
	{	/* Conic Interpolation. */
		rb = r[i-2];
		hb = hr[i-2];
		rl = rb - (rb - r[i]) / (hr[i] - hb) * (htl - hb);
		sumv += (M_PI / 3.0) * (rb * rb + rb * rl + rl * rl) * (htl -hb);
	}
	else
	{	/* Hyperbolic or parabolic interlopation. */
		x = calcx(r[i], pvlh, qvlh, avlh) - htl + hr[i];
		rl = radius(x, pvlh, qvlh, avlh);
		if(avlh > 0.0 )
			sumv += vhyper(r[i-2], rl, pvlh, qvlh);
		else
		{
			if( r[i-2] < pvlh/2.0)
				sumv += vpara(r[i-2], rl, pvlh, qvlh);
			else
			{	/* Conic Interpolation. */
				rb = r[i-2];
				hb = hr[i-2];
				rl = rb - (rb - r[i]) / (hr[i] - hb) * (htl - hb);
				sumv += (M_PI / 3.0) * (rb * rb + rb * rl + rl * rl) * (htl -hb);
			}
		}
	}

/* #ifdef DEBUG
	fprintf(stderr, "volh:  i: %i  vol = %.6f\n", (i-1)/2, sumv);
    fprintf(stderr, "volh:  section 'last': \na: %.6f  p: %.6f  q:%.6f\nfirst: (%.2f, %.6f) \nsecond: (%.2f, %.6f) \nthird: (%.2f, %.6f)\n", 
        		avlh, pvlh, qvlh, hr[i-2], r[i-2], hr[i-1], r[i-1], hr[i], r[i]);
    fprintf(stderr, "volh:  lastpoint: (%.2f, %.6f)\n", htl, rl);
#endif */

	return(sumv);
}
/*	**********************************************************************	*/
/*
	For a passed diameter in centimetres , the volume in cubic metres
	is calculated from above the stump height to the height of the 
	supplied diameter.
	Returns:
		The volume in cubic metres
		or
		-HUGE if an error occurs.
*/
double
vold( double dl )
{
	double	x, radi, sumv = 0.0, pvld, qvld, avld;
	int	i;
	x = 0.0;
	radi = dl/200.0;
	if( radi < 0.0 || radi > sr )
		return(-HUGE);	/* Error Return. */
/* ************************* NOTE: **********************************
*	The test of sh == 0.0  is different to the FORTRAN version.
******************************************************************** */
	if( sh == 0.0 )
		sumv = 0.0;
	else
		sumv = -(calcv(sr, sh));

/* Process Special Cases. */
	if( radi == r[0] )	/* Passed diameter == to DGL. */
		return( sumv);
	if( radi == r[1] )	/* Passed diameter == first measure point. */
		return(sumv + calcv( r[1], hr[1]));
	if( radi > r[1] )	/* Passed radii > that of first measured point. */
		return( sumv + calcv( radi, x));
	if( radi == r[numpts-1] )	/* Passed diameter occurs at total height. */
		return( sumv + volt() );
/* End of special cases. */
/* Passed diameter is above the first measured point. */
	sumv += calcv(r[1], hr[1]);
	for(i = 3; i < numpts && radi <= r[i]; i += 2)
	{
		sumv += ((M_PI / 6.0) * (r[i-2] * r[i-2] + 4.0 * r[i-1] * r[i-1]
			+ r[i] * r[i]) * (hr[i] - hr[i-2]));
		if(radi == r[i] )
			return(sumv);
	}
	param(hr[i-2], r[i-2], hr[i-1], r[i-1], hr[i], r[i], &pvld, &qvld, &avld);
	if( avld > 0.0 )
		sumv += vhyper(r[i-2], radi, pvld, qvld);
	else if( avld < 0.0 && r[i-2] < pvld / 2.0 )
			sumv += vpara(r[i-2], radi, pvld, qvld);
	else
		sumv += (M_PI / 3.0) * (pow(r[i-2],3) - pow(radi,3))
			/ (r[i-2] - r[i]) * (hr[i] - hr[i-2]);
	return(sumv);
}

#ifdef	DEBUG
/* #include	"treeread.h" */
main(int argc, char *argv[])
{
	/* FILE	*fi;
	tree_T	t;
	int	i;
	double	h7, d7, vt, v7, v7h;
	char	*usage = "Usage: volpak InputFileName > OutputFile\n";
	if(argc != 2)
	{
		fprintf(stderr, usage);
		exit(1);
	}
	if( (fi = fopen(argv[1], "r")) == (FILE *)0 )
	{
		fprintf(stderr,"volpak: main: Could not open %s\n", argv[1]);
		exit(1);
	}
	while( read_head( &t, fi) == 0 )
	{
		while(read_tree( &t, fi, 0) == 0 )
		{
			vpakinit(t.hag, t.dub, t.tht, t.nom);
			vt = volt();
			h7 = htd(7.0);
			if( h7 > 0.0 )
			{
				d7 = dht(h7);
				v7 = vold(7.0);
				v7h = volh(h7);
			}
			else
				h7 = v7h = d7 = v7 = 0.0;
			printf("%4d%6.2lf%6.2lf%7.4lf%7.4lf%7.4lf\n",
				t.iid, h7, d7, vt, v7h, v7);
		}
	} */

/* 	double hr[6] = {0.2, 0.5, 1.3, 4.7, 7.2, 10.3};
	double d[6] = {17.5, 16.4, 14.2, 8.7, 4.8, 0}; */
	double hr[13] = {0.2, 0.5, 1.3, 5, 8, 11.1, 13.2, 17.2, 20, 21.2, 24.3};
	double d[13] = {21.2, 22.3, 18.7, 15.5, 13.6, 11.7, 11, 7.7, 5.2, 2.8, 0.0};
	double tht, vtotal;
	int noelts;

	tht = 24.3;


	noelts = 11;
	fprintf(stderr, "Volpak Tree (vpakinit):\n", noelts);

	vpakinit(hr, d, tht, noelts);

	fprintf(stderr, "vtm(): %.6f\n", vtm());

	fprintf(stderr, "volh(3.0): %.6f\n", volh(3.0));
	fprintf(stderr, "volh(6.0): %.6f\n", volh(6.0));

	fprintf(stderr, "vold(15.0): %.6f\n", vold(15.0));
	fprintf(stderr, "vold(7.0): %.6f\n", vold(7.0));

	fprintf(stderr, "\n");

}
#endif
