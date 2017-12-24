// ap4_list.cxx

#include "testap4.hxx"
#include <CommCtrl.h>


// LVCreateListView: Creates a list-view control in report view.
// Returns the handle to the new control
// TO DO:  The calling procedure should determine whether the handle is NULL, in case 
// of an error in creation.
//
// HINST hInst: The global handle to the applicadtion instance.
// HWND  hWndParent: The handle to the control's parent window. 
//
HWND LVCreateListView (HWND hwndParent) 
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    ZeroMemory(&icex,sizeof(INITCOMMONCONTROLSEX));
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    RECT rc;                       // The parent window's client area.
    GetClientRect (hwndParent, &rc);
    rc.left = (rc.right / 4) * 3;

    DWORD style = WS_CHILD | LVS_LIST | WS_BORDER | WS_VISIBLE;

    // Create the list-view window in report view with label editing enabled.
    HWND hWndListView = CreateWindow(WC_LISTVIEW, 
                                     TEXT(""),
                                     style, // WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
                                     rc.left, rc.top,       // x,y position
                                     rc.right - rc.left,    // cx width
                                     rc.bottom - rc.top,    // cy height
                                     hwndParent,
                                     (HMENU)0, //IDM_CODE_SAMPLES,
                                     g_hInst,
                                     NULL); 

    return (hWndListView);
}

int LVSize( HWND hWnd, PRECT pr )
{
    int i = -1;
    if (g_hListView) {
        i = MoveWindow( g_hListView,
			pr->left,              // x
			pr->top,               // y
			(pr->right  - pr->left), // cx
			(pr->bottom - pr->top),  // cy
			TRUE );
    }
    return i;
}

#define MX_TX_SIZE 256
BOOL LVInitColumns( int cols, char **pstgs) 
{ 
    TCHAR szText[MX_TX_SIZE+2];     // Temporary buffer.
    LVCOLUMN lvc;
    int iCol;
    RECT rc;
    int width;

    GetClientRect( g_hListView, &rc );
    width = rc.right - rc.left;
    width /= cols;

    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text,
    // and subitem members of the structure are valid.
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    if (cols > 1)
        lvc.mask |= LVCF_SUBITEM;

    // Add the columns.
    for (iCol = 0; iCol < cols; iCol++)
    {
        // set text
        strncpy(szText,pstgs[iCol], MX_TX_SIZE);
        szText[MX_TX_SIZE] = 0;

        lvc.iSubItem = iCol;
        lvc.pszText = szText;
        lvc.cx = width;               // Width of column in pixels.

        if ( iCol < 2 )
            lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
        else
            lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

        // Insert the columns into the list view.
        if (ListView_InsertColumn(g_hListView, iCol, &lvc) == -1)
            return FALSE;
    }
    
    return TRUE;
} 

BOOL LVInsertItems(int cItems, char **pstgs)
{
    LVITEM lvI;

    // Initialize LVITEM members that are common to all items.
    // lvI.pszText   = pstg; // LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
    lvI.mask      = LVIF_TEXT; // | LVIF_STATE; // | LVIF_IMAGE 
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    // Initialize LVITEM members that are different for each item.
    for (int index = 0; index < cItems; index++)
    {
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.pszText = pstgs[index];
        // Insert items into the list.
        if (ListView_InsertItem(g_hListView, &lvI) == -1)
            return FALSE;
    }

    return TRUE;
}

BOOL LVInsertItem(char *pstg)
{
    LVITEM lvI = {0};
    // Initialize LVITEM members that are common to all items.
    lvI.pszText   = pstg; // LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
    lvI.cchTextMax = (int)strlen(pstg);
    lvI.mask      = LVIF_TEXT; // | LVIF_STATE; // | LVIF_IMAGE 
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;
    lvI.iItem     = (int)SendMessage(g_hListView, LVM_GETITEMCOUNT, 0, 0);
    if (ListView_InsertItem(g_hListView, &lvI) == -1)
        return FALSE;
    ListView_EnsureVisible(g_hListView, lvI.iItem, 0);
    return TRUE;
}


// eof - ap4_list.cxx
