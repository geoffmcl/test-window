// testap4.cpp : Defines the entry point for the application.
//
#include "testap4.hxx"
#include <windowsx.h>

// Global Variables:
HINSTANCE g_hInst;								// current instance
HWND g_hWnd;                                    // main window
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
BOOL g_bShiftDown = FALSE;
#ifdef ADD_LISTVIEW
HWND g_hListView = 0;
#endif // ADD_LISTVIEW

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
VOID Do_WM_KEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam );
VOID Do_WM_KEYUP( HWND hWnd, WPARAM wParam, LPARAM lParam );
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
//   case WM_MOUSEWHEEL:
VOID Do_WM_MOUSEWHEEL( HWND hWnd, WPARAM wParam, LPARAM lParam );
#endif   // #if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)

#define  MB(a) MessageBox( NULL, a, "CRITICAL ERROR", MB_OK | MB_ICONINFORMATION )

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_testap4, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

    ReadINI();

    // ParseCommandLine( lpCmdLine );
    // OR
    // ParseArgs(__argc, __argv)


	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

#ifdef	ADD_TIMER1
	if( !(uiTimer1 = SetTimer( g_hWnd, TIMER_ID1, TIMER_INTERVAL1, NULL)) )
	{
      MB( "Failed in get Timer ... aborting ..." );
      return FALSE;
	}
#endif	/* ADD_TIMER1 */

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_testap4));

    out_time_message( "application begins" );

    // Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    delete_drawing_tools();
    WriteINI();
    out_time_message( "application ends" );
    close_log_file();

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_testap4));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_testap4);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   g_hInst = hInstance; // Store instance handle in our global variable

   g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }

#ifdef ADD_LISTVIEW
   g_hListView = LVCreateListView (g_hWnd);
   if (!g_hListView) 
   {
       return FALSE;
   }
#endif // ADD_LISTVIEW

   create_drawing_tools();
   Init_Vector();

#ifdef ADD_LISTVIEW
   HFONT hf = Get_Font();
   if (hf) SendMessage(g_hListView, WM_SETFONT, (WPARAM)hf, TRUE);

   char *hdg = "Output Strings";
   char *stgs[1];
   stgs[0] = hdg;
   if ( LVInitColumns( 1, stgs) ) {
       ShowWindow(g_hListView, SW_SHOW);  // let the PAINT go
   } else {
       return FALSE;
   }
#endif // ADD_LISTVIEW
   if( gbGotWP )
      SetWindowPlacement(g_hWnd,&g_sWP);
   else
      ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   return TRUE;
}

SIZE g_wm_size;
// WM_SIZE
VOID Do_WM_SIZE( HWND hWnd, LPARAM lParam )
{
    g_wm_size.cx = LOWORD(lParam);
    g_wm_size.cy = HIWORD(lParam);   // width and height

    GetClientRect(hWnd, &g_ParentRect);
    g_ClientRect = g_ParentRect;
    //double width = (double)(g_ClientRect.right - (2 * border));
    //double height = (double)(g_ClientRect.bottom - (2 * border));
    g_ClientRect.top += 20;
    g_ClientRect.bottom -= 20;
    g_ClientRect.left += 20;
    g_ClientRect.right -= 20;
   //SetScrollRanges(hWnd, LOWORD(lParam), HIWORD(lParam));

    UpdateWP( hWnd );

#ifdef ADD_LISTVIEW
    RECT rc = g_ParentRect;
    rc.left = (rc.right / 4) * 3;
    LVSize( hWnd, &rc );
    g_ClientRect.right = rc.left - 20;
#endif // ADD_LISTVIEW

   //Do_ResizeStatus( hWnd );
    ResetSet_Factor();

}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
	switch (message)
	{
	case WM_COMMAND:
        lRes = Do_WM_COMMAND( hWnd, message, wParam, lParam);
		break;
   case WM_SIZE:
      Do_WM_SIZE( hWnd, lParam );
      break;
   case WM_MOVE:
      UpdateWP( hWnd );
      break;
	case WM_PAINT:
        Do_WM_PAINT( hWnd );
		break;
   case WM_MOUSEMOVE:
      Do_WM_MOUSEMOVE( hWnd, wParam, lParam );
      break;
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
   case WM_MOUSEWHEEL:
      Do_WM_MOUSEWHEEL( hWnd, wParam, lParam );
      lRes = TRUE;
      break;
#endif   // #if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
   case WM_KEYDOWN:
      Do_WM_KEYDOWN( hWnd, wParam, lParam );
      break;
   case WM_KEYUP:
      Do_WM_KEYUP( hWnd, wParam, lParam );
      break;
#ifdef	ADD_TIMER1
   case WM_TIMER:
      Do_WM_TIMER( hWnd );
      break;
#endif	/* ADD_TIMER1 */
#ifdef ADD_MOUSE_CAPTURE
    // If the user hits the left mouse button, track mouse
	case WM_LBUTTONDOWN:
        Do_WM_LBUTTONDOWN( hWnd, wParam, lParam );
        break;
#endif // #ifndef ADD_MOUSE_CAPTURE
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		lRes = DefWindowProc(hWnd, message, wParam, lParam);
        break;
	}
	return lRes;
}

VOID Do_WM_KEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
   /* Translate keyboard messages to scroll commands */
   switch (wParam)
   {
   case VK_UP:
        //PostMessage (hWnd, WM_VSCROLL, SB_LINEUP,   0L);
        break;

    case VK_DOWN:
        //PostMessage (hWnd, WM_VSCROLL, SB_LINEDOWN, 0L);
        break;

    case VK_PRIOR:
        //PostMessage (hWnd, WM_VSCROLL, SB_PAGEUP,   0L);
        break;

    case VK_NEXT:
        //PostMessage (hWnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
        break;

    case VK_HOME:
        //PostMessage (hWnd, WM_HSCROLL, SB_PAGEUP,   0L);
        break;

    case VK_END:
        //PostMessage (hWnd, WM_HSCROLL, SB_PAGEDOWN, 0L);
        break;

    case VK_LEFT:
        //PostMessage (hWnd, WM_HSCROLL, SB_LINEUP,   0L);
        break;

    case VK_RIGHT:
        //PostMessage (hWnd, WM_HSCROLL, SB_LINEDOWN, 0L);
        break;
//#ifdef   ADDMAG2
    case VK_SHIFT:
       g_bShiftDown = TRUE;
       break;
//#endif   // ADDMAG2
   }
}

VOID Do_WM_KEYUP( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
   switch (wParam)
   {
   case VK_UP:
   case VK_DOWN:
   case VK_PRIOR:
   case VK_NEXT:
      //PostMessage (hWnd, WM_VSCROLL, SB_ENDSCROLL, 0L);
      break;

   case VK_HOME:
   case VK_END:
   case VK_LEFT:
   case VK_RIGHT:
      //PostMessage (hWnd, WM_HSCROLL, SB_ENDSCROLL, 0L);
      break;
//#ifdef   ADDMAG2
   case VK_SHIFT:
       g_bShiftDown = FALSE;
       break;
//#endif   // ADDMAG2
   }
}

#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
/* =======
wParam
The high-order word indicates the distance the wheel is rotated, 
expressed in multiples or divisions of WHEEL_DELTA, which is 120. 

A positive value indicates that the wheel was rotated forward, away 
from the user; a negative value indicates that the wheel was rotated 
backward, toward the user. 

The low-order word indicates whether various virtual keys are down. 
This parameter can be one or more of the following values. 
MK_CONTROL = The CTRL key is down.
MK_LBUTTON = The left mouse button is down.
MK_MBUTTON = The middle mouse button is down.
MK_RBUTTON = The right mouse button is down.
MK_SHIFT   = The SHIFT key is down.
MK_XBUTTON1 = Windows 2000/XP: The first X button is down.
MK_XBUTTON2 = Windows 2000/XP: The second X button is down.
lParam
The low-order word specifies the x-coordinate of the pointer,
 relative to the upper-left corner of the screen. 
The high-order word specifies the y-coordinate of the pointer,
 relative to the upper-left corner of the screen. 
   ======= */
VOID Do_WM_MOUSEWHEEL( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
   int fwKeys = GET_KEYSTATE_WPARAM(wParam);
   int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
   int xPos = GET_X_LPARAM(lParam); 
   int yPos = GET_Y_LPARAM(lParam); 
   WPARAM Direction;
   Direction = (zDelta < 0) ? SB_LINEDOWN : SB_LINEUP;
    //gtab_msg_vscroll(hwnd, ptab,
    //((HIWORD(wParam) & 0x8000) ? SB_LINEDOWN : SB_LINEUP), 0 );
   //PostMessage (hWnd, WM_VSCROLL, Direction, 0L);
    Change_Factor( (int)Direction );
    InvalidateRect(hWnd, NULL, TRUE);
}
#endif   // #if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)

// eof - testap4.cxx
