#pragma once

#include "resource.h"

typedef struct _CONTROL_DATA {
    HWND hWnd;
    LPCWSTR lpszClass;
    LPCWSTR lpszCaption;
    DWORD dwStyle;
    int x, y, w, h;
    UINT nId;
}CONTROL_DATA;