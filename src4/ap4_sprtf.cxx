// ap4_sprtf.cxx
// sprtf_tri.cxx

#include "testap4.hxx"
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning( disable:4996 )
#endif // #ifndef _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define USE_MODULE_PATH
#define  MXIO     256
char def_log[] = "tempap4.txt";
char logfile[256] = "\0";
FILE * outfile = NULL;

int   open_log_file( void )
{
    if (logfile[0] == 0) {
#ifdef USE_MODULE_PATH
      GetModulePath( &logfile[0] );    // does   GetModuleFileName( NULL, lpini, 256 );
      strcat(logfile, def_log);
#else
      strcpy(logfile,def_log);
#endif
    }
    outfile = fopen(logfile, "wb");
    if( outfile == 0 ) {
        outfile = (FILE *)-1;
        printf("WARNING: Failed to open log file [%s] ...\n", logfile);
        return 0;   /* failed */
    }
    return 1; /* success */
}

void close_log_file( void )
{
   if( VFP(outfile) ) {
      fclose(outfile);
   }
   outfile = NULL;
}

void   set_log_file( char * nf )
{
   if (logfile[0] == 0)
      strcpy(logfile,def_log);
   if ( nf && *nf && strcmpi(nf,logfile) ) {
      close_log_file(); // remove any previous
      strcpy(logfile,nf); // set new name
      open_log_file();  // and open it ... anything previous written is 'lost'
   }
}

char * get_log_file(void)
{ 
    if (logfile[0] == 0)
        strcpy(logfile, def_log);
    return &logfile[0];
}

static void oi( char * pc )
{
   int len = (int)strlen(pc);
   if(len) {
      if( outfile == 0 ) {
         open_log_file();
      }
      if (g_hListView)
          LVInsertItem(pc);

      if( VFP(outfile) ) {
         int w = (int)fwrite( pc, 1, len, outfile );
         if( w != len ) {
            fclose(outfile);
            outfile = (FILE *)-1;
            printf("WARNING: Failed write to log file [%s] ...\n", logfile);
         }
         fflush( outfile );
      }
      //fwrite( pc, 1, len, stdout );
      //printf(pc);
   }
}

static void	prt( char * ps )
{
   static char buf[MXIO + 32];
	char * pb = buf;
	size_t i, j, k;
	char   c, d;
   i = strlen(ps);
	if(i) {
		k = 0;
		d = 0;
		for( j = 0; j < i; j++ )
		{
			c = ps[j];
			if( c == 0x0d ) {
				if( (j+1) < i ) {
					if( ps[j+1] != 0x0a ) {
						pb[k++] = c;
						c = 0x0a;
					}
            } else {
					pb[k++] = c;
					c = 0x0a;
				}
			} else if( c == 0x0a ) {
				if( d != 0x0d ) {
					pb[k++] = 0x0d;
				}
			}
			pb[k++] = c;
			d = c;
			if( k >= MXIO ) {
				pb[k] = 0;
				oi( pb );
				k = 0;
			}
		}	// for length of string
		if( k ) {
			//if( ( gbCheckCrLf ) &&
			//	( d != 0x0a ) ) {
				// add Cr/Lf pair
				//pb[k++] = 0x0d;
				//pb[k++] = 0x0a;
				//pb[k] = 0;
			//}
			pb[k] = 0;
			oi( pb );
		}
	}
}

int _cdecl sprtf( char * pf, ... )
{
   static char _s_sprtfbuf[1024];
   char * pb = _s_sprtfbuf;
   int   i;
   va_list arglist;
   va_start(arglist, pf);
   i = vsprintf( pb, pf, arglist );
   va_end(arglist);
   prt(pb);
   return i;
}

// eof - sprtf_tri.cxx
// eof - ap4_sprtf.cxx
