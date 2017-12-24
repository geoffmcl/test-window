// ap4_about.cxx

#include "testap4.hxx"
#include <direct.h>     // for _getcwd()

// Center a CHILD on a PARENT window
// =================================
BOOL CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT    rcChild, rcParent;
    int     cxChild, cyChild, cxParent, cyParent;
    int     cxScreen, cyScreen, xNew, yNew;
    HDC     hdc;

    // Get the Height and Width of the child window
    GetWindowRect(hwndChild, &rcChild);
    cxChild = rcChild.right - rcChild.left;
    cyChild = rcChild.bottom - rcChild.top;

    // Get the Height and Width of the parent window
    GetWindowRect(hwndParent, &rcParent);
    cxParent = rcParent.right - rcParent.left;
    cyParent = rcParent.bottom - rcParent.top;

    // Get the display limits
    hdc = GetDC(hwndChild);
    cxScreen = GetDeviceCaps(hdc, HORZRES);
    cyScreen = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);

    // Calculate new X position, then adjust for screen
    xNew = rcParent.left + ((cxParent - cxChild) / 2);
    if (xNew < 0)
        xNew = 0;
    else if ((xNew + cxChild) > cxScreen)
        xNew = cxScreen - cxChild;

    // Calculate new Y position, then adjust for screen
    yNew = rcParent.top  + ((cyParent - cyChild) / 2);
    if (yNew < 0)
        yNew = 0;
    else if ((yNew + cyChild) > cyScreen)
        yNew = cyScreen - cyChild;

    // Set it, and return
    return SetWindowPos(hwndChild,
                        NULL,
                        xNew, yNew,
                        0, 0,
                        SWP_NOSIZE | SWP_NOZORDER);
}	// end - BOOL CenterWindow(HWND hwndChild, HWND hwndParent)

char * get_cwd(void)
{
    char * nb = getnxtbuf();
    *nb = 0;
    _getcwd(nb,256);
    return nb;
}

INT_PTR Do_About_WM_INITDIALOG( HWND hDlg )
{
    char * cp = getnxtbuf();
    out_time_message("Doing: Do_About_WM_INITDIALOG()");

    sprintf(cp,"testap4 Version %s", VERS_DATE);
    SetDlgItemText(hDlg,IDC_VERSION,cp);
    sprtf("Set IDC_VERSION: %s\n", cp);

    sprintf(cp,"Build: %s, at %s", __DATE__, __TIME__);
    SetDlgItemText(hDlg,IDC_DATE,cp);
    sprtf("Set IDC_DATE: %s\n", cp);

    strcpy(cp,"Some line, triangle, and polygon tests");
    SetDlgItemText(hDlg,IDC_LINE1,cp);
    sprtf("Set IDC_LINE1: %s\n", cp);

    sprintf(cp,"INI: [%s]", GetINIFile());
    SetDlgItemText(hDlg,IDC_LINE2,cp);
    sprtf("Set IDC_LINE2: %s\n", cp);

    sprintf(cp,"LOG: [%s]",get_log_file());
    SetDlgItemText(hDlg,IDC_LINE3,cp);
    sprtf("Set IDC_LINE3: %s\n", cp);

    sprintf(cp,"CWD: [%s]",get_cwd());
    SetDlgItemText(hDlg,IDC_LINE4,cp);
    sprtf("Set IDC_LINE4: %s\n", cp);

    CenterWindow( hDlg, GetWindow( hDlg, GW_OWNER ) );
    return (INT_PTR)TRUE;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_About_WM_INITDIALOG(hDlg);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// eof - ap4_about.cxx
