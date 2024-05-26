/*
 * OpenFileDialog.cpp
 *
 * Project: PKS Edit for Windows
 *
 * purpose: integration of Vista Common File Dialogs.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "pch.h"
#include <objidl.h>
#include <ShObjIdl.h>
#include <Shlwapi.h>
#include "..\include\fileselector.h"

constexpr int CONTROL_GROUP = 2000;
constexpr int CONTROL_GROUP_ENCODING = 2001;
constexpr int CONTROL_ENCRYPT = 1;
constexpr int CONTROL_ENCODING = 2;

static int _codepages[] = {
    -1,
    CP_ACP,
    CP_OEMCP,
    CP_UTF8,
    37,
    437,
    500,
    708,
    709,
    710,
    720,
    737,
    775,
    850,
    852,
    855,
    857,
    858,
    860,
    861,
    862,
    863,
    864,
    865,
    866,
    869,
    870,
    874,
    875,
    932,
    936,
    949,
    950,
    1026,
    1047,
    1140,
    1141,
    1142,
    1143,
    1144,
    1145,
    1146,
    1147,
    1148,
    1149,
    1200,
    1201,
    1252,
    1253,
    1254,
    1255,
    1256,
    1257,
    1258,
    1361,
    10000,
    10001,
    10002,
    10003,
    10004,
    10005,
    10006,
    10007,
    10008,
    12000,
    12001,
    65000,
    65001
};


static void file_open_free_filters(COMDLG_FILTERSPEC* pFilters, int nCount) {
    for (int i = 0; i < nCount; i++) {
        free((void*) (pFilters[i].pszName));
        free((void*) pFilters[i].pszSpec);
    }
    free(pFilters);
}

static COMDLG_FILTERSPEC* file_openBuildFilters(char* pszFileTypes, char* pszCurrent, UINT*pNumberOfTypes, UINT* pSelected) {
    UINT nCount = 0;
    UINT nSize = 10;
    COMDLG_FILTERSPEC* pSpec = reinterpret_cast<COMDLG_FILTERSPEC*>(calloc(sizeof(COMDLG_FILTERSPEC), nSize));
    if (pSpec == NULL) {
        return NULL;
    }
    int selected = 0;
    while (*pszFileTypes) {
        char* pszName = pszFileTypes;
        pszFileTypes += strlen(pszFileTypes) + 1;
        if (!*pszFileTypes) {
            break;
        }
        char* pszFilter = pszFileTypes;
        if (strcmp(pszFilter, pszCurrent) == 0) {
            selected = nCount;
        }
        pszFileTypes += strlen(pszFileTypes) + 1;
        if (nCount >= nSize) {
            nSize += 10;
            COMDLG_FILTERSPEC* pSpec2 = reinterpret_cast<COMDLG_FILTERSPEC*>(realloc(pSpec, nSize * sizeof(COMDLG_FILTERSPEC)));
            if (pSpec2 == NULL) {
                break;
            }
            pSpec = pSpec2;
        }
        size_t l1 = static_cast<size_t>(lstrlenA(pszName)) + 1;
        pSpec[nCount].pszName = reinterpret_cast<LPCWSTR>(calloc(l1, sizeof(WCHAR)));
        MultiByteToWideChar(CP_ACP, 0, pszName, -1, (LPWSTR) pSpec[nCount].pszName, (int)l1);
        size_t l2 = static_cast<size_t>(lstrlenA(pszFilter)) + 1;
        pSpec[nCount].pszSpec = reinterpret_cast<LPCWSTR>(calloc(l2, sizeof(WCHAR)));
        MultiByteToWideChar(CP_ACP, 0, pszFilter, -1, (LPWSTR) pSpec[nCount].pszSpec, (int)l2);
        nCount++;
    }
    *pSelected = selected;
    *pNumberOfTypes = nCount;
    return pSpec;
}

/*
 * Return the multi-selection result from a file dialog. In the case of multi-selection the result is returned in
 * the fillowing format:
 * dirname\0baseFileName1\0baseFileName2...\0\0
 */
static int file_getResults(IFileOpenDialog* pFileOpenDialog, char* pszResult) {
    IShellItemArray* psiaResults;
    HRESULT hr = pFileOpenDialog->GetResults(&psiaResults);
    if (!SUCCEEDED(hr)) {
        return 0;
    }
    char fullPath[MAX_PATH];
    char fileName[MAX_PATH];
    DWORD nCount;
    char* pEnd = pszResult + 4095;
    int bFirst = 1;
    psiaResults->GetCount(&nCount);
    for (DWORD i = 0; i < nCount; i++) {
        IShellItem* pItem;
        psiaResults->GetItemAt(i, &pItem);
        PWSTR pszFilePath = NULL;
        pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        if (pszFilePath == NULL) {
            continue;
        }
        WideCharToMultiByte(CP_ACP, 0, pszFilePath, -1, fullPath, sizeof(fullPath), 0, 0);
        strcpy_s(fileName, MAX_PATH, fullPath);
        PathStripPathA(fileName);
        if (bFirst) {
            fullPath[strlen(fullPath) - 1 - strlen(fileName)] = 0;
            strcpy_s(pszResult, MAX_PATH, fullPath);
            pszResult += strlen(pszResult) + 1;
            bFirst = 0;
        }
        if (pszResult + strlen(fileName) + 1 > pEnd) {
            break;
        }
        strcpy_s(pszResult, MAX_PATH, fileName);
        pszResult += strlen(pszResult) + 1;
        *pszResult = 0;
        CoTaskMemFree(pszFilePath);
    }
    psiaResults->Release();
    return 1;
}

/*
 * Return the single selected result from a file dialog.
 */
static int file_getResult(IFileDialog* pFileDialog, char* pszResult) {
    IShellItem* pItem;
    HRESULT hr = pFileDialog->GetResult(&pItem);
    if (!SUCCEEDED(hr)) {
        return 0;
    }
    int ret = 0;
    PWSTR pszFilePath;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    // Display the file name to the user.
    if (SUCCEEDED(hr)) {
        ret = 1;
        WideCharToMultiByte(CP_ACP, 0, pszFilePath, -1, pszResult, 1024, 0, 0);
        CoTaskMemFree(pszFilePath);
    }
    pItem->Release();
    return ret;
}

void file_addEncodingItems(IFileDialogCustomize* pfdc, int bAllowAuto, int nSelectedEncoding) {
    int nSelectedIndex = 0;
    for (int i = 0; i < sizeof(_codepages)/sizeof(_codepages[0]); i++) {
        CPINFOEX cpInfo;
        DWORD nCp = _codepages[i];
        PWSTR pszText;
        if (nCp == -1) {
            if (!bAllowAuto) {
                continue;
            }
            pszText = (PWSTR)L"Auto-detect";
        } else {
            if (!GetCPInfoEx(nCp, 0, &cpInfo)) {
                continue;
            }
            pszText = cpInfo.CodePageName;
        }
        pfdc->AddControlItem(CONTROL_ENCODING, i, pszText);
        if (nSelectedEncoding == nCp) {
            nSelectedIndex = (WPARAM)i;
        }
    }
    pfdc->SetSelectedControlItem(CONTROL_ENCODING, nSelectedIndex);
}

IFileDialogCustomize* file_customizeDialog(IFileDialog *pfd, FILE_SELECT_PARAMS* pParams) {
    IFileDialogCustomize* pfdc = NULL;
    HRESULT hr = pfd->QueryInterface(IID_PPV_ARGS(&pfdc));
    if (SUCCEEDED(hr)) {
        pfdc->StartVisualGroup(CONTROL_GROUP_ENCODING, L"Encoding:");
        pfdc->AddComboBox(CONTROL_ENCODING);
        file_addEncodingItems(pfdc, pParams->fsp_saveAs == 0, pParams->fsp_codepage);
        pfdc->EndVisualGroup();
        if (pParams->fsp_saveAs) {
            pfdc->StartVisualGroup(CONTROL_GROUP_ENCODING, L"Options:");
            pfdc->AddCheckButton(CONTROL_ENCRYPT, L"Encrypted", pParams->fsp_encrypted);
            pfdc->EndVisualGroup();
        }
    }
    return pfdc;
}

static COMDLG_FILTERSPEC* file_openInitialize(IFileDialog* pFileDialog, FILE_SELECT_PARAMS* pParams, UINT* pCount) {
    UINT nSelected;
    COMDLG_FILTERSPEC* pFileTypes = file_openBuildFilters(pParams->fsp_filters, pParams->fsp_namePatterns, pCount, &nSelected);
    pFileDialog->SetFileTypes(*pCount, pFileTypes);
    pFileDialog->SetFileTypeIndex(nSelected);
    WCHAR wText[256];
    MultiByteToWideChar(CP_ACP, 0, pParams->fsp_title, -1, wText, sizeof wText / sizeof(wText[0]));
    pFileDialog->SetTitle(wText);
    size_t nLen = strlen(pParams->fsp_inputFile);
    IShellItem* pCurFolder = NULL;
    if (nLen > 0 && pParams->fsp_inputFile[nLen - 1] == '\\') {
        MultiByteToWideChar(CP_ACP, 0, pParams->fsp_inputFile, -1, wText, sizeof wText / sizeof(wText[0]));
        wText[nLen - 1] = 0;
    }
    else {
        MultiByteToWideChar(CP_ACP, 0, pParams->fsp_inputFile, -1, wText, sizeof wText / sizeof(wText[0]));
        PathStripPath(wText);
        pFileDialog->SetFileName(wText);
        nLen = wcslen(wText);
        MultiByteToWideChar(CP_ACP, 0, pParams->fsp_inputFile, -1, wText, sizeof wText / sizeof(wText[0]));
        wText[wcslen(wText) - 1 - nLen] = 0;
    }
    HRESULT hr = SHCreateItemFromParsingName(wText, NULL, IID_PPV_ARGS(&pCurFolder));
    if (SUCCEEDED(hr)) {
        pFileDialog->SetFolder(pCurFolder);
        pCurFolder->Release();
    }
    DWORD dwOptions;
    // In case of multi-select - set options.
    pFileDialog->GetOptions(&dwOptions);
    if (pParams->fsp_saveAs) {
        dwOptions &= ~FOS_OVERWRITEPROMPT;
        pFileDialog->SetOptions(dwOptions);
    } else if (pParams->fsp_multiSelect) {
        dwOptions |= FOS_ALLOWMULTISELECT;
        pFileDialog->SetOptions(dwOptions);
    }
    return pFileTypes;
}

extern "C" __declspec(dllexport) int file_open_vista_version(FILE_SELECT_PARAMS* pParams) {
    int ret = 0;
    IFileDialog* pFileDialog;

    // Create the FileOpenDialog object.
    HRESULT hr = CoCreateInstance(pParams->fsp_saveAs ? CLSID_FileSaveDialog : CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
        pParams->fsp_saveAs ? IID_IFileSaveDialog : IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileDialog));
    if (SUCCEEDED(hr)) {
        IFileDialogCustomize* pfdc = file_customizeDialog(pFileDialog, pParams);
        UINT count;
        COMDLG_FILTERSPEC* pFileTypes = file_openInitialize(pFileDialog, pParams, &count);
        hr = pFileDialog->Show(NULL);
        // Get the file name from the dialog box.
        if (SUCCEEDED(hr)) {
            if (pParams->fsp_multiSelect && !pParams->fsp_saveAs) {
                // Obtain the result of the user interaction.
                IFileOpenDialog* pFileOpenDialog = (IFileOpenDialog*)pFileDialog;
                file_getResults(pFileOpenDialog, pParams->fsp_resultFile);
            } else {
                file_getResult(pFileDialog, pParams->fsp_resultFile);
            }
            UINT nSelected;
            pFileDialog->GetFileTypeIndex(&nSelected);
            WideCharToMultiByte(CP_ACP, 0, pFileTypes[nSelected].pszSpec, -1, pParams->fsp_namePatterns, 1024, 0, 0);
            DWORD encodingIndex = 0;
            pfdc->GetSelectedControlItem(CONTROL_ENCODING, &encodingIndex);
            pParams->fsp_codepage = _codepages[encodingIndex];
            if (pParams->fsp_saveAs) {
                BOOL encryptSelected = false;
                pfdc->GetCheckButtonState(CONTROL_ENCRYPT, &encryptSelected);
                pParams->fsp_encrypted = encryptSelected;
            }
            ret = 1;
        }
        file_open_free_filters(pFileTypes, count);
        pFileDialog->Release();
        pfdc->Release();
    }
    return ret;
}


