#pragma once

#include <phdk.h>

INT_PTR CALLBACK OptionsDlgProc(
	_In_ HWND hwndDlg,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	);

BOOLEAN NTAPI VBoxMemoryCallback(
	_In_ PPH_MEMORY_PROVIDER Provider,
	_In_ _Assume_refs_(1) PPH_MEMORY_ITEM MemoryItem
	);

VOID VBoxProcessMemoryList(
	_In_ HWND hwndDlg,
	_In_ PPH_PROCESS_PROPPAGECONTEXT PropPageContext
	);



VOID LoadCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	);

VOID ShowOptionsCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	);

VOID GetProcessHighlightingColorCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	);

VOID GetProcessTooltipTextCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	);

PPH_PLUGIN PluginInstance;
PH_CALLBACK_REGISTRATION PluginLoadCallbackRegistration;
PH_CALLBACK_REGISTRATION PluginShowOptionsCallbackRegistration;
PH_CALLBACK_REGISTRATION GetProcessHighlightingColorCallbackRegistration;
PH_CALLBACK_REGISTRATION GetProcessTooltipTextCallbackRegistration;

COLORREF color;
ULONG colorEnable;