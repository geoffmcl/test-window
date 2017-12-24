// ap4_list.hxx
#ifndef _AP4_LIST_HXX_
#define _AP4_LIST_HXX_


#pragma comment ( lib, "COMCTL32" )

extern HWND LVCreateListView (HWND hwndParent);
extern int LVSize( HWND hWnd, PRECT pr );

extern BOOL LVInitColumns( int cols, char **pstgs);
extern BOOL LVInsertItem(char *pstg);
extern BOOL LVInsertItems(int cItems, char **pstgs);

#endif // #ifndef _AP4_LIST_HXX_
