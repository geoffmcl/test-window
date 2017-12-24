// ap4_sprtf.hxx
#ifndef _ap4_sprtf_hxx_
#define _ap4_sprtf_hxx_

extern int open_log_file( void );
extern void set_log_file( char * nf );
extern char * get_log_file(void);
extern void close_log_file( void );
extern int _cdecl sprtf( char * pf, ... );

#define  VFP(a)   ( a && ( a != (FILE *)-1 ))

#undef printf
#define printf sprtf

#endif // #ifndef _ap4_sprtf_hxx_
// eof - ap4_sprtf.hxx
