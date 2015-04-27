#pragma once

#include <phdk.h>
#include <providers.h>

PPH_OBJECT_TYPE PhMemoryItemType;

typedef struct _PHP_KNOWN_MEMORY_REGION
{
	LIST_ENTRY ListEntry;
	PH_AVL_LINKS Links;
	ULONG_PTR Address;
	SIZE_T Size;
	PPH_STRING Name;
} PHP_KNOWN_MEMORY_REGION, *PPHP_KNOWN_MEMORY_REGION;

typedef struct _PH_PROCESS_PROPCONTEXT
{
	PPH_PROCESS_ITEM ProcessItem;
	HWND WindowHandle;
	PH_EVENT CreatedEvent;
	PPH_STRING Title;
	PROPSHEETHEADER PropSheetHeader;
	HPROPSHEETPAGE *PropSheetPages;

	HANDLE SelectThreadId;
} PH_PROCESS_PROPCONTEXT, *PPH_PROCESS_PROPCONTEXT;

FORCEINLINE BOOLEAN PhpPropPageDlgProcHeader(
	_In_ HWND hwndDlg,
	_In_ UINT uMsg,
	_In_ LPARAM lParam,
	_Out_ LPPROPSHEETPAGE *PropSheetPage,
	_Out_ PPH_PROCESS_PROPPAGECONTEXT *PropPageContext,
	_Out_ PPH_PROCESS_ITEM *ProcessItem
	);

PWSTR PhMakeContextAtom(
	VOID
	);

VOID PhInitializeMemoryProvider(
	_Out_ PPH_MEMORY_PROVIDER Provider,
	_In_ HANDLE ProcessId,
	_In_ PPH_MEMORY_PROVIDER_CALLBACK Callback,
	_In_opt_ PVOID Context
	);

VOID PhMemoryProviderUpdate(
	_In_ PPH_MEMORY_PROVIDER Provider
	);

VOID PhpCreateKnownMemoryRegions(
	_In_ HANDLE ProcessId,
	_In_ HANDLE ProcessHandle,
	_Out_ PPH_AVL_TREE KnownMemoryRegionsSet,
	_Out_ PLIST_ENTRY KnownMemoryRegionsListHead
	);

VOID PhpAddKnownMemoryRegion(
	_Inout_ PPH_AVL_TREE KnownMemoryRegionsSet,
	_Inout_ PLIST_ENTRY KnownMemoryRegionsListHead,
	_In_ ULONG_PTR Address,
	_In_ SIZE_T Size,
	_In_ _Assume_refs_(1) PPH_STRING Name
	);

static LONG NTAPI PhpCompareKnownMemoryRegionCompareFunction(
	_In_ PPH_AVL_LINKS Links1,
	_In_ PPH_AVL_LINKS Links2
	);

PPH_MEMORY_ITEM PhCreateMemoryItem(
	VOID
	);

PPHP_KNOWN_MEMORY_REGION PhpFindKnownMemoryRegion(
	_In_ PPH_AVL_TREE KnownMemoryRegionsSet,
	_In_ ULONG_PTR Address
	);

VOID PhpFreeKnownMemoryRegions(
	_In_ PLIST_ENTRY KnownMemoryRegionsListHead
	);

VOID PhpMemoryItemDeleteProcedure(
	_In_ PVOID Object,
	_In_ ULONG Flags
	);

BOOLEAN PhMemoryProviderInitialization(
	VOID
	);