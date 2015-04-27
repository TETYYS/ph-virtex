/*
 * From original Process Hacker source code, as plugin SDK doesn't have this.
 */

#ifndef _PH_COLORBOX_H
#define _PH_COLORBOX_H

#include <windows.h>

#define PH_COLORBOX_CLASSNAME L"PhColorBox"

BOOLEAN PhColorBoxInitialization(
	VOID
	);

#define CBCM_SETCOLOR (WM_APP + 1501)
#define CBCM_GETCOLOR (WM_APP + 1502)

#define ColorBox_SetColor(hWnd, Color) \
	SendMessage((hWnd), CBCM_SETCOLOR, (WPARAM)(Color), 0)

#define ColorBox_GetColor(hWnd) \
	((COLORREF)SendMessage((hWnd), CBCM_GETCOLOR, 0, 0))

#endif
