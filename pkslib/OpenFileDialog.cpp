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

static void file_open_free_filters(COMDLG_FILTERSPEC* pFilters, int nCount) {
    for (int i = 0; i < nCount; i++) {
        free((void*) pFilters[i].pszName);
        free((void*) pFilters[i].pszSpec);
    }
    free((void*) pFilters);
}

static COMDLG_FILTERSPEC* file_open_build_filters(char* pszFileTypes, char* pszCurrent, UINT*pNumberOfTypes, UINT* pSelected) {
    UINT nCount = 0;
    UINT nSize = 10;
    COMDLG_FILTERSPEC* pSpec = (COMDLG_FILTERSPEC*)calloc(sizeof(COMDLG_FILTERSPEC), nSize);
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
            pSpec = (COMDLG_FILTERSPEC*)realloc(pSpec, nSize * sizeof(COMDLG_FILTERSPEC));
        }
        size_t l1 = lstrlenA(pszName) + 1;
        pSpec[nCount].pszName = (LPCWSTR) calloc(l1, sizeof(WCHAR));
        MultiByteToWideChar(CP_UTF8, 0, pszName, -1, (LPWSTR) pSpec[nCount].pszName, (int)(l1 * sizeof(WCHAR)));
        size_t l2 = lstrlenA(pszFilter) + 1;
        pSpec[nCount].pszSpec = (LPCWSTR) calloc(l2, sizeof(WCHAR));
        MultiByteToWideChar(CP_UTF8, 0, pszFilter, -1, (LPWSTR) pSpec[nCount].pszSpec, (int)(l2 * sizeof(WCHAR)));
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
        PWSTR pszFilePath;
        pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, fullPath, sizeof(fullPath), 0, 0);
        strcpy_s(fileName, MAX_PATH, fullPath);
        PathStripPathA(fileName);
        if (bFirst) {
            fullPath[strlen(fullPath) - 1 - strlen(fileName)] = 0;
            strcpy_s(pszResult, MAX_PATH, fullPath);
            pszResult += strlen(pszResult) + 1;
            bFirst = 0;
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
        WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, pszResult, 1024, 0, 0);
        CoTaskMemFree(pszFilePath);
        ret = 1;
    }
    pItem->Release();
    return ret;
}

extern "C" __declspec(dllexport) int file_open_vista_version(FILE_SELECT_PARAMS* pParams) {
    int ret = 0;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileDialog* pFileDialog;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(pParams->fsp_saveAs ? CLSID_FileSaveDialog : CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            pParams->fsp_saveAs ? IID_IFileSaveDialog : IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileDialog));

        if (SUCCEEDED(hr)) {
            UINT count;
            UINT nSelected;
            COMDLG_FILTERSPEC* pFileTypes = file_open_build_filters(pParams->fsp_filters, pParams->fsp_namePatterns, &count, &nSelected);
            pFileDialog->SetFileTypes(count, pFileTypes);
            pFileDialog->SetFileTypeIndex(nSelected);
            WCHAR wTitle[256];
            MultiByteToWideChar(CP_UTF8, 0, pParams->fsp_title, -1, wTitle, sizeof wTitle / 4);
            pFileDialog->SetTitle(wTitle);
            DWORD dwOptions;
            // In case of multi-select - set options.
            pFileDialog->GetOptions(&dwOptions);
            if (pParams->fsp_multiSelect && !pParams->fsp_saveAs) {
                dwOptions |= FOS_ALLOWMULTISELECT;
                pFileDialog->SetOptions(dwOptions);
            }
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
                pFileDialog->GetFileTypeIndex(&nSelected);
                WideCharToMultiByte(CP_UTF8, 0, pFileTypes[nSelected].pszSpec, -1, pParams->fsp_namePatterns, 1024, 0, 0);
                ret = 1;
            }
            file_open_free_filters(pFileTypes, count);
            pFileDialog->Release();
        }
        CoUninitialize();
    }
    return ret;
}


