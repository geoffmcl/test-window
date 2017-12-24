// ap4_range.hxx
#ifndef _ap4_range_hxx_
#define _ap4_range_hxx_

typedef struct tagMRNG {
    double _min;
    double _max;
    double _d, _D, _first, _last;
    int _intervals;
    char _format[16];
}MRNG, * PMRNG;

extern PMRNG get_range( double lat1, double lat2, int ticks );

// OR

class GetTicks 
{
public:
    GetTicks();
    ~GetTicks() { }
    int init_ticks(double d1, double d2, int ticks);
    double _min,_max,_d,_D,_range,_actual,_ticks,_mantissa,_first,_last,_mult;
    int _exponent, _decimals, _intervals, _sub;
    char _fmt[256];
    bool _initted;
};


// some workers
extern void get_mult_and_sub(int ticks, double mantissa, double * pmult, int * psub, int f);
extern int diff_lt_epsilon(double d1, double d2);
extern bool test_set_values(int intervals, char * fmt, double from, double _d);

#endif // #ifndef _ap4_range_hxx_
// eof - ap4_range.hxx



