// ap4_paint.hxx
#ifndef _ap4_paint_hxx_
#define _ap4_paint_hxx_

extern VOID Do_WM_PAINT( HWND hWnd );
extern void Do_WM_MOUSEMOVE( HWND hWnd, WPARAM wParam, LPARAM lParam );

extern void delete_drawing_tools(void);
extern void create_drawing_tools(void);

extern void Init_Vector(void);
extern void ResetSet_Factor(void);
extern void paint_factor(HDC hdc);
extern int Change_Factor(int dir);

extern RECT g_ParentRect, g_ClientRect;

extern HFONT Get_Font();

#endif // #ifndef _ap4_paint_hxx_
// eof - ap4_paint.hxx

