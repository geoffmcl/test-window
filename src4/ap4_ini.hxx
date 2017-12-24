// ap4_ini.hxx
#ifndef _ap4_Ini_hxx_
#define _ap4_Ini_hxx_

extern void WriteINI( void );
extern void ReadINI( void );
extern VOID UpdateWP( HWND hwnd );

extern PTSTR GetINIFile( void );
extern void  GetModulePath( PTSTR lpb );

extern BOOL  gbGotWP;
extern WINDOWPLACEMENT   g_sWP;

#endif // #ifndef _ap4_Ini_hxx_
// eof - ap4_ini.hxx

