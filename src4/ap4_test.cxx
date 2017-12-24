// ap4_test.cxx

#include "testap4.hxx"

void Get_Limits_of_Vector( Vector2dVector &a, PVLIMS pvl )
{
    size_t max, i;
    double minx;
    double maxx;
    double miny;
    double maxy;
    double x,y;
    DMM dmm;

    dmm.pmaxx = &maxx;
    dmm.pmaxy = &maxy;
    dmm.pminx = &minx;
    dmm.pminy = &miny;

    set_dbl_max_min( &dmm );

    max = a.size();
    for (i = 0; i < max; i++) {
        Vector2d v = a[i];
        x = v.GetX();
        y = v.GetY();
        if (x > maxx)
            maxx = x;
        if (x < minx)
            minx = x;
        if (y > maxy)
            maxy = y;
        if (y < miny)
            miny = y;
    }
    pvl->maxx = maxx;
    pvl->maxy = maxy;
    pvl->minx = minx;
    pvl->miny = miny;
    pvl->valid = TRUE;
}

void Get_Polygon_Vectors_org( Vector2dVector &a )
{
    a.push_back( Vector2d(0,6));
    a.push_back( Vector2d(0,0));
    a.push_back( Vector2d(3,0));
    a.push_back( Vector2d(4,1));
    a.push_back( Vector2d(6,1));
    a.push_back( Vector2d(8,0));
    a.push_back( Vector2d(12,0));
    a.push_back( Vector2d(13,2));
    a.push_back( Vector2d(8,2));
    a.push_back( Vector2d(8,4));
    a.push_back( Vector2d(11,4));
    a.push_back( Vector2d(11,6));
    a.push_back( Vector2d(6,6));
    a.push_back( Vector2d(4,3));
    a.push_back( Vector2d(2,6));
}
void Get_Polygon_Vectors( Vector2dVector &a )
{
    a.push_back( Vector2d(1,6));
    a.push_back( Vector2d(0,0));
    a.push_back( Vector2d(3,0));
    a.push_back( Vector2d(4,1));
    a.push_back( Vector2d(6,1));
    a.push_back( Vector2d(8,0));
    a.push_back( Vector2d(12,1));
    a.push_back( Vector2d(13,3));
    a.push_back( Vector2d(9,2));
    a.push_back( Vector2d(8,5));
    a.push_back( Vector2d(11,4));
    a.push_back( Vector2d(12,8));
    a.push_back( Vector2d(6,7));
    a.push_back( Vector2d(4,3));
    a.push_back( Vector2d(2,7));
}

void run_test(void)
{
    size_t i, tcount;
    // Small test application demonstrating the usage of the triangulate
    // class.
    // Create a pretty complicated little contour by pushing them onto
    // an stl vector.

    Vector2dVector a;
    Get_Polygon_Vectors(a);

    tcount = a.size();
    printf("Got polygon of %d points... x,y as follows...\n", tcount);
    for (i = 0; i < tcount; i++)
    {
        const Vector2d &p = a[i];
        printf("%0.0f,%0.0f ", p.GetX(), p.GetY());
    }
    printf("\nto split into triangles...\n");

    // allocate an STL vector to hold the answer.
    Vector2dVector result;  //  Invoke the triangulator to triangulate this polygon.

    Triangulate::Process(a,result);  // get triange results.

    tcount = result.size()/3;
    printf("Got %d triangles, as follows...\n", tcount);
    for (i = 0; i < tcount; i++)
    {
        const Vector2d &p1 = result[i*3+0];
        const Vector2d &p2 = result[i*3+1];
        const Vector2d &p3 = result[i*3+2];
        printf("Triangle %d => A(%0.0f,%0.0f) B(%0.0f,%0.0f) C(%0.0f,%0.0f) ",
            i+1,p1.GetX(),p1.GetY(),p2.GetX(),p2.GetY(),p3.GetX(),p3.GetY());
        //show_angles(p1,p2,p3);
    }

    Vector2dVector b;
    if ( Triangulate::ReverseProcess(b,result) ) { // get polygon back.
        printf("Success\n");
    } else {
        //printf("FAILED\n");
    }
    //compare_polys(a,b);
    //test_intersection();

}

// eof - ap4_test.cxx
