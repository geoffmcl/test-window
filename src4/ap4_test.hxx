// ap4_test.hxx
#ifndef _ap4_test_hxx_
#define _ap4_test_hxx_

typedef struct tagVLIMS {
    bool valid;
    double minx, maxx;
    double miny, maxy;
}VLIMS, * PVLIMS;

extern void Get_Polygon_Vectors( Vector2dVector &a );
extern void Get_Limits_of_Vector( Vector2dVector &a, PVLIMS pvl );

#endif // #ifndef _ap4_test_hxx_
// eof - ap4_test.hxx


