// ap4_utils.hxx
#ifndef _ap4_utils_hxx_
#define _ap4_utils_hxx_

#define BUF_LEN 512     // size of each buffer
#define BUF_CNT 256     // 256 cyclic buffers
#define BUF_BIG 1024    // one BIG buffer

extern char * getnxtbuf(void);
extern char * get_vpoint_stg( Vector2d & v );
extern char * get_vpoint_stg( double * v );
extern char * get_vline_stg( Vector2d & v1, Vector2d & v2 );
extern char * get_vline_stg( Vector2dLine line );
extern char * get_vtri_stg( Vector2d v1, Vector2d v2, Vector2d v3 );
extern char * get_vtri_stg( Vector2dTri & t );
extern char * get_vpoly_stg( Vector2dVector v );

char * get_formatted_double( double d, int max = 6 );

#ifndef EndBuf
#define EndBuf(a)   ( a + strlen(a) )
#endif

extern void out_time_message( char * msg );
extern char * get_time_hhmmss_stg(void);

#endif //#ifndef _ap4_utils_hxx_
// eof - ap4_utils.hxx

