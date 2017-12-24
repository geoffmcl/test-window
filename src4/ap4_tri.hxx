// ap4_tri.hxx
#ifndef _ap4_tri_hxx_
#define _ap4_tri_hxx_

static const double EPSILON = 0.0000000001f;

#define EQUAL(a,b)  (fabs(a - b) < EPSILON)
#define EQUALZ(a)  EQUAL(a,0.0)
#define SAME(a,b) (EQUAL(a.GetX(),b.GetX()) && EQUAL(a.GetY(),b.GetY()))

// COTD Entry submitted by John W. Ratcliff [jratcliff@verant.com]

// ** THIS IS A CODE SNIPPET WHICH WILL EFFICIEINTLY TRIANGULATE ANY
// ** POLYGON/CONTOUR (without holes) AS A STATIC CLASS.  THIS SNIPPET
// ** IS COMPRISED OF 3 FILES, TRIANGULATE.H, THE HEADER FILE FOR THE
// ** TRIANGULATE BASE CLASS, TRIANGULATE.CPP, THE IMPLEMENTATION OF
// ** THE TRIANGULATE BASE CLASS, AND TEST.CPP, A SMALL TEST PROGRAM
// ** DEMONSTRATING THE USAGE OF THE TRIANGULATOR.  THE TRIANGULATE
// ** BASE CLASS ALSO PROVIDES TWO USEFUL HELPER METHODS, ONE WHICH
// ** COMPUTES THE AREA OF A POLYGON, AND ANOTHER WHICH DOES AN EFFICENT
// ** POINT IN A TRIANGLE TEST.
// ** SUBMITTED BY JOHN W. RATCLIFF (jratcliff@verant.com) July 22, 2000

/**********************************************************************/
/************ HEADER FILE FOR TRIANGULATE.H ***************************/
/**********************************************************************/
#ifndef TRIANGULATE_H
#define TRIANGULATE_H
/*****************************************************************/
/** Static class to triangulate any contour/polygon efficiently **/
/** You should replace Vector2d with whatever your own Vector   **/
/** class might be.  Does not support polygons with holes.      **/
/** Uses STL vectors to represent a dynamic array of vertices.  **/
/** This code snippet was submitted to FlipCode.com by          **/
/** John W. Ratcliff (jratcliff@verant.com) on July 22, 2000    **/
/** I did not write the original code/algorithm for this        **/
/** this triangulator, in fact, I can't even remember where I   **/
/** found it in the first place.  However, I did rework it into **/
/** the following black-box static class so you can make easy   **/
/** use of it in your own code.  Simply replace Vector2d with   **/
/** whatever your own Vector implementation might be.           **/
/*****************************************************************/
#include <vector>  // Include STL vector class.

class Vector2d
{
public:
    Vector2d() { };
    ~Vector2d() { };
  Vector2d(double x,double y) { Set(x,y); };
  double GetX(void) const { return mX; };
  double GetY(void) const { return mY; };
  void  Set(double x,double y) { mX = x; mY = y; };
  friend bool operator == (const Vector2d & a, const Vector2d & b);  // p1 == p2?
  friend bool operator != (const Vector2d & a, const Vector2d & b);  // p1 == p2?
private:
  double mX;
  double mY;
};

inline bool operator == (const Vector2d & a, const Vector2d & b) { return SAME(a,b); }
inline bool operator != (const Vector2d & a, const Vector2d & b) { return !SAME(a,b); }

class Vector2dTri
{
public:
    Vector2dTri() { };
    ~Vector2dTri() { };
    Vector2dTri( Vector2d A, Vector2d B, Vector2d C )
    {
        Set(A,B,C);
    }
    void  Set(Vector2d A, Vector2d B, Vector2d C)
    {
        t1.Set( A.GetX(), A.GetY() );
        t2.Set( B.GetX(), B.GetY() );
        t3.Set( C.GetX(), C.GetY() );
    };
    Vector2d GetA(void) const { return t1; };
    Vector2d GetB(void) const { return t2; };
    Vector2d GetC(void) const { return t3; };
private:
    Vector2d t1;
    Vector2d t2;
    Vector2d t3;
};

class Vector2dLine
{
public:
    Vector2dLine() { };
    ~Vector2dLine() { };
    Vector2dLine( Vector2d A, Vector2d B )
    {
        Set(A,B);
    }
    void  Set(Vector2d A, Vector2d B)
    {
        v1.Set( A.GetX(), A.GetY() );
        v2.Set( B.GetX(), B.GetY() );
    };
    Vector2d GetA(void) const { return v1; };
    Vector2d GetB(void) const { return v2; };
private:
    Vector2d v1;
    Vector2d v2;
};

// Typedef an STL vector of vertices which are used to represent
// a polygon/contour and a series of triangles.
typedef std::vector< Vector2d > Vector2dVector;
typedef std::vector< Vector2dTri > Vector2dTriVector;

class Triangulate
{
public:  // triangulate a contour/polygon, places results in STL vector
  // as series of triangles.
  static bool Process(const Vector2dVector &contour,
                      Vector2dVector &result);  // compute area of a contour/polygon
  static double Area(const Vector2dVector &contour);  // decide if point Px/Py is inside triangle defined by
  // (Ax,Ay) (Bx,By) (Cx,Cy)
  static bool InsideTriangle(double Ax, double Ay,
                      double Bx, double By,
                      double Cx, double Cy,
                      double Px, double Py);

  // de-triangulate a set of triangles, creating a polygon, places 
  // results in STL vector as series of points in a 'contour'.
  static bool ReverseProcess(Vector2dVector &contour, Vector2dVector &result);

private:
  static bool Snip(const Vector2dVector &contour,size_t u,size_t v,size_t w,size_t n,size_t *V);

};

// helper function for ReverseProcess
extern int SharedEdgeLine2Tri( Vector2dLine line, Vector2dTri tri, Vector2d &p );
// finding a 'shared' edge of two triangles
extern int SharedEdge( Vector2dTri t1, Vector2dTri t2 );


#endif
// eof - triangulate.h
#endif // #ifndef _ap4_tri_hxx_
// eof - ap4_tri.hxx
