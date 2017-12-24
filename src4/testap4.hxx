// testap4.hxx
#ifndef _testap4_hxx_
#define _testap4_hxx_

#pragma warning(disable:4996)

#include "ap4_vers.hxx"

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#include "resource.h"
#include "ap4_paint.hxx"
#include "ap4_command.hxx"
#include "ap4_about.hxx"
#include "ap4_tri.hxx"
#include "ap4_maths.hxx"
#include "ap4_utils.hxx"
#include "ap4_sprtf.hxx"
#include "ap4_test.hxx"
#include "ap4_ini.hxx"
#include "ap4_timer.hxx"
#include "ap4_range.hxx"
#include "ap4_list.hxx"
#include "ap4_track.hxx"

extern HINSTANCE g_hInst;								// current instance
extern HWND g_hWnd;                                    // main window
extern SIZE g_wm_size;
extern BOOL g_bShiftDown;

#define MAX_LOADSTRING 100
#define  MALLOC(a)      new BYTE[a]
#define  MFREE(a)       delete a

#ifdef ADD_LISTVIEW
extern HWND g_hListView;
#endif // ADD_LISTVIEW

#endif // #ifndef _testap4_hxx_
// eof -testap4.hxx
