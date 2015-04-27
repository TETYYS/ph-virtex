#include <phdk.h>
#include <stdlib.h>
#include <windowsx.h>
#include "colorbox.h"
#include "resource.h"

INT_PTR CALLBACK OptionsDlgProc(
	_In_ HWND hwndDlg,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
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

LOGICAL DllMain(
	__in HINSTANCE Instance,
	__in ULONG Reason,
	__reserved PVOID Reserved
	)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		{
			PPH_PLUGIN_INFORMATION info;

			PluginInstance = PhRegisterPlugin(L"TT.VirtEx", Instance, &info);

			if (!PluginInstance)
				return FALSE;

			info->DisplayName = L"Virtualization extensions";
			info->Author = L"TETYYS";
			info->Description = L"Additional information for OS virtualization applications";
			info->HasOptions = TRUE;

			PhRegisterCallback(
				PhGetPluginCallback(PluginInstance, PluginCallbackLoad),
				LoadCallback,
				NULL,
				&PluginLoadCallbackRegistration
				);
			PhRegisterCallback(
				PhGetPluginCallback(PluginInstance, PluginCallbackShowOptions),
				ShowOptionsCallback,
				NULL,
				&PluginShowOptionsCallbackRegistration
				);
			PhRegisterCallback(
				PhGetGeneralCallback(GeneralCallbackGetProcessHighlightingColor),
				GetProcessHighlightingColorCallback,
				NULL,
				&GetProcessHighlightingColorCallbackRegistration
				);
			PhRegisterCallback(
				PhGetGeneralCallback(GeneralCallbackGetProcessTooltipText),
				GetProcessTooltipTextCallback,
				NULL,
				&GetProcessTooltipTextCallbackRegistration
				);

			{
				static PH_SETTING_CREATE settings[] =
				{
					{ IntegerSettingType, L"TT.VirtEx.HighlightColor", L"787800" }, //	0, 120, 120
					{ IntegerSettingType, L"TT.VirtEx.HighlightingEnable", L"1" } //	TRUE
				};

				PhAddSettings(settings, sizeof(settings) / sizeof(PH_SETTING_CREATE));
			}
		}
		break;
	}

	return TRUE;
}

VOID LoadCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	)
{
	//PhColorBoxInitialization();
	color = PhGetIntegerSetting(L"TT.VirtEx.HighlightColor");

	colorEnable = PhGetIntegerSetting(L"TT.VirtEx.HighlightingEnable");
}

VOID ShowOptionsCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	)
{
	DialogBox(	PluginInstance->DllBase,
				MAKEINTRESOURCE(IDD_OPTIONS),
				(HWND)Parameter,
				OptionsDlgProc);
}

INT_PTR CALLBACK OptionsDlgProc(
	_In_ HWND hwndDlg,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		ColorBox_SetColor(GetDlgItem(hwndDlg, IDC_COLOR), color);
		Button_SetCheck(GetDlgItem(hwndDlg, IDC_COLENABLE), colorEnable);
	}
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hwndDlg, IDCANCEL);
			break;
		case IDOK:
		{
			PhSetIntegerSetting(L"TT.VirtEx.HighlightColor", ColorBox_GetColor(GetDlgItem(hwndDlg, IDC_COLOR)));
			PhSetIntegerSetting(L"TT.VirtEx.HighlightingEnable", Button_GetCheck(GetDlgItem(hwndDlg, IDC_COLENABLE)) == BST_CHECKED);

			EndDialog(hwndDlg, IDOK);

			if (PhShowMessage(	PhMainWndHandle,
								MB_ICONQUESTION | MB_YESNO,
								L"One or more options you have changed requires a restart of Process Hacker. "
								L"Do you want to restart Process Hacker now?") == IDYES) {
				ProcessHacker_PrepareForEarlyShutdown(PhMainWndHandle);
				PhShellProcessHacker(	PhMainWndHandle,
										L"-v",
										SW_SHOW,
										0,
										PH_SHELL_APP_PROPAGATE_PARAMETERS | PH_SHELL_APP_PROPAGATE_PARAMETERS_IGNORE_VISIBILITY,
										0,
										NULL);
				ProcessHacker_Destroy(PhMainWndHandle);
			}
		}
			break;
		}
	}
		break;
	}

	return FALSE;
}

VOID GetProcessHighlightingColorCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	)
{
	PPH_PLUGIN_GET_HIGHLIGHTING_COLOR getHighlightingColor = Parameter;
	PPH_PROCESS_ITEM processItem;

	processItem = getHighlightingColor->Parameter;

	if (getHighlightingColor->Handled || colorEnable != BST_CHECKED)
		return;

	if (PhEqualString2(processItem->ProcessName, L"vmware-vmx.exe", TRUE)) {
		getHighlightingColor->BackColor = color;
		getHighlightingColor->Handled = TRUE;
		getHighlightingColor->Cache = TRUE;
	}
}

VOID GetProcessTooltipTextCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	)
{
	PPH_PLUGIN_GET_TOOLTIP_TEXT getTooltipText = Parameter;
	PPH_PROCESS_ITEM processItem;
	PPH_STRING vmx;

	ULONG_PTR extPos;
	ULONG extIndex = -1;
	WCHAR *exts[] = { L".vmx", L".vmtm", L".vmc", L".ovf", L".ova" };

	processItem = getTooltipText->Parameter;

	if (!PhEqualString2(processItem->ProcessName, L"vmware-vmx.exe", FALSE))
		return;
	
	for (int x = 0; x < 5; x++) {
		extPos = PhFindStringInString(processItem->CommandLine, 0, exts[x]);
		if (extPos != -1) {
			extIndex = x;
			break;
		}
	}

	ULONG_PTR pathPos = PhFindLastCharInString(processItem->CommandLine, 0, L'\\');

	if (extPos == -1 || pathPos == -1)
		return;

	vmx = PhSubstring(processItem->CommandLine, pathPos + 1, extPos - pathPos + (wcslen(exts[extIndex])) - 1);
	PhAppendFormatStringBuilder(getTooltipText->StringBuilder,
		L"Virtual Machine:\n"
		L"    %s\n"
		L"    RAM usage: %s\n",
		vmx->Buffer,
		PhaFormatSize(processItem->VmCounters.WorkingSetSize, -1)->Buffer);
	getTooltipText->ValidForMs = 0;
}
