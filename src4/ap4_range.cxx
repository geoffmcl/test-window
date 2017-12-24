// ap4_range.cxx
// Given a double max,min, and tick, compute range
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "testap4.hxx"
#include <functional> 
#include <iostream> 
#include <utility>
#include <limits>
#include <math.h>
#include <float.h>  // for things like DBL_EPSILON
#include <algorithm> // for std::max, ...

#define TA_PRINTF   sprtf

const double epsilon = std::numeric_limits<double>::epsilon();
const double epsilon2 = DBL_EPSILON; // same as above, i think...

static char _s_fmt[256];
static char _s_tmpbuf[1024];


GetTicks::GetTicks(): _initted(false)
{
}


int GetTicks::init_ticks(double d1, double d2, int ticks)
{
    _ticks = (double)ticks;
    if (d1 < d2) {
        _min = d1;
        _max = d2;
    } else {
        _min = d2;
        _max = d1;
    }
    _range = _max - _min;   // range
    _actual = _range / _ticks;  // get average

    // get exponent and matissa - as power of 10
    _exponent = (int)(floor(log10(_actual)));
    _mantissa = _actual / pow(10.0, _exponent);

    // why these ranges - seem related to wanting 10 divisions???
    get_mult_and_sub(ticks, _mantissa, &_mult, &_sub, 1);

    _decimals = -_exponent - _sub;
    _decimals = std::max(_decimals, 0);

    _d = _mult * pow(10.0, _exponent);
    _D = _d * 5;

    _first = floor(_min / _d) * _d;
    _last  = ceil(_max / _d) * _d;
    _intervals = (int)((_last - _first) / _d);
    sprintf(_fmt,"%%.%df", _decimals);
    while (test_set_values(_intervals, _fmt, _first, _d)) {
        _decimals++;
        sprintf(_fmt,"%%.%df", _decimals);
    }
    _initted = true;

    return _intervals;
}


int show_range( GetTicks & gt )
{
    double  * pd;
    char * cp = _s_tmpbuf;
    int i, j;
    double val,tst;
    int max;
    if (!gt._initted)
        return 0;

    max = (gt._intervals + 1);
    TA_PRINTF("Ticks %d[", max );
    pd = (double *)malloc( sizeof(double) * (max + 1));

    for (i = 0; i < max; i++) {
        val = gt._first + ((double)i * gt._d);
        sprintf(cp,gt._fmt,val);
        TA_PRINTF(cp);
        //TA_PRINTF("(%f)",val);
        tst = atof(cp);
        pd[i] = tst;
        if (i < gt._intervals)
            TA_PRINTF(" ");
    }
    TA_PRINTF("] from %d %g %g\n", (int)gt._ticks, gt._min, gt._max);
    for (i = 0; i < max; i++) {
        for (j = i + 1; j < max; j++) {
            if (diff_lt_epsilon(pd[i],pd[j]))
            {
                TA_PRINTF("ERROR:2: offset %d == %d - %f and %f\n",
                    i, j, pd[i], pd[j] );
            }
        }
    }
    free(pd);
    TA_PRINTF("(mult=%d, mant=%g, exp=%d, sub=%d, _d=%f)",
        (int)gt._mult, gt._mantissa, gt._exponent, gt._sub, gt._d);
    TA_PRINTF(" form=[%s]", gt._fmt);

    TA_PRINTF("\n");
    return 1;
}

void get_mult_and_sub(int ticks, double mantissa, double * pmult, int * psub, int f)
{
    if (mantissa < 2.0) {
        *pmult = 2.0;
        *psub = 1;
    } else if (mantissa < 5.0) {
        *pmult = 5.0;
        //if (f)
        //    *psub = 1;
        //else
            *psub = 0;
    } else {
        *pmult = 10.0;
        *psub = 1;
    }
}

int diff_lt_epsilon(double d1, double d2)
{
    double diff = fabs(d1 - d2);
    if (diff > epsilon)
        return 0;
    return 1;
}

bool test_set_values(int intervals, char * fmt, double from, double _d)
{
    bool bret = false;
    char * cp = _s_tmpbuf;
    double *pd = (double *)malloc(sizeof(double) * (intervals + 2));
    int i, j;
    double tick, tst;
    for (i = 0; i <= intervals; i++) {
        tick = from + ((double)i * _d);
        sprintf(cp,fmt,tick);
        tst = atof(cp);
        pd[i] = tst;
    }
    for (i = 0; i <= intervals; i++) {
        for (j = (i + 1); j <= intervals; j++) {
            if (diff_lt_epsilon(pd[i],pd[j])) {
                bret = true;
                break;
            }
        }
    }
    free(pd);
    return bret;
}

static MRNG m_rng;
PMRNG get_range( double lat1, double lat2, int ticks )
{
    PMRNG pmr = &m_rng;
    char * cp = _s_tmpbuf;
    double * pd;
    double _d, _D, _first, _last;
    int _decimals, exponent, sub, intervals, i, j;
    char * fmt = _s_fmt;
    double range;
    double mult, mantissa;
    double actual, _min, _max, from, tick, act2, act3, tst;
    int exp2, exp10, dec10;
    double mant2, mant10;
    if (lat1 < lat2) {
        _min = lat1;
        _max = lat2;
    } else {
        _min = lat2;
        _max = lat1;
    }
    pmr->_min = _min;
    pmr->_max = _max;

    //if (get_range_class(lat1,lat2,ticks))
    // return;
    TA_PRINTF("\n");

    range = _max - _min;

    actual = range / (double)ticks;

    exponent = (int)(floor(log10(actual)));
    mantissa = actual / pow(10.0, exponent);

    mant2 = frexp( actual, &exp2 );
    act2 = ldexp( mant2, exp2 );
    act3 = mant2 * pow(2.0, exp2);

    TA_PRINTF("_min = %f, _max = %f, ticks = %d, range = %f,\n",
        _min, _max, ticks, range );
    TA_PRINTF("actual = %f, exponent %d, matissa %f (e2=%d, m2=%g, a2=%g a3=%f)\n",
        actual, exponent, mantissa, exp2, mant2, act2, act3);

    get_mult_and_sub(ticks, mantissa, &mult, &sub, 0);

    _decimals = -exponent - sub;
    _decimals = std::max(_decimals, 0);

    _d = mult * pow(10.0, exponent);
    _D = _d * 5;

    _first = floor(_min / _d) * _d;
    _last  = ceil(_max / _d) * _d;
    intervals = (int)((_last - _first) / _d);

    sprintf(fmt, "%%.%df", _decimals);
    exp10 = (int)(floor(log10(_d)));
    mant10 = _d / pow(10.0, exp10);
    dec10 = -exp10;
    dec10 = std::max(dec10, 0);
    TA_PRINTF("mult = %.0f, sub = %d, _d = %f, _D = %f, dec %d, fmt = [%s] (e10=%d, d10=%d, m10=%f)\n",
        mult, sub, _d, _D, _decimals, fmt, exp10, dec10, mant10 );

    while (test_set_values(intervals, fmt, _first, _d)) {
        _decimals++;
        sprintf(fmt, "%%.%df", _decimals);
        TA_PRINTF("mult = %.0f, sub = %d, _d = %f, _D = %f, dec %d, fmt = [%s]\n",
            mult, sub, _d, _D, _decimals, fmt );
    }

    TA_PRINTF("First ");
    TA_PRINTF(fmt, _first);
    TA_PRINTF(" %d[", intervals + 1);
    from = _first;
    pmr->_intervals = intervals;
    strcpy(pmr->_format,fmt);
    pmr->_d = _d;
    pmr->_D = _D;
    pd = (double *)malloc(sizeof(double) * (intervals + 2));
    for (i = 0; i <= intervals; i++) {
        tick = from + ((double)i * _d);
        sprintf(cp,fmt,tick);
        TA_PRINTF(cp);
        if (i < intervals)
            TA_PRINTF(" ");
        tst = atof(cp);
        pd[i] = tst;
    }

    TA_PRINTF("]%d Last ", ticks);
    TA_PRINTF(fmt, _last);
    TA_PRINTF(" fmt = [%s]", fmt);

    TA_PRINTF("\n");
    for (i = 0; i <= intervals; i++) {
        for (j = (i + 1); j <= intervals; j++) {
            if (diff_lt_epsilon(pd[i],pd[j])) {
                TA_PRINTF("ERROR:1: offset %d == %d - %f and %f\n",
                    i, j, pd[i], pd[j] );
            }
        }
    }
    free(pd);
    //get_range_class(lat1,lat2,ticks);
    return pmr;
}


// eof - ap4_range.cxx
