// ap4_tri.cxx
// triangulate.cxx

#include "testap4.hxx"

// triangulate.cxx
// from : http://www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml

using namespace std;

double Triangulate::Area(const Vector2dVector &contour)
{
    size_t n = contour.size();
    double A = 0.0;
    for(size_t p = n-1, q = 0; q < n; p = q++)
    {
        A += contour[p].GetX()*contour[q].GetY() - contour[q].GetX()*contour[p].GetY();
    }
    return (A * 0.5);
}

/*
     InsideTriangle decides if a point P is Inside of the triangle
     defined by A, B, C.
   */
bool Triangulate::InsideTriangle(double Ax, double Ay,
                      double Bx, double By,
                      double Cx, double Cy,
                      double Px, double Py)
{
    double ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
    double cCROSSap, bCROSScp, aCROSSbp;
    ax  = Cx - Bx;
    ay  = Cy - By;
    bx  = Ax - Cx;
    by  = Ay - Cy;
    cx  = Bx - Ax;
    cy  = By - Ay;
    apx = Px - Ax;
    apy = Py - Ay;
    bpx = Px - Bx;
    bpy = Py - By;
    cpx = Px - Cx;
    cpy = Py - Cy;

    aCROSSbp = ax*bpy - ay*bpx;
    cCROSSap = cx*apy - cy*apx;
    bCROSScp = bx*cpy - by*cpx;

    return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
};

//bool Triangulate::Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V)
bool Triangulate::Snip(const Vector2dVector &contour,size_t u,size_t v,size_t w,size_t n,size_t *V)
{
    size_t p;
    double Ax, Ay, Bx, By, Cx, Cy, Px, Py;

    Ax = contour[V[u]].GetX();
    Ay = contour[V[u]].GetY();
    Bx = contour[V[v]].GetX();
    By = contour[V[v]].GetY();
    Cx = contour[V[w]].GetX();
    Cy = contour[V[w]].GetY();

    if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) )
      return false;

    for ( p = 0; p < n; p++ )
    {
        if( (p == u) || (p == v) || (p == w) )
            continue;
        Px = contour[V[p]].GetX();
        Py = contour[V[p]].GetY();
        if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py))
            return false;
    }
    return true;
}

bool Triangulate::Process(const Vector2dVector &contour,Vector2dVector &result)
{
    size_t v, m;
    /* allocate and initialize list of Vertices in polygon */
    size_t n = contour.size();
    if ( n < 3 )
        return false;

    size_t *V = new size_t[n];  /* we want a counter-clockwise polygon in V */
    if ( 0.0f < Area(contour) )
        for (v=0; v<n; v++) V[v] = v;
    else
        for(v=0; v<n; v++) V[v] = (n-1)-v;
    
    size_t nv = n;  /*  remove nv-2 Vertices, creating 1 triangle every time */
    size_t count = 2*nv;   /* error detection */
    for(m = 0, v = nv-1; nv > 2; )
    {
        /* if we loop, it is probably a non-simple polygon */
        if (0 >= (count--))
        {
            //** Triangulate: ERROR - probable bad polygon!
            delete V;
            return false;
        }
        /* three consecutive vertices in current polygon, <u,v,w> */
        size_t u = v  ;
        if (nv <= u)
            u = 0;     /* previous */
        v = u+1;
        if (nv <= v)
            v = 0;     /* new v    */
        size_t w = v+1;
        if (nv <= w)
            w = 0;     /* next     */

        if ( Snip(contour,u,v,w,nv,V) )
        {
            size_t a,b,c,s,t;      /* true names of the vertices */
            a = V[u];
            b = V[v];
            c = V[w];      /* output Triangle */

            result.push_back( contour[a] );
            result.push_back( contour[b] );
            result.push_back( contour[c] );

            m++;      /* remove v from remaining polygon */
            for(s = v, t = v+1; t < nv; s++,t++)
                V[s] = V[t];

            nv--;      /* resest error detection counter */
            count = 2*nv;
        }
    }

    delete V;
    return true;
}

// from : http://ubuntu-gamedev.wikispaces.com/2D+Collision+Detection
bool in2dRect(double* topLeft, double* bottomRight, double* pos)
{
    return pos[0] >= topLeft[0] && pos[0] < bottomRight[0] &&
     pos[1] >= topLeft[1] && pos[1] < bottomRight[1]; // [o] = x and [1] = y coordinates
}
void sub_2d_vector(float *p1, float *p2, float *r)
{
    r[0] = fabs(p1[0] - p2[0]);
    r[1] = fabs(p1[1] - p2[1]);
}
bool inLeftHalfspace2d(float* start, float* end, float* pos)  //left if at start and facing towards end
{
     float line[2],v[2];
     sub_2d_vector(end,start,line);
     sub_2d_vector(pos,start,v);
     //quarterRight2d(line,line);//  turning a 2D vector by 90 degrees
     // is two assignments and a negation
     //return dotproduct2d(line,v) <= 0;  //if the point is exactly on the line,
     // lets say its in the halfspace
     return false;
}

bool point_on_line(Vector2d A, Vector2d B, Vector2d C, double *pr)
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

    // generate a 4th point
    Dx = Cx + EPSILON;
    Dy = Cy + EPSILON;
    Cx -= EPSILON;
    Cy -= EPSILON;

    //r = -----------------------------  (eqn 1)
    double d1 = ((Ay-Cy)*(Dx-Cx))-((Ax-Cx)*(Dy-Cy));
    double d2 = ((Bx-Ax)*(Dy-Cy))-((By-Ay)*(Dx-Cx));
    //s = -----------------------------  (eqn 2)
    double d3 = ((Bx-Ax)*(Dy-Cy))-((By-Ay)*(Dx-Cx));
    double d4 = ((Bx-Ax)*(Dy-Cy))-((By-Ay)*(Dx-Cx));

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
    pr[0] = r;
    pr[1] = s;

    double Px = Ax + (r * (Bx-Ax)); 
    double Py = Ay + (r * (By-Ay)); 

    pr[2] = Px;
    pr[3] = Py;
    //If 0 <= r <= 1 and 0 <= s <= 1, intersection exists 
    if ((0.0 <= r) && (r <= 1.0) && (0.0 <= s) && (s <= 1.0)) {
        if ( !d2_z && !d4_z )
            result = true;
    }
    return result;
}


string SharedResults(int results)
{
    string s = "";
    if (results & 1)
        s += "AA"; // if (SAME(pt1A,pt2A))
    if (results & 2)
        s += "AB"; // if (SAME(pt1A,pt2B))
    if (results & 4)
        s += "AC"; // if (SAME(pt1A,pt2C))
    if (results & 8)
        s += "BA";  // if (SAME(pt1B,pt2A))
    if (results & 16)
        s += "BB";  // if (SAME(pt1B,pt2B))
    if (results & 32)
        s += "BC";  // if (SAME(pt1B,pt2C))
    if (results & 64)
        s += "CA";  // if (SAME(pt1C,pt2A))
    if (results & 128)
        s += "CB";  // if (SAME(pt1C,pt2B))
    if (results & 256)
        s += "CC";  // if (SAME(pt1C,pt2C))
    return s;
}

int SharedResCount(int res)
{
    int count = 0;
    int test = 1;
    while (test) {
        if (res & test)
            count++;
        test = test << 1;
    }
    return count;
}

int ShareCommonPoints( Vector2dTri &t1, Vector2dTri &t2, Vector2dVector &nvecs )
{
    int results = 0;
    Vector2dVector nv;
    int count = 0;
    nv.clear();
    // triangle 1
    Vector2d pt1A = t1.GetA();
    Vector2d pt1B = t1.GetB();
    Vector2d pt1C = t1.GetC();
    // triangle 2
    Vector2d pt2A = t2.GetA();
    Vector2d pt2B = t2.GetB();
    Vector2d pt2C = t2.GetC();
    // now test the corners

    // 1 - A with ABC
    if (SAME(pt1A,pt2A)) {
        results |= 1;
        nv.push_back(pt1A);
        //nv.push_back(pt2A);
        count++;
    }
    if (SAME(pt1A,pt2B)) {
        results |= 2;
        nv.push_back(pt1A);
        //nv.push_back(pt2B);
        count++;
    }
    if (SAME(pt1A,pt2C)) {
        results |= 4;
        nv.push_back(pt1A);
        //nv.push_back(pt2C);
        count++;
    }

    // 2 - B with ABC
    if (SAME(pt1B,pt2A)) {
        results |= 8;
        nv.push_back(pt1B);
        //nv.push_back(pt2A);
        count++;
    }
    if (SAME(pt1B,pt2B)) {
        results |= 16;
        nv.push_back(pt1B);
        //nv.push_back(pt2B);
        count++;
    }
    if (SAME(pt1B,pt2C)) {
        results |= 32;
        nv.push_back(pt1B);
        //nv.push_back(pt2C);
        count++;
    }

    // 3 - C with ABC
    if (SAME(pt1C,pt2A)) {
        results |= 64;
        nv.push_back(pt1C);
        //nv.push_back(pt2A);
        count++;
    }
    if (SAME(pt1C,pt2B)) {
        results |= 128;
        nv.push_back(pt1C);
        //nv.push_back(pt2B);
        count++;
    }
    if (SAME(pt1C,pt2C)) {
        results |= 256;
        nv.push_back(pt1C);
        //nv.push_back(pt2C);
        count++;
    }

    //if (SharedResCount(results) >= 2) {
    if (count >= 2) {
        size_t max = nv.size();
        for( size_t i = 0; i < max; i++ ) {
            nvecs.push_back( nv[i] );
        }
    }
    return results;
}

bool vector2d_not_in_poly( Vector2dVector &polygon, Vector2d n )
{
    size_t i, cnt;
    cnt = polygon.size();
    for (i = 0; i < cnt; i++) {
        if ( SAME(n,polygon[i]) )
            return false;
    }
    return true;
}

int CommonLinesPoly2Tri( Vector2dVector &polygon, Vector2dTri &t1 )
{
    int count = 0;
    // triangle - get 3 points
    Vector2d ptA = t1.GetA();
    Vector2d ptB = t1.GetB();
    Vector2d ptC = t1.GetC();
    size_t pcount = polygon.size();
    size_t i, nxt;
    for (i = 0; i < pcount; i++)
    {
        nxt = i + 1;
        if (nxt >= pcount)
            nxt = 0;
        Vector2d A = polygon[i];
        Vector2d B = polygon[nxt];
        if ((SAME(A,ptA) && SAME(B,ptB))||(SAME(A,ptB) && SAME(B,ptA))) {
            printf("Tri (%s) has common side AA - BB or AB - BA (%s) with poly.\n",
                get_vtri_stg(t1), get_vline_stg(A,B));
            count++;
        } else if ((SAME(A,ptB) && SAME(B,ptC))||(SAME(A,ptC) && SAME(B,ptB))) {
            printf("Tri (%s) has common side AB - BC or AC - BB (%s) with poly.\n",
                get_vtri_stg(t1), get_vline_stg(A,B));
            count++;
        } else if ((SAME(A,ptC) && SAME(B,ptA))||(SAME(A,ptA) && SAME(B,ptC))) {
            printf("Tri (%s) has common side AC - BA or AA - BC (%s) with poly.\n",
                get_vtri_stg(t1), get_vline_stg(A,B));
            count++;
        }
    }
    return count;
}

// try to find a common side betweeen the developing polygon,
// and the triangle. If found, expand the polygon to
// include this triangle...
bool ExpandPoly( Vector2dVector &polygon, Vector2dTri t1 )
{
    bool bret = false;
    size_t bpcount = polygon.size();
    size_t i, nxt;
    //double p1x, p1y, p2x, p2y;
    //double ptAx,ptAy, ptBx,ptBy, ptCx,ptCy;
    Vector2dVector pnew;
    // triangle - get 3 points
    Vector2d ptA = t1.GetA();
    Vector2d ptB = t1.GetB();
    Vector2d ptC = t1.GetC();
    size_t pcount = bpcount;
    // get x,y of 3 points
    //ptAx = ptA.GetX();
    //ptAy = ptA.GetY();
    //ptBx = ptB.GetX();
    //ptBy = ptB.GetY();
    //ptCx = ptC.GetX();
    //ptCy = ptC.GetY();
    if ( !CommonLinesPoly2Tri( polygon, t1 ) ) {
        printf("No common line segments with %d pt poly for tri (%s)\n", polygon.size(), get_vtri_stg(t1));
        return bret;
    }

    pnew.clear();
    for (i = 0; i < pcount; i++)
    {
        nxt = i + 1;
        if (nxt >= pcount)
            nxt = 0;
        Vector2d A = polygon[i];
        Vector2d B = polygon[nxt];

        // got one edge of the polygon
        //p1x = A.GetX();
        //p1y = A.GetY();
        //p2x = B.GetX();
        //p2y = B.GetY();
        if (i == 0) {
            pnew.push_back(A);
        }
        if ((SAME(A,ptA) && SAME(B,ptB))||(SAME(A,ptB) && SAME(B,ptA))) {
            // got common side AA - BB or AB - BA
            pnew.push_back(ptC);
            bret = true;
        } else if ((SAME(A,ptB) && SAME(B,ptC))||(SAME(A,ptC) && SAME(B,ptB))) {
            // got common side AB - BC or AC - BB
            pnew.push_back(ptA);
            bret = true;
        } else if ((SAME(A,ptC) && SAME(B,ptA))||(SAME(A,ptA) && SAME(B,ptC))) {
            // got common side AC - BA or AA - BC
            bret = true;
            pnew.push_back(ptB);
        }

        // add this point to poly
        pnew.push_back(B);
    }
    pcount = pnew.size();
    polygon.clear();
    for( i = 0; i < pcount; i++ )
    {
        Vector2d n = pnew[i];
        //if ((i == 0) || vector2d_not_in_poly( polygon, n ))
            polygon.push_back(n);
    }
    pcount = polygon.size();
    if (bpcount == pcount) {
        // no change
    } else if (pcount > bpcount) {
        printf("Poly count increased to %d, from %d\n", pcount, bpcount);
    } else { // if (pcount < bpcount)
        printf("Poly count DECREASED to %d, from %d\n", pcount, bpcount);
        bpcount = 0;
    }
    bpcount = 0;
    return bret;
}

// ==========================================
// give a set of TRIANGLES in result,
// attempt to construct a POLYGON in contour
// ==========================================

int get_bit_count(int res)
{
    int count = 0;
    int test = 1;
    while (test && res) {
        if (res & test) {
            count++;
            res &= ~test;
        }
        test = test << 1;
    }
    return count;
}

// this was only used to develop and test the final function
void checkme( Vector2dTri t1, Vector2dTri t2 )
{
    int count = 0;
    Vector2d t1A = t1.GetA();
    Vector2d t1B = t1.GetB();
    Vector2d t1C = t1.GetC();
    Vector2d t2A = t2.GetA();
    Vector2d t2B = t2.GetB();
    Vector2d t2C = t2.GetC();
    sprtf("Comp (%s)\nwith (%s)\n",
        get_vtri_stg( t1 ),
        get_vtri_stg( t2 ) );

    sprtf("Test 11 (%s) with (%s) ", 
        get_vline_stg( t1A, t1B ),
        get_vline_stg( t2A, t2B ) );
    if ((SAME(t1A,t2A) && SAME(t1B,t2B)) || (SAME(t1A,t2B) && SAME(t1B,t2A))) {
        sprtf("Common\n");
        count++;
    } else {
        sprtf("No\n");
    }
    sprtf("Test 12 (%s) with (%s) ", 
        get_vline_stg( t1A, t1B ),
        get_vline_stg( t2B, t2C ) );
        //get_vline_stg( t2A, t2B ) );
    if ((SAME(t1A,t2C) && SAME(t1B,t2C)) || (SAME(t1A,t2C) && SAME(t1B,t2C))) {
        sprtf("Common\n");
        count++;
    } else {
        sprtf("No\n");
    }

    sprtf("Test 13 (%s) with (%s) ", 
        get_vline_stg( t1A, t1B ),
        get_vline_stg( t2C, t2A ) );
        //get_vline_stg( t2A, t2B ) );
    if ((SAME(t1A,t2C) && SAME(t1B,t2A)) || (SAME(t1A,t2A) && SAME(t1B,t2C))) {
        sprtf("Common\n");
        count++;
    } else {
        sprtf("No\n");
    }


    // ==========================================
    sprtf("Test 21 (%s) with (%s) ", 
        get_vline_stg( t1B, t1C ),
        get_vline_stg( t2B, t2C ) );
    if ((SAME(t1B,t2B) && SAME(t1C,t2C))||(SAME(t1B,t2C) && SAME(t1C,t2B))) {
        sprtf("Common\n");
        count++;
    } else {
        sprtf("No\n");
    }
    sprtf("Test 22 (%s) with (%s) ", 
        get_vline_stg( t1B, t1C ),
        get_vline_stg( t2C, t2A ) );
        //get_vline_stg( t2B, t2C ) );
    if ((SAME(t1B,t2C) && SAME(t1C,t2A))||(SAME(t1B,t2A) && SAME(t1C,t2C))) {
        sprtf("Common\n");
        count++;
    } else {
        sprtf("No\n");
    }
    sprtf("Test 23 (%s) with (%s) ", 
        get_vline_stg( t1B, t1C ),
        get_vline_stg( t2A, t2B ) );
        //get_vline_stg( t2B, t2C ) );
    if ((SAME(t1B,t2A) && SAME(t1C,t2B))||(SAME(t1B,t2B) && SAME(t1C,t2A))) {
        sprtf("Common\n");
        count++;
    } else {
        sprtf("No\n");
    }

    // ==========================================
    sprtf("Test 31 (%s) with (%s) ", 
        get_vline_stg( t1C, t1A ),
        get_vline_stg( t2C, t2A ) );
    if ((SAME(t1C,t2C) && SAME(t1A,t2A))||(SAME(t1C,t2A) && SAME(t1A,t2C))) {
        count++;
        sprtf("Common\n");
    } else {
        sprtf("No\n");
    }

    sprtf("Test 32 (%s) with (%s) ", 
        get_vline_stg( t1C, t1A ),
        get_vline_stg( t2A, t2B ) );
        //get_vline_stg( t2C, t2A ) );
    if ((SAME(t1C,t2A) && SAME(t1A,t2B))||(SAME(t1C,t2B) && SAME(t1A,t2A))) {
        count++;
        sprtf("Common\n");
    } else {
        sprtf("No\n");
    }
    sprtf("Test 33 (%s) with (%s) ", 
        get_vline_stg( t1C, t1A ),
        get_vline_stg( t2B, t2C ) );
        //get_vline_stg( t2C, t2A ) );
    if ((SAME(t1C,t2B) && SAME(t1A,t2C))||(SAME(t1C,t2C) && SAME(t1A,t2B))) {
        count++;
        sprtf("Common\n");
    } else {
        sprtf("No\n");
    }

    if (count == 0)
        sprtf("No common line!\n");
    else
        sprtf("Got %d common faces.\n", get_bit_count(count) );
}

/* ---------------------------------------------------------------------
   int SharedEdgeLine2Tri( Vector2dLine line, Vector2dTri tri, Vector2d &p )
    input:
    Vector2dLine line - line to find (XY)
    Vector2dTri tri   - triangle (3 lines - AB BC CA)
    Vector2d &p       - if found, the 3rd tri point - odd man out
    result: int, with bit set
    1 if (XY == AB)||(XY == BA)
    2 if (XY == BC)||(XY == CB)
    4 if (XY == CA)||(XY == AC)
    0 if no match found!
   --------------------------------------------------------------------- */
int SharedEdgeLine2Tri( Vector2dLine line, Vector2dTri tri, Vector2d &p )
{
    int count = 0;

    Vector2d X = line.GetA();
    Vector2d Y = line.GetB();

    Vector2d tA = tri.GetA();
    Vector2d tB = tri.GetB();
    Vector2d tC = tri.GetC();

    // Test line with 3 lines in tri
    if ((SAME(X,tA) && SAME(Y,tB)) || (SAME(X,tB) && SAME(Y,tA))) {
        p.Set(tC.GetX(),tC.GetY()); // return C
        count |= 1; // XY == AB or BA
    } else
    if ((SAME(X,tB) && SAME(Y,tC)) || (SAME(X,tC) && SAME(Y,tB))) {
        p.Set(tA.GetX(),tA.GetY()); // return A
        count |= 2; // XY == BC or CB
    } else
    if ((SAME(X,tC) && SAME(Y,tA)) || (SAME(X,tA) && SAME(Y,tC))) {
        p.Set(tB.GetX(),tB.GetY()); // return B
        count |= 4; // XY == CA or AC
    }
    return count;
}


int SharedEdge( Vector2dTri t1, Vector2dTri t2 )
{
    int count = 0;
    Vector2d p;
    // triangle - get 3 points
    Vector2d t1A = t1.GetA();
    Vector2d t1B = t1.GetB();
    Vector2d t1C = t1.GetC();
    Vector2dLine l1, l2, l3;

    l1.Set(t1A,t1B);
    l2.Set(t1B,t1C);
    l3.Set(t1C,t1A);

    count = SharedEdgeLine2Tri( l1, t2, p );
    if (count)
        return count;
    count = SharedEdgeLine2Tri( l2, t2, p );
    if (count)
        return (count << 3);
    count = SharedEdgeLine2Tri( l3, t2, p );
    if (count)
        return (count << 6);

    return 0;   // NO MATCH
}

bool Triangulate::ReverseProcess(Vector2dVector &polygon, Vector2dVector &result)
{
    bool bret = true;   // assume SUCCESS
    int match;
    Vector2dTriVector tris, missed;
    size_t i, j, j2;
    size_t tcount = result.size();
    if (tcount < 3) {
        // must have at least THREE points
        return false;
    }
    size_t tricnt = tcount / 3; // sets of TRIANGLES
    Vector2dTri tri1, tri2;
    Vector2d v1, v2, v3;
    // massage results poly points into triangles (tris)
    tris.clear();
    for (i = 0; i < tricnt; i++) {
        v1 = result[i*3+0];
        v2 = result[i*3+1];
        v3 = result[i*3+2];
        // got a triangle
        tri1.Set( v1, v2, v3 );
        tris.push_back(tri1);
    }

    size_t polycnt = polygon.size();
    Vector2dLine line;
    size_t misscnt = 0;
    missed.clear();
    // setup to enter loop
    bool repeat = true;
    int fndcnt = 1;
    while (repeat && fndcnt) {
        misscnt = missed.size();
        if (misscnt) {
            tris.clear();
            for(i = 0; i < misscnt; i++)
                tris.push_back(missed[i]);
        }
        missed.clear();
        tricnt = tris.size();
        repeat = false;
        fndcnt = 0;
        for (i = 0; i < tricnt; i++) {
            tri1 = tris[i];
            polycnt = polygon.size();
            if (polycnt < 3) {
                // first gets added always
                v1 = tri1.GetA();
                v2 = tri1.GetB();
                v3 = tri1.GetC();
                polygon.push_back(v1);
                polygon.push_back(v2);
                polygon.push_back(v3);
                sprtf("Tri %d(%s) added as first.\n",
                        (i + 1),
                        get_vtri_stg(tri1) );
            } else {
                // need the interator for the 'insert', so
                Vector2dVector::iterator it = polygon.begin();
                Vector2dVector::iterator ite = polygon.end();
                bool found = false;
                for (j = 0; j < polycnt, it != ite; j++, it++) {
                    j2 = j + 1;
                    if (j2 >= polycnt)
                        j2 = 0;
                    v1 = polygon[j];
                    v2 = polygon[j2];
                    line.Set(v1,v2);
                    match = SharedEdgeLine2Tri( line, tri1, v3 );
                    if (match) {
                        // add TRI to POLYGON
                        it++;   // is this ALWAYS the case??? ***TBC***
                        if (it == ite)
                            it = polygon.begin();
                        polygon.insert(it,v3);
                        fndcnt++;
                        found = true;
                        sprtf("Poly line (%s) shares side with (%s), added pt (%s)\n",
                            get_vline_stg(line),
                            get_vtri_stg(tri1),
                            get_vpoint_stg(v3) );
                        break;
                    }
                }
                if (!found) {
                    missed.push_back(tri1);
                    sprtf("Tri %d(%s) NO share yet [%s]\n",
                        (i + 1),
                        get_vtri_stg(tri1),
                        get_vpoly_stg(polygon) );
                    repeat = true;
                }
            }
        }
    }
    sprtf("Built poly [%s]\n", get_vpoly_stg(polygon) );
    misscnt = missed.size();
    if (misscnt) {
        sprtf("But MISSED %d tris...\n");
        for (i = 0; i < misscnt; i++)
            sprtf("(%s)", get_vtri_stg(missed[i]));
        sprtf("\n");
        bret = false;
    }
    return bret;
}



// eof - triangulate.cxx
// eof - ap4_tri.cxx
