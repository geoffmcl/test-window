// ap4_maths.hxx
#ifndef _ap4_maths_hxx_
#define _ap4_maths_hxx_

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* from unix math.h */
#endif

#define DEGREES_TO_RADIANS  (M_PI/180.0)
#define RADIANS_TO_DEGREES  (180.0/M_PI)

#define MAX_RADS    1.63312e+016

extern bool get_tri_centroid( int x1, int y1, int x2, int y2, int x3, int y3, Vector2d & v );
extern bool get_tri_centroid( Vector2d & A, Vector2d & B, Vector2d & C, Vector2d & v );

extern double GetAngle( Vector2d A, Vector2d B, Vector2d C );
extern double ATan2(double opp, double adj);
extern double CrossProductLength( Vector2d A, Vector2d B, Vector2d C );
extern double DotProduct( Vector2d A, Vector2d B, Vector2d C );

extern bool PointInTriangle( Vector2d p, Vector2d a, Vector2d b, Vector2d c );
extern bool PointInTriangle2( Vector2d P, Vector2d A, Vector2d B, Vector2d C );

extern double line_slope( Vector2d A, Vector2d B );
extern double DistToSegment( Vector2d p, Vector2d A, Vector2d B, Vector2d & N );

extern bool point_on_line1( Vector2d p, Vector2d A, Vector2d B );   // uses slope
extern bool point_on_line2( Vector2d p, Vector2d A, Vector2d B ); // NOT COMPLETED
extern bool point_on_line3( Vector2d p, Vector2d A, Vector2d B );   // uses DistToSegment

typedef struct tagISRES {
    bool intersect;
    double r,s;
    double x,y;
    bool para,colin;
    double eq1_num, eq1_den;
    double eq2_num, eq2_den;
}ISRES, * PISRES;
extern bool lines_intersect(Vector2d A, Vector2d B, Vector2d C, Vector2d D, PISRES pr);

#define  win_int(a)  ( a < 0.0 ) ? (int)(a - 0.5) : (int)(a + 0.5)

typedef struct tagDMM {
    double * pminx;
    double * pmaxx;
    double * pminy;
    double * pmaxy;
}DMM, * PDMM;
extern void set_dbl_max( double * pd );
extern void set_dbl_min( double * pd );
extern void set_dbl_max_min( PDMM pdmm );

#endif // #ifndef _ap4_maths_hxx_
// eof - ap4_maths.hxx
