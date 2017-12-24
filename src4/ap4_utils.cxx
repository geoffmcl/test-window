// ap4_utils.cxx
#include "testap4.hxx"

static char _s_buffer[ BUF_LEN * BUF_CNT ];
static int _s_nxt_buf = 0;
char * getnxtbuf(void)
{
    _s_nxt_buf++;
    if (_s_nxt_buf >= BUF_CNT)
        _s_nxt_buf = 0;

    return &_s_buffer[ BUF_LEN * _s_nxt_buf ];
}

static char _s_bigbuffer[BUF_BIG];
char * getbigbuf(void) { return _s_bigbuffer; }

char * get_vpoint_stg( Vector2d & v )
{
    char * nb = getnxtbuf();
    sprintf(nb,"%s,%s",
        get_formatted_double(v.GetX()),
        get_formatted_double(v.GetY()));
    return nb;
}
char * get_vpoint_stg( double * pv )
{
    Vector2d v(pv[0],pv[1]);
    return get_vpoint_stg(v);
}

#if 0 // =========================
char * get_vpoint_stg( Vector2d & v )
{
    char * nb = getnxtbuf();
    sprintf(nb,"%0.0f,%0.0f", v.GetX(), v.GetY());
    return nb;
}
char * get_vpoint_stg( double * v )
{
    char * nb = getnxtbuf();
    sprintf(nb,"%0.0f,%0.0f", v[0], v[1]);
    return nb;
}
#endif // 0

char * get_vline_stg( Vector2d & v1, Vector2d & v2 )
{
    char * p1 = get_vpoint_stg(v1);
    char * p2 = get_vpoint_stg(v2);
    char * nb = getnxtbuf();
    sprintf(nb,"%s-%s", p1, p2);
    return nb;
}
char * get_vline_stg( Vector2dLine line )
{
    Vector2d v1 = line.GetA();
    Vector2d v2 = line.GetB();
    return get_vline_stg( v1, v2 );
}

char * get_vtri_stg( Vector2d tA, Vector2d tB, Vector2d tC )
{
    char * p1 = get_vpoint_stg(tA);
    char * p2 = get_vpoint_stg(tB);
    char * p3 = get_vpoint_stg(tC);
    char * nb = getnxtbuf();
    sprintf(nb,"%s,%s,%s", p1, p2, p3);
    return nb;
}

char * get_vtri_stg( Vector2dTri & t )
{
    Vector2d tA = t.GetA();
    Vector2d tB = t.GetB();
    Vector2d tC = t.GetC();
    return get_vtri_stg( tA, tB, tC );
}

char * get_vpoly_stg( Vector2dVector v )
{
    char * bb = getbigbuf();
    Vector2dVector::iterator i = v.begin();
    Vector2dVector::iterator ie = v.end();
    *bb = 0;
    for( ; i != ie; i++ ) {
        Vector2d v1 = *i;
        if (*bb) strcat(bb,",");
        sprintf(EndBuf(bb),"(%s)", get_vpoint_stg(v1));
    }
    return bb;
}

char * get_formatted_double( double d, int max )
{
    char form[64];
    char * nb = getnxtbuf();
    sprintf(form,"%%.%df",max);
    int len = sprintf(nb,form,d);
    while (len--) {
        if (nb[len] == '0')
            nb[len] = 0;
        else if (nb[len] == '.') {
            nb[len] = 0;
            break;
        } else
            break;
    }
    return nb;
}

// =========================================
// some time functions
// =========================================
void out_time_message( char * msg )
{
    SYSTEMTIME st;
    char * nb = getnxtbuf();
    GetLocalTime(&st);
    sprtf("%02d:%02d:%02d: %s\n",
        st.wHour & 0xffff,
        st.wMinute & 0xffff,
        st.wSecond & 0xffff,
        msg );
}

char * get_time_hhmmss_stg(void)
{
    SYSTEMTIME st;
    char * nb = getnxtbuf();
    GetLocalTime(&st);
    sprintf(nb,"%02d:%02d:%02d",
        st.wHour & 0xffff,
        st.wMinute & 0xffff,
        st.wSecond & 0xffff );
    return nb;
}

// ===============================================

// eof - ap4_utils.cxx
