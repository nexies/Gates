#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Windows.h>
#include <Commctrl.h>
#include <shlobj.h>  // Для создания нового ShellView
#include <winuser.h>

// hWnd[1] = CreateListView((HWND)hWnd[0]);
// InitListViewImageLists((HWND)hWnd[1], NULL, NULL);
// SetView((HWND)hWnd[1], LVS_ICON);
// InitListViewColumns((HWND)hWnd[1], 1, IDS_FIRSTCOLUMN);
// InsertListViewItems((HWND)hWnd[1], "Windows", NULL);



void ToggleDesktopIcons(bool hide) {
    HWND hProgman = FindWindow(L"Progman", NULL);
    if (!hProgman) return;

    HWND hDefView = FindWindowEx(hProgman, NULL, L"SHELLDLL_DefView", NULL);
    if (!hDefView) {
        HWND hWorkerW = FindWindowEx(NULL, NULL, L"WorkerW", NULL);
        while (hWorkerW) {
            hDefView = FindWindowEx(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hDefView) break;
            hWorkerW = FindWindowEx(NULL, hWorkerW, L"WorkerW", NULL);
        }
    }

    if (hDefView) {
        HWND hSysListView32 = FindWindowEx(hDefView, NULL, L"SysListView32", NULL);
        if (hSysListView32) {
            ShowWindow(hSysListView32, hide ? SW_HIDE : SW_SHOW);
        }
    }
}

#endif // FUNCTIONS_H
