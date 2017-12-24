// ap4_maths.cxx

#include "testap4.hxx"
#include <limits>

#define M_DBL_MAX   99999999.9
#define M_DBL_MIN   -M_DBL_MAX

//minx = DBL_MAX;
//maxx = DBL_MIN;
//miny = DBL_MAX;
//maxy = DBL_MIN;
//minx = M_DBL_MAX;
//maxx = M_DBL_MIN;
//miny = M_DBL_MAX;
//maxy = M_DBL_MIN;

void set_dbl_max( double * pd )
{ 
    double d = M_DBL_MAX;
    *pd = d;
}

void set_dbl_min( double * pd )
{
    double d = M_DBL_MIN;
    *pd = d;
}

void set_dbl_max_min( PDMM pdmm )
{
    set_dbl_max( pdmm->pminx );
    set_dbl_min( pdmm->pmaxx );
    set_dbl_max( pdmm->pminy );
    set_dbl_min( pdmm->pmaxy );
}

/* =================================================================
from : http://www.cgafaq.info/wiki/Intersecting_line_segments_(2D)
Let A,B,C,D be 2-space position vectors. Then the directed line 
segments AB and CD are given by: 

AB = A+r(B-A), r ? [0,1] 
CD = C+s(D-C), s ? [0,1] 
If AB and CD intersect, then 

A+r(B-A)=C+s(D-C), or 
Ax+r(Bx-Ax)=Cx+s(Dx-Cx) 
Ay+r(By-Ay)=Cy+s(Dy-Cy) for some r,s ? [0,1] 
Solving the above for r and s yields 

    (Ay-Cy)(Dx-Cx)-(Ax-Cx)(Dy-Cy)
r = -----------------------------  (eqn 1)
    (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)

    (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
s = -----------------------------  (eqn 2)
    (Bx-Ax)(Dy-Cy)-(By-Ay)(Dx-Cx)

Let P be the position vector of the intersection point, then 

P  = A+ r(B-A) or 
Px = Ax + r(Bx-Ax) 
Py = Ay + r(By-Ay) 
By examining the values of r and s, you can also determine some
other limiting conditions: 

If 0 <= r <= 1 and 0 <= s <= 1, intersection exists 

r < 0 or r > 1 or s < 0 or s > 1 line segments do not intersect 

If the denominator in eqn 1 is zero, AB and CD are parallel 

If the numerator in eqn 1 is also zero, AB and CD are collinear. 

If they are collinear, then the segments may be projected to 
the x- or y-axis, and overlap of the projected intervals checked. 

If the intersection point of the two lines are needed (lines 
in this context mean infinite lines) regardless whether the two 
line segments intersect, then 

If r > 1, P is located on extension of AB 
If r < 0, P is located on extension of BA 
If s > 1, P is located on extension of CD 
If s < 0, P is located on extension of DC 
Also note that the denominators of eqn 1 and 2 are identical

   ================================================================= */

bool lines_intersect(Vector2d A, Vector2d B, Vector2d C, Vector2d D, PISRES pr)
{
    bool result = false;
    bool d2_z = false;
    bool d4_z = false;
    double Ax,Ay,Bx,By,Cx,Cy,Dx,Dy;
    //    ((Ay-Cy)*(Dx-Cx))-((Ax-Cx)*(Dy-Cy))
    //r = -----------------------------  (eqn 1)
    //    ((Bx-Ax)*(Dy-Cy))-((By-Ay)*(Dx-Cx))
    //    ((Ay-Cy)*(Bx-Ax))-((Ax-Cx)*(By-Ay))
    //s = -----------------------------  (eqn 2)
    //    ((Bx-Ax)*(Dy-Cy))-((By-Ay)*(Dx-Cx))
    Ax = A.GetX();
    Ay = A.GetY();

    Bx = B.GetX();
    By = B.GetY();

    Cx = C.GetX();
    Cy = C.GetY();

    Dx = D.GetX();
    Dy = D.GetY();

    //r = -----------------------------  (eqn 1)
    double d1 = ((Ay-Cy)*(Dx-Cx)) - ((Ax-Cx)*(Dy-Cy));
    double d2 = ((Bx-Ax)*(Dy-Cy)) - ((By-Ay)*(Dx-Cx));
    //s = -----------------------------  (eqn 2)
    double d3 = ((Bx-Ax)*(Dy-Cy)) - ((By-Ay)*(Dx-Cx));
    double d4 = ((Bx-Ax)*(Dy-Cy)) - ((By-Ay)*(Dx-Cx));

    if (pr) {
        pr->colin = false;
        pr->para  = false;
        // An example is 3/4, 
        // in which the numerator, 3, tells us that the fraction represents 3 equal parts, 
        // and the denominator, 4, tells us that 4 parts make up a whole.
        // If the denominator in eqn 1 is zero, AB and CD are parallel 
        // If the numerator in eqn 1 is also zero, AB and CD are collinear. 
        if ( EQUALZ(d2) ) {
            if ( EQUALZ(d1) )
                pr->colin = true;   // both are zero
            else
                pr->para = true;
        }
    }

    double r = 0.0;
    if ((d2 > 0.0) || (d2 < 0.0))
        r = d1 / d2;
    else {
        //printf("d2 is ZERO - parallel\n");
        d2_z = true;    // lines are parallel
    }
    double s = 0.0;
    if ((d4 > 0.0) || (d4 < 0.0))
        s = d3 / d4;
    else {
        //printf("d4 is ZERO - collinear\n");
        d4_z = true;
    }


    //If 0 <= r <= 1 and 0 <= s <= 1, intersection exists 
    if ((0.0 <= r) && (r <= 1.0) && (0.0 <= s) && (s <= 1.0)) {
        if ( !d2_z && !d4_z )
            result = true;
    }

    if (pr) {
        pr->intersect = result;
        pr->r = r;
        pr->s = s;
        pr->x = Ax + (r * (Bx-Ax)); 
        pr->y = Ay + (r * (By-Ay));
    }
    return result;
}


bool get_tri_centroid( int x1, int y1, int x2, int y2, int x3, int y3, Vector2d & v )
{
    bool res = false;
    ISRES pr1,pr2,pr3;
    Vector2d v1( (double)x1, (double)y1 );
    Vector2d v1o( (double)(x2 + x3) / 2.0, (double)(y2 + y3) / 2.0 );
    Vector2d v2( (double)x2, (double)y2 );
    Vector2d v2o( (double)(x1 + x3) / 2.0, (double)(y1 + y3) / 2.0 );
    Vector2d v3( (double)x3, (double)y3 );
    Vector2d v3o( (double)(x1 + x2) / 2.0, (double)(y1 + y2) / 2.0 );
    lines_intersect(v1, v1o, v2, v2o, &pr1);
    lines_intersect(v2, v2o, v3, v3o, &pr2);
    lines_intersect(v1, v1o, v3, v3o, &pr3);
    if ( pr1.intersect && pr2.intersect && pr3.intersect )
    {
        double x = ( pr1.x + pr2.x + pr3.x ) / 3.0;
        double y = ( pr1.y + pr2.y + pr3.y ) / 3.0;
        v.Set(x,y);
        res = true;
    }
    return res;
}

bool get_tri_centroid( Vector2d & A, Vector2d & B, Vector2d & C, Vector2d & v )
{
    return get_tri_centroid( win_int(A.GetX()), win_int(A.GetY()),
        win_int(B.GetX()), win_int(B.GetY()),
        win_int(C.GetX()), win_int(C.GetY()), v );
}


// Yahoo! Get angle between two lines
// from : http://www.tpub.com/math2/5.htm
// tan(&) = tan(&2 - &1) = ((tan(&2) - tan(&1)) / (1 + tan(&1) * tan(&2)))
// slope m = (y2 - y1) / (x2 - x1)
// cos(@) / A = k; sin(@) / B = k; so cos^2(@) + sin^2(@) = k^2 ( A^2 + B^2 )
// therefore 1 = k^2 ( A^2 + B^2 ); k = 1 / sqrt( A^2 + B^2 )
// angle betweeen two lines : @ = atan( ( m2 - m1 ) / ( 1 - (m1 * m2) )
// from : http://www.vb-helper.com/howto_find_angles.html
// The vector from point (x1, y1) to point (x2, y2) is: 
// V = <x2 - x1, y2 - y1>
// The length of a vector <x, y> is written |<x, y>| .
// Using the Pythagorean Theorem it is easy to show that: 
// |<x, y>| = Sqr(x * x + y * y)
// The dot product of two vectors A = <x1, y1> and B = <x2, y2> 
// is written A · B and has the value: 
// A · B = |A| * |B| * Cos(theta)
// You can easily calculate the dot product using this equation: 
// A · B = x1 * x2 + y1 * y2
/* ---------------------------------------
' Return the dot product AB · BC.
' Note that AB · BC = |AB| * |BC| * Cos(theta).
Private Function DotProduct( _
    ByVal Ax As Single, ByVal Ay As Single, _
    ByVal Bx As Single, ByVal By As Single, _
    ByVal Cx As Single, ByVal Cy As Single _
  ) As Single
Dim BAx As Single
Dim BAy As Single
Dim BCx As Single
Dim BCy As Single

    ' Get the vectors' coordinates.
    BAx = Ax - Bx
    BAy = Ay - By
    BCx = Cx - Bx
    BCy = Cy - By

    ' Calculate the dot product.
    DotProduct = BAx * BCx + BAy * BCy
End Function
  ------------------------------------------- */
// The cross product of two vectors A = <x1, y1> and B = <x2, y2>
// is written A × B. The result is a new vector that is 
// prependicular to both A and B and that has length: 
//    |A × B| = |A| * |B| * Sin(theta)
// You can calculate the cross product of two vectors in 
// the X-Y plane using this equation: 
// A × B = <0, 0, x1 * y2 - x2 * y1>
/* ----------------------------------
' Return the cross product AB x BC.
' The cross product is a vector perpendicular to AB
' and BC having length |AB| * |BC| * Sin(theta) and
' with direction given by the right-hand rule.
' For two vectors in the X-Y plane, the result is a
' vector with X and Y components 0 so the Z component
' gives the vector's length and direction.
Public Function CrossProductLength( _
    ByVal Ax As Single, ByVal Ay As Single, _
    ByVal Bx As Single, ByVal By As Single, _
    ByVal Cx As Single, ByVal Cy As Single _
  ) As Single
Dim BAx As Single
Dim BAy As Single
Dim BCx As Single
Dim BCy As Single

    ' Get the vectors' coordinates.
    BAx = Ax - Bx
    BAy = Ay - By
    BCx = Cx - Bx
    BCy = Cy - By

    ' Calculate the Z coordinate of the cross product.
    CrossProductLength = BAx * BCy - BAy * BCx
End Function

Finding Angles 
Now you can use the dot product and cross product to find the sine and cosine of the angle between the two vectors. 

    Cos(theta) =  A · B  / (|A| * |B|)
    Sin(theta) = |A × B| / (|A| * |B|)
A little trigonometry gives you: 


    Tan(theta) = Sin(theta) / Cos(theta)
So 

    Tan(theta) = [|A × B| / (|A| * |B|)] / [A · B  / (|A| * |B|)]
               = |A × B| / A · B
The following routines use these facts to calculate the angle between two vectors. 

' Return the angle with tangent opp/hyp. The returned
' value is between PI and -PI.
Public Function ATan2(ByVal opp As Single, ByVal adj As Single) As Single
Dim angle As Single

    ' Get the basic angle.
    If Abs(adj) < 0.0001 Then
        angle = PI / 2
    Else
        angle = Abs(Atn(opp / adj))
    End If

    ' See if we are in quadrant 2 or 3.
    If adj < 0 Then
        ' angle > PI/2 or angle < -PI/2.
        angle = PI - angle
    End If

    ' See if we are in quadrant 3 or 4.
    If opp < 0 Then
        angle = -angle
    End If

    ' Return the result.
    ATan2 = angle
End Function

' Return the angle ABC.
' Return a value between PI and -PI.
' Note that the value is the opposite of what you might
' expect because Y coordinates increase downward.
Public Function GetAngle(ByVal Ax As Single, ByVal Ay As _
    Single, ByVal Bx As Single, ByVal By As Single, ByVal _
    Cx As Single, ByVal Cy As Single) As Single
Dim dot_product As Single
Dim cross_product As Single

    ' Get the dot product and cross product.
    dot_product = DotProduct(Ax, Ay, Bx, By, Cx, Cy)
    cross_product = CrossProductLength(Ax, Ay, Bx, By, Cx, Cy)

    ' Calculate the angle.
    GetAngle = ATan2(cross_product, dot_product)
End Function

  ------------------------------------------ */

//    ByVal Ax As Single, ByVal Ay As Single, _
//    ByVal Bx As Single, ByVal By As Single, _
//    ByVal Cx As Single, ByVal Cy As Single _
double DotProduct( Vector2d A, Vector2d B, Vector2d C )
{
    double BAx, BAy, BCx, BCy;
    double Ax,Ay,Bx,By,Cx,Cy;

    Ax = A.GetX();
    Ay = A.GetY();

    Bx = B.GetX();
    By = B.GetY();

    Cx = C.GetX();
    Cy = C.GetY();

    //' Get the vectors' coordinates.
    BAx = Ax - Bx;
    BAy = Ay - By;
    BCx = Cx - Bx;
    BCy = Cy - By;

    //' Calculate the dot product.
    return ((BAx * BCx) + (BAy * BCy));
}

// ByVal Ax As Single, ByVal Ay As Single, _
// ByVal Bx As Single, ByVal By As Single, _
// ByVal Cx As Single, ByVal Cy As Single _
double CrossProductLength( Vector2d A, Vector2d B, Vector2d C )
{
    double BAx,BAy,BCx,BCy;
    double Ax,Ay,Bx,By,Cx,Cy;

    Ax = A.GetX();
    Ay = A.GetY();

    Bx = B.GetX();
    By = B.GetY();

    Cx = C.GetX();
    Cy = C.GetY();

    //' Get the vectors' coordinates.
    BAx = Ax - Bx;
    BAy = Ay - By;
    BCx = Cx - Bx;
    BCy = Cy - By;

    //' Calculate the Z coordinate of the cross product.
    return ((BAx * BCy) - (BAy * BCx));
}

//' Return the angle with tangent opp/hyp. The returned
//' value is between PI and -PI.
// ATan2(ByVal opp As Single, ByVal adj As Single) As Single
double ATan2(double opp, double adj)
{
    double angle;

    //' Get the basic angle.
    if ( fabs(adj) < EPSILON )
        angle = M_PI / 2.0;
    else
        angle = fabs(atan(opp / adj));

    //' See if we are in quadrant 2 or 3.
    if ( adj < 0.0 ) {
        //' angle > PI/2 or angle < -PI/2.
        angle = M_PI - angle;
    }

    //' See if we are in quadrant 3 or 4.
    if ( opp > 0.0 ) {
        angle = -angle;
    }

    //' Return the result.
    return angle;
}

//' Return the angle ABC.
//' Return a value between PI and -PI.
//' Note that the value is the opposite of what you might
//' expect because Y coordinates increase downward.
//Public Function GetAngle(ByVal Ax As Single, ByVal Ay As _
//    Single, ByVal Bx As Single, ByVal By As Single, ByVal _
//    Cx As Single, ByVal Cy As Single) As Single
double GetAngle( Vector2d A, Vector2d B, Vector2d C )
{
    double dot_product, cross_product;

    //' Get the dot product and cross product.
    dot_product = DotProduct( A, B, C );
    cross_product = CrossProductLength( A, B, C );

    //' Calculate the angle.
    return ATan2(cross_product, dot_product);
}

/* -------------------------------
    from : http://www.blackpawn.com/texts/pointinpoly/default.html
    Same Side Technique
    A common way to check if a point is in a triangle ABC is to 
    find the vectors connecting the point to each of the triangle's 
    three vertices and sum the angles between those vectors. If the 
    sum of the angles is 2*pi then the point is inside the triangle, 
    otherwise it is not. It works, but it is very slow. 
    This text explains a faster and much easier method. 

    Assume:
                  p
           A ----------- B
             |        /
             |   p1  /
             |      /
             |     /
             |    /
             |   /
             |  /
             | /
             |/
           C
    For a point to be inside the traingle A B C it must be below AB 
    and left of BC and right of AC. If any one of these tests fails 
    we can return early. 

    If you take the cross product of [B-A] and [p-A], you'll get a 
    vector pointing out of the screen. On the other hand, if you take 
    the cross product of [B-A] and [p'-A] you'll get a vector pointing 
    into the screen. Ah ha! In fact if you cross [B-A] with the vector 
    from A to any point above the line AB, the resulting vector points 
    out of the screen while using any point below AB yields a vector 
    pointing into the screen. So all we need to do to distinguish which 
    side of a line a point lies on is take a cross product. 

    The only question remaining is: how do we know what direction the 
    cross product should point in? Because the triangle can be oriented 
    in any way in 3d-space, there isn't some set value we can compare with. 
    Instead what we need is a reference point - a point that we know is on 
    a certain side of the line. For our triangle, this is just the third point C. 

    So, any point p where [B-A] cross [p-A] does not point in the same direction 
    as [B-A] cross [C-A] isn't inside the triangle. If the cross products do 
    point in the same direction, then we need to test p with the other lines as 
    well. If the point was on the same side of AB as C and is also on the same 
    side of BC as A and on the same side of CA as B, then it is in the triangle. 

    Implementing this is a breeze. We'll make a function that tells us if 
    two points are on the same side of a line and have the actual point-in-triangle 
    function call this for each edge. 

function SameSide(p1,p2, a,b)
    cp1 = CrossProduct(b-a, p1-a)
    cp2 = CrossProduct(b-a, p2-a)
    if DotProduct(cp1, cp2) >= 0 then return true
    else return false

function PointInTriangle(p, a,b,c)
    if SameSide(p,a, b,c) and SameSide(p,b, a,c)
        and SameSide(p,c, a,b) then return true
    else return false

   ------------------------------- */

bool SameSide( Vector2d p1, Vector2d p2, Vector2d a, Vector2d b )
{
    double cp1 = CrossProductLength( b, a, p1 );
    double cp2 = CrossProductLength( b, a, p2 );
    if ((cp1 * cp2) >= 0.0)
        return true;
    return false;
}

bool PointInTriangle( Vector2d p, Vector2d a, Vector2d b, Vector2d c )
{
    if (SameSide( p, a, b, c ) && SameSide( p, b, a, c ) && SameSide( p, c, a, b ))
        return true;
    return false;
}

/* --------------------------------------------
    Barycentric Technique

    So remember that the three points of the triangle define a 
    plane in space. Pick one of the points and we can consider 
    all other locations on the plane as relative to that point. 
    Let's go with A -- it'll be our origin on the plane. Now 
    what we need are basis vectors so we can give coordinate 
    values to all the locations on the plane. We'll pick the 
    two edges of the triangle that touch A, (C - A) and (B - A).
    Now we can get to any point on the plane just by starting 
    at A and walking some distance along (C - A) and then 
    from there walking some more in the direction (B - A). 

    With that in mind we can now describe any point on the plane as 

    P = A + u * (C - A) + v * (B - A)

    Notice now that if u or v < 0 then we've walked in the wrong 
    direction and must be outside the triangle. Also if u or v > 1 
    then we've walked too far in a direction and are outside the 
    triangle. Finally if u + v > 1 then we've crossed the edge BC 
    again leaving the triangle. 

    Given u and v we can easily calculate the point P with the 
    above equation, but how can we go in the reverse direction 
    and calculate u and v from a given point P? Time for some math! 

    P = A + u * (C - A) + v * (B - A)       // Original equation
    (P - A) = u * (C - A) + v * (B - A)     // Subtract A from both sides
    v2 = u * v0 + v * v1                    // Substitute v0, v1, v2 for less writing
    
    // We have two unknowns (u and v) so we need two equations to solve
    // for them.  Dot both sides by v0 to get one and dot both sides by
    // v1 to get a second.
    (v2) . v0 = (u * v0 + v * v1) . v0
    (v2) . v1 = (u * v0 + v * v1) . v1

    // Distribute v0 and v1
    v2 . v0 = u * (v0 . v0) + v * (v1 . v0)
    v2 . v1 = u * (v0 . v1) + v * (v1 . v1)

    // Now we have two equations and two unknowns and can solve one 
    // equation for one variable and substitute into the other.  Or
    // if you're lazy like me, fire up Mathematica and save yourself
    // some handwriting.

    Solve[v2.v0 == {u(v0.v0) + v(v1.v0), v2.v1 == u(v0.v1) + v(v1.v1)}, {u, v}]

    u = ((v1.v1)(v2.v0)-(v1.v0)(v2.v1)) / ((v0.v0)(v1.v1) - (v0.v1)(v1.v0))

    v = ((v0.v0)(v2.v1)-(v0.v1)(v2.v0)) / ((v0.v0)(v1.v1) - (v0.v1)(v1.v0))

    Here's an implementation in Flash that you can play with. :)
    from : http://www.blackpawn.com/texts/pointinpoly/default.html
    // Compute vectors        
    v0 = C - A
    v1 = B - A
    v2 = P - A

    // Compute dot products
    dot00 = dot(v0, v0)
    dot01 = dot(v0, v1)
    dot02 = dot(v0, v2)
    dot11 = dot(v1, v1)
    dot12 = dot(v1, v2)

    // Compute barycentric coordinates
    invDenom = 1 / (dot00 * dot11 - dot01 * dot01)

    u = (dot11 * dot02 - dot01 * dot12) * invDenom
    
    v = (dot00 * dot12 - dot01 * dot02) * invDenom

    // Check if point is in triangle
    return (u > 0) && (v > 0) && (u + v < 1)

   -------------------------------------------- */
bool PointInTriangle2( Vector2d P, Vector2d A, Vector2d B, Vector2d C )
{
    // Compute vectors        
    Vector2d v0( C.GetX() - A.GetX(), C.GetY() - A.GetY() );
    Vector2d v1( B.GetX() - A.GetX(), B.GetY() - A.GetY() );
    Vector2d v2( P.GetX() - A.GetX(), P.GetY() - A.GetY() );
    // Compute dot products
#define dot(a,b)    (a.GetX() * b.GetX()) + (a.GetY() * b.GetY())
    double dot00 = dot(v0, v0);
    double dot01 = dot(v0, v1);
    double dot02 = dot(v0, v2);
    double dot11 = dot(v1, v1);
    double dot12 = dot(v1, v2);
    // Compute barycentric coordinates
    double invDenom = 1.0 / ((dot00 * dot11) - (dot01 * dot01));
    double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    // Check if point is in triangle
    return (u > 0.0) && (v > 0.0) && ((u + v) < 1.0);
}

/* --------------------------
    from : http://en.wikipedia.org/wiki/Line_(geometry)
    The gradient (slope, m) of the line through points A(ax, ay) and B(bx, by) 
    is given by m = (by-ay)/(bx-ax) and the Cartesian equation of this line 
    can be written: y - ay = m(x - ax)
   -------------------------- */
double line_slope( Vector2d A, Vector2d B )
{
    double dx = (B.GetX() - A.GetX());
    double dy = (B.GetY() - A.GetY());
    double m;
    if (EQUALZ(dx)) {
        dx = EPSILON;
    }
    m = dy / dx;
    return m;
}

bool point_on_line1( Vector2d p, Vector2d A, Vector2d B )
{
    double m1 = line_slope( A, B );
    double m2 = line_slope( A, p );
    if ( EQUAL(m1,m2) )
        return true;

    return false;
}

/* -------------------------------
    from : http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/

    This note describes the technique and gives the solution to 
    finding the shortest distance from a point to a line or line 
    segment. The equation of a line defined through two 
    points P1 (x1,y1) and P2 (x2,y2) is 

    P = P1 + u (P2 - P1) 

    The point P3 (x3,y3) is closest to the line at the tangent to 
    the line which passes through P3, that is, the dot product of 
    the tangent and line is 0, thus 

    (P3 - P) dot (P2 - P1) = 0 

    Substituting the equation of the line gives 

    [P3 - P1 - u(P2 - P1)] dot (P2 - P1) = 0 

    Solving this gives the value of u

    P1 (x1,y1), P2 (x2,y2), P3 (x3,y3)
    u = (((x3 - x1)*(x2 - x1)+((y3 - y1)*(y2 - y1))) / (|p2 - p2|^2)

    Substituting this into the equation of the line gives the point 
    of intersection (x,y) of the tangent as 

    x = x1 + u (x2 - x1)
    y = y1 + u (y2 - y1) 

    The distance therefore between the point P3 and the line is the 
    distance between (x,y) above and P3. 

   ------------------------------- */
bool point_on_line2( Vector2d p, Vector2d A, Vector2d B )
{
    Vector2d v1;
    v1.Set( B.GetX() - A.GetX(), B.GetY() - A.GetY() );
    //A(x1,y1), B(x2,y2), p(x3,y3)
    //u = (((x3 - x1)*(x2 - x1)+((y3 - y1)*(y2 - y1))) / (|p2 - p2|^2)
    double n = ( ( (p.GetX() - A.GetX()) * (B.GetX() - A.GetX() ) +
        ( (p.GetY() - A.GetY()) * (B.GetY() - A.GetY()) ) ));
    // (|P2 - P1|^2);
#define dot(a,b)    (a.GetX() * b.GetX()) + (a.GetY() * b.GetY())
    double d = dot(B,A);
    d = d * d;
    if ( EQUALZ(d) )
        d = EPSILON;
    double u = n / d;
    double x = A.GetX() + (u * (B.GetX() - A.GetX()));
    double y = A.GetY() + (u * (B.GetY() - A.GetY()));
    // *** TBD *** NOT YET COMPLETED!!!
    return false;
}

/* -------------------------------------------
    from : http://www.vb-helper.com/howto_distance_point_to_line.html
This example treats the segment as parameterized vector where the parameter 
t varies from 0 to 1. It finds the value of t that minimizes the distance 
from the point to the line. If t is between 0.0 and 1.0, then the 
closest point lies on the segment, otherwise the closest point is 
one of the segment's end points. The program finds this closest point 
and calculates the dstance between it and the target point. 
 
' Calculate the distance between the point and the segment.
Private Function DistToSegment(ByVal px As Single, ByVal py _
    As Single, ByVal X1 As Single, ByVal Y1 As Single, _
    ByVal X2 As Single, ByVal Y2 As Single, ByRef near_x As _
    Single, ByRef near_y As Single) As Single
Dim dx As Single
Dim dy As Single
Dim t As Single

    dx = X2 - X1
    dy = Y2 - Y1
    If dx = 0 And dy = 0 Then
        ' It's a point not a line segment.
        dx = px - X1
        dy = py - Y1
        near_x = X1
        near_y = Y1
        DistToSegment = Sqr(dx * dx + dy * dy)
        Exit Function
    End If

    ' Calculate the t that minimizes the distance.
    t = ((px - X1) * dx + (py - Y1) * dy) / (dx * dx + dy * _
        dy)

    ' See if this represents one of the segment's
    ' end points or a point in the middle.
    If t < 0 Then
        dx = px - X1
        dy = py - Y1
        near_x = X1
        near_y = Y1
    ElseIf t > 1 Then
        dx = px - X2
        dy = py - Y2
        near_x = X2
        near_y = Y2
    Else
        near_x = X1 + t * dx
        near_y = Y1 + t * dy
        dx = px - near_x
        dy = py - near_y
    End If

    DistToSegment = Sqr(dx * dx + dy * dy)
End Function

   ------------------------------------------- */

//Private Function DistToSegment(ByVal px As Single, ByVal py _
//    As Single, ByVal X1 As Single, ByVal Y1 As Single, _
//    ByVal X2 As Single, ByVal Y2 As Single, ByRef near_x As _
//    Single, ByRef near_y As Single) As Single
static int _s_use_direct = 0;
double DistToSegment( Vector2d p, Vector2d A, Vector2d B, Vector2d & N )
{
    double dx, dy, dist, t, n, d;
    double near_x, near_y;
#define px  p.GetX()
#define py  p.GetY()
#define X1  A.GetX()
#define Y1  A.GetY()
#define X2  B.GetX()
#define Y2  B.GetY()
    if (_s_use_direct) {
        dx = B.GetX() - A.GetX();
        dy = B.GetY() - A.GetY();
    } else {
        dx = X2 - X1;
        dy = Y2 - Y1;
    }
    if ( EQUALZ(dx) && EQUALZ(dy) ) {
        // It's a point not a line segment.
        if (_s_use_direct) {
            dx = p.GetX() - A.GetX();
            dy = p.GetY() - A.GetY();
            near_x = A.GetX();
            near_y = A.GetY();
        } else {
            dx = px - X1;
            dy = py - Y1;
            near_x = X1;
            near_y = Y1;
        }
    } else {
        //' Calculate the t that minimizes the distance.
        if (_s_use_direct) {
            n = (((p.GetX() - A.GetX()) * dx) + ((p.GetY() - A.GetY()) * dy));
            d = (dx * dx) + (dy * dy);  // already checked for NOT ZERO
            t = n / d;
        } else {
            t = ((px - X1) * dx + (py - Y1) * dy) / (dx * dx + dy * dy);
        }

        //' See if this represents one of the segment's
        //' end points or a point in the middle.
        if ( t < 0.0 ) {    // Then
            if (_s_use_direct) {
                dx = p.GetX() - A.GetX();
                dy = p.GetY() - A.GetY();
                near_x = A.GetX();
                near_y = A.GetY();
            } else {
                dx = px - X1;
                dy = py - Y1;
                near_x = X1;
                near_y = Y1;
            }
        } else if ( t > 1.0 ) { // Then
            if (_s_use_direct) {
                dx = p.GetX() - B.GetX();
                dy = p.GetY() - B.GetY();
                near_x = B.GetX();
                near_y = B.GetY();
            } else {
                dx = px - X2;
                dy = py - Y2;
                near_x = X2;
                near_y = Y2;
            }
        } else { // Else
            if (_s_use_direct) {
                near_x = A.GetX() + t * dx;
                near_y = A.GetY() + t * dy;
                dx = p.GetX() - near_x;
                dy = p.GetY() - near_y;
            } else {
                near_x = X1 + t * dx;
                near_y = Y1 + t * dy;
                dx = px - near_x;
                dy = py - near_y;
            }
        } // End If
    }
    dist = sqrt((dx * dx) + (dy * dy));
    N.Set(near_x,near_y);
#undef px
#undef py
#undef X1
#undef Y1
#undef X2
#undef Y2
    return dist;
}

bool point_on_line3( Vector2d p, Vector2d A, Vector2d B )
{
    Vector2d N;
    double dist = DistToSegment( p, A, B, N );
    bool bret = EQUALZ(dist);

    // test the direct stuff
    if (_s_use_direct == 0) {
        Vector2d N2;
        _s_use_direct = 1;
        double d2 = DistToSegment( p, A, B, N2 );
        bool br2 = EQUALZ(d2);
        if ((bret != br2) || !SAME(N,N2) || !EQUAL(dist,d2)) {
            sprtf("ERROR: Direct did NOT yield SAME results!\n*** FIX ME ***\n");
            sprtf("Dir=%s, Def=%s, N1=(%s), N2=(%s), d1=%f, d2=%f\n*** NEED FIX ***\n",
                (br2 ? "TRUE" : "FALSE"),
                (bret ? "TRUE" : "FALSE"),
                get_vpoint_stg(N),
                get_vpoint_stg(N2),
                dist, d2 );
            exit(1);
        }
        _s_use_direct = 0;
    }
#if 0 // =========================
    if (bret) {
        sprtf("Pt (%s) is on line (%s), at (%s) dist = 0\n",
            get_vpoint_stg(p),
            get_vline_stg(A,B),
            get_vpoint_stg(N) );
    } else {
        sprtf("Pt (%s) is NOT on line (%s), near (%s)! Distance %g units\n",
            get_vpoint_stg(p),
            get_vline_stg(A,B),
            get_vpoint_stg(N),
            dist );
    }
#endif // ===========================

    return bret;
}

// eof - ap4_maths.cxx
