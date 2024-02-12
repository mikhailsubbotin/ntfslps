/* +============================================================================================+ */
/* | ##    ## ######## ######## ######## ##       ######## ########          © Mikhail Subbotin | */
/* | ###   ##    ##    ##       ##    ## ##       ##    ## ##    ##                             | */
/* | ####  ##    ##    ##       ##       ##       ##    ## ##                                   | */
/* | ## ## ##    ##    ######## ######## ##       ######## ########                             | */
/* | ##  ####    ##    ##             ## ##       ##             ##                             | */
/* | ##   ###    ##    ##       ##    ## ##       ##       ##    ##                             | */
/* | ##    ##    ##    ##       ######## ######## ##       ########                             | */
/* +============================================================================================+ */
/* | ntfslps.c                                                                                  | */
/* +--------------------------------------------------------------------------------------------+ */

#include "ntfslpsp.h"

/* +==================+ */
/* | GLOBAL VARIABLES | */
/* +==================+ */

HANDLE g_hProcessHeap;

HMODULE g_hModulePSAPI = NULL;
BOOL IsPSAPIModuleManuallyLoaded = FALSE;

pRtlAnsiStringToUnicodeString g_pRtlAnsiStringToUnicodeString;
pRtlGetLastWin32Error g_pRtlGetLastWin32Error;
pRtlNtStatusToDosError g_pRtlNtStatusToDosError;
pRtlOemStringToUnicodeString g_pRtlOemStringToUnicodeString;
pRtlSetLastWin32Error g_pRtlSetLastWin32Error;
pRtlUnicodeStringToAnsiString g_pRtlUnicodeStringToAnsiString;
pRtlUnicodeStringToOemString g_pRtlUnicodeStringToOemString;

//#define NTFS_I30_FILTER

#ifdef NTFS_I30_FILTER
BOOL IsNTFSi30PathCheckEnabled;
#endif

pCopyFile2 g_pCopyFile2;
pCopyFileTransactedA g_pCopyFileTransactedA;
pCopyFileTransactedW g_pCopyFileTransactedW;
pCreateDirectoryTransactedA g_pCreateDirectoryTransactedA;
pCreateDirectoryTransactedW g_pCreateDirectoryTransactedW;
pCreateFile2 g_pCreateFile2;
pCreateFileTransactedA g_pCreateFileTransactedA;
pCreateFileTransactedW g_pCreateFileTransactedW;
pCreateHardLinkTransactedA g_pCreateHardLinkTransactedA;
pCreateHardLinkTransactedW g_pCreateHardLinkTransactedW;
pCreateSymbolicLinkA g_pCreateSymbolicLinkA;
pCreateSymbolicLinkTransactedA g_pCreateSymbolicLinkTransactedA;
pCreateSymbolicLinkTransactedW g_pCreateSymbolicLinkTransactedW;
pCreateSymbolicLinkW g_pCreateSymbolicLinkW;
pDeleteFileTransactedA g_pDeleteFileTransactedA;
pDeleteFileTransactedW g_pDeleteFileTransactedW;
pFindFirstFileNameTransactedW g_pFindFirstFileNameTransactedW;
pFindFirstFileNameW g_pFindFirstFileNameW;
pFindFirstFileTransactedA g_pFindFirstFileTransactedA;
pFindFirstFileTransactedW g_pFindFirstFileTransactedW;
pFindFirstStreamTransactedW g_pFindFirstStreamTransactedW;
pFindFirstStreamW g_pFindFirstStreamW;
pFindNextFileNameW g_pFindNextFileNameW;
pFindNextStreamW g_pFindNextStreamW;
pGetCompressedFileSizeTransactedA g_pGetCompressedFileSizeTransactedA;
pGetCompressedFileSizeTransactedW g_pGetCompressedFileSizeTransactedW;
pGetFileAttributesTransactedA g_pGetFileAttributesTransactedA;
pGetFileAttributesTransactedW g_pGetFileAttributesTransactedW;
pGetFileSizeEx g_pGetFileSizeEx;
pGetFinalPathNameByHandleA g_pGetFinalPathNameByHandleA;
pGetFinalPathNameByHandleW g_pGetFinalPathNameByHandleW;
pGetFullPathNameTransactedA g_pGetFullPathNameTransactedA;
pGetFullPathNameTransactedW g_pGetFullPathNameTransactedW;
pGetLongPathNameTransactedA g_pGetLongPathNameTransactedA;
pGetLongPathNameTransactedW g_pGetLongPathNameTransactedW;
pGetMappedFileNameA g_pGetMappedFileNameA = preloadGetMappedFileNameA;
pGetMappedFileNameW g_pGetMappedFileNameW = preloadGetMappedFileNameW;
pGetModuleFileNameExA g_pGetModuleFileNameExA = preloadGetModuleFileNameExA;
pGetModuleFileNameExW g_pGetModuleFileNameExW = preloadGetModuleFileNameExW;
pMoveFileTransactedA g_pMoveFileTransactedA;
pMoveFileTransactedW g_pMoveFileTransactedW;
pMoveFileWithProgressA g_pMoveFileWithProgressA;
pMoveFileWithProgressW g_pMoveFileWithProgressW;
pRemoveDirectoryTransactedA g_pRemoveDirectoryTransactedA;
pRemoveDirectoryTransactedW g_pRemoveDirectoryTransactedW;
pReplaceFileA g_pReplaceFileA;
pReplaceFileW g_pReplaceFileW;
pSearchPathA g_pSearchPathA;
pSearchPathW g_pSearchPathW;
pSetFileAttributesTransactedA g_pSetFileAttributesTransactedA;
pSetFileAttributesTransactedW g_pSetFileAttributesTransactedW;
pSetSearchPathMode g_pSetSearchPathMode;
pWow64DisableWow64FsRedirection g_pWow64DisableWow64FsRedirection;
pWow64EnableWow64FsRedirection g_pWow64EnableWow64FsRedirection;
pWow64RevertWow64FsRedirection g_pWow64RevertWow64FsRedirection;

/* +==================+ */
/* | GLOBAL CONSTANTS | */
/* +==================+ */

const WCHAR wcszBackupFileExtension[] = L".bak";

/* +====================+ */
/* | INTERNAL FUNCTIONS | */
/* +====================+ */

// AllocateUnicodeString
// ---------------------
// Implemented: 100%

LPWSTR WINAPI AllocateUnicodeString(IN LPCSTR lpString)
{
    ANSI_STRING AnsiString;
    UNICODE_STRING UnicodeString;
    UINT StringLength;
    LPWSTR lpUnicodeString;
    NTSTATUS Status;

    if (lpString)
    {
        StringLength = strlen(lpString);

        if (StringLength < (UNICODE_STRING_MAX_CHARS - 1))
        {
            AnsiString.Length = (USHORT) StringLength;
            AnsiString.MaximumLength = AnsiString.Length + 1;
            AnsiString.Buffer = (PCHAR) lpString;

            UnicodeString.Length = 0;
            UnicodeString.MaximumLength = AnsiString.MaximumLength * sizeof(WCHAR);
            UnicodeString.Buffer = (PWCHAR) RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, UnicodeString.MaximumLength);

            if (UnicodeString.Buffer)
            {
                if (StringLength)
                {
                    if (Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE))
                    {
                        RtlFreeHeap(g_hProcessHeap, 0, UnicodeString.Buffer);

                        g_pRtlSetLastWin32Error(g_pRtlNtStatusToDosError(Status));

                        goto loc_ReturnNullPointer;
                    }
                }
                lpUnicodeString = UnicodeString.Buffer;
            }
            else goto loc_ReturnNullPointer;
        }
        else { g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnNullPointer; }
    }
    else { g_pRtlSetLastWin32Error(NO_ERROR); loc_ReturnNullPointer: lpUnicodeString = NULL; }

    return lpUnicodeString;
}

// AnsiStringToUnicodeString
// -------------------------
// Implemented: 100%

UINT WINAPI AnsiStringToUnicodeString(IN LPCSTR lpAnsiString, OUT LPWSTR lpUnicodeStringBuffer, IN UINT nBufferLength)
{
    ANSI_STRING AnsiString;
    UNICODE_STRING UnicodeString;
    UINT StringLength;
    NTSTATUS Status;

    if (lpAnsiString)
    {
        if (lpUnicodeStringBuffer)
        {
            StringLength = strlen(lpAnsiString);

            if ((StringLength < nBufferLength) && (StringLength < (UNICODE_STRING_MAX_CHARS - 1)))
            {
                AnsiString.Length = (USHORT) StringLength;
                AnsiString.MaximumLength = AnsiString.Length + 1;
                AnsiString.Buffer = (PCHAR) lpAnsiString;

                UnicodeString.Length = 0;
                if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                    UnicodeString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                else UnicodeString.MaximumLength = (USHORT) nBufferLength;
                UnicodeString.Buffer = (PWCHAR) lpUnicodeStringBuffer;

                if (Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE))
                { g_pRtlSetLastWin32Error(g_pRtlNtStatusToDosError(Status)); goto loc_ReturnZero; }

                StringLength = UnicodeString.Length / sizeof(WCHAR) + 1;
            }
            else { g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnZero; }
        }
        else { g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER); goto loc_ReturnZero; }
    }
    else { g_pRtlSetLastWin32Error(ERROR_INVALID_PARAMETER); loc_ReturnZero: StringLength = 0; }

    return StringLength;
}

// UnicodePathStringToAnsiPathString
// ---------------------------------
// Implemented: 100%

UINT WINAPI UnicodePathStringToAnsiPathString(IN LPCWSTR lpUnicodeString, OUT LPSTR lpAnsiStringBuffer, IN UINT nBufferLength)
{
    ANSI_STRING AnsiString;
    UNICODE_STRING UnicodeString;
    UINT StringLength;
    NTSTATUS Status;

    if (lpUnicodeString)
    {
       if (lpAnsiStringBuffer)
        {
            StringLength = wcslen(lpUnicodeString);

            if ((StringLength < nBufferLength) && (StringLength < (UNICODE_STRING_MAX_CHARS - 1)))
            {
                UnicodeString.Length = (USHORT) StringLength * sizeof(WCHAR);
                UnicodeString.MaximumLength = UnicodeString.Length + sizeof(WCHAR);
                UnicodeString.Buffer = (PWCHAR) lpUnicodeString;

                AnsiString.Length = 0;
                if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                    AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                else AnsiString.MaximumLength = (USHORT) nBufferLength;
                AnsiString.Buffer = (PCHAR) lpAnsiStringBuffer;

                if (AreFileApisANSI())
                    Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
                else Status = g_pRtlUnicodeStringToOemString(&AnsiString, &UnicodeString, FALSE);
                if (Status) { g_pRtlSetLastWin32Error(g_pRtlNtStatusToDosError(Status)); goto loc_ReturnZero; }

                StringLength = AnsiString.Length + 1;
            }
            else { g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnZero; }
        }
        else { g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER); goto loc_ReturnZero; }
    }
    else { g_pRtlSetLastWin32Error(ERROR_INVALID_PARAMETER); loc_ReturnZero: StringLength = 0; }

    return StringLength;
}

// UnicodeStringToAnsiString
// -------------------------
// Implemented: 100%

UINT WINAPI UnicodeStringToAnsiString(IN LPCWSTR lpUnicodeString, OUT LPSTR lpAnsiStringBuffer, IN UINT nBufferLength)
{
    ANSI_STRING AnsiString;
    UNICODE_STRING UnicodeString;
    UINT StringLength;
    NTSTATUS Status;

    if (lpUnicodeString)
    {
        if (lpAnsiStringBuffer)
        {
            StringLength = wcslen(lpUnicodeString);

            if ((StringLength < nBufferLength) && (StringLength < (UNICODE_STRING_MAX_CHARS - 1)))
            {
                UnicodeString.Length = (USHORT) StringLength * sizeof(WCHAR);
                UnicodeString.MaximumLength = UnicodeString.Length + sizeof(WCHAR);
                UnicodeString.Buffer = (PWCHAR) lpUnicodeString;

                AnsiString.Length = 0;
                if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                    AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                else AnsiString.MaximumLength = (USHORT) nBufferLength;
                AnsiString.Buffer = (PCHAR) lpAnsiStringBuffer;

                if (Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE))
                { g_pRtlSetLastWin32Error(g_pRtlNtStatusToDosError(Status)); goto loc_ReturnZero; }

                StringLength = AnsiString.Length + 1;
            }
            else { g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnZero; }
        }
        else { g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER); goto loc_ReturnZero; }
    }
    else { g_pRtlSetLastWin32Error(ERROR_INVALID_PARAMETER); loc_ReturnZero: StringLength = 0; }

    return StringLength;
}

// GetFullPathExAW
// ---------------
// Implemented: 100%

DWORD WINAPI GetFullPathExAW(IN LPCSTR lpObjectPath, OUT LPWSTR lpBuffer, IN DWORD nBufferLength, BOOL bForcedLongPathPrefix)
{
    SIZE_T ObjectPathLength;
    ANSI_STRING AnsiString;
    UNICODE_STRING UnicodeString;
    DWORD dwFilePathLength;
    LPWSTR lpTemporaryBuffer;
    NTSTATUS Status;
    DWORD Win32ErrorCode;

    if (!lpObjectPath || !lpObjectPath[0])
    {
        g_pRtlSetLastWin32Error(ERROR_INVALID_NAME);

        return 0;
    }

    ObjectPathLength = strlen(lpObjectPath);

    if (ObjectPathLength >= UNICODE_STRING_MAX_CHARS)
    {
        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);

        return 0;
    }

    if (lpObjectPath[0] == '\\' && lpObjectPath[1] == '\\')
    {
        if (lpObjectPath[2] == '?' && lpObjectPath[3] == '\\')
        {
            if ((lpObjectPath[4] == 'U' || lpObjectPath[4] == 'u') &&
                (lpObjectPath[5] == 'N' || lpObjectPath[5] == 'n') &&
                (lpObjectPath[6] == 'C' || lpObjectPath[6] == 'c') &&
                 lpObjectPath[7] == '\\')
            {
                lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 6 + 1) * sizeof(WCHAR));

                if (lpTemporaryBuffer)
                {
                    AnsiString.Length = (USHORT) ObjectPathLength - 7;
                    AnsiString.MaximumLength = (USHORT) ObjectPathLength - 7 + 1;
                    AnsiString.Buffer = (LPSTR) lpObjectPath + 7;

                    UnicodeString.Length = 0;
                    UnicodeString.MaximumLength = (USHORT) (ObjectPathLength - 6 + 1) * sizeof(WCHAR);
                    UnicodeString.Buffer = lpTemporaryBuffer + 1;

                    if (AreFileApisANSI())
                        Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                    else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                    if (!Status)
                    {
                        lpTemporaryBuffer[0] = '\\';

                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) lpBuffer, NULL);

                        if (dwFilePathLength)
                        {
                            dwFilePathLength += 6;

                            if (dwFilePathLength > MAX_PATH16 || bForcedLongPathPrefix)
                            {
                                if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                                {
                                    Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                    dwFilePathLength = 0;
                                }
                                else if (dwFilePathLength <= nBufferLength)
                                {
                                    if (lpBuffer)
                                    {
                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 6, lpBuffer + 6, NULL);

                                        if (dwFilePathLength)
                                        {
                                            dwFilePathLength += 6;

                                            if (dwFilePathLength < nBufferLength)
                                            {
                                                memcpy(lpBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                                Win32ErrorCode = NO_ERROR;
                                            }
                                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                        }
                                        else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                    }
                                    else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                                }
                                else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                            }
                            else
                            {
                                if (lpBuffer)
                                {
                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, nBufferLength, lpBuffer, NULL);

                                    Win32ErrorCode = g_pRtlGetLastWin32Error();
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                        }
                        else Win32ErrorCode = g_pRtlGetLastWin32Error();
                    }
                    else
                    {
                        Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                        dwFilePathLength = 0;
                    }
                    RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                    g_pRtlSetLastWin32Error(Win32ErrorCode);
                }
                else dwFilePathLength = 0;
            }
            else
            {
                if ((ObjectPathLength == 6) &&
                   ((lpObjectPath[4] >= 'A' && lpObjectPath[4] <= 'Z') ||
                    (lpObjectPath[4] >= 'a' && lpObjectPath[4] <= 'z')) &&
                     lpObjectPath[5] == ':' && !lpObjectPath[6])
                {
                    dwFilePathLength = 3;

                    if (dwFilePathLength <= nBufferLength)
                    {
                        if (lpBuffer)
                        {
                            lpBuffer[0] = lpObjectPath[4] & (0xFF ^ ' ');
                            lpBuffer[1] = ':';
                            lpBuffer[2] = 0;

                            dwFilePathLength--;

                            g_pRtlSetLastWin32Error(NO_ERROR);
                        }
                        else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                    }
                    else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                }
                else
                {
                    lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 2 + 1) * sizeof(WCHAR));

                    if (lpTemporaryBuffer)
                    {
                        AnsiString.Length = (USHORT) ObjectPathLength - 2;
                        AnsiString.MaximumLength = (USHORT) ObjectPathLength - 2 + 1;
                        AnsiString.Buffer = (LPSTR) lpObjectPath;

                        UnicodeString.Length = 0;
                        UnicodeString.MaximumLength = (USHORT) (ObjectPathLength - 2) * sizeof(WCHAR);
                        UnicodeString.Buffer = lpTemporaryBuffer + 1;

                        if (AreFileApisANSI())
                            Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                        else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                        if (!Status)
                        {
                            lpTemporaryBuffer[0] = '\\';

                            dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) lpBuffer, NULL);

                            if (dwFilePathLength)
                            {
                                dwFilePathLength += 2;

                                if (dwFilePathLength > MAX_PATH16 || bForcedLongPathPrefix)
                                {
                                    if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                                    {
                                        Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                        dwFilePathLength = 0;
                                    }
                                    else if (dwFilePathLength <= nBufferLength)
                                    {
                                        if (lpBuffer)
                                        {
                                            dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 2, lpBuffer + 2, NULL);

                                            if (dwFilePathLength)
                                            {
                                                dwFilePathLength += 2;

                                                if (dwFilePathLength < nBufferLength)
                                                {
                                                    memcpy(lpBuffer, L"\\\\?", 3 * sizeof(WCHAR));

                                                    Win32ErrorCode = NO_ERROR;
                                                }
                                                else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                            }
                                            else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                        }
                                        else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                                    }
                                    else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                }
                                else
                                {
                                    if (lpBuffer)
                                    {
                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, nBufferLength, lpBuffer, NULL);

                                        Win32ErrorCode = g_pRtlGetLastWin32Error();
                                    }
                                    else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                                }
                            }
                            else Win32ErrorCode = g_pRtlGetLastWin32Error();
                        }
                        else
                        {
                            Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                            dwFilePathLength = 0;
                        }
                        RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                        g_pRtlSetLastWin32Error(Win32ErrorCode);
                    }
                    else dwFilePathLength = 0;
                }
            }
        }
        else if (lpObjectPath[2] == '.' && lpObjectPath[3] == '\\')
        {
            dwFilePathLength = ObjectPathLength + 1;

            if (dwFilePathLength <= nBufferLength)
            {
                if (lpBuffer)
                {
                    memcpy(lpBuffer, lpObjectPath, dwFilePathLength);

                    dwFilePathLength--;

                    g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
            }
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        else
        {
            lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength + 1) * sizeof(WCHAR));

            if (lpTemporaryBuffer)
            {
                AnsiString.Length = (USHORT) ObjectPathLength;
                AnsiString.MaximumLength = (USHORT) ObjectPathLength + 1;
                AnsiString.Buffer = (LPSTR) lpObjectPath;

                UnicodeString.Length = 0;
                UnicodeString.MaximumLength = (USHORT) (ObjectPathLength + 1) * sizeof(WCHAR);
                UnicodeString.Buffer = lpTemporaryBuffer;

                if (AreFileApisANSI())
                    Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                if (!Status)
                {
                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) lpBuffer, NULL);

                    if (dwFilePathLength)
                    {
                        if (dwFilePathLength > MAX_PATH16 || bForcedLongPathPrefix)
                        {
                            dwFilePathLength += 6;

                            if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                            {
                                Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                dwFilePathLength = 0;
                            }
                            else if (dwFilePathLength <= nBufferLength)
                            {
                                if (lpBuffer)
                                {
                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 6, lpBuffer + 6, NULL);

                                    if (dwFilePathLength)
                                    {
                                        dwFilePathLength += 6;

                                        if (dwFilePathLength < nBufferLength)
                                        {
                                            memcpy(lpBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                            Win32ErrorCode = NO_ERROR;
                                        }
                                        else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                    }
                                    else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                        }
                        else
                        {
                            if (lpBuffer)
                            {
                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, nBufferLength, lpBuffer, NULL);

                                Win32ErrorCode = g_pRtlGetLastWin32Error();
                            }
                            else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                        }
                    }
                    else Win32ErrorCode = g_pRtlGetLastWin32Error();
                }
                else
                {
                    Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                    dwFilePathLength = 0;
                }
                RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);
            }
            else dwFilePathLength = 0;
        }
    }
    else
    {
        if ((ObjectPathLength == 2) &&
           ((lpObjectPath[0] >= 'A' && lpObjectPath[0] <= 'Z') ||
            (lpObjectPath[0] >= 'a' && lpObjectPath[0] <= 'z')) &&
             lpObjectPath[1] == ':' && !lpObjectPath[2])
        {
            dwFilePathLength = 3;

            if (dwFilePathLength <= nBufferLength)
            {
                if (lpBuffer)
                {
                    lpBuffer[0] = lpObjectPath[0] & (0xFF ^ ' ');
                    lpBuffer[1] = ':';
                    lpBuffer[2] = 0;

                    dwFilePathLength--;

                    g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
            }
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        else
        {
            lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength + 1) * sizeof(WCHAR));

            if (lpTemporaryBuffer)
            {
                AnsiString.Length = (USHORT) ObjectPathLength;
                AnsiString.MaximumLength = (USHORT) ObjectPathLength + 1;
                AnsiString.Buffer = (LPSTR) lpObjectPath;

                UnicodeString.Length = 0;
                UnicodeString.MaximumLength = (USHORT) (ObjectPathLength + 1) * sizeof(WCHAR);
                UnicodeString.Buffer = lpTemporaryBuffer;

                if (AreFileApisANSI())
                    Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                if (!Status)
                {
                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) lpBuffer, NULL);

                    if (dwFilePathLength)
                    {
                        if (dwFilePathLength > MAX_PATH16 || bForcedLongPathPrefix)
                        {
                            dwFilePathLength += 4;

                            if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                            {
                                Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                dwFilePathLength = 0;
                            }
                            else if (dwFilePathLength <= nBufferLength)
                            {
                                if (lpBuffer)
                                {
                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 4, lpBuffer + 4, NULL);

                                    if (dwFilePathLength)
                                    {
                                        dwFilePathLength += 4;

                                        if (dwFilePathLength < nBufferLength)
                                        {
                                            memcpy(lpBuffer, L"\\\\?\\", 4 * sizeof(WCHAR));

                                            Win32ErrorCode = NO_ERROR;
                                        }
                                        else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                    }
                                    else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                        }
                        else
                        {
                            if (lpBuffer)
                            {
                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, nBufferLength, lpBuffer, NULL);

                                Win32ErrorCode = g_pRtlGetLastWin32Error();
                            }
                            else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                        }
                    }
                    else Win32ErrorCode = g_pRtlGetLastWin32Error();
                }
                else
                {
                    Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                    dwFilePathLength = 0;
                }
                RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);
            }
            else dwFilePathLength = 0;
        }
    }
    return dwFilePathLength;
}

// GetFullPathExW
// --------------
// Implemented: 100%

DWORD WINAPI GetFullPathExW(IN LPCWSTR lpObjectPath, OUT LPWSTR lpBuffer, IN DWORD nBufferLength, BOOL bForcedLongPathPrefix)
{
    SIZE_T ObjectPathLength;
    LPWSTR lpTemporaryBuffer;
    DWORD Win32ErrorCode;
    DWORD dwFilePathLength;

    if (!lpObjectPath || !lpObjectPath[0])
    {
        g_pRtlSetLastWin32Error(ERROR_INVALID_NAME);

        return 0;
    }

    ObjectPathLength = wcslen(lpObjectPath);

    if (ObjectPathLength >= UNICODE_STRING_MAX_CHARS)
    {
        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);

        return 0;
    }

    if (lpObjectPath[0] == '\\' && lpObjectPath[1] == '\\')
    {
        if (lpObjectPath[2] == '?' && lpObjectPath[3] == '\\')
        {
            if ((lpObjectPath[4] == 'U' || lpObjectPath[4] == 'u') &&
                (lpObjectPath[5] == 'N' || lpObjectPath[5] == 'n') &&
                (lpObjectPath[6] == 'C' || lpObjectPath[6] == 'c') &&
                 lpObjectPath[7] == '\\')
            {
                lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 6 + 1) * sizeof(WCHAR));

                if (lpTemporaryBuffer)
                {
                    lpTemporaryBuffer[0] = '\\';

                    memcpy(lpTemporaryBuffer + sizeof(WCHAR), lpObjectPath + (7 * sizeof(WCHAR)), (ObjectPathLength - 7 + 1) * sizeof(WCHAR));

                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, lpBuffer, NULL);

                    if (dwFilePathLength)
                    {
                        if (dwFilePathLength > MAX_PATH16 || bForcedLongPathPrefix)
                        {
                            dwFilePathLength += 6;

                            if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                            {
                                Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                dwFilePathLength = 0;
                            }
                            else if (dwFilePathLength <= nBufferLength)
                            {
                                if (lpBuffer)
                                {
                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 6, lpBuffer + 6, NULL);

                                    if (dwFilePathLength)
                                    {
                                        dwFilePathLength += 6;

                                        if (dwFilePathLength < nBufferLength)
                                        {
                                            memcpy(lpBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                            Win32ErrorCode = NO_ERROR;
                                        }
                                        else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                    }
                                    else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                        }
                        else
                        {
                            if (lpBuffer)
                            {
                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, nBufferLength, lpBuffer, NULL);

                                Win32ErrorCode = g_pRtlGetLastWin32Error();
                            }
                            else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                        }
                    }
                    else Win32ErrorCode = g_pRtlGetLastWin32Error();

                    RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                    g_pRtlSetLastWin32Error(Win32ErrorCode);
                }
                else dwFilePathLength = 0;
            }
            else
            {
                if ((ObjectPathLength == 6) &&
                   ((lpObjectPath[4] >= 'A' && lpObjectPath[4] <= 'Z') ||
                    (lpObjectPath[4] >= 'a' && lpObjectPath[4] <= 'z')) &&
                     lpObjectPath[5] == ':' && !lpObjectPath[6])
                {
                    dwFilePathLength = 3;

                    if (dwFilePathLength <= nBufferLength)
                    {
                        if (lpBuffer)
                        {
                            lpBuffer[0] = lpObjectPath[4] & (0xFF ^ ' ');
                            lpBuffer[1] = ':';
                            lpBuffer[2] = 0;

                            dwFilePathLength--;

                            g_pRtlSetLastWin32Error(NO_ERROR);
                        }
                        else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                    }
                    else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                }
                else
                {
                    lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 2 + 1) * sizeof(WCHAR));

                    if (lpTemporaryBuffer)
                    {
                        lpTemporaryBuffer[0] = '\\';

                        memcpy(lpTemporaryBuffer + sizeof(WCHAR), lpObjectPath + (3 * sizeof(WCHAR)), (ObjectPathLength - 3 + 1) * sizeof(WCHAR));

                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, lpBuffer, NULL);

                        if (dwFilePathLength)
                        {
                            if (dwFilePathLength > MAX_PATH16 || bForcedLongPathPrefix)
                            {
                                dwFilePathLength += 2;

                                if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                                {
                                    Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                    dwFilePathLength = 0;
                                }
                                else if (dwFilePathLength <= nBufferLength)
                                {
                                    if (lpBuffer)
                                    {
                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 2, lpBuffer + 2, NULL);

                                        if (dwFilePathLength)
                                        {
                                            dwFilePathLength += 2;

                                            if (dwFilePathLength < nBufferLength)
                                            {
                                                memcpy(lpBuffer, L"\\\\?", 3 * sizeof(WCHAR));

                                                Win32ErrorCode = NO_ERROR;
                                            }
                                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                        }
                                        else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                    }
                                    else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                                }
                                else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                            }
                            else
                            {
                                if (lpBuffer)
                                {
                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, nBufferLength, lpBuffer, NULL);

                                    Win32ErrorCode = g_pRtlGetLastWin32Error();
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                        }
                        else Win32ErrorCode = g_pRtlGetLastWin32Error();

                        RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                        g_pRtlSetLastWin32Error(Win32ErrorCode);
                    }
                    else dwFilePathLength = 0;
                }
            }
        }
        else if (lpObjectPath[2] == '.' && lpObjectPath[3] == '\\')
        {
            dwFilePathLength = ObjectPathLength + 1;

            if (dwFilePathLength <= nBufferLength)
            {
                if (lpBuffer)
                {
                    memcpy(lpBuffer, lpObjectPath, dwFilePathLength * sizeof(WCHAR));

                    dwFilePathLength--;

                    g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
            }
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        else
        {
            dwFilePathLength = GetFullPathNameW(lpObjectPath, 0, lpBuffer, NULL);

            if (dwFilePathLength)
            {
                dwFilePathLength += 6;

                if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                {
                    g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);

                    dwFilePathLength = 0;
                }
                else if (dwFilePathLength <= nBufferLength)
                {
                    if (lpBuffer)
                    {
                        dwFilePathLength = GetFullPathNameW(lpObjectPath, dwFilePathLength - 6, lpBuffer + 6, NULL);

                        if (dwFilePathLength)
                        {
                            dwFilePathLength += 6;

                            if (dwFilePathLength < nBufferLength)
                            {
                                memcpy(lpBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                Win32ErrorCode = NO_ERROR;
                            }
                            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                        }
                    }
                    else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                }
                else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
            }
        }
    }
    else
    {
        if ((ObjectPathLength == 2) &&
           ((lpObjectPath[0] >= 'A' && lpObjectPath[0] <= 'Z') ||
            (lpObjectPath[0] >= 'a' && lpObjectPath[0] <= 'z')) &&
             lpObjectPath[1] == ':' && !lpObjectPath[2])
        {
            dwFilePathLength = 3;

            if (dwFilePathLength <= nBufferLength)
            {
                if (lpBuffer)
                {
                    lpBuffer[0] = lpObjectPath[0] & (0xFF ^ ' ');
                    lpBuffer[1] = ':';
                    lpBuffer[2] = 0;

                    dwFilePathLength--;

                    g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
            }
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        else
        {
            dwFilePathLength = GetFullPathNameW(lpObjectPath, 0, lpBuffer, NULL);

            if (dwFilePathLength)
            {
                if (dwFilePathLength > MAX_PATH16 || bForcedLongPathPrefix)
                {
                    dwFilePathLength += 4;

                    if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                    {
                        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);

                        dwFilePathLength = 0;
                    }
                    else if (dwFilePathLength <= nBufferLength)
                    {
                        if (lpBuffer)
                        {
                            dwFilePathLength = GetFullPathNameW(lpObjectPath, dwFilePathLength - 4, lpBuffer + 4, NULL);

                            if (dwFilePathLength)
                            {
                                dwFilePathLength += 4;

                                if (dwFilePathLength < nBufferLength)
                                {
                                    memcpy(lpBuffer, L"\\\\?\\", 4 * sizeof(WCHAR));

                                    g_pRtlSetLastWin32Error(NO_ERROR);
                                }
                                else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                            }
                        }
                        else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                    }
                    else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                }
                else
                {
                    if (lpBuffer)
                        dwFilePathLength = GetFullPathNameW(lpObjectPath, nBufferLength, lpBuffer, NULL);
                    else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                }
            }
        }
    }
    return dwFilePathLength;
}

// GetFullObjectPathA
// ------------------
// Implemented: 100%

LPWSTR WINAPI GetFullObjectPathA(IN LPCSTR lpObjectPath)
{
    return GetFullObjectPathExA(lpObjectPath, FALSE);
}

// GetFullObjectPathExA
// --------------------
// Implemented: 100%

LPWSTR WINAPI GetFullObjectPathExA(IN LPCSTR lpObjectPath, BOOL bForcedLongPathPrefix)
{
    SIZE_T ObjectPathLength;
    ANSI_STRING AnsiString;
    UNICODE_STRING UnicodeString;
    DWORD dwFilePathLength;
    LPWSTR lpTemporaryBuffer;
    LPWSTR lpBuffer;
    NTSTATUS Status;
    DWORD Win32ErrorCode;

    if (lpObjectPath)
    {
        if (lpObjectPath[0])
        {
            ObjectPathLength = strlen(lpObjectPath) + 1;

            if (ObjectPathLength < UNICODE_STRING_MAX_CHARS)
            {
                if (lpObjectPath[0] == '\\' && lpObjectPath[1] == '\\')
                {
                    if (lpObjectPath[2] == '?' && lpObjectPath[3] == '\\')
                    {
                        if ((lpObjectPath[4] == 'U' || lpObjectPath[4] == 'u') &&
                            (lpObjectPath[5] == 'N' || lpObjectPath[5] == 'n') &&
                            (lpObjectPath[6] == 'C' || lpObjectPath[6] == 'c') &&
                             lpObjectPath[7] == '\\')
                        {
                            lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 6) * sizeof(WCHAR));

                            if (lpTemporaryBuffer)
                            {
                                AnsiString.Length = (USHORT) ObjectPathLength - 7 - 1;
                                AnsiString.MaximumLength = (USHORT) ObjectPathLength - 7;
                                AnsiString.Buffer = (LPSTR) lpObjectPath + 7;

                                UnicodeString.Length = 0;
                                UnicodeString.MaximumLength = AnsiString.MaximumLength * sizeof(WCHAR);
                                UnicodeString.Buffer = lpTemporaryBuffer + 1;

                                if (AreFileApisANSI())
                                    Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                                else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                                if (!Status)
                                {
                                    lpTemporaryBuffer[0] = '\\';

                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) &lpBuffer, NULL);

                                    if (dwFilePathLength)
                                    {
                                        if (dwFilePathLength + 6 > MAX_PATH16 || bForcedLongPathPrefix)
                                        {
                                            dwFilePathLength += 6;

                                            if (dwFilePathLength < UNICODE_STRING_MAX_CHARS)
                                            {
                                                lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                                if (lpBuffer)
                                                {
                                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 6, lpBuffer + 6, NULL);

                                                    if (dwFilePathLength)
                                                    {
                                                        memcpy(lpBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                                        Win32ErrorCode = NO_ERROR;
                                                    }
                                                    else
                                                    {
                                                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                        RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                                                        lpBuffer = NULL;
                                                    }
                                                }
                                                else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                            }
                                            else
                                            {
                                                Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                                lpBuffer = NULL;
                                            }
                                        }
                                        else
                                        {
                                            lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                            if (lpBuffer)
                                            {
                                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpBuffer, NULL);

                                                Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                if (!dwFilePathLength)
                                                {
                                                    RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                                                    lpBuffer = NULL;
                                                }
                                            }
                                            else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                        }
                                    }
                                    else
                                    {
                                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                                        lpBuffer = NULL;
                                    }
                                }
                                else
                                {
                                    Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                    lpBuffer = NULL;
                                }
                                RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                                g_pRtlSetLastWin32Error(Win32ErrorCode);
                            }
                            else lpBuffer = NULL;
                        }
                        else
                        {
                            if ((ObjectPathLength == 7) &&
                               (((lpObjectPath[4] >= 'A') && (lpObjectPath[4] <= 'Z')) ||
                                ((lpObjectPath[4] >= 'a') && (lpObjectPath[4] <= 'z'))) &&
                                 (lpObjectPath[5] == ':') && !lpObjectPath[6])
                            {
                                lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, 3 * sizeof(WCHAR));

                                if (lpBuffer)
                                {
                                    lpBuffer[0] = lpObjectPath[4] & (0xFF ^ ' '); lpBuffer[1] = ':'; lpBuffer[2] = 0;

                                    g_pRtlSetLastWin32Error(NO_ERROR);
                                }
                            }
                            else
                            {
                                lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 2) * sizeof(WCHAR));

                                if (lpTemporaryBuffer)
                                {
                                    AnsiString.Length = (USHORT) ObjectPathLength - 2 - 1;
                                    AnsiString.MaximumLength = (USHORT) ObjectPathLength - 2;
                                    AnsiString.Buffer = (LPSTR) lpObjectPath;

                                    UnicodeString.Length = 0;
                                    UnicodeString.MaximumLength = AnsiString.MaximumLength * sizeof(WCHAR);
                                    UnicodeString.Buffer = lpTemporaryBuffer + 1;

                                    if (AreFileApisANSI())
                                        Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                                    else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                                    if (!Status)
                                    {
                                        lpTemporaryBuffer[0] = '\\';

                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) &lpBuffer, NULL);

                                        if (dwFilePathLength)
                                        {
                                            if (dwFilePathLength + 2 > MAX_PATH16 || bForcedLongPathPrefix)
                                            {
                                                dwFilePathLength += 2;

                                                if (dwFilePathLength < UNICODE_STRING_MAX_CHARS)
                                                {
                                                    lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                                    if (lpBuffer)
                                                    {
                                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 2, lpBuffer + 2, NULL);

                                                        if (dwFilePathLength)
                                                        {
                                                            memcpy(lpBuffer, L"\\\\?", 3 * sizeof(WCHAR));

                                                            Win32ErrorCode = NO_ERROR;
                                                        }
                                                        else
                                                        {
                                                            Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                            RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                                                            lpBuffer = NULL;
                                                        }
                                                    }
                                                    else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                                }
                                                else
                                                {
                                                    Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                                    lpBuffer = NULL;
                                                }
                                            }
                                            else
                                            {
                                                lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                                if (lpBuffer)
                                                {
                                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpBuffer, NULL);

                                                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                    if (!dwFilePathLength)
                                                    {
                                                        RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                                                        lpBuffer = NULL;
                                                    }
                                                }
                                                else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                            }
                                        }
                                        else
                                        {
                                            Win32ErrorCode = g_pRtlGetLastWin32Error();

                                            lpBuffer = NULL;
                                        }
                                    }
                                    else
                                    {
                                        Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                        lpBuffer = NULL;
                                    }
                                    RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                                    g_pRtlSetLastWin32Error(Win32ErrorCode);
                                }
                                else lpBuffer = NULL;
                            }
                        }
                    }
                    else if (lpObjectPath[2] == '.' && lpObjectPath[3] == '\\')
                    {
                        lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, ObjectPathLength * sizeof(WCHAR));

                        if (lpBuffer)
                        {
                            AnsiString.Length = (USHORT) ObjectPathLength - 1;
                            AnsiString.MaximumLength = (USHORT) ObjectPathLength;
                            AnsiString.Buffer = (LPSTR) lpObjectPath;

                            UnicodeString.Length = 0;
                            UnicodeString.MaximumLength = AnsiString.MaximumLength * sizeof(WCHAR);
                            UnicodeString.Buffer = lpBuffer;

                            Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                            if (!Status)
                            {
                                RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                                g_pRtlSetLastWin32Error(g_pRtlNtStatusToDosError(Status));

                                lpBuffer = NULL;
                            }
                            else g_pRtlSetLastWin32Error(NO_ERROR);
                        }
                    }
                    else
                    {
                        lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, ObjectPathLength * sizeof(WCHAR));

                        if (lpTemporaryBuffer)
                        {
                            AnsiString.Length = (USHORT) ObjectPathLength - 1;
                            AnsiString.MaximumLength = (USHORT) ObjectPathLength;
                            AnsiString.Buffer = (LPSTR) lpObjectPath;

                            UnicodeString.Length = 0;
                            UnicodeString.MaximumLength = AnsiString.MaximumLength * sizeof(WCHAR);
                            UnicodeString.Buffer = lpTemporaryBuffer;

                            if (AreFileApisANSI())
                                Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                            else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                            if (!Status)
                            {
                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) &lpBuffer, NULL);

                                if (dwFilePathLength)
                                {
                                    if (dwFilePathLength > MAX_PATH16 || bForcedLongPathPrefix)
                                    {
                                        dwFilePathLength += 6;

                                        if (dwFilePathLength < UNICODE_STRING_MAX_CHARS)
                                        {
                                            lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                            if (lpBuffer)
                                            {
                                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 6, lpBuffer + 6, NULL);

                                                if (dwFilePathLength)
                                                {
                                                    memcpy(lpBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                                    Win32ErrorCode = NO_ERROR;
                                                }
                                                else
                                                {
                                                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                    RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                                                    lpBuffer = NULL;
                                                }
                                            }
                                            else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                        }
                                        else
                                        {
                                            Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                            lpBuffer = NULL;
                                        }
                                    }
                                    else
                                    {
                                        lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                        if (lpBuffer)
                                        {
                                            dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpBuffer, NULL);

                                            Win32ErrorCode = g_pRtlGetLastWin32Error();

                                            if (!dwFilePathLength)
                                            {
                                                RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                                                lpBuffer = NULL;
                                            }
                                        }
                                        else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                    }
                                }
                                else
                                {
                                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                                    lpBuffer = NULL;
                                }
                            }
                            else
                            {
                                Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                lpBuffer = NULL;
                            }
                            RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                            g_pRtlSetLastWin32Error(Win32ErrorCode);
                        }
                        else lpBuffer = NULL;
                    }
                }
                else
                {
                    if ((ObjectPathLength == 3) &&
                       (((lpObjectPath[0] >= 'A') && (lpObjectPath[0] <= 'Z')) ||
                        ((lpObjectPath[0] >= 'a') && (lpObjectPath[0] <= 'z'))) &&
                         (lpObjectPath[1] == ':') && !lpObjectPath[2])
                    {
                        lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, 3 * sizeof(WCHAR));

                        if (lpBuffer)
                        {
                            lpBuffer[0] = lpObjectPath[0] & (0xFF ^ ' '); lpBuffer[1] = ':'; lpBuffer[2] = 0;

                            g_pRtlSetLastWin32Error(NO_ERROR);
                        }
                    }
                    else
                    {
                        lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, ObjectPathLength * sizeof(WCHAR));

                        if (lpTemporaryBuffer)
                        {
                            AnsiString.Length = (USHORT) ObjectPathLength - 1;
                            AnsiString.MaximumLength = (USHORT) ObjectPathLength;
                            AnsiString.Buffer = (LPSTR) lpObjectPath;

                            UnicodeString.Length = 0;
                            UnicodeString.MaximumLength = AnsiString.MaximumLength * sizeof(WCHAR);
                            UnicodeString.Buffer = lpTemporaryBuffer;

                            if (AreFileApisANSI())
                                Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                            else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                            if (!Status)
                            {
                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) &lpBuffer, NULL);

                                if (dwFilePathLength)
                                {
                                    if (dwFilePathLength > MAX_PATH16 || bForcedLongPathPrefix)
                                    {
                                        dwFilePathLength += 4;

                                        if (dwFilePathLength < UNICODE_STRING_MAX_CHARS)
                                        {
                                            lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                            if (lpBuffer)
                                            {
                                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 4, lpBuffer + 4, NULL);

                                                if (dwFilePathLength)
                                                {
                                                    memcpy(lpBuffer, L"\\\\?\\", 4 * sizeof(WCHAR));

                                                    Win32ErrorCode = NO_ERROR;
                                                }
                                                else
                                                {
                                                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                    RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                                                    lpBuffer = NULL;
                                                }
                                            }
                                            else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                        }
                                        else
                                        {
                                            Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                            lpBuffer = NULL;
                                        }
                                    }
                                    else
                                    {
                                        lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                        if (lpBuffer)
                                        {
                                            dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpBuffer, NULL);

                                            Win32ErrorCode = g_pRtlGetLastWin32Error();

                                            if (!dwFilePathLength)
                                            {
                                                RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                                                lpBuffer = NULL;
                                            }
                                        } 
                                        else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                    }
                                }
                                else
                                {
                                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                                    lpBuffer = NULL;
                                }
                            }
                            else
                            {
                                Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                lpBuffer = NULL;
                            }
                            RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                            g_pRtlSetLastWin32Error(Win32ErrorCode);
                        }
                        else lpBuffer = NULL;
                    }
                }
            }
            else
            {
                if ((lpObjectPath[0] == '\\') && (lpObjectPath[1] == '\\') && (lpObjectPath[2] == '.') && (lpObjectPath[3] == '\\'))
                    g_pRtlSetLastWin32Error(ERROR_INVALID_NAME);
                else g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);

                lpBuffer = NULL;
            }
        }
        else
        {
            lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, sizeof(WCHAR));

            if (lpBuffer) goto loc_SetNoErrorCodeAndReturn;
        }
    }
    else { lpBuffer = NULL; loc_SetNoErrorCodeAndReturn: g_pRtlSetLastWin32Error(NO_ERROR); }

    return lpBuffer;
}

// GetFullObjectPathExW
// --------------------
// Implemented: 100%

LPWSTR WINAPI GetFullObjectPathExW(IN LPCWSTR lpObjectPath, BOOL bForcedLongPathPrefix)
{
    DWORD dwObjectPathLength;
    LPWSTR lpBuffer;

    dwObjectPathLength = GetFullPathExW(lpObjectPath, NULL, 0, bForcedLongPathPrefix);

    if (dwObjectPathLength)
    {
        lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwObjectPathLength * sizeof(WCHAR));

        if (lpBuffer)
        {
            dwObjectPathLength = GetFullPathExW(lpObjectPath, lpBuffer, dwObjectPathLength, bForcedLongPathPrefix);

            if (!dwObjectPathLength)
            {
                DWORD Win32ErrorCode = g_pRtlGetLastWin32Error();

                RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);

                lpBuffer = 0;
            }
        }
    }
    else
    {
        if (g_pRtlGetLastWin32Error() == ERROR_INVALID_NAME)
        {
            if (lpObjectPath)
            {
                if (!lpObjectPath[0])
                {
                    lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, sizeof(WCHAR));

                    if (lpBuffer) g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else lpBuffer = 0;
            }
            else
            {
                g_pRtlSetLastWin32Error(NO_ERROR);

                lpBuffer = 0;
            }
        }
        else lpBuffer = 0;
    }
    return lpBuffer;
}

// GetFullObjectPathW
// ------------------
// Implemented: 100%

LPWSTR WINAPI GetFullObjectPathW(IN LPCWSTR lpObjectPath)
{
    DWORD dwObjectPathLength;
    LPWSTR lpBuffer;

    dwObjectPathLength = GetFullPathExW(lpObjectPath, NULL, 0, FALSE);

    if (dwObjectPathLength)
    {
        lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwObjectPathLength * sizeof(WCHAR));

        if (lpBuffer)
        {
            dwObjectPathLength = GetFullPathExW(lpObjectPath, lpBuffer, dwObjectPathLength, FALSE);

            if (!dwObjectPathLength)
            {
                DWORD Win32ErrorCode = g_pRtlGetLastWin32Error();

                RtlFreeHeap(g_hProcessHeap, 0, lpBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);

                lpBuffer = 0;
            }
        }
    }
    else
    {
        if (g_pRtlGetLastWin32Error() == ERROR_INVALID_NAME)
        {
            if (lpObjectPath)
            {
                if (!lpObjectPath[0])
                {
                    lpBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, sizeof(WCHAR));

                    if (lpBuffer) g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else lpBuffer = 0;
            }
            else
            {
                g_pRtlSetLastWin32Error(NO_ERROR);

                lpBuffer = 0;
            }
        }
        else lpBuffer = 0;
    }
    return lpBuffer;
}

// FreeFullObjectPathBuffer
// ------------------------
// Implemented: 100%

BOOL WINAPI FreeFullObjectPathBuffer(IN PVOID lpFullObjectPathBuffer)
{
    if (lpFullObjectPathBuffer)
        return RtlFreeHeap(g_hProcessHeap, 0, lpFullObjectPathBuffer);
    return TRUE;
}

// IsNTFSi30PathA
// --------------
// Implemented: 100%

#ifdef NTFS_I30_FILTER

DWORD WINAPI IsNTFSi30PathA(IN LPCSTR lpObjectPath)
{
    LPCSTR lpLastPathObjectName, lpObjectPathPosition;
    CHAR chr;

    if (lpObjectPath)
    {
        lpLastPathObjectName = lpObjectPath;
        lpObjectPathPosition = lpObjectPath;
        chr = lpObjectPathPosition[0];
        while (chr)
        {
            lpObjectPathPosition++;
            if ((chr == '/') || (chr == '\\')) lpLastPathObjectName = lpObjectPathPosition;
            chr = *lpObjectPathPosition;
        }
        if (((lpObjectPathPosition - lpLastPathObjectName) > 4) &&
            ((lpLastPathObjectName[0] == ':') &&
             (lpLastPathObjectName[1] == '$') &&
             (lpLastPathObjectName[2] == 'i') &&
             (lpLastPathObjectName[3] == '3') &&
             (lpLastPathObjectName[4] == '0') &&
            ((lpLastPathObjectName[5] == ':') || (lpLastPathObjectName[5] == 0))))
            return ERROR_BAD_PATHNAME;
    }
    return NO_ERROR;
}

#endif

// IsNTFSi30PathW
// --------------
// Implemented: 100%

#ifdef NTFS_I30_FILTER

DWORD WINAPI IsNTFSi30PathW(IN LPCWSTR lpObjectPath)
{
    LPCWSTR lpLastPathObjectName, lpObjectPathPosition;
    WCHAR chr;

    if (lpObjectPath)
    {
        lpLastPathObjectName = lpObjectPath;
        lpObjectPathPosition = lpObjectPath;
        chr = lpObjectPathPosition[0];
        while (chr)
        {
            lpObjectPathPosition++;
            if ((chr == '/') || (chr == '\\')) lpLastPathObjectName = lpObjectPathPosition;
            chr = *lpObjectPathPosition;
        }
        if (((lpObjectPathPosition - lpLastPathObjectName) > 4) &&
            ((lpLastPathObjectName[0] == ':') &&
             (lpLastPathObjectName[1] == '$') &&
             (lpLastPathObjectName[2] == 'i') &&
             (lpLastPathObjectName[3] == '3') &&
             (lpLastPathObjectName[4] == '0') &&
            ((lpLastPathObjectName[5] == ':') || (lpLastPathObjectName[5] == 0))))
            return ERROR_BAD_PATHNAME;
    }
    return NO_ERROR;
}

#endif

// getwinerrnocode
// ---------------
// Implemented: 100%

typedef struct _errcodes {
    unsigned long winerrcode;
    int errnocode;
} errcodes;

static errcodes errtable[] = {
    { ERROR_INVALID_FUNCTION, EINVAL },
    { ERROR_FILE_NOT_FOUND, ENOENT },
    { ERROR_PATH_NOT_FOUND, ENOENT },
    { ERROR_TOO_MANY_OPEN_FILES, EMFILE },
    { ERROR_ACCESS_DENIED, EACCES },
    { ERROR_INVALID_HANDLE, EBADF },
    { ERROR_ARENA_TRASHED, ENOMEM },
    { ERROR_NOT_ENOUGH_MEMORY, ENOMEM },
    { ERROR_INVALID_BLOCK, ENOMEM },
    { ERROR_BAD_ENVIRONMENT, E2BIG },
    { ERROR_BAD_FORMAT, ENOEXEC },
    { ERROR_INVALID_ACCESS, EINVAL },
    { ERROR_INVALID_DATA, EINVAL },
    { ERROR_INVALID_DRIVE, ENOENT },
    { ERROR_CURRENT_DIRECTORY, EACCES },
    { ERROR_NOT_SAME_DEVICE, EXDEV },
    { ERROR_NO_MORE_FILES, ENOENT },
    { ERROR_LOCK_VIOLATION, EACCES },
    { ERROR_BAD_NETPATH, ENOENT },
    { ERROR_NETWORK_ACCESS_DENIED, EACCES },
    { ERROR_BAD_NET_NAME, ENOENT },
    { ERROR_FILE_EXISTS, EEXIST },
    { ERROR_CANNOT_MAKE, EACCES },
    { ERROR_FAIL_I24, EACCES },
    { ERROR_INVALID_PARAMETER, EINVAL },
    { ERROR_NO_PROC_SLOTS, EAGAIN },
    { ERROR_DRIVE_LOCKED, EACCES },
    { ERROR_BROKEN_PIPE, EPIPE },
    { ERROR_DISK_FULL, ENOSPC },
    { ERROR_INVALID_TARGET_HANDLE, EBADF },
    { ERROR_INVALID_HANDLE, EINVAL },
    { ERROR_WAIT_NO_CHILDREN, ECHILD },
    { ERROR_CHILD_NOT_COMPLETE, ECHILD },
    { ERROR_DIRECT_ACCESS_HANDLE, EBADF },
    { ERROR_NEGATIVE_SEEK, EINVAL },
    { ERROR_SEEK_ON_DEVICE, EACCES },
    { ERROR_DIR_NOT_EMPTY, ENOTEMPTY },
    { ERROR_NOT_LOCKED, EACCES },
    { ERROR_BAD_PATHNAME, ENOENT },
    { ERROR_MAX_THRDS_REACHED, EAGAIN },
    { ERROR_LOCK_FAILED, EACCES },
    { ERROR_ALREADY_EXISTS, EEXIST },
    { ERROR_FILENAME_EXCED_RANGE, ENOENT },
    { ERROR_NESTING_NOT_ALLOWED, EAGAIN },
    { ERROR_NOT_ENOUGH_QUOTA, ENOMEM }
};

int __cdecl getwinerrnocode(IN unsigned long winerrcode)
{
    _doserrno = winerrcode;

    for (int i = 0; i < (sizeof(errtable) / sizeof(errtable[0])); ++i)
    {
        if (winerrcode == errtable[i].winerrcode)
            return errtable[i].errnocode;
    }

    if (winerrcode >= ERROR_WRITE_PROTECT &&
        winerrcode <= ERROR_SHARING_BUFFER_EXCEEDED)
        return EACCES;
    else if (winerrcode >= ERROR_INVALID_STARTING_CODESEG &&
             winerrcode <= ERROR_INFLOOP_IN_RELOC_CHAIN)
             return ENOEXEC;

    return EINVAL;
}

/* +===================+ */
/* | PRELOAD FUNCTIONS | */
/* +===================+ */

// LoadSystemLibrary
// -----------------
// Implemented: 100%

HMODULE WINAPI LoadSystemLibrary(LPCTSTR lpLibFileName)
{
    HMODULE hModule = NULL;

    if (lpLibFileName && lpLibFileName[0])
    {
        LPTSTR lpLibPathBuffer;

        UINT cchBuffer = GetSystemDirectory((LPTSTR) &lpLibPathBuffer, 0);

        if (cchBuffer)
        {
            UINT cchLibFilePath = _tcslen(lpLibFileName);

            cchBuffer += cchLibFilePath + 1;

            lpLibPathBuffer = RtlAllocateHeap(g_hProcessHeap, 0, cchBuffer * sizeof(TCHAR));

            if (lpLibPathBuffer)
            {
                DWORD Win32ErrorCode;

                UINT cchTemporary = GetSystemDirectory(lpLibPathBuffer, cchBuffer);

                if (cchTemporary + cchLibFilePath + 2 == cchBuffer)
                {
                    *(lpLibPathBuffer + cchTemporary) = (TCHAR) '\\';

                    memcpy(lpLibPathBuffer + cchTemporary + 1, lpLibFileName, (cchBuffer - cchTemporary - 2) * sizeof(TCHAR));

                    *(lpLibPathBuffer + cchTemporary + cchLibFilePath + 1) = (TCHAR) 0;

                    hModule = LoadLibrary(lpLibPathBuffer);

                    Win32ErrorCode = g_pRtlGetLastWin32Error();
                }
                else
                {
                    if (cchTemporary) Win32ErrorCode = ERROR_INTERNAL_ERROR;
                    else Win32ErrorCode = g_pRtlGetLastWin32Error();

                    hModule = LoadLibrary(lpLibFileName);
                }
                RtlFreeHeap(g_hProcessHeap, 0, lpLibPathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);
            }
            else hModule = LoadLibrary(lpLibFileName);
        }
        else hModule = LoadLibrary(lpLibFileName);
    }
    else g_pRtlSetLastWin32Error(ERROR_MOD_NOT_FOUND);

    return hModule;
}

// PreloadPSAPILibrary
// -------------------
// Implemented: 100%

DWORD PreloadPSAPILibrary()
{
    g_hModulePSAPI = GetModuleHandle(TEXT("PSAPI.DLL"));

    if (!g_hModulePSAPI)
    {
        g_hModulePSAPI = LoadSystemLibrary(TEXT("PSAPI.DLL"));
        if (g_hModulePSAPI) IsPSAPIModuleManuallyLoaded = TRUE;
        else return g_pRtlGetLastWin32Error();
    }
    else IsPSAPIModuleManuallyLoaded = FALSE;

    return NO_ERROR;
}

// PreloadPSAPIProcedure
// ---------------------
// Implemented: 100%

DWORD PreloadPSAPIProcedure(IN LPCSTR lpszProcedureName, IN FARPROC lpProcedureStubAddress, OUT FARPROC *lpOut)
{
    DWORD Win32ErrorCode;

    if (!g_hModulePSAPI)
    {
        Win32ErrorCode = PreloadPSAPILibrary();
        if (Win32ErrorCode)
            return Win32ErrorCode;
    }

    FARPROC pProcedureAddress = GetProcAddress(g_hModulePSAPI, lpszProcedureName);
    if (pProcedureAddress)
    {
        *lpOut = pProcedureAddress;
        Win32ErrorCode = NO_ERROR;
    }
    else
    {
        *lpOut = lpProcedureStubAddress;
        Win32ErrorCode = g_pRtlGetLastWin32Error();
    }

    return Win32ErrorCode;
}

// preloadGetMappedFileNameA
// -------------------------
// Implemented: 100%

DWORD WINAPI preloadGetMappedFileNameA(IN HANDLE hProcess, IN LPVOID lpv, OUT LPSTR lpFilename, IN DWORD nSize)
{
    if (PreloadPSAPIProcedure("GetMappedFileNameA", stubGetMappedFileNameA, &g_pGetMappedFileNameA)) return 0;
    return g_pGetMappedFileNameA(hProcess, lpv, lpFilename, nSize);
}

// preloadGetMappedFileNameW
// -------------------------
// Implemented: 100%

DWORD WINAPI preloadGetMappedFileNameW(IN HANDLE hProcess, IN LPVOID lpv, OUT LPWSTR lpFilename, IN DWORD nSize)
{
    if (PreloadPSAPIProcedure("GetMappedFileNameW", stubGetMappedFileNameW, &g_pGetMappedFileNameW)) return 0;
    return g_pGetMappedFileNameW(hProcess, lpv, lpFilename, nSize);
}

// preloadGetModuleFileNameExA
// ---------------------------
// Implemented: 100%

DWORD WINAPI preloadGetModuleFileNameExA(IN HANDLE hProcess, IN HMODULE hModule, OUT LPSTR lpFilename, IN DWORD nSize)
{
    if (PreloadPSAPIProcedure("GetModuleFileNameExA", stubGetModuleFileNameExA, &g_pGetModuleFileNameExA)) return 0;
    return g_pGetModuleFileNameExA(hProcess, hModule, lpFilename, nSize);
}

// preloadGetModuleFileNameExW
// ---------------------------
// Implemented: 100%

DWORD WINAPI preloadGetModuleFileNameExW(IN HANDLE hProcess, IN HMODULE hModule, OUT LPWSTR lpFilename, IN DWORD nSize)
{
    if (PreloadPSAPIProcedure("GetModuleFileNameExW", stubGetModuleFileNameExW, &g_pGetModuleFileNameExW)) return 0;
    return g_pGetModuleFileNameExW(hProcess, hModule, lpFilename, nSize);
}

/* +====================+ */
/* | EXTERNAL FUNCTIONS | */
/* +====================+ */

// NTFSLPS_AddPathEndingBackslashA
// -------------------------------
// Implemented: 100%

UINT WINAPI NTFSLPS_AddPathEndingBackslashA(IN OUT LPSTR lpPathBuffer, IN UINT cchBuffer)
{
    UINT cchPath;

    if (lpPathBuffer)
    {
        if (cchBuffer)
        {
            for (LPSTR lpCurrentBufferPosition = lpPathBuffer; lpCurrentBufferPosition < (lpPathBuffer + cchBuffer); lpCurrentBufferPosition++)
            {
                if (!(*lpCurrentBufferPosition))
                {
                    do
                    {
                        CHAR cSymbol = *--lpCurrentBufferPosition;
                        if ((cSymbol != '\\') && (cSymbol != '/'))
                            { lpCurrentBufferPosition++; break; }
                    } while ((lpCurrentBufferPosition > lpPathBuffer));
                    cchPath = lpCurrentBufferPosition - lpPathBuffer;
                    if (cchPath >= (UNICODE_STRING_MAX_CHARS - 1))
                    {
                        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);
                        cchPath = cchBuffer;
                    }
                    else if ((cchBuffer - cchPath) >= 2)
                    {
                        lpCurrentBufferPosition[0] = '\\';
                        lpCurrentBufferPosition[1] = 0;
                        cchPath++;
                    }
                    else goto loc_ErrorInsufficientBuffer;

                    return cchPath;
                }
            }
            g_pRtlSetLastWin32Error(ERROR_MORE_DATA);

            goto loc_ReturnBufferLength;
        }
        else { loc_ErrorInsufficientBuffer: g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnBufferLength; }
    }
    else { g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER); loc_ReturnBufferLength: cchPath = cchBuffer; }

    return cchPath;
}

// NTFSLPS_AddPathEndingBackslashW
// -------------------------------
// Implemented: 100%

UINT WINAPI NTFSLPS_AddPathEndingBackslashW(IN OUT LPWSTR lpPathBuffer, IN UINT cchBuffer)
{
    UINT cchPath;

    if (lpPathBuffer)
    {
        if (cchBuffer)
        {
            for (LPWSTR lpCurrentBufferPosition = lpPathBuffer; lpCurrentBufferPosition < (lpPathBuffer + cchBuffer); lpCurrentBufferPosition++)
            {
                if (!(*lpCurrentBufferPosition))
                {
                    do
                    {
                        WCHAR cSymbol = *--lpCurrentBufferPosition;
                        if ((cSymbol != '\\') && (cSymbol != '/'))
                            { lpCurrentBufferPosition++; break; }
                    } while ((lpCurrentBufferPosition > lpPathBuffer));
                    cchPath = lpCurrentBufferPosition - lpPathBuffer;
                    if (cchPath >= (UNICODE_STRING_MAX_CHARS - 1))
                    {
                        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);
                        cchPath = cchBuffer;
                    }
                    else if ((cchBuffer - cchPath) >= 2)
                    {
                        lpCurrentBufferPosition[0] = '\\';
                        lpCurrentBufferPosition[1] = 0;
                        cchPath++;
                    }
                    else goto loc_ErrorInsufficientBuffer;

                    return cchPath;
                }
            }
            g_pRtlSetLastWin32Error(ERROR_MORE_DATA);

            goto loc_ReturnBufferLength;
        }
        else { loc_ErrorInsufficientBuffer: g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnBufferLength; }
    }
    else { g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER); loc_ReturnBufferLength: cchPath = cchBuffer; }

    return cchPath;
}

// NTFSLPS_AppendPathComponentA
// ----------------------------
// Implemented: 100%

UINT WINAPI NTFSLPS_AppendPathComponentA(IN OUT LPSTR lpPathBuffer, IN UINT cchBuffer, IN LPCSTR lpComponent)
{
    UINT cchPath;

    if (lpPathBuffer)
    {
        if (cchBuffer)
        {
            for (LPSTR lpCurrentBufferPosition = lpPathBuffer; lpCurrentBufferPosition < (lpPathBuffer + cchBuffer); lpCurrentBufferPosition++)
            {
                if (!(*lpCurrentBufferPosition))
                {
                    CHAR cSymbol;
                    do { cSymbol = *--lpCurrentBufferPosition;
                        if ((cSymbol != '\\') && (cSymbol != '/'))
                            { lpCurrentBufferPosition++; break; }
                    } while ((lpCurrentBufferPosition > lpPathBuffer));
                    cchPath = lpCurrentBufferPosition - lpPathBuffer;
                    if (cchPath >= (UNICODE_STRING_MAX_CHARS - 1))
                    {
                        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);
                        cchPath = cchBuffer;
                    }
                    else if ((cchBuffer - cchPath) >= 2)
                    {
                        *lpCurrentBufferPosition++ = '\\';
                        cchPath++;
                        if (lpComponent)
                        {
                            cchPath = cchBuffer - cchPath;
                            LPSTR lpComponentCurrentPosition = (LPSTR) lpComponent;
                            do { cSymbol = *lpComponentCurrentPosition++;
                                *lpCurrentBufferPosition++ = cSymbol;
                                if (!--cchPath)
                                    goto loc_ErrorInsufficientBuffer;
                            } while (cSymbol);
                            cchPath = (cchBuffer - cchPath) - 1;
                        }
                        else *lpCurrentBufferPosition = 0;
                    }
                    else goto loc_ErrorInsufficientBuffer;

                    return cchPath;
                }
            }
            g_pRtlSetLastWin32Error(ERROR_MORE_DATA);

            goto loc_ReturnBufferLength;
        }
        else { loc_ErrorInsufficientBuffer: g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnBufferLength; }
    }
    else { g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER); loc_ReturnBufferLength: cchPath = cchBuffer; }

    return cchPath;
}

// NTFSLPS_AppendPathComponentW
// ----------------------------
// Implemented: 100%

UINT WINAPI NTFSLPS_AppendPathComponentW(IN OUT LPWSTR lpPathBuffer, IN UINT cchBuffer, IN LPCWSTR lpComponent)
{
    UINT cchPath;

    if (lpPathBuffer)
    {
        if (cchBuffer)
        {
            for (LPWSTR lpCurrentBufferPosition = lpPathBuffer; lpCurrentBufferPosition < (lpPathBuffer + cchBuffer); lpCurrentBufferPosition++)
            {
                if (!(*lpCurrentBufferPosition))
                {
                    WCHAR cSymbol;
                    do { cSymbol = *--lpCurrentBufferPosition;
                        if ((cSymbol != '\\') && (cSymbol != '/'))
                            { lpCurrentBufferPosition++; break; }
                    } while ((lpCurrentBufferPosition > lpPathBuffer));
                    cchPath = lpCurrentBufferPosition - lpPathBuffer;
                    if (cchPath >= (UNICODE_STRING_MAX_CHARS - 1))
                    {
                        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);
                        cchPath = cchBuffer;
                    }
                    else if ((cchBuffer - cchPath) >= 2)
                    {
                        *lpCurrentBufferPosition++ = '\\';
                        cchPath++;
                        if (lpComponent)
                        {
                            cchPath = cchBuffer - cchPath;
                            LPWSTR lpComponentCurrentPosition = (LPWSTR) lpComponent;
                            do { cSymbol = *lpComponentCurrentPosition++;
                                *lpCurrentBufferPosition++ = cSymbol;
                                if (!--cchPath)
                                    goto loc_ErrorInsufficientBuffer;
                            } while (cSymbol);
                            cchPath = (cchBuffer - cchPath) - 1;
                        }
                        else *lpCurrentBufferPosition = 0;
                    }
                    else goto loc_ErrorInsufficientBuffer;

                    return cchPath;
                }
            }
            g_pRtlSetLastWin32Error(ERROR_MORE_DATA);

            goto loc_ReturnBufferLength;
        }
        else { loc_ErrorInsufficientBuffer: g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnBufferLength; }
    }
    else { g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER); loc_ReturnBufferLength: cchPath = cchBuffer; }

    return cchPath;
}

// NTFSLPS_CopyFile2
// -----------------
// Implemented: 100%

HRESULT WINAPI NTFSLPS_CopyFile2(IN PCWSTR pwszExistingFileName, IN PCWSTR pwszNewFileName, IN COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters)
{
    LPWSTR lpExistingObjectPathBuffer;
    LPWSTR lpNewObjectPathBuffer;
    DWORD Win32ErrorCode;
    HRESULT hResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(pwszExistingFileName) ||
        IsNTFSi30PathW(pwszNewFileName)))
        return STATUS_OBJECT_PATH_SYNTAX_BAD;
    #endif

    lpExistingObjectPathBuffer = GetFullObjectPathW(pwszExistingFileName);

    if (lpExistingObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewObjectPathBuffer = GetFullObjectPathW(pwszNewFileName);

        if (lpNewObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            hResult = g_pCopyFile2(lpExistingObjectPathBuffer, lpNewObjectPathBuffer, pExtendedParameters);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewObjectPathBuffer);
        }
        else
        {
            hResult = g_pCopyFile2(pwszExistingFileName, pwszNewFileName, pExtendedParameters);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hResult = g_pCopyFile2(pwszExistingFileName, pwszNewFileName, pExtendedParameters);

    return hResult;
}

// NTFSLPS_CopyFileA
// -----------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CopyFileA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN BOOL bFailIfExists)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpExistingFileName) ||
        IsNTFSi30PathA(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingFilePathBuffer = GetFullObjectPathA(lpExistingFileName);

    if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewFilePathBuffer = GetFullObjectPathA(lpNewFileName);

        if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = CopyFileW(lpExistingFilePathBuffer, lpNewFilePathBuffer, bFailIfExists);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewFilePathBuffer);
        }
        else
        {
            bResult = CopyFileA(lpExistingFileName, lpNewFileName, bFailIfExists);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CopyFileA(lpExistingFileName, lpNewFileName, bFailIfExists);

    return bResult;
}

// NTFSLPS_CopyFileExA
// -------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CopyFileExA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags)
{
    LPWSTR lpExistingObjectPathBuffer;
    LPWSTR lpNewObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpExistingFileName) ||
        IsNTFSi30PathA(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingObjectPathBuffer = GetFullObjectPathA(lpExistingFileName);

    if (lpExistingObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewObjectPathBuffer = GetFullObjectPathA(lpNewFileName);

        if (lpNewObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = CopyFileExW(lpExistingObjectPathBuffer, lpNewObjectPathBuffer, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewObjectPathBuffer);
        }
        else
        {
            bResult = CopyFileExA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CopyFileExA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);

    return bResult;
}

// NTFSLPS_CopyFileExW
// -------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CopyFileExW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags)
{
    LPWSTR lpExistingObjectPathBuffer;
    LPWSTR lpNewObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpExistingFileName) ||
        IsNTFSi30PathW(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingObjectPathBuffer = GetFullObjectPathW(lpExistingFileName);

    if (lpExistingObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewObjectPathBuffer = GetFullObjectPathW(lpNewFileName);

        if (lpNewObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = CopyFileExW(lpExistingObjectPathBuffer, lpNewObjectPathBuffer, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewObjectPathBuffer);
        }
        else
        {
            bResult = CopyFileExW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CopyFileExW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);

    return bResult;
}

// NTFSLPS_CopyFileTransactedA
// ---------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CopyFileTransactedA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction)
{
    LPWSTR lpExistingObjectPathBuffer;
    LPWSTR lpNewObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathA(lpExistingFileName) ||
            IsNTFSi30PathA(lpNewFileName))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpExistingObjectPathBuffer = GetFullObjectPathA(lpExistingFileName);

        if (lpExistingObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpNewObjectPathBuffer = GetFullObjectPathA(lpNewFileName);

            if (lpNewObjectPathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pCopyFileTransactedW(lpExistingObjectPathBuffer, lpNewObjectPathBuffer, lpProgressRoutine, lpData, pbCancel, dwCopyFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpNewObjectPathBuffer);
            }
            else
            {
                bResult = g_pCopyFileTransactedA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpExistingObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pCopyFileTransactedA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags, hTransaction);
    }
    else bResult = NTFSLPS_CopyFileExA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);

    return bResult;
}

// NTFSLPS_CopyFileTransactedW
// ---------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CopyFileTransactedW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction)
{
    LPWSTR lpExistingObjectPathBuffer;
    LPWSTR lpNewObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathW(lpExistingFileName) ||
            IsNTFSi30PathW(lpNewFileName))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpExistingObjectPathBuffer = GetFullObjectPathW(lpExistingFileName);

        if (lpExistingObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpNewObjectPathBuffer = GetFullObjectPathW(lpNewFileName);

            if (lpNewObjectPathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pCopyFileTransactedW(lpExistingObjectPathBuffer, lpNewObjectPathBuffer, lpProgressRoutine, lpData, pbCancel, dwCopyFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpNewObjectPathBuffer);
            }
            else
            {
                bResult = g_pCopyFileTransactedW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpExistingObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pCopyFileTransactedW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags, hTransaction);
    }
    else bResult = NTFSLPS_CopyFileExW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, pbCancel, dwCopyFlags);

    return bResult;
}

// NTFSLPS_CopyFileW
// -----------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CopyFileW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN BOOL bFailIfExists)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpExistingFileName) ||
        IsNTFSi30PathW(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingFilePathBuffer = GetFullObjectPathW(lpExistingFileName);

    if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewFilePathBuffer = GetFullObjectPathW(lpNewFileName);

        if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = CopyFileW(lpExistingFilePathBuffer, lpNewFilePathBuffer, bFailIfExists);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewFilePathBuffer);
        }
        else
        {
            bResult = CopyFileW(lpExistingFileName, lpNewFileName, bFailIfExists);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CopyFileW(lpExistingFileName, lpNewFileName, bFailIfExists);

    return bResult;
}

// NTFSLPS_CreateDirectoryA
// ------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateDirectoryA(IN LPCSTR lpPathName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpPathName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = CreateDirectoryW(lpObjectPathBuffer, lpSecurityAttributes);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CreateDirectoryA(lpPathName, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateDirectoryExA
// --------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateDirectoryExA(IN LPCSTR lpTemplateDirectory, IN LPCSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    LPWSTR lpTemplateDirectoryPathBuffer;
    LPWSTR lpNewDirectoryPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpTemplateDirectory) ||
        IsNTFSi30PathA(lpNewDirectory))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpTemplateDirectoryPathBuffer = GetFullObjectPathA(lpTemplateDirectory);

    if (lpTemplateDirectoryPathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewDirectoryPathBuffer = GetFullObjectPathA(lpNewDirectory);

        if (lpNewDirectoryPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = CreateDirectoryExW(lpTemplateDirectoryPathBuffer, lpNewDirectoryPathBuffer, lpSecurityAttributes);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewDirectoryPathBuffer);
        }
        else
        {
            bResult = CreateDirectoryExA(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpTemplateDirectoryPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CreateDirectoryExA(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateDirectoryExW
// --------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateDirectoryExW(IN LPCWSTR lpTemplateDirectory, IN LPCWSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    LPWSTR lpTemplateDirectoryPathBuffer;
    LPWSTR lpNewDirectoryPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpTemplateDirectory) ||
        IsNTFSi30PathW(lpNewDirectory))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpTemplateDirectoryPathBuffer = GetFullObjectPathW(lpTemplateDirectory);

    if (lpTemplateDirectoryPathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewDirectoryPathBuffer = GetFullObjectPathW(lpNewDirectory);

        if (lpNewDirectoryPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = CreateDirectoryExW(lpTemplateDirectoryPathBuffer, lpNewDirectoryPathBuffer, lpSecurityAttributes);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewDirectoryPathBuffer);
        }
        else
        {
            bResult = CreateDirectoryExW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpTemplateDirectoryPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CreateDirectoryExW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateDirectoryTransactedA
// ----------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateDirectoryTransactedA(IN LPCSTR lpTemplateDirectory, IN LPCSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction)
{
    LPWSTR lpTemplateDirectoryPathBuffer;
    LPWSTR lpNewDirectoryPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathA(lpTemplateDirectory) ||
            IsNTFSi30PathA(lpNewDirectory))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpTemplateDirectoryPathBuffer = GetFullObjectPathA(lpTemplateDirectory);

        if (lpTemplateDirectoryPathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpNewDirectoryPathBuffer = GetFullObjectPathA(lpNewDirectory);

            if (lpNewDirectoryPathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pCreateDirectoryTransactedW(lpTemplateDirectoryPathBuffer, lpNewDirectoryPathBuffer, lpSecurityAttributes, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpNewDirectoryPathBuffer);
            }
            else
            {
                bResult = g_pCreateDirectoryTransactedA(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpTemplateDirectoryPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pCreateDirectoryTransactedA(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes, hTransaction);
    }
    else bResult = NTFSLPS_CreateDirectoryExA(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateDirectoryTransactedW
// ----------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateDirectoryTransactedW(IN LPCWSTR lpTemplateDirectory, IN LPCWSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction)
{
    LPWSTR lpTemplateDirectoryPathBuffer;
    LPWSTR lpNewDirectoryPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathW(lpTemplateDirectory) ||
            IsNTFSi30PathW(lpNewDirectory))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpTemplateDirectoryPathBuffer = GetFullObjectPathW(lpTemplateDirectory);

        if (lpTemplateDirectoryPathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpNewDirectoryPathBuffer = GetFullObjectPathW(lpNewDirectory);

            if (lpNewDirectoryPathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pCreateDirectoryTransactedW(lpTemplateDirectoryPathBuffer, lpNewDirectoryPathBuffer, lpSecurityAttributes, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpNewDirectoryPathBuffer);
            }
            else
            {
                bResult = g_pCreateDirectoryTransactedW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpTemplateDirectoryPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pCreateDirectoryTransactedW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes, hTransaction);
    }
    else bResult = NTFSLPS_CreateDirectoryExW(lpTemplateDirectory, lpNewDirectory, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateDirectoryW
// ------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateDirectoryW(IN LPCWSTR lpPathName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpPathName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = CreateDirectoryW(lpObjectPathBuffer, lpSecurityAttributes);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CreateDirectoryW(lpPathName, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateFile2
// -------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_CreateFile2(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN DWORD dwCreationDisposition, IN LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hObject;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hObject = g_pCreateFile2(lpObjectPathBuffer, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hObject = g_pCreateFile2(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, pCreateExParams);

    return hObject;
}

// NTFSLPS_CreateFileA
// -------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_CreateFileA(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hObject;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hObject = CreateFileW(lpObjectPathBuffer, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hObject = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

    return hObject;
}

// NTFSLPS_CreateFileBackupA
// -------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_CreateFileBackupA(IN LPCSTR lpExistingFileName)
{
    LPWSTR lpExistingFilePathBuffer;
    UINT cExistingFilePathLength;
    LPWSTR lpBackupFilePathBuffer;
    UINT cBackupFilePathBufferLength;
    DWORD Win32ErrorCode;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpExistingFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return ERROR_BAD_PATHNAME; }
    #endif

    if (lpExistingFileName && lpExistingFileName[0])
    {
        lpExistingFilePathBuffer = GetFullObjectPathExA(lpExistingFileName, TRUE);

        if (lpExistingFilePathBuffer)
        {
            cExistingFilePathLength = wcslen(lpExistingFilePathBuffer);

            cBackupFilePathBufferLength = (cExistingFilePathLength + sizeof(wcszBackupFileExtension) / sizeof(WCHAR)) * sizeof(WCHAR);

            lpBackupFilePathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, cBackupFilePathBufferLength);

            memcpy(lpBackupFilePathBuffer, lpExistingFilePathBuffer, cExistingFilePathLength * sizeof(WCHAR));
            memcpy(lpBackupFilePathBuffer + cExistingFilePathLength, wcszBackupFileExtension, sizeof(wcszBackupFileExtension) - sizeof(WCHAR));

            if (CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, FALSE))
                Win32ErrorCode = NO_ERROR;
            else Win32ErrorCode = g_pRtlGetLastWin32Error();

            RtlFreeHeap(g_hProcessHeap, 0, lpBackupFilePathBuffer);

            FreeFullObjectPathBuffer(lpExistingFilePathBuffer);
        }
        else return g_pRtlGetLastWin32Error();
    }
    else Win32ErrorCode = ERROR_PATH_NOT_FOUND;

    g_pRtlSetLastWin32Error(Win32ErrorCode);

    return Win32ErrorCode;
}

// NTFSLPS_CreateFileBackupExA
// ---------------------------
// Implemented: 100%

UINT WINAPI NTFSLPS_CreateFileBackupExA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN BOOL bNumerate)
{
    LPWSTR lpExistingFilePathBuffer;
    UINT cExistingFilePathLength;
    LPWSTR lpBackupFilePathBuffer;
    UINT cBackupFilePathBufferLength;
    DWORD Win32ErrorCode;
    UINT uCounter;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpExistingFileName) || 
        IsNTFSi30PathA(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return -1; }
    #endif

    if (lpExistingFileName && lpExistingFileName[0])
    {
        if (lpNewFileName && lpNewFileName[0])
        {
            lpExistingFilePathBuffer = GetFullObjectPathA(lpExistingFileName);

            if (lpExistingFilePathBuffer)
            {
                lpBackupFilePathBuffer = GetFullObjectPathExA(lpNewFileName, TRUE);

                if (lpBackupFilePathBuffer)
                {
                    if (!CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, TRUE))
                    {
                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                        if (Win32ErrorCode == ERROR_FILE_EXISTS)
                        {
                            uCounter = 1;

                            cExistingFilePathLength = wcslen(lpBackupFilePathBuffer);

                            *(lpBackupFilePathBuffer + cExistingFilePathLength) = (WCHAR) '.';

                            _ultow_s(uCounter, lpBackupFilePathBuffer + (cExistingFilePathLength + 1), 11 * sizeof(WCHAR), 10);

                            while (!CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, TRUE))
                            {
                                Win32ErrorCode = g_pRtlGetLastWin32Error();

                                if (Win32ErrorCode != ERROR_FILE_EXISTS)
                                {
                                    uCounter = -1;

                                    break;
                                }
                                if (uCounter >= 4294967294)
                                {
                                    Win32ErrorCode = ERROR_CANNOT_MAKE;

                                    uCounter = -1;

                                    break;
                                }
                                uCounter++;

                                _ultow_s(uCounter, lpBackupFilePathBuffer + (cExistingFilePathLength + 1), 11 * sizeof(WCHAR), 10);
                            }
                            if (uCounter != -1) Win32ErrorCode = NO_ERROR;
                        }
                        else uCounter = -1;
                    }
                    else
                    {
                        Win32ErrorCode = NO_ERROR;

                        uCounter = 0;
                    }
                    FreeFullObjectPathBuffer(lpBackupFilePathBuffer);
                }
                else
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    uCounter = -1;
                }
                FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);
            }
            else uCounter = -1;
        }
        else
        {
            lpExistingFilePathBuffer = GetFullObjectPathExA(lpExistingFileName, TRUE);

            if (lpExistingFilePathBuffer)
            {
                cExistingFilePathLength = wcslen(lpExistingFilePathBuffer);

                cBackupFilePathBufferLength = (cExistingFilePathLength + sizeof(wcszBackupFileExtension) + 10) * sizeof(WCHAR);

                lpBackupFilePathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, cBackupFilePathBufferLength);

                if (lpBackupFilePathBuffer)
                {
                    memcpy(lpBackupFilePathBuffer, lpExistingFilePathBuffer, cExistingFilePathLength * sizeof(WCHAR));
                    memcpy(lpBackupFilePathBuffer + cExistingFilePathLength, wcszBackupFileExtension, sizeof(wcszBackupFileExtension) - sizeof(WCHAR));

                    if (!CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, TRUE))
                    {
                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                        if (g_pRtlGetLastWin32Error() == ERROR_FILE_EXISTS)
                        {
                            uCounter = 1;

                            *(lpBackupFilePathBuffer + cExistingFilePathLength + sizeof(wcszBackupFileExtension) / sizeof(WCHAR) - 1) = (WCHAR) '.';

                            _ultow_s(uCounter, lpBackupFilePathBuffer + cExistingFilePathLength + sizeof(wcszBackupFileExtension) / sizeof(WCHAR), 11 * sizeof(WCHAR), 10);

                            while (!CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, TRUE))
                            {
                                Win32ErrorCode = g_pRtlGetLastWin32Error();

                                if (Win32ErrorCode != ERROR_FILE_EXISTS)
                                {
                                    uCounter = -1;

                                    break;
                                }
                                if (uCounter >= 4294967294)
                                {
                                    Win32ErrorCode = ERROR_CANNOT_MAKE;

                                    uCounter = -1;

                                    break;
                                }
                                uCounter++;

                                _ultow_s(uCounter, lpBackupFilePathBuffer + cExistingFilePathLength + sizeof(wcszBackupFileExtension) / sizeof(WCHAR), 11 * sizeof(WCHAR), 10);
                            }
                            if (uCounter != -1) Win32ErrorCode = NO_ERROR;
                        }
                        else uCounter = -1;
                    }
                    else
                    {
                        Win32ErrorCode = NO_ERROR;

                        uCounter = 0;
                    }
                    RtlFreeHeap(g_hProcessHeap, 0, lpBackupFilePathBuffer);
                }
                else
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    uCounter = -1;
                }
                FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);
            }
            else uCounter = -1;
        }
    }
    else
    {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);

        uCounter = -1;
    }
    return uCounter;
}

// NTFSLPS_CreateFileBackupExW
// ---------------------------
// Implemented: 100%

UINT WINAPI NTFSLPS_CreateFileBackupExW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN BOOL bNumerate)
{
    LPWSTR lpExistingFilePathBuffer;
    UINT cExistingFilePathLength;
    LPWSTR lpBackupFilePathBuffer;
    UINT cBackupFilePathBufferLength;
    DWORD Win32ErrorCode;
    UINT uCounter;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpExistingFileName) || 
        IsNTFSi30PathW(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return -1; }
    #endif

    if (lpExistingFileName && lpExistingFileName[0])
    {
        if (lpNewFileName && lpNewFileName[0])
        {
            lpExistingFilePathBuffer = GetFullObjectPathW(lpExistingFileName);

            if (lpExistingFilePathBuffer)
            {
                lpBackupFilePathBuffer = GetFullObjectPathExW(lpNewFileName, TRUE);

                if (lpBackupFilePathBuffer)
                {
                    if (!CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, TRUE))
                    {
                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                        if (Win32ErrorCode == ERROR_FILE_EXISTS)
                        {
                            uCounter = 1;

                            cExistingFilePathLength = wcslen(lpBackupFilePathBuffer);

                            *(lpBackupFilePathBuffer + cExistingFilePathLength) = (WCHAR) '.';

                            _ultow_s(uCounter, lpBackupFilePathBuffer + (cExistingFilePathLength + 1), 11 * sizeof(WCHAR), 10);

                            while (!CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, TRUE))
                            {
                                Win32ErrorCode = g_pRtlGetLastWin32Error();

                                if (Win32ErrorCode != ERROR_FILE_EXISTS)
                                {
                                    uCounter = -1;

                                    break;
                                }
                                if (uCounter >= 4294967294)
                                {
                                    Win32ErrorCode = ERROR_CANNOT_MAKE;

                                    uCounter = -1;

                                    break;
                                }
                                uCounter++;

                                _ultow_s(uCounter, lpBackupFilePathBuffer + (cExistingFilePathLength + 1), 11 * sizeof(WCHAR), 10);
                            }
                            if (uCounter != -1) Win32ErrorCode = NO_ERROR;
                        }
                        else uCounter = -1;
                    }
                    else
                    {
                        Win32ErrorCode = NO_ERROR;

                        uCounter = 0;
                    }
                    FreeFullObjectPathBuffer(lpBackupFilePathBuffer);
                }
                else
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    uCounter = -1;
                }
                FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);
            }
            else uCounter = -1;
        }
        else
        {
            lpExistingFilePathBuffer = GetFullObjectPathExW(lpExistingFileName, TRUE);

            if (lpExistingFilePathBuffer)
            {
                cExistingFilePathLength = wcslen(lpExistingFilePathBuffer);

                cBackupFilePathBufferLength = (cExistingFilePathLength + sizeof(wcszBackupFileExtension) + 10) * sizeof(WCHAR);

                lpBackupFilePathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, cBackupFilePathBufferLength);

                if (lpBackupFilePathBuffer)
                {
                    memcpy(lpBackupFilePathBuffer, lpExistingFilePathBuffer, cExistingFilePathLength * sizeof(WCHAR));
                    memcpy(lpBackupFilePathBuffer + cExistingFilePathLength, wcszBackupFileExtension, sizeof(wcszBackupFileExtension) - sizeof(WCHAR));

                    if (!CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, TRUE))
                    {
                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                        if (g_pRtlGetLastWin32Error() == ERROR_FILE_EXISTS)
                        {
                            uCounter = 1;

                            *(lpBackupFilePathBuffer + cExistingFilePathLength + sizeof(wcszBackupFileExtension) / sizeof(WCHAR) - 1) = (WCHAR) '.';

                            _ultow_s(uCounter, lpBackupFilePathBuffer + cExistingFilePathLength + sizeof(wcszBackupFileExtension) / sizeof(WCHAR), 11 * sizeof(WCHAR), 10);

                            while (!CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, TRUE))
                            {
                                Win32ErrorCode = g_pRtlGetLastWin32Error();

                                if (Win32ErrorCode != ERROR_FILE_EXISTS)
                                {
                                    uCounter = -1;

                                    break;
                                }
                                if (uCounter >= 4294967294)
                                {
                                    Win32ErrorCode = ERROR_CANNOT_MAKE;

                                    uCounter = -1;

                                    break;
                                }
                                uCounter++;

                                _ultow_s(uCounter, lpBackupFilePathBuffer + cExistingFilePathLength + sizeof(wcszBackupFileExtension) / sizeof(WCHAR), 11 * sizeof(WCHAR), 10);
                            }
                            if (uCounter != -1) Win32ErrorCode = NO_ERROR;
                        }
                        else uCounter = -1;
                    }
                    else
                    {
                        Win32ErrorCode = NO_ERROR;

                        uCounter = 0;
                    }
                    RtlFreeHeap(g_hProcessHeap, 0, lpBackupFilePathBuffer);
                }
                else
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    uCounter = -1;
                }
                FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);
            }
            else uCounter = -1;
        }
    }
    else
    {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);

        uCounter = -1;
    }
    return uCounter;
}

// NTFSLPS_CreateFileBackupW
// -------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_CreateFileBackupW(IN LPCWSTR lpExistingFileName)
{
    LPWSTR lpExistingFilePathBuffer;
    UINT cExistingFilePathLength;
    LPWSTR lpBackupFilePathBuffer;
    UINT cBackupFilePathBufferLength;
    DWORD Win32ErrorCode;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpExistingFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return ERROR_BAD_PATHNAME; }
    #endif

    if (lpExistingFileName && lpExistingFileName[0])
    {
        lpExistingFilePathBuffer = GetFullObjectPathExW(lpExistingFileName, TRUE);

        if (lpExistingFilePathBuffer)
        {
            cExistingFilePathLength = wcslen(lpExistingFilePathBuffer);

            cBackupFilePathBufferLength = (cExistingFilePathLength + sizeof(wcszBackupFileExtension) / sizeof(WCHAR)) * sizeof(WCHAR);

            lpBackupFilePathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, cBackupFilePathBufferLength);

            memcpy(lpBackupFilePathBuffer, lpExistingFilePathBuffer, cExistingFilePathLength * sizeof(WCHAR));
            memcpy(lpBackupFilePathBuffer + cExistingFilePathLength, wcszBackupFileExtension, sizeof(wcszBackupFileExtension) - sizeof(WCHAR));

            if (CopyFileW(lpExistingFilePathBuffer, lpBackupFilePathBuffer, FALSE))
                Win32ErrorCode = NO_ERROR;
            else Win32ErrorCode = g_pRtlGetLastWin32Error();

            RtlFreeHeap(g_hProcessHeap, 0, lpBackupFilePathBuffer);

            FreeFullObjectPathBuffer(lpExistingFilePathBuffer);
        }
        else return g_pRtlGetLastWin32Error();
    }
    else Win32ErrorCode = ERROR_PATH_NOT_FOUND;

    g_pRtlSetLastWin32Error(Win32ErrorCode);

    return Win32ErrorCode;
}

// NTFSLPS_CreateFileTransactedA
// -----------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_CreateFileTransactedA(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hObject;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hObject = g_pCreateFileTransactedW(lpObjectPathBuffer, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, hTransaction, pusMiniVersion, lpExtendedParameter);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hObject = g_pCreateFileTransactedA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, hTransaction, pusMiniVersion, lpExtendedParameter);

    return hObject;
}

// NTFSLPS_CreateFileTransactedW
// -----------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_CreateFileTransactedW(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hObject;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hObject = g_pCreateFileTransactedW(lpObjectPathBuffer, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, hTransaction, pusMiniVersion, lpExtendedParameter);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hObject = g_pCreateFileTransactedW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, hTransaction, pusMiniVersion, lpExtendedParameter);

    return hObject;
}

// NTFSLPS_CreateFileW
// -------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_CreateFileW(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hObject;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hObject = CreateFileW(lpObjectPathBuffer, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hObject = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

    return hObject;
}

// NTFSLPS_CreateFolderA
// ---------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateFolderA(IN LPCSTR lpPathName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    UINT uPathLength;
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpSubdirectory;
    UINT cSubdirectories = 0;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    if (!lpPathName || (lpPathName[0] == (CHAR) 0))
    {
        g_pRtlSetLastWin32Error(ERROR_INVALID_NAME);

        return FALSE;
    }

    uPathLength = (UINT) GetFullPathExAW(lpPathName, NULL, 0, FALSE);

    if (!uPathLength)
        return FALSE;

    lpObjectPathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, uPathLength * sizeof(WCHAR));

    if (!lpObjectPathBuffer)
        return FALSE;

    uPathLength = (UINT) GetFullPathExAW(lpPathName, lpObjectPathBuffer, uPathLength, FALSE);

    if (!uPathLength)
    {
        Win32ErrorCode = g_pRtlGetLastWin32Error();

        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);

        return FALSE;
    }

    if (!wcsncmp(lpObjectPathBuffer, L"\\\\?\\UNC\\", 8))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 8, (unsigned int) '\\') + 1;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\?\\", 4))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 4, (unsigned int) '\\') + 1;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\.\\", 4))
    {
        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(ERROR_CANNOT_MAKE);

        return FALSE;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\", 2))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 2, (unsigned int) '\\') + 1;
    }
    else lpSubdirectory = wcschr(lpObjectPathBuffer, (unsigned int) '\\') + 1;

    uPathLength = wcslen(lpSubdirectory);

    if (!uPathLength)
    {
        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);

        return FALSE;
    }

    if (*(lpSubdirectory + uPathLength - 1) == (WCHAR) '\\')
        *(lpSubdirectory + uPathLength - 1) = (WCHAR) 0;

    lpSubdirectory = wcschr(lpSubdirectory, (unsigned int) '\\');

    while (lpSubdirectory)
    { 
        lpSubdirectory[0] = (WCHAR) 0;

        bResult = CreateDirectoryW(lpObjectPathBuffer, lpSecurityAttributes);

        if (bResult) cSubdirectories++;
        else
        {
            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (Win32ErrorCode != ERROR_ALREADY_EXISTS)
            {
                while (cSubdirectories)
                {
                    lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                    if (!lpSubdirectory)
                        break;

                    lpSubdirectory[0] = (WCHAR) 0;

                    if (!RemoveDirectoryW(lpObjectPathBuffer))
                        break;

                    cSubdirectories--;
                }

                RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);

                return FALSE;
            }
        }

        lpSubdirectory[0] = (WCHAR) '\\';

        lpSubdirectory = wcschr(lpSubdirectory + 1, (unsigned int) '\\');
    }

    bResult = CreateDirectoryW(lpObjectPathBuffer, lpSecurityAttributes);

    if (!bResult)
    {
        Win32ErrorCode = g_pRtlGetLastWin32Error();

        if (Win32ErrorCode != ERROR_ALREADY_EXISTS)
        {
            while (cSubdirectories)
            {
                lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                if (!lpSubdirectory)
                    break;

                lpSubdirectory[0] = (WCHAR) 0;

                if (!RemoveDirectoryW(lpObjectPathBuffer))
                    break;

                cSubdirectories--;
            }

            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);

            return FALSE;
        }
    }

    RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

    return TRUE;
}

// NTFSLPS_CreateFolderW
// ---------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateFolderW(IN LPCWSTR lpPathName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    UINT uPathLength;
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpSubdirectory;
    UINT cSubdirectories = 0;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    if (!lpPathName || (lpPathName[0] == 0))
    {
        g_pRtlSetLastWin32Error(ERROR_INVALID_NAME);

        return FALSE;
    }

    uPathLength = (UINT) NTFSLPS_GetFullPathW(lpPathName, NULL, 0);

    if (!uPathLength)
        return FALSE;

    lpObjectPathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, uPathLength * sizeof(WCHAR));

    if (!lpObjectPathBuffer)
        return FALSE;

    uPathLength = (UINT) NTFSLPS_GetFullPathW(lpPathName, lpObjectPathBuffer, uPathLength);

    if (!uPathLength)
    {
        Win32ErrorCode = g_pRtlGetLastWin32Error();

        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);

        return FALSE;
    }

    if (!wcsncmp(lpObjectPathBuffer, L"\\\\?\\UNC\\", 8))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 8, (unsigned int) '\\') + 1;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\?\\", 4))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 4, (unsigned int) '\\') + 1;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\.\\", 4))
    {
        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(ERROR_CANNOT_MAKE);

        return FALSE;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\", 2))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 2, (unsigned int) '\\') + 1;
    }
    else lpSubdirectory = wcschr(lpObjectPathBuffer, (unsigned int) '\\') + 1;

    uPathLength = wcslen(lpSubdirectory);

    if (!uPathLength)
    {
        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);

        return FALSE;
    }

    if (*(lpSubdirectory + uPathLength - 1) == (WCHAR) '\\')
        *(lpSubdirectory + uPathLength - 1) = (WCHAR) 0;

    lpSubdirectory = wcschr(lpSubdirectory, (unsigned int) '\\');

    while (lpSubdirectory)
    { 
        lpSubdirectory[0] = (WCHAR) 0;

        bResult = CreateDirectoryW(lpObjectPathBuffer, lpSecurityAttributes);

        if (bResult) cSubdirectories++;
        else
        {
            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (Win32ErrorCode != ERROR_ALREADY_EXISTS)
            {
                while (cSubdirectories)
                {
                    lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                    if (!lpSubdirectory)
                        break;

                    lpSubdirectory[0] = (WCHAR) 0;

                    if (!RemoveDirectoryW(lpObjectPathBuffer))
                        break;

                    cSubdirectories--;
                }

                RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);

                return FALSE;
            }
        }

        lpSubdirectory[0] = (WCHAR) '\\';

        lpSubdirectory = wcschr(lpSubdirectory + 1, (unsigned int) '\\');
    }

    bResult = CreateDirectoryW(lpObjectPathBuffer, lpSecurityAttributes);

    if (!bResult)
    {
        Win32ErrorCode = g_pRtlGetLastWin32Error();

        if (Win32ErrorCode != ERROR_ALREADY_EXISTS)
        {
            while (cSubdirectories)
            {
                lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                if (!lpSubdirectory)
                    break;

                lpSubdirectory[0] = (WCHAR) 0;

                if (!RemoveDirectoryW(lpObjectPathBuffer))
                    break;

                cSubdirectories--;
            }

            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);

            return FALSE;
        }
    }

    RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

    return TRUE;
}

// NTFSLPS_CreateHardLinkA
// -----------------------
// Implemented: 100%

NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateHardLinkA(IN LPCSTR lpFileName, IN LPCSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    LPWSTR lpFilePathBuffer;
    LPWSTR lpExistingFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpFileName) || 
        IsNTFSi30PathA(lpExistingFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpFilePathBuffer = GetFullObjectPathA(lpFileName);

    if (lpFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpExistingFilePathBuffer = GetFullObjectPathA(lpExistingFileName);

        if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = CreateHardLinkW(lpFilePathBuffer, lpExistingFilePathBuffer, lpSecurityAttributes);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpExistingFilePathBuffer);
        }
        else
        {
            bResult = CreateHardLinkA(lpFileName, lpExistingFileName, lpSecurityAttributes);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CreateHardLinkA(lpFileName, lpExistingFileName, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateHardLinkTransactedA
// ---------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateHardLinkTransactedA(IN LPCSTR lpFileName, IN LPCSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction)
{
    LPWSTR lpFilePathBuffer;
    LPWSTR lpExistingFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathA(lpFileName) || 
            IsNTFSi30PathA(lpExistingFileName))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpFilePathBuffer = GetFullObjectPathA(lpFileName);

        if (lpFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpExistingFilePathBuffer = GetFullObjectPathA(lpExistingFileName);

            if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pCreateHardLinkTransactedW(lpFilePathBuffer, lpExistingFilePathBuffer, lpSecurityAttributes, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpExistingFilePathBuffer);
            }
            else
            {
                bResult = g_pCreateHardLinkTransactedA(lpFileName, lpExistingFileName, lpSecurityAttributes, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpFilePathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pCreateHardLinkTransactedA(lpFileName, lpExistingFileName, lpSecurityAttributes, hTransaction);
    }
    else bResult = NTFSLPS_CreateHardLinkA(lpFileName, lpExistingFileName, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateHardLinkTransactedW
// ---------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateHardLinkTransactedW(IN LPCWSTR lpFileName, IN LPCWSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction)
{
    LPWSTR lpFilePathBuffer;
    LPWSTR lpExistingFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathW(lpFileName) || 
            IsNTFSi30PathW(lpExistingFileName))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpFilePathBuffer = GetFullObjectPathW(lpFileName);

        if (lpFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpExistingFilePathBuffer = GetFullObjectPathW(lpExistingFileName);

            if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pCreateHardLinkTransactedW(lpFilePathBuffer, lpExistingFilePathBuffer, lpSecurityAttributes, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpExistingFilePathBuffer);
            }
            else
            {
                bResult = g_pCreateHardLinkTransactedW(lpFileName, lpExistingFileName, lpSecurityAttributes, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpFilePathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pCreateHardLinkTransactedW(lpFileName, lpExistingFileName, lpSecurityAttributes, hTransaction);
    }
    else bResult = NTFSLPS_CreateHardLinkW(lpFileName, lpExistingFileName, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateHardLinkW
// -----------------------
// Implemented: 100%

NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateHardLinkW(IN LPCWSTR lpFileName, IN LPCWSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
    LPWSTR lpFilePathBuffer;
    LPWSTR lpExistingFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpFileName) || 
        IsNTFSi30PathW(lpExistingFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpFilePathBuffer = GetFullObjectPathW(lpFileName);

    if (lpFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpExistingFilePathBuffer = GetFullObjectPathW(lpExistingFileName);

        if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = CreateHardLinkW(lpFilePathBuffer, lpExistingFilePathBuffer, lpSecurityAttributes);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpExistingFilePathBuffer);
        }
        else
        {
            bResult = CreateHardLinkW(lpFileName, lpExistingFileName, lpSecurityAttributes);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = CreateHardLinkW(lpFileName, lpExistingFileName, lpSecurityAttributes);

    return bResult;
}

// NTFSLPS_CreateSymbolicLinkA
// ---------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateSymbolicLinkA(IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags)
{
    LPWSTR lpSymlinkFilePathBuffer;
    LPWSTR lpTargetFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpSymlinkFileName) || 
        IsNTFSi30PathA(lpTargetFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpSymlinkFilePathBuffer = GetFullObjectPathA(lpSymlinkFileName);

    if (lpSymlinkFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpTargetFilePathBuffer = GetFullObjectPathA(lpTargetFileName);

        if (lpTargetFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pCreateSymbolicLinkW(lpSymlinkFilePathBuffer, lpTargetFilePathBuffer, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpTargetFilePathBuffer);
        }
        else
        {
            bResult = g_pCreateSymbolicLinkA(lpSymlinkFileName, lpTargetFileName, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpSymlinkFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = g_pCreateSymbolicLinkA(lpSymlinkFileName, lpTargetFileName, dwFlags);

    return bResult;
}

// NTFSLPS_CreateSymbolicLinkTransactedA
// -------------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateSymbolicLinkTransactedA(IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    LPWSTR lpSymlinkFilePathBuffer;
    LPWSTR lpTargetFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathA(lpSymlinkFileName) || 
            IsNTFSi30PathA(lpTargetFileName))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpSymlinkFilePathBuffer = GetFullObjectPathA(lpSymlinkFileName);

        if (lpSymlinkFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpTargetFilePathBuffer = GetFullObjectPathA(lpTargetFileName);

            if (lpTargetFilePathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pCreateSymbolicLinkTransactedW(lpSymlinkFilePathBuffer, lpTargetFilePathBuffer, dwFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpTargetFilePathBuffer);
            }
            else
            {
                bResult = g_pCreateSymbolicLinkTransactedA(lpSymlinkFileName, lpTargetFileName, dwFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpSymlinkFilePathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pCreateSymbolicLinkTransactedA(lpSymlinkFileName, lpTargetFileName, dwFlags, hTransaction);
    }
    else bResult = NTFSLPS_CreateSymbolicLinkA(lpSymlinkFileName, lpTargetFileName, dwFlags);

    return bResult;
}

// NTFSLPS_CreateSymbolicLinkTransactedW
// -------------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateSymbolicLinkTransactedW(IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    LPWSTR lpSymlinkFilePathBuffer;
    LPWSTR lpTargetFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathW(lpSymlinkFileName) || 
            IsNTFSi30PathW(lpTargetFileName))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpSymlinkFilePathBuffer = GetFullObjectPathW(lpSymlinkFileName);

        if (lpSymlinkFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpTargetFilePathBuffer = GetFullObjectPathW(lpTargetFileName);

            if (lpTargetFilePathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pCreateSymbolicLinkTransactedW(lpSymlinkFilePathBuffer, lpTargetFilePathBuffer, dwFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpTargetFilePathBuffer);
            }
            else
            {
                bResult = g_pCreateSymbolicLinkTransactedW(lpSymlinkFileName, lpTargetFileName, dwFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpSymlinkFilePathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pCreateSymbolicLinkTransactedW(lpSymlinkFileName, lpTargetFileName, dwFlags, hTransaction);
    }
    else bResult = NTFSLPS_CreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags);

    return bResult;
}

// NTFSLPS_CreateSymbolicLinkW
// ---------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_CreateSymbolicLinkW(IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags)
{
    LPWSTR lpSymlinkFilePathBuffer;
    LPWSTR lpTargetFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpSymlinkFileName) || 
        IsNTFSi30PathW(lpTargetFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpSymlinkFilePathBuffer = GetFullObjectPathW(lpSymlinkFileName);

    if (lpSymlinkFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpTargetFilePathBuffer = GetFullObjectPathW(lpTargetFileName);

        if (lpTargetFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pCreateSymbolicLinkW(lpSymlinkFilePathBuffer, lpTargetFilePathBuffer, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpTargetFilePathBuffer);
        }
        else
        {
            bResult = g_pCreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpSymlinkFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = g_pCreateSymbolicLinkW(lpSymlinkFileName, lpTargetFileName, dwFlags);

    return bResult;
}

// NTFSLPS_DeleteFileA
// -------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_DeleteFileA(IN LPCSTR lpFileName)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = DeleteFileW(lpObjectPathBuffer);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = DeleteFileA(lpFileName);

    return bResult;
}

// NTFSLPS_DeleteFileTransactedA
// -----------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_DeleteFileTransactedA(IN LPCSTR lpFileName, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pDeleteFileTransactedW(lpObjectPathBuffer, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pDeleteFileTransactedA(lpFileName, hTransaction);
    }
    else bResult = NTFSLPS_DeleteFileA(lpFileName);

    return bResult;
}

// NTFSLPS_DeleteFileTransactedW
// -----------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_DeleteFileTransactedW(IN LPCWSTR lpFileName, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pDeleteFileTransactedW(lpObjectPathBuffer, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pDeleteFileTransactedW(lpFileName, hTransaction);
    }
    else bResult = NTFSLPS_DeleteFileW(lpFileName);

    return bResult;
}

// NTFSLPS_DeleteFileW
// -------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_DeleteFileW(IN LPCWSTR lpFileName)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = DeleteFileW(lpObjectPathBuffer);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = DeleteFileW(lpFileName);

    return bResult;
}

// DeleteSubFolderW
// ----------------
// Implemented: 0%

UINT WINAPI DeleteSubFolderW(IN LPCWSTR lpPathName)
{
    UINT uPathLength;
    LPWSTR lpTemporary;
    WIN32_FIND_DATAW FindFileData;
    HANDLE hFind;
    DWORD Win32ErrorCode;

    uPathLength = wcslen(lpPathName);

    lpTemporary = (LPWSTR) (lpPathName + uPathLength);

    if (uPathLength + 2 >= UNICODE_STRING_MAX_CHARS)
    {
        g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

        return DFS_ERROR;
    }

    *lpTemporary = (WCHAR) '\\';
    lpTemporary++;
    *lpTemporary = (WCHAR) '*';
    *(lpTemporary + 1) = (WCHAR) 0;

    hFind = FindFirstFileW(lpPathName, &FindFileData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (wcsncmp(FindFileData.cFileName, L".", 2) && wcsncmp(FindFileData.cFileName, L"..", 3))
            {
                if (FAILED(StringCchCopyW(lpTemporary, UNICODE_STRING_MAX_CHARS - (lpTemporary - lpPathName), FindFileData.cFileName)))
                {
                    g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

                    return DFS_ERROR;
                }

                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (DeleteSubFolderW(lpPathName))
                        return DFS_ERROR;
                }
                else DeleteFileW(lpPathName);
            }
        }
        while (FindNextFileW(hFind, &FindFileData));

        goto IsFindFileCriticalError;
    }
    else
    {
        IsFindFileCriticalError:

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        if (Win32ErrorCode != ERROR_NO_MORE_FILES)
        {
            g_pRtlSetLastWin32Error(Win32ErrorCode);

            return DFS_ERROR;
        }
    }

    FindClose(hFind);

    *(lpTemporary - 1) = (WCHAR) 0;

    if (!RemoveDirectoryW(lpPathName))
    {
        if (g_pRtlGetLastWin32Error() != ERROR_DIR_NOT_EMPTY)
            return DFS_ERROR;
    }

    return 0;
}

// NTFSLPS_DeleteFolderA
// ---------------------
// Implemented: 0%

UINT WINAPI NTFSLPS_DeleteFolderA(IN LPCSTR lpPathName)
{
    UINT uPathLength;
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpTemporary;
    WIN32_FIND_DATAW FindFileData;
    HANDLE hFind;
    DWORD Win32ErrorCode;
    UINT uStatus;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return DFS_ERROR; }
    #endif

    if (!lpPathName || (lpPathName[0] == (CHAR) 0))
    {
        g_pRtlSetLastWin32Error(ERROR_INVALID_NAME);

        return DFS_ERROR;
    }

    lpObjectPathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, UNICODE_STRING_MAX_CHARS * sizeof(WCHAR));

    if (!lpObjectPathBuffer)
        return DFS_ERROR;

    uPathLength = (UINT) GetFullPathExAW(lpPathName, lpObjectPathBuffer, UNICODE_STRING_MAX_CHARS, TRUE);

    if (!uPathLength)
    {
        Win32ErrorCode = g_pRtlGetLastWin32Error();

        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);

        return DFS_ERROR;
    }

    lpTemporary = lpObjectPathBuffer + uPathLength;

    if (*(lpTemporary - 1) == (WCHAR) '\\')
    {
        if (uPathLength + 1 == UNICODE_STRING_MAX_CHARS)
        {
            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

            return DFS_ERROR;
        }

        *lpTemporary = (WCHAR) '*';
        *(lpTemporary + 1) = (WCHAR) 0;
    }
    else
    {
        if (uPathLength + 2 >= UNICODE_STRING_MAX_CHARS)
        {
            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

            return DFS_ERROR;
        }

        *lpTemporary = (WCHAR) '\\';
        lpTemporary++;
        *lpTemporary = (WCHAR) '*';
        *(lpTemporary + 1) = (WCHAR) 0;
    }

    hFind = FindFirstFileW(lpObjectPathBuffer, &FindFileData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (wcsncmp(FindFileData.cFileName, L".", 2) && wcsncmp(FindFileData.cFileName, L"..", 3))
            {
                if (FAILED(StringCchCopyW(lpTemporary, UNICODE_STRING_MAX_CHARS - (lpTemporary - lpObjectPathBuffer), FindFileData.cFileName)))
                {
                    RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                    g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

                    return DFS_ERROR;
                }

                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    uStatus = DeleteSubFolderW(lpObjectPathBuffer);

                    if (uStatus)
                    {
                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                        g_pRtlSetLastWin32Error(Win32ErrorCode);

                        return DFS_ERROR;
                    }
                }
                else DeleteFileW(lpObjectPathBuffer);
            }
        }
        while (FindNextFileW(hFind, &FindFileData));

        goto IsFindFileCriticalError;
    }
    else
    {
        IsFindFileCriticalError:

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        if (Win32ErrorCode != ERROR_NO_MORE_FILES)
        {
            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);

            return DFS_ERROR;
        }
    }

    FindClose(hFind);

    *(lpTemporary - 1) = (WCHAR) 0;

    if (!RemoveDirectoryW(lpObjectPathBuffer))
    {
        if (g_pRtlGetLastWin32Error() == ERROR_DIR_NOT_EMPTY)
            uStatus |= DFS_PARTIALLY_MISSED;
        else uStatus = DFS_ERROR;
    }

    RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

    return uStatus;
}

// NTFSLPS_DeleteFolderW
// ---------------------
// Implemented: 100%

UINT WINAPI NTFSLPS_DeleteFolderW(IN LPCWSTR lpPathName)
{
    UINT uPathLength;
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpTemporary;
    WIN32_FIND_DATAW FindFileData;
    HANDLE hFind;
    DWORD Win32ErrorCode;
    UINT uStatus;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return DFS_ERROR; }
    #endif

    if (!lpPathName || (lpPathName[0] == 0))
    {
        g_pRtlSetLastWin32Error(ERROR_INVALID_NAME);

        return DFS_ERROR;
    }

    lpObjectPathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, UNICODE_STRING_MAX_CHARS * sizeof(WCHAR));

    if (!lpObjectPathBuffer)
        return DFS_ERROR;

    uPathLength = (UINT) GetFullPathExW(lpPathName, lpObjectPathBuffer, UNICODE_STRING_MAX_CHARS, TRUE);

    if (!uPathLength)
    {
        Win32ErrorCode = g_pRtlGetLastWin32Error();

        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);

        return DFS_ERROR;
    }

    lpTemporary = lpObjectPathBuffer + uPathLength;

    if (*(lpTemporary - 1) == (WCHAR) '\\')
    {
        if (uPathLength + 1 == UNICODE_STRING_MAX_CHARS)
        {
            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

            return DFS_ERROR;
        }

        *lpTemporary = (WCHAR) '*';
        *(lpTemporary + 1) = (WCHAR) 0;
    }
    else
    {
        if (uPathLength + 2 >= UNICODE_STRING_MAX_CHARS)
        {
            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

            return DFS_ERROR;
        }

        *lpTemporary = (WCHAR) '\\';
        lpTemporary++;
        *lpTemporary = (WCHAR) '*';
        *(lpTemporary + 1) = (WCHAR) 0;
    }

    hFind = FindFirstFileW(lpObjectPathBuffer, &FindFileData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (wcsncmp(FindFileData.cFileName, L".", 2) && wcsncmp(FindFileData.cFileName, L"..", 3))
            {
                if (FAILED(StringCchCopyW(lpTemporary, UNICODE_STRING_MAX_CHARS - (lpTemporary - lpObjectPathBuffer), FindFileData.cFileName)))
                {
                    RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                    g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

                    return DFS_ERROR;
                }

                if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    uStatus = DeleteSubFolderW(lpObjectPathBuffer);

                    if (uStatus)
                    {
                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                        g_pRtlSetLastWin32Error(Win32ErrorCode);

                        return DFS_ERROR;
                    }
                }
                else DeleteFileW(lpObjectPathBuffer);
            }
        }
        while (FindNextFileW(hFind, &FindFileData));

        goto IsFindFileCriticalError;
    }
    else
    {
        IsFindFileCriticalError:

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        if (Win32ErrorCode != ERROR_NO_MORE_FILES)
        {
            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);

            return DFS_ERROR;
        }
    }

    FindClose(hFind);

    *(lpTemporary - 1) = (WCHAR) 0;

    if (!RemoveDirectoryW(lpObjectPathBuffer))
    {
        if (g_pRtlGetLastWin32Error() == ERROR_DIR_NOT_EMPTY)
            uStatus |= DFS_PARTIALLY_MISSED;
        else uStatus = DFS_ERROR;
    }

    RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

    return uStatus;
}

// NTFSLPS_DumpToFileA
// -------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_DumpToFileA(IN LPCSTR lpFileName, IN LPVOID lpMemory, IN UINT nLength, OUT LPUINT lpNumberOfBytesProcessed)
{
    LPWSTR lpObjectPathBuffer;
    HANDLE hObject;
    DWORD Win32ErrorCode;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return ERROR_BAD_PATHNAME; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hObject = CreateFileW(lpObjectPathBuffer, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hObject != INVALID_HANDLE_VALUE)
        {
            Win32ErrorCode = NO_ERROR;

            if (nLength)
            {
                if (SetFilePointer(hObject, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                if (!Win32ErrorCode && !WriteFile(hObject, lpMemory, nLength, lpNumberOfBytesProcessed, NULL))
                    Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            CloseHandle(hObject);
        }
        else Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);
    }
    else Win32ErrorCode = g_pRtlGetLastWin32Error();

    if (Win32ErrorCode && lpNumberOfBytesProcessed)
        *lpNumberOfBytesProcessed = 0;

    g_pRtlSetLastWin32Error(Win32ErrorCode);

    return Win32ErrorCode;
}

// NTFSLPS_DumpToFileW
// -------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_DumpToFileW(IN LPCWSTR lpFileName, IN LPVOID lpMemory, IN UINT nLength, OUT LPUINT lpNumberOfBytesProcessed)
{
    LPWSTR lpObjectPathBuffer;
    HANDLE hObject;
    DWORD Win32ErrorCode;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return ERROR_BAD_PATHNAME; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hObject = CreateFileW(lpObjectPathBuffer, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hObject != INVALID_HANDLE_VALUE)
        {
            Win32ErrorCode = NO_ERROR;

            if (nLength)
            {
                if (SetFilePointer(hObject, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER)
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                if (!Win32ErrorCode && !WriteFile(hObject, lpMemory, nLength, lpNumberOfBytesProcessed, NULL))
                    Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            CloseHandle(hObject);
        }
        else Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);
    }
    else Win32ErrorCode = g_pRtlGetLastWin32Error();

    if (Win32ErrorCode && lpNumberOfBytesProcessed)
        *lpNumberOfBytesProcessed = 0;

    g_pRtlSetLastWin32Error(Win32ErrorCode);

    return Win32ErrorCode;
}

// NTFSLPS_FindFirstFileA
// ----------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstFileA(IN LPCSTR lpFileName, OUT LPWIN32_FIND_DATAA lpFindFileData)
{
    LPWSTR lpObjectPathBuffer;
    WIN32_FIND_DATAW FindFileData;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hResult = FindFirstFileW(lpObjectPathBuffer, &FindFileData);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        if (hResult != INVALID_HANDLE_VALUE)
        {
            if (UnicodeStringToAnsiString((LPCWSTR) &FindFileData.cFileName, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName)) &&
                UnicodeStringToAnsiString((LPCWSTR) &FindFileData.cAlternateFileName, lpFindFileData->cAlternateFileName, sizeof(lpFindFileData->cAlternateFileName)))
                memcpy(lpFindFileData, &FindFileData, offsetof(WIN32_FIND_DATAW, cFileName));
            else
            {
                Win32ErrorCode = g_pRtlGetLastWin32Error();

                RtlZeroMemory(lpFindFileData, sizeof(WIN32_FIND_DATAA));

                hResult = INVALID_HANDLE_VALUE;
            }
        }
        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hResult = FindFirstFileA(lpFileName, lpFindFileData);

    return hResult;
}

// NTFSLPS_FindFirstFileExA
// ------------------------
// Implemented: 50%

HANDLE WINAPI NTFSLPS_FindFirstFileExA(IN LPCSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAA lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags)
{
    LPWSTR lpObjectPathBuffer;
    WIN32_FIND_DATAW FindFileData;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hResult = FindFirstFileExW(lpObjectPathBuffer, fInfoLevelId, &FindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        if (hResult != INVALID_HANDLE_VALUE)
        {
            if (UnicodeStringToAnsiString((LPCWSTR) &FindFileData.cFileName, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName)) &&
                UnicodeStringToAnsiString((LPCWSTR) &FindFileData.cAlternateFileName, lpFindFileData->cAlternateFileName, sizeof(lpFindFileData->cAlternateFileName)))
                memcpy(lpFindFileData, &FindFileData, offsetof(WIN32_FIND_DATAW, cFileName));
            else
            {
                Win32ErrorCode = g_pRtlGetLastWin32Error();

                RtlZeroMemory(lpFindFileData, sizeof(WIN32_FIND_DATAA));

                hResult = INVALID_HANDLE_VALUE;
            }
        }
        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hResult = FindFirstFileExA(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);

    return hResult;
}

// NTFSLPS_FindFirstFileExW
// ------------------------
// Implemented: 50%

HANDLE WINAPI NTFSLPS_FindFirstFileExW(IN LPCWSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAW lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hResult = FindFirstFileExW(lpObjectPathBuffer, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hResult = FindFirstFileExW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);

    return hResult;
}

// NTFSLPS_FindFirstFileNameA
// --------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstFileNameA(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName)
{
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpLinkNameBuffer;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    if (StringLength)
    {
        lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpLinkNameBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, *StringLength * sizeof(WCHAR));

            if (lpLinkNameBuffer)
            {
                hResult = g_pFindFirstFileNameW(lpObjectPathBuffer, dwFlags, StringLength, lpLinkNameBuffer);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                *StringLength = UnicodeStringToAnsiString(lpLinkNameBuffer, LinkName, *StringLength);

                if (!*StringLength)
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    hResult = INVALID_HANDLE_VALUE;
                }
                RtlFreeHeap(g_hProcessHeap, 0, lpLinkNameBuffer);
            }
            else
            {
                hResult = FindFirstFileNameA(lpFileName, dwFlags, StringLength, LinkName);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else hResult = FindFirstFileNameA(lpFileName, dwFlags, StringLength, LinkName);
    }
    else hResult = FindFirstFileNameA(lpFileName, dwFlags, StringLength, LinkName);

    return hResult;
}

// NTFSLPS_FindFirstFileNameTransactedA
// ------------------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstFileNameTransactedA(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpLinkNameBuffer;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return INVALID_HANDLE_VALUE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            if (StringLength && *StringLength && LinkName)
            {
                lpLinkNameBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, *StringLength * sizeof(WCHAR));

                if (lpLinkNameBuffer)
                {
                    hResult = g_pFindFirstFileNameTransactedW(lpObjectPathBuffer, dwFlags, StringLength, lpLinkNameBuffer, hTransaction);

                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    *StringLength = UnicodeStringToAnsiString(lpLinkNameBuffer, LinkName, *StringLength);

                    if (!*StringLength)
                    {
                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                        hResult = INVALID_HANDLE_VALUE;
                    }
                    RtlFreeHeap(g_hProcessHeap, 0, lpLinkNameBuffer);
                }
                else
                {
                    hResult = FindFirstFileNameTransactedA(lpFileName, dwFlags, StringLength, LinkName, hTransaction);

                    Win32ErrorCode = g_pRtlGetLastWin32Error();
                }
            }
            else
            {
                hResult = g_pFindFirstFileNameTransactedW(lpObjectPathBuffer, dwFlags, StringLength, (PWSTR) LinkName, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else hResult = FindFirstFileNameTransactedA(lpFileName, dwFlags, StringLength, LinkName, hTransaction);
    }
    else hResult = NTFSLPS_FindFirstFileNameA(lpFileName, dwFlags, StringLength, LinkName);

    return hResult;
}

// NTFSLPS_FindFirstFileNameTransactedW
// ------------------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstFileNameTransactedW(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return INVALID_HANDLE_VALUE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            hResult = g_pFindFirstFileNameTransactedW(lpObjectPathBuffer, dwFlags, StringLength, LinkName, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else hResult = g_pFindFirstFileNameTransactedW(lpFileName, dwFlags, StringLength, LinkName, hTransaction);
    }
    else hResult = NTFSLPS_FindFirstFileNameW(lpFileName, dwFlags, StringLength, LinkName);

    return hResult;
}

// NTFSLPS_FindFirstFileNameW
// --------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstFileNameW(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hResult = g_pFindFirstFileNameW(lpObjectPathBuffer, dwFlags, StringLength, LinkName);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hResult = g_pFindFirstFileNameW(lpFileName, dwFlags, StringLength, LinkName);

    return hResult;
}

// NTFSLPS_FindFirstFileTransactedA
// --------------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstFileTransactedA(IN LPCSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAA lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    WIN32_FIND_DATAW FindFileData;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return INVALID_HANDLE_VALUE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            hResult = g_pFindFirstFileTransactedW(lpObjectPathBuffer, fInfoLevelId, &FindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (hResult != INVALID_HANDLE_VALUE)
            {
                if (UnicodeStringToAnsiString((LPCWSTR) &FindFileData.cFileName, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName)) &&
                    UnicodeStringToAnsiString((LPCWSTR) &FindFileData.cAlternateFileName, lpFindFileData->cAlternateFileName, sizeof(lpFindFileData->cAlternateFileName)))
                    memcpy(lpFindFileData, &FindFileData, offsetof(WIN32_FIND_DATAW, cFileName));
                else
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    RtlZeroMemory(lpFindFileData, sizeof(WIN32_FIND_DATAW));

                    hResult = INVALID_HANDLE_VALUE;
                }
            }
            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else hResult = g_pFindFirstFileTransactedA(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags, hTransaction);
    }
    else hResult = NTFSLPS_FindFirstFileExA(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);

    return hResult;
}

// NTFSLPS_FindFirstFileTransactedW
// --------------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstFileTransactedW(IN LPCWSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAW lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return INVALID_HANDLE_VALUE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            hResult = g_pFindFirstFileTransactedW(lpObjectPathBuffer, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else hResult = g_pFindFirstFileTransactedW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags, hTransaction);
    }
    else hResult = NTFSLPS_FindFirstFileExW(lpFileName, fInfoLevelId, lpFindFileData, fSearchOp, lpSearchFilter, dwAdditionalFlags);

    return hResult;
}

// NTFSLPS_FindFirstFileW
// ----------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstFileW(IN LPCWSTR lpFileName, OUT LPWIN32_FIND_DATAW lpFindFileData)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hResult = FindFirstFileW(lpObjectPathBuffer, lpFindFileData);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hResult = FindFirstFileW(lpFileName, lpFindFileData);

    return hResult;
}

// NTFSLPS_FindFirstStreamA
// ------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstStreamA(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData, DWORD dwFlags)
{
    LPWSTR lpObjectPathBuffer;
    WIN32_FIND_STREAM_DATAW FindStreamData;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hResult = g_pFindFirstStreamW(lpObjectPathBuffer, InfoLevel, &FindStreamData, dwFlags);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        if (hResult != INVALID_HANDLE_VALUE)
        {
            if (!UnicodeStringToAnsiString((LPCWSTR) &FindStreamData.cStreamName, lpFindStreamData->cStreamName, sizeof(lpFindStreamData->cStreamName)))
            {
                Win32ErrorCode = g_pRtlGetLastWin32Error();

                RtlZeroMemory(lpFindStreamData, sizeof(WIN32_FIND_STREAM_DATAA));

                hResult = INVALID_HANDLE_VALUE;
            }
            else memcpy(lpFindStreamData, &FindStreamData, offsetof(WIN32_FIND_STREAM_DATAW, cStreamName));
        }
        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hResult = FindFirstStreamA(lpFileName, InfoLevel, lpFindStreamData, dwFlags);

    return hResult;
}

// NTFSLPS_FindFirstStreamTransactedA
// ----------------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstStreamTransactedA(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    WIN32_FIND_STREAM_DATAW FindStreamData;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return INVALID_HANDLE_VALUE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            hResult = g_pFindFirstStreamTransactedW(lpObjectPathBuffer, InfoLevel, &FindStreamData, dwFlags, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (hResult != INVALID_HANDLE_VALUE)
            {
                if (!UnicodeStringToAnsiString((LPCWSTR) &FindStreamData.cStreamName, lpFindStreamData->cStreamName, sizeof(lpFindStreamData->cStreamName)))
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    RtlZeroMemory(lpFindStreamData, sizeof(WIN32_FIND_STREAM_DATAA));

                    hResult = INVALID_HANDLE_VALUE;
                }
                else memcpy(lpFindStreamData, &FindStreamData, offsetof(WIN32_FIND_STREAM_DATAW, cStreamName));
            }
            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else hResult = FindFirstStreamTransactedA(lpFileName, InfoLevel, lpFindStreamData, dwFlags, hTransaction);
    }
    else hResult = NTFSLPS_FindFirstStreamA(lpFileName, InfoLevel, lpFindStreamData, dwFlags);

    return hResult;
}

// NTFSLPS_FindFirstStreamTransactedW
// ----------------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstStreamTransactedW(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return INVALID_HANDLE_VALUE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            hResult = g_pFindFirstStreamTransactedW(lpObjectPathBuffer, InfoLevel, lpFindStreamData, dwFlags, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else hResult = g_pFindFirstStreamTransactedW(lpFileName, InfoLevel, lpFindStreamData, dwFlags, hTransaction);
    }
    else hResult = NTFSLPS_FindFirstStreamW(lpFileName, InfoLevel, lpFindStreamData, dwFlags);

    return hResult;
}

// NTFSLPS_FindFirstStreamW
// ------------------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_FindFirstStreamW(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData, DWORD dwFlags)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    HANDLE hResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        hResult = g_pFindFirstStreamW(lpObjectPathBuffer, InfoLevel, lpFindStreamData, dwFlags);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else hResult = g_pFindFirstStreamW(lpFileName, InfoLevel, lpFindStreamData, dwFlags);

    return hResult;
}

// NTFSLPS_FindNextFileA
// ---------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_FindNextFileA(IN HANDLE hFindFile, OUT LPWIN32_FIND_DATAA lpFindFileData)
{
    WIN32_FIND_DATAW FindFileData;
    BOOL bResult;

    bResult = FindNextFileW(hFindFile, &FindFileData);

    if (bResult)
    {
        if (UnicodeStringToAnsiString((LPCWSTR) &FindFileData.cFileName, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName)) &&
            UnicodeStringToAnsiString((LPCWSTR) &FindFileData.cAlternateFileName, lpFindFileData->cAlternateFileName, sizeof(lpFindFileData->cAlternateFileName)))
            memcpy(lpFindFileData, &FindFileData, offsetof(WIN32_FIND_DATAW, cFileName));
        else
        {
            RtlZeroMemory(lpFindFileData, sizeof(WIN32_FIND_DATAA));

            return FALSE;
        }
    }
    else bResult = FindNextFileA(hFindFile, lpFindFileData);

    return bResult;
}

// NTFSLPS_FindNextFileNameA
// -------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_FindNextFileNameA(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PSTR LinkName)
{
    LPWSTR lpLinkNameBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (StringLength)
    {
        lpLinkNameBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, *StringLength * sizeof(WCHAR));

        if (lpLinkNameBuffer)
        {
            bResult = g_pFindNextFileNameW(hFindStream, StringLength, lpLinkNameBuffer);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (bResult)
            {
                *StringLength = UnicodeStringToAnsiString(lpLinkNameBuffer, LinkName, *StringLength);

                if (!*StringLength)
                {
                    DWORD Win32ErrorCode = g_pRtlGetLastWin32Error();

                    bResult = FALSE;
                }
            }
            RtlFreeHeap(g_hProcessHeap, 0, lpLinkNameBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = FindNextFileNameA(hFindStream, StringLength, LinkName);
    }
    else bResult = FindNextFileNameA(hFindStream, StringLength, LinkName);

    return bResult;
}

// NTFSLPS_FindNextFileNameW
// -------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_FindNextFileNameW(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PWSTR LinkName)
{
    return g_pFindNextFileNameW(hFindStream, StringLength, LinkName);
}

// NTFSLPS_FindNextFileW
// ---------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_FindNextFileW(IN HANDLE hFindFile, OUT LPWIN32_FIND_DATAW lpFindFileData)
{
    return FindNextFileW(hFindFile, lpFindFileData);
}

// NTFSLPS_FindNextStreamA
// -----------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_FindNextStreamA(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData)
{
    WIN32_FIND_STREAM_DATAW FindStreamData;
    BOOL bResult;

    bResult = g_pFindNextStreamW(hFindStream, &FindStreamData);

    if (bResult)
    {
        if (!UnicodeStringToAnsiString((LPCWSTR) &FindStreamData.cStreamName, lpFindStreamData->cStreamName, sizeof(lpFindStreamData->cStreamName)))
        {
            RtlZeroMemory(lpFindStreamData, sizeof(WIN32_FIND_STREAM_DATAA));

            bResult = FALSE;
        }
        else memcpy(lpFindStreamData, &FindStreamData, offsetof(WIN32_FIND_STREAM_DATAW, cStreamName));
    }
    else bResult = FindNextStreamA(hFindStream, lpFindStreamData);

    return bResult;
}

// NTFSLPS_FindNextStreamW
// -----------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_FindNextStreamW(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData)
{
    return FindNextStreamW(hFindStream, lpFindStreamData);
}

// NTFSLPS_GetCompressedFileSizeA
// ------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetCompressedFileSizeA(IN LPCSTR lpFileName, OUT LPDWORD lpFileSizeHigh)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_FILE_SIZE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        dwResult = GetCompressedFileSizeW(lpObjectPathBuffer, lpFileSizeHigh);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = GetCompressedFileSizeA(lpFileName, lpFileSizeHigh);

    return dwResult;
}

// NTFSLPS_GetCompressedFileSizeTransactedA
// ----------------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetCompressedFileSizeTransactedA(IN LPCSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return INVALID_FILE_SIZE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            dwResult = g_pGetCompressedFileSizeTransactedW(lpObjectPathBuffer, lpFileSizeHigh, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else dwResult = g_pGetCompressedFileSizeTransactedA(lpFileName, lpFileSizeHigh, hTransaction);
    }
    else dwResult = NTFSLPS_GetCompressedFileSizeA(lpFileName, lpFileSizeHigh);

    return dwResult;
}

// NTFSLPS_GetCompressedFileSizeTransactedW
// ----------------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetCompressedFileSizeTransactedW(IN LPCWSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return INVALID_FILE_SIZE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            dwResult = g_pGetCompressedFileSizeTransactedW(lpObjectPathBuffer, lpFileSizeHigh, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else dwResult = g_pGetCompressedFileSizeTransactedW(lpFileName, lpFileSizeHigh, hTransaction);
    }
    else dwResult = NTFSLPS_GetCompressedFileSizeW(lpFileName, lpFileSizeHigh);

    return dwResult;
}

// NTFSLPS_GetCompressedFileSizeW
// ------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetCompressedFileSizeW(IN LPCWSTR lpFileName, OUT LPDWORD lpFileSizeHigh)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_FILE_SIZE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        dwResult = GetCompressedFileSizeW(lpObjectPathBuffer, lpFileSizeHigh);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = GetCompressedFileSizeW(lpFileName, lpFileSizeHigh);

    return dwResult;
}

// NTFSLPS_GetFileAttributesA
// --------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFileAttributesA(IN LPCSTR lpFileName)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_FILE_ATTRIBUTES; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        dwResult = GetFileAttributesW(lpObjectPathBuffer);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = GetFileAttributesA(lpFileName);

    return dwResult;
}

// NTFSLPS_GetFileAttributesExA
// ----------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_GetFileAttributesExA(IN LPCSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = GetFileAttributesExW(lpObjectPathBuffer, fInfoLevelId, lpFileInformation);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = GetFileAttributesExA(lpFileName, fInfoLevelId, lpFileInformation);

    return bResult;
}

// NTFSLPS_GetFileAttributesExW
// ----------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_GetFileAttributesExW(IN LPCWSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = GetFileAttributesExW(lpObjectPathBuffer, fInfoLevelId, lpFileInformation);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = GetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);

    return bResult;
}

// NTFSLPS_GetFileAttributesTransactedA
// ------------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_GetFileAttributesTransactedA(IN LPCSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pGetFileAttributesTransactedW(lpObjectPathBuffer, fInfoLevelId, lpFileInformation, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pGetFileAttributesTransactedA(lpFileName, fInfoLevelId, lpFileInformation, hTransaction);
    }
    else bResult = NTFSLPS_GetFileAttributesExA(lpFileName, fInfoLevelId, lpFileInformation);

    return bResult;
}

// NTFSLPS_GetFileAttributesTransactedW
// ------------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_GetFileAttributesTransactedW(IN LPCWSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pGetFileAttributesTransactedW(lpObjectPathBuffer, fInfoLevelId, lpFileInformation, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pGetFileAttributesTransactedW(lpFileName, fInfoLevelId, lpFileInformation, hTransaction);
    }
    else bResult = NTFSLPS_GetFileAttributesExW(lpFileName, fInfoLevelId, lpFileInformation);

    return bResult;
}

// NTFSLPS_GetFileAttributesW
// --------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFileAttributesW(IN LPCWSTR lpFileName)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_FILE_ATTRIBUTES; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        dwResult = GetFileAttributesW(lpObjectPathBuffer);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = GetFileAttributesW(lpFileName);

    return dwResult;
}

// NTFSLPS_GetFileSizeEx
// ---------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_GetFileSizeEx(IN HANDLE hFile, OUT PLARGE_INTEGER lpFileSize)
{
    return g_pGetFileSizeEx(hFile, lpFileSize);
}

// NTFSLPS_GetFinalPathNameByHandleA
// ---------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFinalPathNameByHandleA(IN HANDLE hFile, OUT LPSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags)
{
    LPWSTR lpFilePathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    lpFilePathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, cchFilePath * sizeof(WCHAR));

    if (lpFilePathBuffer)
    {
        dwResult = g_pGetFinalPathNameByHandleW(hFile, lpFilePathBuffer, cchFilePath, dwFlags);

        if (dwResult)
        {
            if (dwResult < cchFilePath)
            {
                if (!UnicodeStringToAnsiString((LPCWSTR) lpFilePathBuffer, lpszFilePath, cchFilePath))
                    dwResult = 0;
            }
        }
        else dwResult = g_pGetFinalPathNameByHandleA(hFile, lpszFilePath, cchFilePath, dwFlags);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        RtlFreeHeap(g_hProcessHeap, 0, lpFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = g_pGetFinalPathNameByHandleA(hFile, lpszFilePath, cchFilePath, dwFlags);

    return dwResult;
}

// NTFSLPS_GetFinalPathNameByHandleW
// ---------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFinalPathNameByHandleW(IN HANDLE hFile, OUT LPWSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags)
{
    return g_pGetFinalPathNameByHandleW(hFile, lpszFilePath, cchFilePath, dwFlags);
}

// NTFSLPS_GetFullPathA
// --------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFullPathA(IN LPCSTR lpObjectPath, OUT LPSTR lpBuffer, IN DWORD nBufferLength)
{
    SIZE_T ObjectPathLength;
    ANSI_STRING AnsiString;
    UNICODE_STRING UnicodeString;
    DWORD dwFilePathLength;
    LPWSTR lpTemporaryBuffer;
    LPWSTR lpFullPathTemporaryBuffer;
    NTSTATUS Status;
    DWORD Win32ErrorCode;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpObjectPath)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return 0; }
    #endif

    if (!lpObjectPath || !lpObjectPath[0])
    {
        dwFilePathLength = GetCurrentDirectoryA(nBufferLength, lpBuffer);

        if (dwFilePathLength)
        {
            if (dwFilePathLength < nBufferLength)
                g_pRtlSetLastWin32Error(NO_ERROR);
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        return dwFilePathLength;
    }

    ObjectPathLength = strlen(lpObjectPath);

    if (ObjectPathLength >= UNICODE_STRING_MAX_CHARS)
    {
        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);

        return 0;
    }

    if (lpObjectPath[0] == '\\' && lpObjectPath[1] == '\\')
    {
        if (lpObjectPath[2] == '?' && lpObjectPath[3] == '\\')
        {
            if ((lpObjectPath[4] == 'U' || lpObjectPath[4] == 'u') &&
                (lpObjectPath[5] == 'N' || lpObjectPath[5] == 'n') &&
                (lpObjectPath[6] == 'C' || lpObjectPath[6] == 'c') &&
                 lpObjectPath[7] == '\\')
            {
                lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 6 + 1) * sizeof(WCHAR));

                if (lpTemporaryBuffer)
                {
                    AnsiString.Length = (USHORT) ObjectPathLength - 7;
                    AnsiString.MaximumLength = (USHORT) ObjectPathLength - 7 + 1;
                    AnsiString.Buffer = (LPSTR) lpObjectPath + 7;

                    UnicodeString.Length = 0;
                    UnicodeString.MaximumLength = (USHORT) (ObjectPathLength - 6 + 1) * sizeof(WCHAR);
                    UnicodeString.Buffer = lpTemporaryBuffer + 1;

                    if (AreFileApisANSI())
                        Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                    else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                    if (!Status)
                    {
                        lpTemporaryBuffer[0] = '\\';

                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) lpBuffer, NULL);

                        if (dwFilePathLength)
                        {
                            if (dwFilePathLength > MAX_PATH16)
                            {
                                dwFilePathLength += 6;

                                if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                                {
                                    Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                    dwFilePathLength = 0;
                                }
                                else if (dwFilePathLength <= nBufferLength)
                                {
                                    if (lpBuffer)
                                    {
                                        lpFullPathTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (dwFilePathLength + 6) * sizeof(WCHAR));

                                        if (lpFullPathTemporaryBuffer)
                                        {
                                            dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 6, lpFullPathTemporaryBuffer + 6, NULL);

                                            if (dwFilePathLength)
                                            {
                                                memcpy(lpFullPathTemporaryBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                                dwFilePathLength += 6;

                                                AnsiString.Length = 0;
                                                if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                                                    AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                                                else AnsiString.MaximumLength = (USHORT) nBufferLength;
                                                AnsiString.Buffer = lpBuffer;

                                                UnicodeString.Length = (USHORT) dwFilePathLength * sizeof(WCHAR);
                                                UnicodeString.MaximumLength = (USHORT) (dwFilePathLength + 1) * sizeof(WCHAR);
                                                UnicodeString.Buffer = lpFullPathTemporaryBuffer;

                                                if (AreFileApisANSI())
                                                    Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
                                                else Status = g_pRtlUnicodeStringToOemString(&AnsiString, &UnicodeString, FALSE);

                                                if (Status)
                                                {
                                                    Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                                    dwFilePathLength = 0;
                                                }
                                                else Win32ErrorCode = NO_ERROR;
                                            }
                                            else Win32ErrorCode = g_pRtlGetLastWin32Error();

                                            RtlFreeHeap(g_hProcessHeap, 0, lpFullPathTemporaryBuffer);
                                        }
                                        else
                                        {
                                            Win32ErrorCode = g_pRtlGetLastWin32Error();

                                            dwFilePathLength = 0;
                                        }
                                    }
                                    else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                                }
                                else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                            }
                            else
                            {
                                if (dwFilePathLength <= nBufferLength)
                                {
                                    if (lpBuffer)
                                    {
                                        lpFullPathTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                        if (lpFullPathTemporaryBuffer)
                                        {
                                            dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpFullPathTemporaryBuffer, NULL);

                                            if (dwFilePathLength)
                                            {
                                                AnsiString.Length = 0;
                                                if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                                                    AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                                                else AnsiString.MaximumLength = (USHORT) nBufferLength;
                                                AnsiString.Buffer = lpBuffer;

                                                UnicodeString.Length = (USHORT) dwFilePathLength * sizeof(WCHAR);
                                                UnicodeString.MaximumLength = (USHORT) (dwFilePathLength + 1) * sizeof(WCHAR);
                                                UnicodeString.Buffer = lpFullPathTemporaryBuffer;

                                                if (AreFileApisANSI())
                                                    Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
                                                else Status = g_pRtlUnicodeStringToOemString(&AnsiString, &UnicodeString, FALSE);

                                                if (Status)
                                                {
                                                    Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                                    dwFilePathLength = 0;
                                                }
                                                else Win32ErrorCode = NO_ERROR;
                                            }
                                            else Win32ErrorCode = g_pRtlGetLastWin32Error();

                                            RtlFreeHeap(g_hProcessHeap, 0, lpFullPathTemporaryBuffer);
                                        }
                                        else
                                        {
                                            Win32ErrorCode = g_pRtlGetLastWin32Error();

                                            dwFilePathLength = 0;
                                        }
                                    }
                                    else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                                }
                                else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                            }
                        }
                        else Win32ErrorCode = g_pRtlGetLastWin32Error();
                    }
                    else
                    {
                        Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                        dwFilePathLength = 0;
                    }
                    RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                    g_pRtlSetLastWin32Error(Win32ErrorCode);
                }
                else dwFilePathLength = 0;
            }
            else
            {
                if ((ObjectPathLength == 6) &&
                   ((lpObjectPath[4] >= 'A' && lpObjectPath[4] <= 'Z') ||
                    (lpObjectPath[4] >= 'a' && lpObjectPath[4] <= 'z')) &&
                     lpObjectPath[5] == ':' && !lpObjectPath[6])
                {
                    dwFilePathLength = 3;

                    if (dwFilePathLength <= nBufferLength)
                    {
                        if (lpBuffer)
                        {
                            lpBuffer[0] = lpObjectPath[4] & (0xFF ^ ' ');
                            lpBuffer[1] = ':';
                            lpBuffer[2] = 0;

                            dwFilePathLength--;

                            g_pRtlSetLastWin32Error(NO_ERROR);
                        }
                        else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                    }
                    else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                }
                else
                {
                    lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 2 + 1) * sizeof(WCHAR));

                    if (lpTemporaryBuffer)
                    {
                        AnsiString.Length = (USHORT) ObjectPathLength - 2;
                        AnsiString.MaximumLength = (USHORT) ObjectPathLength - 2 + 1;
                        AnsiString.Buffer = (LPSTR) lpObjectPath;

                        UnicodeString.Length = 0;
                        UnicodeString.MaximumLength = (USHORT) (ObjectPathLength - 2) * sizeof(WCHAR);
                        UnicodeString.Buffer = lpTemporaryBuffer + 1;

                        if (AreFileApisANSI())
                            Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                        else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                        if (!Status)
                        {
                            lpTemporaryBuffer[0] = '\\';

                            dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) lpBuffer, NULL);

                            if (dwFilePathLength)
                            {
                                if (dwFilePathLength > MAX_PATH16)
                                {
                                    dwFilePathLength += 2;

                                    if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                                    {
                                        Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                        dwFilePathLength = 0;
                                    }
                                    else if (dwFilePathLength <= nBufferLength)
                                    {
                                        if (lpBuffer)
                                        {
                                            lpFullPathTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (dwFilePathLength + 2) * sizeof(WCHAR));

                                            if (lpFullPathTemporaryBuffer)
                                            {
                                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpFullPathTemporaryBuffer + 2, NULL);

                                                if (dwFilePathLength)
                                                {
                                                    memcpy(lpFullPathTemporaryBuffer, L"\\\\?", 3 * sizeof(WCHAR));

                                                    dwFilePathLength += 2;

                                                    AnsiString.Length = 0;
                                                    if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                                                        AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                                                    else AnsiString.MaximumLength = (USHORT) nBufferLength;
                                                    AnsiString.Buffer = lpBuffer;

                                                    UnicodeString.Length = (USHORT) dwFilePathLength * sizeof(WCHAR);
                                                    UnicodeString.MaximumLength = (USHORT) (dwFilePathLength + 1) * sizeof(WCHAR);
                                                    UnicodeString.Buffer = lpFullPathTemporaryBuffer;

                                                    if (AreFileApisANSI())
                                                        Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
                                                    else Status = g_pRtlUnicodeStringToOemString(&AnsiString, &UnicodeString, FALSE);

                                                    if (Status)
                                                    {
                                                        Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                                        dwFilePathLength = 0;
                                                    }
                                                    else Win32ErrorCode = NO_ERROR;
                                                }
                                                else Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                RtlFreeHeap(g_hProcessHeap, 0, lpFullPathTemporaryBuffer);
                                            }
                                            else
                                            {
                                                Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                dwFilePathLength = 0;
                                            }
                                        }
                                        else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                                    }
                                    else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                }
                                else
                                {
                                    if (dwFilePathLength <= nBufferLength)
                                    {
                                        if (lpBuffer)
                                        {
                                            lpFullPathTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                            if (lpFullPathTemporaryBuffer)
                                            {
                                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpFullPathTemporaryBuffer, NULL);

                                                if (dwFilePathLength)
                                                {
                                                    AnsiString.Length = 0;
                                                    if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                                                        AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                                                    else AnsiString.MaximumLength = (USHORT) nBufferLength;
                                                    AnsiString.Buffer = lpBuffer;

                                                    UnicodeString.Length = (USHORT) dwFilePathLength * sizeof(WCHAR);
                                                    UnicodeString.MaximumLength = (USHORT) (dwFilePathLength + 1) * sizeof(WCHAR);
                                                    UnicodeString.Buffer = lpFullPathTemporaryBuffer;

                                                    if (AreFileApisANSI())
                                                        Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
                                                    else Status = g_pRtlUnicodeStringToOemString(&AnsiString, &UnicodeString, FALSE);

                                                    if (Status)
                                                    {
                                                        Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                                        dwFilePathLength = 0;
                                                    }
                                                    else Win32ErrorCode = NO_ERROR;
                                                }
                                                else Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                RtlFreeHeap(g_hProcessHeap, 0, lpFullPathTemporaryBuffer);
                                            }
                                            else
                                            {
                                                Win32ErrorCode = g_pRtlGetLastWin32Error();

                                                dwFilePathLength = 0;
                                            }
                                        }
                                        else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                                    }
                                    else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                }
                            }
                            else Win32ErrorCode = g_pRtlGetLastWin32Error();
                        }
                        else
                        {
                            Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                            dwFilePathLength = 0;
                        }
                        RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                        g_pRtlSetLastWin32Error(Win32ErrorCode);
                    }
                    else dwFilePathLength = 0;
                }
            }
        }
        else if (lpObjectPath[2] == '.' && lpObjectPath[3] == '\\')
        {
            dwFilePathLength = ObjectPathLength + 1;

            if (dwFilePathLength <= nBufferLength)
            {
                if (lpBuffer)
                {
                    memcpy(lpBuffer, lpObjectPath, dwFilePathLength);

                    dwFilePathLength--;

                    g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
            }
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        else
        {
            lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength + 1) * sizeof(WCHAR));

            if (lpTemporaryBuffer)
            {
                AnsiString.Length = (USHORT) ObjectPathLength;
                AnsiString.MaximumLength = (USHORT) ObjectPathLength + 1;
                AnsiString.Buffer = (LPSTR) lpObjectPath;

                UnicodeString.Length = 0;
                UnicodeString.MaximumLength = (USHORT) (ObjectPathLength + 1) * sizeof(WCHAR);
                UnicodeString.Buffer = lpTemporaryBuffer;

                if (AreFileApisANSI())
                    Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                if (!Status)
                {
                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) lpBuffer, NULL);

                    if (dwFilePathLength)
                    {
                        if (dwFilePathLength > MAX_PATH16)
                        {
                            dwFilePathLength += 6;

                            if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                            {
                                Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                dwFilePathLength = 0;
                            }
                            else if (dwFilePathLength <= nBufferLength)
                            {
                                if (lpBuffer)
                                {
                                    lpFullPathTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (dwFilePathLength + 6) * sizeof(WCHAR));

                                    if (lpFullPathTemporaryBuffer)
                                    {
                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpFullPathTemporaryBuffer + 6, NULL);

                                        if (dwFilePathLength)
                                        {
                                            memcpy(lpFullPathTemporaryBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                            dwFilePathLength += 6;

                                            AnsiString.Length = 0;
                                            if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                                                AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                                            else AnsiString.MaximumLength = (USHORT) nBufferLength;
                                            AnsiString.Buffer = lpBuffer;

                                            UnicodeString.Length = (USHORT) dwFilePathLength * sizeof(WCHAR);
                                            UnicodeString.MaximumLength = (USHORT) (dwFilePathLength + 1) * sizeof(WCHAR);
                                            UnicodeString.Buffer = lpFullPathTemporaryBuffer;

                                            if (AreFileApisANSI())
                                                Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
                                            else Status = g_pRtlUnicodeStringToOemString(&AnsiString, &UnicodeString, FALSE);

                                            if (Status)
                                            {
                                                Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                                dwFilePathLength = 0;
                                            }
                                            else Win32ErrorCode = NO_ERROR;
                                        }
                                        else Win32ErrorCode = g_pRtlGetLastWin32Error();

                                        RtlFreeHeap(g_hProcessHeap, 0, lpFullPathTemporaryBuffer);
                                    }
                                    else
                                    {
                                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                                        dwFilePathLength = 0;
                                    }
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                        }
                        else
                        {
                            if (dwFilePathLength <= nBufferLength)
                            {
                                if (lpBuffer)
                                {
                                    lpFullPathTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                    if (lpFullPathTemporaryBuffer)
                                    {
                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpFullPathTemporaryBuffer, NULL);

                                        if (dwFilePathLength)
                                        {
                                            AnsiString.Length = 0;
                                            if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                                                AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                                            else AnsiString.MaximumLength = (USHORT) nBufferLength;
                                            AnsiString.Buffer = lpBuffer;

                                            UnicodeString.Length = (USHORT) dwFilePathLength * sizeof(WCHAR);
                                            UnicodeString.MaximumLength = (USHORT) (dwFilePathLength + 1) * sizeof(WCHAR);
                                            UnicodeString.Buffer = lpFullPathTemporaryBuffer;

                                            if (AreFileApisANSI())
                                                Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
                                            else Status = g_pRtlUnicodeStringToOemString(&AnsiString, &UnicodeString, FALSE);

                                            if (Status)
                                            {
                                                Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                                dwFilePathLength = 0;
                                            }
                                            else Win32ErrorCode = NO_ERROR;
                                        }
                                        else Win32ErrorCode = g_pRtlGetLastWin32Error();

                                        RtlFreeHeap(g_hProcessHeap, 0, lpFullPathTemporaryBuffer);
                                    }
                                    else
                                    {
                                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                                        dwFilePathLength = 0;
                                    }
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                        }
                    }
                    else Win32ErrorCode = g_pRtlGetLastWin32Error();
                }
                else
                {
                    Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                    dwFilePathLength = 0;
                }
                RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);
            }
            else dwFilePathLength = 0;
        }
    }
    else
    {
        if ((ObjectPathLength == 2) &&
           ((lpObjectPath[0] >= 'A' && lpObjectPath[0] <= 'Z') ||
            (lpObjectPath[0] >= 'a' && lpObjectPath[0] <= 'z')) &&
             lpObjectPath[1] == ':' && !lpObjectPath[2])
        {
            dwFilePathLength = 3;

            if (dwFilePathLength <= nBufferLength)
            {
                if (lpBuffer)
                {
                    lpBuffer[0] = lpObjectPath[0] & (0xFF ^ ' ');
                    lpBuffer[1] = ':';
                    lpBuffer[2] = 0;

                    dwFilePathLength--;

                    g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
            }
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        else
        {
            lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength + 1) * sizeof(WCHAR));

            if (lpTemporaryBuffer)
            {
                AnsiString.Length = (USHORT) ObjectPathLength;
                AnsiString.MaximumLength = (USHORT) ObjectPathLength + 1;
                AnsiString.Buffer = (LPSTR) lpObjectPath;

                UnicodeString.Length = 0;
                UnicodeString.MaximumLength = (USHORT) (ObjectPathLength + 1) * sizeof(WCHAR);
                UnicodeString.Buffer = lpTemporaryBuffer;

                if (AreFileApisANSI())
                    Status = g_pRtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);
                else Status = g_pRtlOemStringToUnicodeString(&UnicodeString, &AnsiString, FALSE);

                if (!Status)
                {
                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, (LPWSTR) lpBuffer, NULL);

                    if (dwFilePathLength)
                    {
                        if (dwFilePathLength > MAX_PATH16)
                        {
                            dwFilePathLength += 4;

                            if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                            {
                                Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                dwFilePathLength = 0;
                            }
                            else if (dwFilePathLength <= nBufferLength)
                            {
                                if (lpBuffer)
                                {
                                    lpFullPathTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (dwFilePathLength + 4) * sizeof(WCHAR));

                                    if (lpFullPathTemporaryBuffer)
                                    {
                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpFullPathTemporaryBuffer + 4, NULL);

                                        if (dwFilePathLength)
                                        {
                                            memcpy(lpFullPathTemporaryBuffer, L"\\\\?\\", 4 * sizeof(WCHAR));

                                            dwFilePathLength += 4;

                                            AnsiString.Length = 0;
                                            if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                                                AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                                            else AnsiString.MaximumLength = (USHORT) nBufferLength;
                                            AnsiString.Buffer = lpBuffer;

                                            UnicodeString.Length = (USHORT) dwFilePathLength * sizeof(WCHAR);
                                            UnicodeString.MaximumLength = (USHORT) (dwFilePathLength + 1) * sizeof(WCHAR);
                                            UnicodeString.Buffer = lpFullPathTemporaryBuffer;

                                            if (AreFileApisANSI())
                                                Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
                                            else Status = g_pRtlUnicodeStringToOemString(&AnsiString, &UnicodeString, FALSE);

                                            if (Status)
                                            {
                                                Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                                dwFilePathLength = 0;
                                            }
                                            else Win32ErrorCode = NO_ERROR;
                                        }
                                        else Win32ErrorCode = g_pRtlGetLastWin32Error();

                                        RtlFreeHeap(g_hProcessHeap, 0, lpFullPathTemporaryBuffer);
                                    }
                                    else
                                    {
                                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                                        dwFilePathLength = 0;
                                    }
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                        }
                        else
                        {
                            if (dwFilePathLength <= nBufferLength)
                            {
                                if (lpBuffer)
                                {
                                    lpFullPathTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, dwFilePathLength * sizeof(WCHAR));

                                    if (lpFullPathTemporaryBuffer)
                                    {
                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength, lpFullPathTemporaryBuffer, NULL);

                                        if (dwFilePathLength)
                                        {
                                            AnsiString.Length = 0;
                                            if (nBufferLength > UNICODE_STRING_MAX_CHARS)
                                                AnsiString.MaximumLength = UNICODE_STRING_MAX_CHARS;
                                            else AnsiString.MaximumLength = (USHORT) nBufferLength;
                                            AnsiString.Buffer = lpBuffer;

                                            UnicodeString.Length = (USHORT) dwFilePathLength * sizeof(WCHAR);
                                            UnicodeString.MaximumLength = (USHORT) (dwFilePathLength + 1) * sizeof(WCHAR);
                                            UnicodeString.Buffer = lpFullPathTemporaryBuffer;

                                            if (AreFileApisANSI())
                                                Status = g_pRtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
                                            else Status = g_pRtlUnicodeStringToOemString(&AnsiString, &UnicodeString, FALSE);

                                            if (Status)
                                            {
                                                Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                                                dwFilePathLength = 0;
                                            }
                                            else Win32ErrorCode = NO_ERROR;
                                        }
                                        else Win32ErrorCode = g_pRtlGetLastWin32Error();

                                        RtlFreeHeap(g_hProcessHeap, 0, lpFullPathTemporaryBuffer);
                                    }
                                    else
                                    {
                                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                                        dwFilePathLength = 0;
                                    }
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                        }
                    }
                    else Win32ErrorCode = g_pRtlGetLastWin32Error();
                }
                else
                {
                    Win32ErrorCode = g_pRtlNtStatusToDosError(Status);

                    dwFilePathLength = 0;
                }
                RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);
            }
            else dwFilePathLength = 0;
        }
    }
    return dwFilePathLength;
}

// NTFSLPS_GetFullPathNameA
// ------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFullPathNameA(IN LPCSTR lpFileName, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart)
{
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpTemporaryBuffer;
    LPWSTR lpTemporaryFilePart;
    DWORD Win32ErrorCode = NO_ERROR;
    DWORD dwResult = 0;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return 0; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, nBufferLength * sizeof(WCHAR));

        if (lpTemporaryBuffer)
        {
            dwResult = GetFullPathNameW(lpObjectPathBuffer, nBufferLength, lpTemporaryBuffer, &lpTemporaryFilePart);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (dwResult && dwResult < nBufferLength)
            {
                if (!UnicodePathStringToAnsiPathString((LPCWSTR) lpTemporaryBuffer, lpBuffer, nBufferLength))
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    dwResult = 0;
                }
                else if (lpFilePart) *lpFilePart = (lpTemporaryFilePart - lpTemporaryBuffer) / sizeof(WCHAR) + lpBuffer;
            }
            RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);
        }
        else
        {
            dwResult = GetFullPathNameA(lpFileName, nBufferLength, lpBuffer, lpFilePart);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = GetFullPathNameA(lpFileName, nBufferLength, lpBuffer, lpFilePart);

    return dwResult;
}

// NTFSLPS_GetFullPathNameTransactedA
// ----------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFullPathNameTransactedA(IN LPCSTR lpFileName, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpTemporaryBuffer;
    LPWSTR lpTemporaryFilePart;
    DWORD Win32ErrorCode = NO_ERROR;
    DWORD dwResult = 0;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return 0; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, nBufferLength * sizeof(WCHAR));

            if (lpTemporaryBuffer)
            {
                dwResult = g_pGetFullPathNameTransactedW(lpObjectPathBuffer, nBufferLength, lpTemporaryBuffer, &lpTemporaryFilePart, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                if (dwResult && dwResult < nBufferLength)
                {
                    if (!UnicodePathStringToAnsiPathString((LPCWSTR) lpTemporaryBuffer, lpBuffer, nBufferLength))
                    {
                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                        dwResult = 0;
                    }
                    else if (lpFilePart) *lpFilePart = (lpTemporaryFilePart - lpTemporaryBuffer) / sizeof(WCHAR) + lpBuffer;
                }
                RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);
            }
            else
            {
                dwResult = g_pGetFullPathNameTransactedA(lpFileName, nBufferLength, lpBuffer, lpFilePart, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else dwResult = g_pGetFullPathNameTransactedA(lpFileName, nBufferLength, lpBuffer, lpFilePart, hTransaction);
    }
    else dwResult = NTFSLPS_GetFullPathNameA(lpFileName, nBufferLength, lpBuffer, lpFilePart);

    return dwResult;
}

// NTFSLPS_GetFullPathNameTransactedW
// ----------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFullPathNameTransactedW(IN LPCWSTR lpFileName, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return 0; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            dwResult = g_pGetFullPathNameTransactedW(lpObjectPathBuffer, nBufferLength, lpBuffer, lpFilePart, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else dwResult = g_pGetFullPathNameTransactedW(lpFileName, nBufferLength, lpBuffer, lpFilePart, hTransaction);
    }
    else dwResult = NTFSLPS_GetFullPathNameW(lpFileName, nBufferLength, lpBuffer, lpFilePart);

    return dwResult;
}

// NTFSLPS_GetFullPathNameW
// ------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFullPathNameW(IN LPCWSTR lpFileName, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return 0; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        dwResult = GetFullPathNameW(lpObjectPathBuffer, nBufferLength, lpBuffer, lpFilePart);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = GetFullPathNameW(lpFileName, nBufferLength, lpBuffer, lpFilePart);

    return dwResult;
}

// NTFSLPS_GetFullPathW
// --------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetFullPathW(IN LPCWSTR lpObjectPath, OUT LPWSTR lpBuffer, IN DWORD nBufferLength)
{
    SIZE_T ObjectPathLength;
    LPWSTR lpTemporaryBuffer;
    DWORD Win32ErrorCode;
    DWORD dwFilePathLength;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpObjectPath)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return 0; }
    #endif

    if (!lpObjectPath || !lpObjectPath[0])
    {
        dwFilePathLength = GetCurrentDirectoryW(nBufferLength, lpBuffer);

        if (dwFilePathLength)
        {
            if (dwFilePathLength < nBufferLength)
                g_pRtlSetLastWin32Error(NO_ERROR);
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        return dwFilePathLength;
    }

    ObjectPathLength = wcslen(lpObjectPath);

    if (ObjectPathLength >= UNICODE_STRING_MAX_CHARS)
    {
        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);

        return 0;
    }

    if (lpObjectPath[0] == '\\' && lpObjectPath[1] == '\\')
    {
        if (lpObjectPath[2] == '?' && lpObjectPath[3] == '\\')
        {
            if ((lpObjectPath[4] == 'U' || lpObjectPath[4] == 'u') &&
                (lpObjectPath[5] == 'N' || lpObjectPath[5] == 'n') &&
                (lpObjectPath[6] == 'C' || lpObjectPath[6] == 'c') &&
                 lpObjectPath[7] == '\\')
            {
                lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 6 + 1) * sizeof(WCHAR));

                if (lpTemporaryBuffer)
                {
                    lpTemporaryBuffer[0] = '\\';

                    memcpy(lpTemporaryBuffer + sizeof(WCHAR), lpObjectPath + (7 * sizeof(WCHAR)), (ObjectPathLength - 7 + 1) * sizeof(WCHAR));

                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, lpBuffer, NULL);

                    if (dwFilePathLength)
                    {
                        if (dwFilePathLength > MAX_PATH16)
                        {
                            dwFilePathLength += 6;

                            if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                            {
                                Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                dwFilePathLength = 0;
                            }
                            else if (dwFilePathLength <= nBufferLength)
                            {
                                if (lpBuffer)
                                {
                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 6, lpBuffer + 6, NULL);

                                    if (dwFilePathLength)
                                    {
                                        dwFilePathLength += 6;

                                        if (dwFilePathLength < nBufferLength)
                                        {
                                            memcpy(lpBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                            Win32ErrorCode = NO_ERROR;
                                        }
                                        else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                    }
                                    else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                        }
                        else
                        {
                            if (lpBuffer)
                            {
                                dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, nBufferLength, lpBuffer, NULL);

                                Win32ErrorCode = g_pRtlGetLastWin32Error();
                            }
                            else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                        }
                    }
                    else Win32ErrorCode = g_pRtlGetLastWin32Error();

                    RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                    g_pRtlSetLastWin32Error(Win32ErrorCode);
                }
                else dwFilePathLength = 0;
            }
            else
            {
                if ((ObjectPathLength == 6) &&
                   ((lpObjectPath[4] >= 'A' && lpObjectPath[4] <= 'Z') ||
                    (lpObjectPath[4] >= 'a' && lpObjectPath[4] <= 'z')) &&
                     lpObjectPath[5] == ':' && !lpObjectPath[6])
                {
                    dwFilePathLength = 3;

                    if (dwFilePathLength <= nBufferLength)
                    {
                        if (lpBuffer)
                        {
                            lpBuffer[0] = lpObjectPath[4] & (0xFF ^ ' ');
                            lpBuffer[1] = ':';
                            lpBuffer[2] = 0;

                            dwFilePathLength--;

                            g_pRtlSetLastWin32Error(NO_ERROR);
                        }
                        else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                    }
                    else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                }
                else
                {
                    lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, (ObjectPathLength - 2 + 1) * sizeof(WCHAR));

                    if (lpTemporaryBuffer)
                    {
                        lpTemporaryBuffer[0] = '\\';

                        memcpy(lpTemporaryBuffer + sizeof(WCHAR), lpObjectPath + (3 * sizeof(WCHAR)), (ObjectPathLength - 3 + 1) * sizeof(WCHAR));

                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, 0, lpBuffer, NULL);

                        if (dwFilePathLength)
                        {
                            if (dwFilePathLength > MAX_PATH16)
                            {
                                dwFilePathLength += 2;

                                if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                                {
                                    Win32ErrorCode = ERROR_FILENAME_EXCED_RANGE;

                                    dwFilePathLength = 0;
                                }
                                else if (dwFilePathLength <= nBufferLength)
                                {
                                    if (lpBuffer)
                                    {
                                        dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, dwFilePathLength - 2, lpBuffer + 2, NULL);

                                        if (dwFilePathLength)
                                        {
                                            dwFilePathLength += 2;

                                            if (dwFilePathLength < nBufferLength)
                                            {
                                                memcpy(lpBuffer, L"\\\\?", 3 * sizeof(WCHAR));

                                                Win32ErrorCode = NO_ERROR;
                                            }
                                            else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                                        }
                                        else Win32ErrorCode = g_pRtlGetLastWin32Error();
                                    }
                                    else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                                }
                                else Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                            }
                            else
                            {
                                if (lpBuffer)
                                {
                                    dwFilePathLength = GetFullPathNameW(lpTemporaryBuffer, nBufferLength, lpBuffer, NULL);

                                    Win32ErrorCode = g_pRtlGetLastWin32Error();
                                }
                                else Win32ErrorCode = ERROR_INVALID_USER_BUFFER;
                            }
                        }
                        else Win32ErrorCode = g_pRtlGetLastWin32Error();

                        RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);

                        g_pRtlSetLastWin32Error(Win32ErrorCode);
                    }
                    else dwFilePathLength = 0;
                }
            }
        }
        else if (lpObjectPath[2] == '.' && lpObjectPath[3] == '\\')
        {
            dwFilePathLength = ObjectPathLength + 1;

            if (dwFilePathLength <= nBufferLength)
            {
                if (lpBuffer)
                {
                    memcpy(lpBuffer, lpObjectPath, dwFilePathLength * sizeof(WCHAR));

                    dwFilePathLength--;

                    g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
            }
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        else
        {
            dwFilePathLength = GetFullPathNameW(lpObjectPath, 0, lpBuffer, NULL);

            if (dwFilePathLength)
            {
                dwFilePathLength += 6;

                if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                {
                    g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);

                    dwFilePathLength = 0;
                }
                else if (dwFilePathLength <= nBufferLength)
                {
                    if (lpBuffer)
                    {
                        dwFilePathLength = GetFullPathNameW(lpObjectPath, dwFilePathLength - 6, lpBuffer + 6, NULL);

                        if (dwFilePathLength)
                        {
                            dwFilePathLength += 6;

                            if (dwFilePathLength < nBufferLength)
                            {
                                memcpy(lpBuffer, L"\\\\?\\UNC", 7 * sizeof(WCHAR));

                                g_pRtlSetLastWin32Error(NO_ERROR);
                            }
                            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                        }
                    }
                    else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                }
                else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
            }
        }
    }
    else
    {
        if ((ObjectPathLength == 2) &&
           ((lpObjectPath[0] >= 'A' && lpObjectPath[0] <= 'Z') ||
            (lpObjectPath[0] >= 'a' && lpObjectPath[0] <= 'z')) &&
             lpObjectPath[1] == ':' && !lpObjectPath[2])
        {
            dwFilePathLength = 3;

            if (dwFilePathLength <= nBufferLength)
            {
                if (lpBuffer)
                {
                    lpBuffer[0] = lpObjectPath[0] & (0xFF ^ ' ');
                    lpBuffer[1] = ':';
                    lpBuffer[2] = 0;

                    dwFilePathLength--;

                    g_pRtlSetLastWin32Error(NO_ERROR);
                }
                else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
            }
            else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
        }
        else
        {
            dwFilePathLength = GetFullPathNameW(lpObjectPath, 0, lpBuffer, NULL);

            if (dwFilePathLength)
            {
                if (dwFilePathLength > MAX_PATH16)
                {
                    dwFilePathLength += 4;

                    if (dwFilePathLength >= UNICODE_STRING_MAX_CHARS)
                    {
                        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);

                        dwFilePathLength = 0;
                    }
                    else if (dwFilePathLength <= nBufferLength)
                    {
                        if (lpBuffer)
                        {
                            dwFilePathLength = GetFullPathNameW(lpObjectPath, dwFilePathLength - 4, lpBuffer + 4, NULL);

                            if (dwFilePathLength)
                            {
                                dwFilePathLength += 4;

                                if (dwFilePathLength < nBufferLength)
                                {
                                    memcpy(lpBuffer, L"\\\\?\\", 4 * sizeof(WCHAR));

                                    g_pRtlSetLastWin32Error(NO_ERROR);
                                }
                                else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                            }
                        }
                        else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                    }
                    else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                }
                else
                {
                    if (lpBuffer)
                        dwFilePathLength = GetFullPathNameW(lpObjectPath, nBufferLength, lpBuffer, NULL);
                    else g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER);
                }
            }
        }
    }
    return dwFilePathLength;
}

// NTFSLPS_GetLongPathNameA
// ------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetLongPathNameA(IN LPCSTR lpszShortPath, OUT LPSTR lpszLongPath, IN DWORD cchBuffer)
{
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpTemporaryLongPathBuffer;
    DWORD Win32ErrorCode = NO_ERROR;
    DWORD dwResult = 0;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpszShortPath)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return 0; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpszShortPath);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpTemporaryLongPathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, cchBuffer * sizeof(WCHAR));

        if (lpTemporaryLongPathBuffer)
        {
            dwResult = GetLongPathNameW(lpObjectPathBuffer, lpTemporaryLongPathBuffer, cchBuffer);

            if (dwResult && dwResult < cchBuffer)
            {
                if (!UnicodeStringToAnsiString((LPCWSTR) lpTemporaryLongPathBuffer, lpszLongPath, cchBuffer))
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    dwResult = 0;
                }
            }
            else Win32ErrorCode = g_pRtlGetLastWin32Error();

            RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryLongPathBuffer);
        }
        else Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = GetLongPathNameA(lpszShortPath, lpszLongPath, cchBuffer);

    return dwResult;
}

// NTFSLPS_GetLongPathNameTransactedA
// ----------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetLongPathNameTransactedA(IN LPCSTR lpszShortPath, OUT LPSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpTemporaryLongPathBuffer;
    DWORD Win32ErrorCode = NO_ERROR;
    DWORD dwResult = 0;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpszShortPath)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return 0; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpszShortPath);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpTemporaryLongPathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, cchBuffer * sizeof(WCHAR));

            if (lpTemporaryLongPathBuffer)
            {
                dwResult = g_pGetLongPathNameTransactedW(lpObjectPathBuffer, lpTemporaryLongPathBuffer, cchBuffer, hTransaction);

                if (dwResult && dwResult < cchBuffer)
                {
                    if (!UnicodeStringToAnsiString((LPCWSTR) lpTemporaryLongPathBuffer, lpszLongPath, cchBuffer))
                    {
                        Win32ErrorCode = g_pRtlGetLastWin32Error();

                        dwResult = 0;
                    }
                }
                else Win32ErrorCode = g_pRtlGetLastWin32Error();

                RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryLongPathBuffer);
            }
            else Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else dwResult = g_pGetLongPathNameTransactedA(lpszShortPath, lpszLongPath, cchBuffer, hTransaction);
    }
    else dwResult = NTFSLPS_GetLongPathNameA(lpszShortPath, lpszLongPath, cchBuffer);

    return dwResult;
}

// NTFSLPS_GetLongPathNameTransactedW
// ----------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetLongPathNameTransactedW(IN LPCWSTR lpszShortPath, OUT LPWSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpszShortPath)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return 0; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpszShortPath);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            dwResult = g_pGetLongPathNameTransactedW(lpObjectPathBuffer, lpszLongPath, cchBuffer, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else dwResult = g_pGetLongPathNameTransactedW(lpszShortPath, lpszLongPath, cchBuffer, hTransaction);
    }
    else dwResult = NTFSLPS_GetLongPathNameW(lpszShortPath, lpszLongPath, cchBuffer);

    return dwResult;
}

// NTFSLPS_GetLongPathNameW
// ------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetLongPathNameW(IN LPCWSTR lpszShortPath, OUT LPWSTR lpszLongPath, IN DWORD cchBuffer)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpszShortPath)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return 0; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpszShortPath);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        dwResult = GetLongPathNameW(lpObjectPathBuffer, lpszLongPath, cchBuffer);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = GetLongPathNameW(lpszShortPath, lpszLongPath, cchBuffer);

    return dwResult;
}

// NTFSLPS_GetMappedFileNameA
// --------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetMappedFileNameA(IN HANDLE hProcess, IN LPVOID lpv, OUT LPSTR lpFilename, IN DWORD nSize)
{
    LPWSTR lpFilePathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    if (nSize > MAX_PATH16)
    {
        lpFilePathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, nSize * sizeof(WCHAR));

        if (lpFilePathBuffer)
        {
            dwResult = g_pGetMappedFileNameW(hProcess, lpv, lpFilePathBuffer, nSize);

            if (dwResult)
            {
                if (!UnicodeStringToAnsiString((LPCWSTR) lpFilePathBuffer, lpFilename, nSize))
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    dwResult = 0;
                }
                else Win32ErrorCode = NO_ERROR;
            }
            else Win32ErrorCode = g_pRtlGetLastWin32Error();

            RtlFreeHeap(g_hProcessHeap, 0, lpFilePathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else dwResult = 0;
    }
    else dwResult = g_pGetMappedFileNameA(hProcess, lpv, lpFilename, nSize);

    return dwResult;
}

// NTFSLPS_GetMappedFileNameW
// --------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetMappedFileNameW(IN HANDLE hProcess, IN LPVOID lpv, OUT LPWSTR lpFilename, IN DWORD nSize)
{
    return g_pGetMappedFileNameW(hProcess, lpv, lpFilename, nSize);
}

// NTFSLPS_GetModuleFileNameA
// --------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetModuleFileNameA(IN HMODULE hModule, OUT LPSTR lpFilename, IN DWORD nSize)
{
    LPWSTR lpFilePathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    if (nSize)
    {
        lpFilePathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, nSize * sizeof(WCHAR));

        if (lpFilePathBuffer)
        {
            Win32ErrorCode = NO_ERROR;

            dwResult = GetModuleFileNameW(hModule, lpFilePathBuffer, nSize);

            if (dwResult)
            {
                if (dwResult == nSize)
                {
                    if (!g_pRtlGetLastWin32Error())
                        *(lpFilePathBuffer + nSize - 1) = (WCHAR) 0;

                    Win32ErrorCode = ERROR_INSUFFICIENT_BUFFER;
                }

                if (!UnicodeStringToAnsiString((LPCWSTR) lpFilePathBuffer, lpFilename, nSize))
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    dwResult = 0;
                }
            }
            else Win32ErrorCode = g_pRtlGetLastWin32Error();

            RtlFreeHeap(g_hProcessHeap, 0, lpFilePathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else
        {
            dwResult = GetModuleFileNameA(hModule, lpFilename, nSize);

            if (dwResult >= nSize)
            {
                if (!g_pRtlGetLastWin32Error())
                {
                    *(lpFilename + nSize - 1) = (CHAR) 0;

                    g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
                }
            }
        }
    }
    else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

    return dwResult;
}

// NTFSLPS_GetModuleFileNameExA
// ----------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetModuleFileNameExA(IN HANDLE hProcess, IN HMODULE hModule, OUT LPSTR lpFilename, IN DWORD nSize)
{
    LPWSTR lpFilePathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    if (nSize > MAX_PATH16)
    {
        lpFilePathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, nSize * sizeof(WCHAR));

        if (lpFilePathBuffer)
        {
            dwResult = g_pGetModuleFileNameExW(hProcess, hModule, lpFilePathBuffer, nSize);

            if (dwResult)
            {
                if (!UnicodeStringToAnsiString((LPCWSTR) lpFilePathBuffer, lpFilename, nSize))
                {
                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    dwResult = 0;
                }
                else Win32ErrorCode = NO_ERROR;
            }
            else Win32ErrorCode = g_pRtlGetLastWin32Error();

            RtlFreeHeap(g_hProcessHeap, 0, lpFilePathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else dwResult = 0;
    }
    else dwResult = g_pGetModuleFileNameExA(hProcess, hModule, lpFilename, nSize);

    return dwResult;
}

// NTFSLPS_GetModuleFileNameExW
// ----------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetModuleFileNameExW(IN HANDLE hProcess, IN HMODULE hModule, OUT LPWSTR lpFilename, IN DWORD nSize)
{
    return g_pGetModuleFileNameExW(hProcess, hModule, lpFilename, nSize);
}

// NTFSLPS_GetModuleFileNameW
// --------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_GetModuleFileNameW(IN HMODULE hModule, OUT LPWSTR lpFilename, IN DWORD nSize)
{
    DWORD dwResult;

    if (nSize)
    {
        g_pRtlSetLastWin32Error(NO_ERROR);

        dwResult = GetModuleFileNameW(hModule, lpFilename, nSize);

        if (dwResult == nSize)
        {
            if (!g_pRtlGetLastWin32Error())
            {
                *(lpFilename + nSize - 1) = (WCHAR) 0;

                g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);
            }
        }
    }
    else g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER);

    return dwResult;
}

// NTFSLPS_IsPathExistA
// --------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_IsPathExistA(LPCSTR lpObjectPath)
{
    UINT uMode;
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpObjectPath)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    uMode = SetErrorMode(SEM_FAILCRITICALERRORS);

    lpObjectPathBuffer = GetFullObjectPathA(lpObjectPath);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = (GetFileAttributesW(lpObjectPathBuffer) != INVALID_FILE_ATTRIBUTES) ? TRUE : FALSE;

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);
    }
    else
    {
        bResult = (GetFileAttributesA(lpObjectPath) != INVALID_FILE_ATTRIBUTES) ? TRUE : FALSE;

        Win32ErrorCode = g_pRtlGetLastWin32Error();
    }
    SetErrorMode(uMode);

    g_pRtlSetLastWin32Error(Win32ErrorCode);

    return bResult;
}

// NTFSLPS_IsPathExistW
// --------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_IsPathExistW(LPCWSTR lpObjectPath)
{
    UINT uMode;
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpObjectPath)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    uMode = SetErrorMode(SEM_FAILCRITICALERRORS);

    lpObjectPathBuffer = GetFullObjectPathW(lpObjectPath);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = (GetFileAttributesW(lpObjectPathBuffer) != INVALID_FILE_ATTRIBUTES) ? TRUE : FALSE;

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);
    }
    else
    {
        bResult = (GetFileAttributesW(lpObjectPath) != INVALID_FILE_ATTRIBUTES) ? TRUE : FALSE;

        Win32ErrorCode = g_pRtlGetLastWin32Error();
    }
    SetErrorMode(uMode);

    g_pRtlSetLastWin32Error(Win32ErrorCode);

    return bResult;
}

// NTFSLPS_MoveFileA
// -----------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_MoveFileA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpExistingFileName) ||
        IsNTFSi30PathA(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingFilePathBuffer = GetFullObjectPathA(lpExistingFileName);

    if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewFilePathBuffer = GetFullObjectPathA(lpNewFileName);

        if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = MoveFileW(lpExistingFilePathBuffer, lpNewFilePathBuffer);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewFilePathBuffer);
        }
        else
        {
            bResult = MoveFileA(lpExistingFileName, lpNewFileName);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = MoveFileA(lpExistingFileName, lpNewFileName);

    return bResult;
}

// NTFSLPS_MoveFileExA
// -------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_MoveFileExA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN DWORD dwFlags)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpExistingFileName) ||
        IsNTFSi30PathA(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingFilePathBuffer = GetFullObjectPathA(lpExistingFileName);

    if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewFilePathBuffer = GetFullObjectPathA(lpNewFileName);

        if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = MoveFileExW(lpExistingFilePathBuffer, lpNewFilePathBuffer, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewFilePathBuffer);
        }
        else
        {
            bResult = MoveFileExA(lpExistingFileName, lpNewFileName, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = MoveFileExA(lpExistingFileName, lpNewFileName, dwFlags);

    return bResult;
}

// NTFSLPS_MoveFileExW
// -------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_MoveFileExW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN DWORD dwFlags)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpExistingFileName) ||
        IsNTFSi30PathW(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingFilePathBuffer = GetFullObjectPathW(lpExistingFileName);

    if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewFilePathBuffer = GetFullObjectPathW(lpNewFileName);

        if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = MoveFileExW(lpExistingFilePathBuffer, lpNewFilePathBuffer, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewFilePathBuffer);
        }
        else
        {
            bResult = MoveFileExW(lpExistingFileName, lpNewFileName, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = MoveFileExW(lpExistingFileName, lpNewFileName, dwFlags);

    return bResult;
}

// NTFSLPS_MoveFileTransactedA
// ---------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_MoveFileTransactedA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathA(lpExistingFileName) ||
            IsNTFSi30PathA(lpNewFileName))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpExistingFilePathBuffer = GetFullObjectPathA(lpExistingFileName);

        if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpNewFilePathBuffer = GetFullObjectPathA(lpNewFileName);

            if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pMoveFileTransactedW(lpExistingFilePathBuffer, lpNewFilePathBuffer, lpProgressRoutine, lpData, dwFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpNewFilePathBuffer);
            }
            else
            {
                bResult = g_pMoveFileTransactedA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pMoveFileTransactedA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags, hTransaction);
    }
    else bResult = NTFSLPS_MoveFileWithProgressA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags);

    return bResult;
}

// NTFSLPS_MoveFileTransactedW
// ---------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_MoveFileTransactedW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
           (IsNTFSi30PathW(lpExistingFileName) ||
            IsNTFSi30PathW(lpNewFileName))) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpExistingFilePathBuffer = GetFullObjectPathW(lpExistingFileName);

        if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpNewFilePathBuffer = GetFullObjectPathW(lpNewFileName);

            if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pMoveFileTransactedW(lpExistingFilePathBuffer, lpNewFilePathBuffer, lpProgressRoutine, lpData, dwFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpNewFilePathBuffer);
            }
            else
            {
                bResult = g_pMoveFileTransactedW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags, hTransaction);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pMoveFileTransactedW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags, hTransaction);
    }
    else bResult = NTFSLPS_MoveFileWithProgressW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags);

    return bResult;
}

// NTFSLPS_MoveFileW
// -----------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_MoveFileW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpExistingFileName) ||
        IsNTFSi30PathW(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingFilePathBuffer = GetFullObjectPathW(lpExistingFileName);

    if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewFilePathBuffer = GetFullObjectPathW(lpNewFileName);

        if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = MoveFileW(lpExistingFilePathBuffer, lpNewFilePathBuffer);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewFilePathBuffer);
        }
        else
        {
            bResult = MoveFileW(lpExistingFileName, lpNewFileName);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = MoveFileW(lpExistingFileName, lpNewFileName);

    return bResult;
}

// NTFSLPS_MoveFileWithProgressA
// -----------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_MoveFileWithProgressA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpExistingFileName) ||
        IsNTFSi30PathA(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingFilePathBuffer = GetFullObjectPathA(lpExistingFileName);

    if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewFilePathBuffer = GetFullObjectPathA(lpNewFileName);

        if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pMoveFileWithProgressW(lpExistingFilePathBuffer, lpNewFilePathBuffer, lpProgressRoutine, lpData, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewFilePathBuffer);
        }
        else
        {
            bResult = g_pMoveFileWithProgressA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = g_pMoveFileWithProgressA(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags);

    return bResult;
}

// NTFSLPS_MoveFileWithProgressW
// -----------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_MoveFileWithProgressW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags)
{
    LPWSTR lpExistingFilePathBuffer;
    LPWSTR lpNewFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpExistingFileName) ||
        IsNTFSi30PathW(lpNewFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpExistingFilePathBuffer = GetFullObjectPathW(lpExistingFileName);

    if (lpExistingFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpNewFilePathBuffer = GetFullObjectPathW(lpNewFileName);

        if (lpNewFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pMoveFileWithProgressW(lpExistingFilePathBuffer, lpNewFilePathBuffer, lpProgressRoutine, lpData, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpNewFilePathBuffer);
        }
        else
        {
            bResult = g_pMoveFileWithProgressW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpExistingFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = g_pMoveFileWithProgressW(lpExistingFileName, lpNewFileName, lpProgressRoutine, lpData, dwFlags);

    return bResult;
}

// NTFSLPS_NewFileA
// ----------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_NewFileA(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN BOOL bCreateAlways, IN DWORD dwFlagsAndAttributes)
{
    UINT uPathLength;
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpSubdirectory;
    UINT cSubdirectories = 0;
    DWORD Win32ErrorCode;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    if (!lpFileName || (lpFileName[0] == (CHAR) 0))
    {
        g_pRtlSetLastWin32Error(ERROR_INVALID_NAME);

        return hFile;
    }

    uPathLength = (UINT) GetFullPathExAW(lpFileName, NULL, 0, FALSE);

    if (!uPathLength)
        return hFile;

    lpObjectPathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, uPathLength * sizeof(WCHAR));

    if (!lpObjectPathBuffer)
        return hFile;

    uPathLength = (UINT) GetFullPathExAW(lpFileName, lpObjectPathBuffer, uPathLength, FALSE);

    if (!uPathLength)
    {
        Win32ErrorCode = g_pRtlGetLastWin32Error();

        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);

        return hFile;
    }

    if (!wcsncmp(lpObjectPathBuffer, L"\\\\?\\UNC\\", 8))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 8, (unsigned int) '\\') + 1;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\?\\", 4))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 4, (unsigned int) '\\') + 1;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\.\\", 4))
    {
        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(ERROR_CANNOT_MAKE);

        return hFile;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\", 2))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 2, (unsigned int) '\\') + 1;
    }
    else lpSubdirectory = wcschr(lpObjectPathBuffer, (unsigned int) '\\') + 1;

    uPathLength = wcslen(lpSubdirectory);

    if (!uPathLength)
    {
        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);

        return hFile;
    }

    if (*(lpSubdirectory + uPathLength - 1) == (WCHAR) '\\')
    {
        *(lpSubdirectory + uPathLength - 1) = (WCHAR) 0;

        uPathLength = 0;
    }

    lpSubdirectory = wcschr(lpSubdirectory, (unsigned int) '\\');

    while (lpSubdirectory)
    { 
        lpSubdirectory[0] = (WCHAR) 0;

        bResult = CreateDirectoryW(lpObjectPathBuffer, NULL);

        if (bResult) cSubdirectories++;
        else
        {
            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (Win32ErrorCode != ERROR_ALREADY_EXISTS)
            {
                while (cSubdirectories)
                {
                    lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                    if (!lpSubdirectory)
                        break;

                    lpSubdirectory[0] = (WCHAR) 0;

                    if (!RemoveDirectoryW(lpObjectPathBuffer))
                        break;

                    cSubdirectories--;
                }

                RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);

                return hFile;
            }
        }

        lpSubdirectory[0] = (WCHAR) '\\';

        lpSubdirectory = wcschr(lpSubdirectory + 1, (unsigned int) '\\');
    }

    if (uPathLength)
    {
        hFile = CreateFileW(lpObjectPathBuffer, dwDesiredAccess, dwShareMode, NULL, ((bCreateAlways & TRUE) ? CREATE_ALWAYS : CREATE_NEW), dwFlagsAndAttributes, NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            Win32ErrorCode = g_pRtlGetLastWin32Error();

            while (cSubdirectories)
            {
                lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                if (!lpSubdirectory)
                    break;

                lpSubdirectory[0] = (WCHAR) 0;

                if (!RemoveDirectoryW(lpObjectPathBuffer))
                    break;

                cSubdirectories--;
            }

            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);

            return hFile;
        }
    }
    else
    {
        if (!CreateDirectoryW(lpObjectPathBuffer, NULL))
        {
            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (Win32ErrorCode != ERROR_ALREADY_EXISTS)
            {
                while (cSubdirectories)
                {
                    lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                    if (!lpSubdirectory)
                        break;

                    lpSubdirectory[0] = (WCHAR) 0;

                    if (!RemoveDirectoryW(lpObjectPathBuffer))
                        break;

                    cSubdirectories--;
                }

                RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);

                return hFile;
            }
        }
    }
    RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

    g_pRtlSetLastWin32Error(NO_ERROR);

    return hFile;
}

// NTFSLPS_NewFileW
// ----------------
// Implemented: 100%

HANDLE WINAPI NTFSLPS_NewFileW(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN BOOL bCreateAlways, IN DWORD dwFlagsAndAttributes)
{
    UINT uPathLength;
    LPWSTR lpObjectPathBuffer;
    LPWSTR lpSubdirectory;
    UINT cSubdirectories = 0;
    DWORD Win32ErrorCode;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return INVALID_HANDLE_VALUE; }
    #endif

    if (!lpFileName || (lpFileName[0] == 0))
    {
        g_pRtlSetLastWin32Error(ERROR_INVALID_NAME);

        return hFile;
    }

    uPathLength = (UINT) NTFSLPS_GetFullPathW(lpFileName, NULL, 0);

    if (!uPathLength)
        return hFile;

    lpObjectPathBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, uPathLength * sizeof(WCHAR));

    if (!lpObjectPathBuffer)
        return hFile;

    uPathLength = (UINT) NTFSLPS_GetFullPathW(lpFileName, lpObjectPathBuffer, uPathLength);

    if (!uPathLength)
    {
        Win32ErrorCode = g_pRtlGetLastWin32Error();

        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);

        return hFile;
    }

    if (!wcsncmp(lpObjectPathBuffer, L"\\\\?\\UNC\\", 8))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 8, (unsigned int) '\\') + 1;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\?\\", 4))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 4, (unsigned int) '\\') + 1;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\.\\", 4))
    {
        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(ERROR_CANNOT_MAKE);

        return hFile;
    }
    else if (!wcsncmp(lpObjectPathBuffer, L"\\\\", 2))
    {
        lpSubdirectory = wcschr(lpObjectPathBuffer + 2, (unsigned int) '\\') + 1;
    }
    else lpSubdirectory = wcschr(lpObjectPathBuffer, (unsigned int) '\\') + 1;

    uPathLength = wcslen(lpSubdirectory);

    if (!uPathLength)
    {
        RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);

        return hFile;
    }

    if (*(lpSubdirectory + uPathLength - 1) == (WCHAR) '\\')
    {
        *(lpSubdirectory + uPathLength - 1) = (WCHAR) 0;

        uPathLength = 0;
    }

    lpSubdirectory = wcschr(lpSubdirectory, (unsigned int) '\\');

    while (lpSubdirectory)
    { 
        lpSubdirectory[0] = (WCHAR) 0;

        bResult = CreateDirectoryW(lpObjectPathBuffer, NULL);

        if (bResult) cSubdirectories++;
        else
        {
            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (Win32ErrorCode != ERROR_ALREADY_EXISTS)
            {
                while (cSubdirectories)
                {
                    lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                    if (!lpSubdirectory)
                        break;

                    lpSubdirectory[0] = (WCHAR) 0;

                    if (!RemoveDirectoryW(lpObjectPathBuffer))
                        break;

                    cSubdirectories--;
                }

                RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);

                return hFile;
            }
        }

        lpSubdirectory[0] = (WCHAR) '\\';

        lpSubdirectory = wcschr(lpSubdirectory + 1, (unsigned int) '\\');
    }

    if (uPathLength)
    {
        hFile = CreateFileW(lpObjectPathBuffer, dwDesiredAccess, dwShareMode, NULL, ((bCreateAlways & TRUE) ? CREATE_ALWAYS : CREATE_NEW), dwFlagsAndAttributes, NULL);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            Win32ErrorCode = g_pRtlGetLastWin32Error();

            while (cSubdirectories)
            {
                lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                if (!lpSubdirectory)
                    break;

                lpSubdirectory[0] = (WCHAR) 0;

                if (!RemoveDirectoryW(lpObjectPathBuffer))
                    break;

                cSubdirectories--;
            }

            RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);

            return hFile;
        }
    }
    else
    {
        if (!CreateDirectoryW(lpObjectPathBuffer, NULL))
        {
            Win32ErrorCode = g_pRtlGetLastWin32Error();

            if (Win32ErrorCode != ERROR_ALREADY_EXISTS)
            {
                while (cSubdirectories)
                {
                    lpSubdirectory = wcsrchr(lpObjectPathBuffer, (unsigned int) '\\');

                    if (!lpSubdirectory)
                        break;

                    lpSubdirectory[0] = (WCHAR) 0;

                    if (!RemoveDirectoryW(lpObjectPathBuffer))
                        break;

                    cSubdirectories--;
                }

                RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

                g_pRtlSetLastWin32Error(Win32ErrorCode);

                return hFile;
            }
        }
    }
    RtlFreeHeap(g_hProcessHeap, 0, lpObjectPathBuffer);

    g_pRtlSetLastWin32Error(NO_ERROR);

    return hFile;
}

// NTFSLPS_RemoveDirectoryA
// ------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_RemoveDirectoryA(IN LPCSTR lpPathName)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpPathName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = RemoveDirectoryW(lpObjectPathBuffer);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = RemoveDirectoryA(lpPathName);

    return bResult;
}

// NTFSLPS_RemoveDirectoryTransactedA
// ----------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_RemoveDirectoryTransactedA(IN LPCSTR lpPathName, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpPathName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpPathName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pRemoveDirectoryTransactedW(lpObjectPathBuffer, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pRemoveDirectoryTransactedA(lpPathName, hTransaction);
    }
    else bResult = NTFSLPS_RemoveDirectoryA(lpPathName);

    return bResult;
}

// NTFSLPS_RemoveDirectoryTransactedW
// ----------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_RemoveDirectoryTransactedW(IN LPCWSTR lpPathName, IN HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpPathName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpPathName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pRemoveDirectoryTransactedW(lpObjectPathBuffer, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pRemoveDirectoryTransactedW(lpPathName, hTransaction);
    }
    else bResult = NTFSLPS_RemoveDirectoryW(lpPathName);

    return bResult;
}

// NTFSLPS_RemoveDirectoryW
// ------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_RemoveDirectoryW(IN LPCWSTR lpPathName)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpPathName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = RemoveDirectoryW(lpObjectPathBuffer);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = RemoveDirectoryW(lpPathName);

    return bResult;
}

// NTFSLPS_RemovePathEndingSlashA
// ------------------------------
// Implemented: 100%

UINT WINAPI NTFSLPS_RemovePathEndingSlashA(IN OUT LPSTR lpPathBuffer, IN UINT cchBuffer)
{
    UINT cchPath;

    if (lpPathBuffer)
    {
        if (cchBuffer)
        {
            for (LPSTR lpCurrentBufferPosition = lpPathBuffer; lpCurrentBufferPosition < (lpPathBuffer + cchBuffer); lpCurrentBufferPosition++)
            {
                if (!(*lpCurrentBufferPosition))
                {
                    do
                    {
                        CHAR cSymbol = *--lpCurrentBufferPosition;
                        if ((cSymbol != '\\') && (cSymbol != '/'))
                            { lpCurrentBufferPosition++; break; }
                    } while ((lpCurrentBufferPosition > lpPathBuffer));
                    cchPath = lpCurrentBufferPosition - lpPathBuffer;
                    if (cchPath >= (UNICODE_STRING_MAX_CHARS - 1))
                    {
                        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);
                        cchPath = cchBuffer;
                    }
                    else *lpCurrentBufferPosition = 0;

                    return cchPath;
                }
            }
            g_pRtlSetLastWin32Error(ERROR_MORE_DATA);

            goto loc_ReturnBufferLength;
        }
        else { g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnBufferLength; }
    }
    else { g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER); loc_ReturnBufferLength: cchPath = cchBuffer; }

    return cchPath;
}

// NTFSLPS_RemovePathEndingSlashW
// ------------------------------
// Implemented: 100%

UINT WINAPI NTFSLPS_RemovePathEndingSlashW(IN OUT LPWSTR lpPathBuffer, IN UINT cchBuffer)
{
    UINT cchPath;

    if (lpPathBuffer)
    {
        if (cchBuffer)
        {
            for (LPWSTR lpCurrentBufferPosition = lpPathBuffer; lpCurrentBufferPosition < (lpPathBuffer + cchBuffer); lpCurrentBufferPosition++)
            {
                if (!(*lpCurrentBufferPosition))
                {
                    do
                    {
                        WCHAR cSymbol = *--lpCurrentBufferPosition;
                        if ((cSymbol != '\\') && (cSymbol != '/'))
                            { lpCurrentBufferPosition++; break; }
                    } while ((lpCurrentBufferPosition > lpPathBuffer));
                    cchPath = lpCurrentBufferPosition - lpPathBuffer;
                    if (cchPath >= (UNICODE_STRING_MAX_CHARS - 1))
                    {
                        g_pRtlSetLastWin32Error(ERROR_FILENAME_EXCED_RANGE);
                        cchPath = cchBuffer;
                    }
                    else *lpCurrentBufferPosition = 0;

                    return cchPath;
                }
            }
            g_pRtlSetLastWin32Error(ERROR_MORE_DATA);

            goto loc_ReturnBufferLength;
        }
        else { g_pRtlSetLastWin32Error(ERROR_INSUFFICIENT_BUFFER); goto loc_ReturnBufferLength; }
    }
    else { g_pRtlSetLastWin32Error(ERROR_INVALID_USER_BUFFER); loc_ReturnBufferLength: cchPath = cchBuffer; }

    return cchPath;
}

// NTFSLPS_ReplaceFileA
// --------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_ReplaceFileA(IN LPCSTR lpReplacedFileName, IN LPCSTR lpReplacementFileName, IN LPCSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved)
{
    LPWSTR lpReplacedFilePathBuffer;
    LPWSTR lpReplacementFilePathBuffer;
    LPWSTR lpBackupFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpReplacedFileName) ||
        IsNTFSi30PathA(lpReplacementFileName) ||
        IsNTFSi30PathA(lpBackupFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpReplacedFilePathBuffer = GetFullObjectPathA(lpReplacedFileName);

    if (lpReplacedFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpReplacementFilePathBuffer = GetFullObjectPathA(lpReplacementFileName);

        if (lpReplacementFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpBackupFilePathBuffer = GetFullObjectPathA(lpBackupFileName);

            if (lpBackupFilePathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pReplaceFileW(lpReplacedFilePathBuffer, lpReplacementFilePathBuffer, lpBackupFilePathBuffer, dwReplaceFlags, lpExclude, lpReserved);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpBackupFilePathBuffer);
            }
            else
            {
                bResult = g_pReplaceFileA(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpReplacementFilePathBuffer);
        }
        else
        {
            bResult = g_pReplaceFileA(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpReplacedFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = g_pReplaceFileA(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);

    return bResult;
}

// NTFSLPS_ReplaceFilePathExtensionA
// ---------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_ReplaceFilePathExtensionA(IN OUT LPSTR lpObjectPath, IN DWORD nSize, LPCSTR lpExtension)
{
    DWORD dwResult;

    if (lpObjectPath)
    {
        if (nSize)
        {
            LPSTR lpTemporary;
            LPSTR lpObjectExtension = 0;

            for (lpTemporary = lpObjectPath; lpTemporary < lpObjectPath + nSize; lpTemporary++)
            {
                register CHAR ch = *lpTemporary;
                if (!ch) break;
                if ((ch == '\\') || (ch == '.') || (ch == '/'))
                    lpObjectExtension = lpTemporary;
            }
            dwResult = lpTemporary - lpObjectPath;

            if (dwResult < UNICODE_STRING_MAX_CHARS)
            {
                if (dwResult < nSize)
                {
                    if ((lpObjectExtension == lpObjectPath + dwResult - 1) &&
                        (lpObjectExtension != lpObjectPath) &&
                      ((*lpObjectExtension == '\\') || (*lpObjectExtension == '/'))) {
                        do { lpObjectExtension--; } while (lpObjectExtension != lpObjectPath && ((*lpObjectExtension == '\\') || (*lpObjectExtension == '/')));
                        lpObjectExtension++;
                    }
                    else if (!lpObjectExtension || ((lpObjectExtension != lpObjectPath) && ((*lpObjectExtension == '\\') || (*lpObjectExtension == '/'))))
                        lpObjectExtension = lpObjectPath + dwResult;

                    if (lpExtension)
                    {
                        (LPCSTR) lpTemporary = lpExtension;

                        while (lpObjectExtension < lpObjectPath + nSize)
                        {
                            register CHAR ch = *lpTemporary;
                            *lpObjectExtension = ch;
                            if (!ch) break;

                            lpObjectExtension++;
                            lpTemporary++;
                        }

                        dwResult = lpObjectExtension - lpObjectPath;

                        if (dwResult == nSize)
                        {
                            SetLastError(ERROR_INSUFFICIENT_BUFFER);

                            *(lpObjectExtension - 1) = 0;
                        }
                        else goto loc_SetNoErrorCodeAndReturn;
                    }
                    else { *lpObjectExtension = 0; loc_SetNoErrorCodeAndReturn: SetLastError(NO_ERROR); }
                }
                else SetLastError(ERROR_MORE_DATA);
            }
            else { SetLastError(ERROR_FILENAME_EXCED_RANGE); goto loc_ReturnZero; }
        }
        else { SetLastError(ERROR_INVALID_USER_BUFFER); goto loc_ReturnZero; }
    }
    else { SetLastError(ERROR_INVALID_NAME); loc_ReturnZero: dwResult = 0; }

    return dwResult;
}

// NTFSLPS_ReplaceFilePathExtensionW
// ---------------------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_ReplaceFilePathExtensionW(IN OUT LPWSTR lpObjectPath, IN DWORD nSize, LPCWSTR lpExtension)
{
    DWORD dwResult;

    if (lpObjectPath)
    {
        if (nSize)
        {
            LPWSTR lpTemporary;
            LPWSTR lpObjectExtension = 0;

            for (lpTemporary = lpObjectPath; lpTemporary < lpObjectPath + nSize; lpTemporary++)
            {
                register WCHAR ch = *lpTemporary;
                if (!ch) break;
                if ((ch == '\\') || (ch == '.') || (ch == '/'))
                    lpObjectExtension = lpTemporary;
            }
            dwResult = lpTemporary - lpObjectPath;

            if (dwResult < UNICODE_STRING_MAX_CHARS)
            {
                if (dwResult < nSize)
                {
                    if ((lpObjectExtension == lpObjectPath + dwResult - 1) &&
                        (lpObjectExtension != lpObjectPath) &&
                      ((*lpObjectExtension == '\\') || (*lpObjectExtension == '/'))) {
                        do { lpObjectExtension--; } while (lpObjectExtension != lpObjectPath && ((*lpObjectExtension == '\\') || (*lpObjectExtension == '/')));
                        lpObjectExtension++;
                    }
                    else if (!lpObjectExtension || ((lpObjectExtension != lpObjectPath) && ((*lpObjectExtension == '\\') || (*lpObjectExtension == '/'))))
                        lpObjectExtension = lpObjectPath + dwResult;

                    if (lpExtension)
                    {
                        (LPCWSTR) lpTemporary = lpExtension;

                        while (lpObjectExtension < lpObjectPath + nSize)
                        {
                            register WCHAR ch = *lpTemporary;
                            *lpObjectExtension = ch;
                            if (!ch) break;

                            lpObjectExtension++;
                            lpTemporary++;
                        }

                        dwResult = lpObjectExtension - lpObjectPath;

                        if (dwResult == nSize)
                        {
                            SetLastError(ERROR_INSUFFICIENT_BUFFER);

                            *(lpObjectExtension - 1) = 0;
                        }
                        else goto loc_SetNoErrorCodeAndReturn;
                    }
                    else { *lpObjectExtension = 0; loc_SetNoErrorCodeAndReturn: SetLastError(NO_ERROR); }
                }
                else SetLastError(ERROR_MORE_DATA);
            }
            else { SetLastError(ERROR_FILENAME_EXCED_RANGE); goto loc_ReturnZero; }
        }
        else { SetLastError(ERROR_INVALID_USER_BUFFER); goto loc_ReturnZero; }
    }
    else { SetLastError(ERROR_INVALID_NAME); loc_ReturnZero: dwResult = 0; }

    return dwResult;
}

// NTFSLPS_ReplaceFileW
// --------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_ReplaceFileW(IN LPCWSTR lpReplacedFileName, IN LPCWSTR lpReplacementFileName, IN LPCWSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved)
{
    LPWSTR lpReplacedFilePathBuffer;
    LPWSTR lpReplacementFilePathBuffer;
    LPWSTR lpBackupFilePathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpReplacedFileName) ||
        IsNTFSi30PathW(lpReplacementFileName) ||
        IsNTFSi30PathW(lpBackupFileName))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpReplacedFilePathBuffer = GetFullObjectPathW(lpReplacedFileName);

    if (lpReplacedFilePathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpReplacementFilePathBuffer = GetFullObjectPathW(lpReplacementFileName);

        if (lpReplacementFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpBackupFilePathBuffer = GetFullObjectPathW(lpBackupFileName);

            if (lpBackupFilePathBuffer || !g_pRtlGetLastWin32Error())
            {
                bResult = g_pReplaceFileW(lpReplacedFilePathBuffer, lpReplacementFilePathBuffer, lpBackupFilePathBuffer, dwReplaceFlags, lpExclude, lpReserved);

                Win32ErrorCode = g_pRtlGetLastWin32Error();

                FreeFullObjectPathBuffer(lpBackupFilePathBuffer);
            }
            else
            {
                bResult = g_pReplaceFileW(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);

                Win32ErrorCode = g_pRtlGetLastWin32Error();
            }
            FreeFullObjectPathBuffer(lpReplacementFilePathBuffer);
        }
        else
        {
            bResult = g_pReplaceFileW(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);

            Win32ErrorCode = g_pRtlGetLastWin32Error();
        }
        FreeFullObjectPathBuffer(lpReplacedFilePathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = g_pReplaceFileW(lpReplacedFileName, lpReplacementFileName, lpBackupFileName, dwReplaceFlags, lpExclude, lpReserved);

    return bResult;
}

// NTFSLPS_SearchPathA
// -------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_SearchPathA(IN LPCSTR lpPath, IN LPCSTR lpFileName, IN LPCSTR lpExtension, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart)
{
    LPWSTR lpPathBuffer;
    LPWSTR lpFilePathBuffer;
    LPWSTR lpExtensionBuffer;
    LPWSTR lpTemporaryBuffer;
    LPWSTR lpTemporaryFilePart;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathA(lpPath) ||
        IsNTFSi30PathA(lpFileName) ||
        IsNTFSi30PathA(lpExtension))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return 0; }
    #endif

    lpPathBuffer = GetFullObjectPathA(lpPath);

    if (lpPathBuffer || !g_pRtlGetLastWin32Error())
    {
        lpFilePathBuffer = AllocateUnicodeString(lpFileName);

        if (lpFilePathBuffer || !g_pRtlGetLastWin32Error())
        {
            lpExtensionBuffer = AllocateUnicodeString(lpExtension);

            if (lpExtensionBuffer || !g_pRtlGetLastWin32Error())
            {
                lpTemporaryBuffer = RtlAllocateHeap(g_hProcessHeap, HEAP_ZERO_MEMORY, nBufferLength * sizeof(WCHAR));

                if (lpTemporaryBuffer)
                {
                    dwResult = g_pSearchPathW(lpPathBuffer, lpFilePathBuffer, lpExtensionBuffer, nBufferLength, lpTemporaryBuffer, &lpTemporaryFilePart);

                    Win32ErrorCode = g_pRtlGetLastWin32Error();

                    if (dwResult && dwResult < nBufferLength)
                    {
                        if (!UnicodeStringToAnsiString((LPCWSTR) lpTemporaryBuffer, lpBuffer, nBufferLength))
                        {
                            Win32ErrorCode = g_pRtlGetLastWin32Error();

                            dwResult = 0;
                        }
                        else *lpFilePart = (lpTemporaryFilePart - lpTemporaryBuffer) / sizeof(WCHAR) + lpBuffer;
                    }
                    RtlFreeHeap(g_hProcessHeap, 0, lpTemporaryBuffer);
                }
                else
                {
                    dwResult = g_pSearchPathA(lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

                    Win32ErrorCode = g_pRtlGetLastWin32Error();
                }
                RtlFreeHeap(g_hProcessHeap, 0, lpExtensionBuffer);
            }
            RtlFreeHeap(g_hProcessHeap, 0, lpFilePathBuffer);
        }
        FreeFullObjectPathBuffer(lpPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = g_pSearchPathA(lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

    return dwResult;
}

// NTFSLPS_SearchPathW
// -------------------
// Implemented: 100%

DWORD WINAPI NTFSLPS_SearchPathW(IN LPCWSTR lpPath, IN LPCWSTR lpFileName, IN LPCWSTR lpExtension, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart)
{
    LPWSTR lpPathBuffer;
    DWORD Win32ErrorCode;
    DWORD dwResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
       (IsNTFSi30PathW(lpPath) ||
        IsNTFSi30PathW(lpFileName) ||
        IsNTFSi30PathW(lpExtension))) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return 0; }
    #endif

    lpPathBuffer = GetFullObjectPathW(lpPath);

    if (lpPathBuffer || !g_pRtlGetLastWin32Error())
    {
        dwResult = g_pSearchPathW(lpPathBuffer, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else dwResult = g_pSearchPathW(lpPath, lpFileName, lpExtension, nBufferLength, lpBuffer, lpFilePart);

    return dwResult;
}

// NTFSLPS_SetCurrentDirectoryA
// ----------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_SetCurrentDirectoryA(IN LPCSTR lpPathName)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpPathName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = SetCurrentDirectoryW(lpObjectPathBuffer);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = SetCurrentDirectoryA(lpPathName);

    return bResult;
}

// NTFSLPS_SetCurrentDirectoryW
// ----------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_SetCurrentDirectoryW(IN LPCWSTR lpPathName)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpPathName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpPathName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = SetCurrentDirectoryW(lpObjectPathBuffer);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = SetCurrentDirectoryW(lpPathName);

    return bResult;
}

// NTFSLPS_SetFileAttributesA
// --------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_SetFileAttributesA(IN LPCSTR lpFileName, IN DWORD dwFileAttributes)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathA(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = SetFileAttributesW(lpObjectPathBuffer, dwFileAttributes);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = SetFileAttributesA(lpFileName, dwFileAttributes);

    return bResult;
}

// NTFSLPS_SetFileAttributesTransactedA
// ------------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_SetFileAttributesTransactedA(IN LPCSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathA(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathA(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pSetFileAttributesTransactedW(lpObjectPathBuffer, dwFileAttributes, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pSetFileAttributesTransactedA(lpFileName, dwFileAttributes, hTransaction);
    }
    else bResult = NTFSLPS_SetFileAttributesA(lpFileName, dwFileAttributes);

    return bResult;
}

// NTFSLPS_SetFileAttributesTransactedW
// ------------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_SetFileAttributesTransactedW(IN LPCWSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    if (hTransaction)
    {
        #ifdef NTFS_I30_FILTER
        if (IsNTFSi30PathCheckEnabled &&
            IsNTFSi30PathW(lpFileName)) {
            g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
            return FALSE; }
        #endif

        lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

        if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
        {
            bResult = g_pSetFileAttributesTransactedW(lpObjectPathBuffer, dwFileAttributes, hTransaction);

            Win32ErrorCode = g_pRtlGetLastWin32Error();

            FreeFullObjectPathBuffer(lpObjectPathBuffer);

            g_pRtlSetLastWin32Error(Win32ErrorCode);
        }
        else bResult = g_pSetFileAttributesTransactedW(lpFileName, dwFileAttributes, hTransaction);
    }
    else bResult = NTFSLPS_SetFileAttributesW(lpFileName, dwFileAttributes);

    return bResult;
}

// NTFSLPS_SetFileAttributesW
// --------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_SetFileAttributesW(IN LPCWSTR lpFileName, IN DWORD dwFileAttributes)
{
    LPWSTR lpObjectPathBuffer;
    DWORD Win32ErrorCode;
    BOOL bResult;

    #ifdef NTFS_I30_FILTER
    if (IsNTFSi30PathCheckEnabled &&
        IsNTFSi30PathW(lpFileName)) {
        g_pRtlSetLastWin32Error(ERROR_BAD_PATHNAME);
        return FALSE; }
    #endif

    lpObjectPathBuffer = GetFullObjectPathW(lpFileName);

    if (lpObjectPathBuffer || !g_pRtlGetLastWin32Error())
    {
        bResult = SetFileAttributesW(lpObjectPathBuffer, dwFileAttributes);

        Win32ErrorCode = g_pRtlGetLastWin32Error();

        FreeFullObjectPathBuffer(lpObjectPathBuffer);

        g_pRtlSetLastWin32Error(Win32ErrorCode);
    }
    else bResult = SetFileAttributesW(lpFileName, dwFileAttributes);

    return bResult;
}

// NTFSLPS_SetSearchPathMode
// -------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_SetSearchPathMode(IN DWORD Flags)
{
    return g_pSetSearchPathMode(Flags);
}

// NTFSLPS_Wow64DisableWow64FsRedirection
// ----------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_Wow64DisableWow64FsRedirection(OUT PBOOL pbOldValue)
{
    return g_pWow64DisableWow64FsRedirection(pbOldValue);
}

// NTFSLPS_Wow64EnableWow64FsRedirection
// ---------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_Wow64EnableWow64FsRedirection(IN BOOL bWow64FsEnableRedirection)
{
    return g_pWow64EnableWow64FsRedirection(bWow64FsEnableRedirection);
}

// NTFSLPS_Wow64RevertWow64FsRedirection
// ---------------------------------
// Implemented: 100%

BOOL WINAPI NTFSLPS_Wow64RevertWow64FsRedirection(OUT PBOOL pbOldValue)
{
    return g_pWow64RevertWow64FsRedirection(pbOldValue);
}

// NTFSLPS_lcreat
// --------------
// Implemented: 100%

HFILE WINAPI NTFSLPS_lcreat(IN LPCSTR lpPathName, IN INT iAttribute)
{
    g_pRtlSetLastWin32Error(NO_ERROR);

    return (HFILE) NTFSLPS_CreateFileA(lpPathName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, iAttribute & 0x5AFFB7, NULL);
}

// NTFSLPS_lopen
// -------------
// Implemented: 100%

HFILE WINAPI NTFSLPS_lopen(IN LPCSTR lpPathName, IN INT iReadWrite)
{
    DWORD dwDesiredAccess;
    DWORD dwShareMode;

    g_pRtlSetLastWin32Error(NO_ERROR);

    if (iReadWrite & OF_READWRITE)
        dwDesiredAccess = GENERIC_WRITE | GENERIC_READ;
    else dwDesiredAccess = (~iReadWrite & OF_WRITE + 1) << 30;

    dwShareMode = iReadWrite & (OF_SHARE_EXCLUSIVE | OF_SHARE_DENY_WRITE | OF_SHARE_DENY_NONE);

    switch (dwShareMode)
    {
    case OF_SHARE_DENY_READ:
        dwShareMode = FILE_SHARE_WRITE;
        break;

    case OF_SHARE_DENY_WRITE:
        dwShareMode = FILE_SHARE_READ;
        break;

    case OF_SHARE_DENY_NONE:
        dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        break;

    default:
        dwShareMode = (dwShareMode - OF_SHARE_EXCLUSIVE) ? (FILE_SHARE_READ | FILE_SHARE_WRITE) : 0;
    }

    return (HFILE) NTFSLPS_CreateFileA(lpPathName, dwDesiredAccess, dwShareMode, NULL, OPEN_EXISTING, 0, NULL);
}

// NTFSLPS_mkdir
// -------------
// Implemented: 100%

int __cdecl NTFSLPS_mkdir(IN const char *dirname)
{
    if (!NTFSLPS_CreateDirectoryA(dirname, NULL))
    {
        errno = getwinerrnocode(g_pRtlGetLastWin32Error());

        return -1;
    }
    return 0;
}

// NTFSLPS_remove
// --------------
// Implemented: 100%

int __cdecl NTFSLPS_remove(IN const char *filename)
{
    if (!NTFSLPS_DeleteFileA(filename))
    {
        errno = getwinerrnocode(g_pRtlGetLastWin32Error());

        return -1;
    }
    return 0;
}

// NTFSLPS_rmdir
// -------------
// Implemented: 100%

int __cdecl NTFSLPS_rmdir(const char *path)
{
    if (!NTFSLPS_RemoveDirectoryA(path))
    {
        errno = getwinerrnocode(g_pRtlGetLastWin32Error());

        return -1;
    }
    return 0;
}

// NTFSLPS_wmkdir
// --------------
// Implemented: 100%

int __cdecl NTFSLPS_wmkdir(IN const wchar_t *dirname)
{
    if (!NTFSLPS_CreateDirectoryW(dirname, NULL))
    {
        errno = getwinerrnocode(g_pRtlGetLastWin32Error());

        return -1;
    }
    return 0;
}

// NTFSLPS_wremove
// ---------------
// Implemented: 100%

int __cdecl NTFSLPS_wremove(IN const wchar_t *filename)
{
    if (!NTFSLPS_DeleteFileW(filename))
    {
        errno = getwinerrnocode(g_pRtlGetLastWin32Error());

        return -1;
    }
    return 0;
}

// NTFSLPS_wrmdir
// --------------
// Implemented: 100%

int __cdecl NTFSLPS_wrmdir(const wchar_t *path)
{
    if (!NTFSLPS_RemoveDirectoryW(path))
    {
        errno = getwinerrnocode(g_pRtlGetLastWin32Error());

        return -1;
    }
    return 0;
}

/* +================================+ */
/* | UNIMPLEMENTED WINAPI FUNCTIONS | */
/* +================================+ */

// FindFirstFileNameA
// ------------------
// Implemented: 0%

HANDLE WINAPI FindFirstFileNameA(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// FindFirstFileNameTransactedA
// ----------------------------
// Implemented: 0%

HANDLE WINAPI FindFirstFileNameTransactedA(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// FindFirstStreamA
// ----------------
// Implemented: 0%

HANDLE WINAPI FindFirstStreamA(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPVOID lpFindStreamData, DWORD dwFlags)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// FindFirstStreamTransactedA
// --------------------------
// Implemented: 0%

HANDLE WINAPI FindFirstStreamTransactedA(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPVOID lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// FindNextFileNameA
// -----------------
// Implemented: 0%

BOOL WINAPI FindNextFileNameA(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PSTR LinkName)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// FindNextStreamA
// ---------------
// Implemented: 0%

BOOL WINAPI FindNextStreamA(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

/* +=================================+ */
/* | STUB FUNCTIONS FOR INTERNAL USE | */
/* +=================================+ */

// stubRtlNtStatusToDosError
// -------------------------
// Implemented: 1%

ULONG NTAPI stubRtlNtStatusToDosError(IN NTSTATUS Status)
{
    DWORD dwErrorCode;
    if (Status)
    {
        if (!(Status & 0x20000000))
        {
            if (((Status & 0xFFFF0000) != 0x80070000) && ((Status & 0xFFFF0000) != 0xC0070000))
            {
                if (Status == STATUS_PENDING)
                    dwErrorCode = ERROR_IO_PENDING; // [8)
                else dwErrorCode = ERROR_CALL_NOT_IMPLEMENTED;
            }
            else dwErrorCode = Status & 0xFFFF;
        }
        else dwErrorCode = Status;
    }
    else dwErrorCode = NO_ERROR;
    return dwErrorCode;
}

// stubRtlAnsiStringToUnicodeString
// --------------------------------
// Implemented: 0%

NTSTATUS NTAPI stubRtlAnsiStringToUnicodeString(OUT PUNICODE_STRING DestinationString, IN PANSI_STRING SourceString, IN BOOL AllocateDestinationString)
{
    return STATUS_NOT_IMPLEMENTED;
}

// stubRtlOemStringToUnicodeString
// -------------------------------
// Implemented: 0%

NTSTATUS NTAPI stubRtlOemStringToUnicodeString(OUT PUNICODE_STRING DestinationString, IN PCOEM_STRING SourceString, IN BOOL AllocateDestinationString)
{
    return STATUS_NOT_IMPLEMENTED;
}

// stubRtlUnicodeStringToAnsiString
// --------------------------------
// Implemented: 0%

NTSTATUS NTAPI stubRtlUnicodeStringToAnsiString(OUT PANSI_STRING DestinationString, IN PCUNICODE_STRING SourceString, IN BOOL AllocateDestinationString)
{
    return STATUS_NOT_IMPLEMENTED;
}

// stubRtlUnicodeStringToOemString
// -------------------------------
// Implemented: 0%

NTSTATUS NTAPI stubRtlUnicodeStringToOemString(OUT POEM_STRING DestinationString, IN PCUNICODE_STRING SourceString, IN BOOL AllocateDestinationString)
{
    return STATUS_NOT_IMPLEMENTED;
}

/* +================+ */
/* | STUB FUNCTIONS | */
/* +================+ */

// stubCopyFile2
// -------------
// Implemented: 0%

HRESULT WINAPI stubCopyFile2(IN PCWSTR pwszExistingFileName, IN PCWSTR pwszNewFileName, IN COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return HRESULT_FROM_WIN32(ERROR_CALL_NOT_IMPLEMENTED);
}

// stubCopyFileTransactedA
// -----------------------
// Implemented: 0%

BOOL WINAPI stubCopyFileTransactedA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCopyFileTransactedW
// -----------------------
// Implemented: 0%

BOOL WINAPI stubCopyFileTransactedW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCreateDirectoryTransactedA
// ------------------------------
// Implemented: 0%

BOOL WINAPI stubCreateDirectoryTransactedA(IN LPCSTR lpTemplateDirectory, IN LPCSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCreateDirectoryTransactedW
// ------------------------------
// Implemented: 0%

BOOL WINAPI stubCreateDirectoryTransactedW(IN LPCWSTR lpTemplateDirectory, IN LPCWSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCreateFile2
// ---------------
// Implemented: 0%

HANDLE WINAPI stubCreateFile2(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN DWORD dwCreationDisposition, IN LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCreateFileTransactedA
// -------------------------
// Implemented: 0%

HANDLE WINAPI stubCreateFileTransactedA(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// stubCreateFileTransactedW
// -------------------------
// Implemented: 0%

HANDLE WINAPI stubCreateFileTransactedW(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// stubCreateHardLinkTransactedA
// -----------------------------
// Implemented: 0%

BOOL WINAPI stubCreateHardLinkTransactedA(IN LPCSTR lpFileName, IN LPCSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCreateHardLinkTransactedW
// -----------------------------
// Implemented: 0%

BOOL WINAPI stubCreateHardLinkTransactedW(IN LPCWSTR lpFileName, IN LPCWSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCreateSymbolicLinkA
// -----------------------
// Implemented: 0%

BOOL WINAPI stubCreateSymbolicLinkA (IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCreateSymbolicLinkTransactedA
// ---------------------------------
// Implemented: 0%

BOOL WINAPI stubCreateSymbolicLinkTransactedA(IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCreateSymbolicLinkTransactedW
// ---------------------------------
// Implemented: 0%

BOOL WINAPI stubCreateSymbolicLinkTransactedW(IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubCreateSymbolicLinkW
// -----------------------
// Implemented: 0%

BOOL WINAPI stubCreateSymbolicLinkW (IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubDeleteFileTransactedA
// -------------------------
// Implemented: 0%

BOOL WINAPI stubDeleteFileTransactedA(IN LPCSTR lpFileName, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubDeleteFileTransactedW
// -------------------------
// Implemented: 0%

BOOL WINAPI stubDeleteFileTransactedW(IN LPCWSTR lpFileName, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubFindFirstFileNameTransactedW
// --------------------------------
// Implemented: 0%

HANDLE WINAPI stubFindFirstFileNameTransactedW(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// stubFindFirstFileNameW
// ----------------------
// Implemented: 0%

HANDLE WINAPI stubFindFirstFileNameW(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// stubFindFirstFileTransactedA
// ----------------------------
// Implemented: 0%

HANDLE WINAPI stubFindFirstFileTransactedA(IN LPCSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// stubFindFirstFileTransactedW
// ----------------------------
// Implemented: 0%

HANDLE WINAPI stubFindFirstFileTransactedW(IN LPCWSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// stubFindFirstStreamTransactedW
// ------------------------------
// Implemented: 0%

HANDLE WINAPI stubFindFirstStreamTransactedW(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPVOID lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// stubFindFirstStreamW
// --------------------
// Implemented: 0%

HANDLE WINAPI stubFindFirstStreamW(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPVOID lpFindStreamData, DWORD dwFlags)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_HANDLE_VALUE;
}

// stubFindNextFileNameW
// ---------------------
// Implemented: 0%

BOOL WINAPI stubFindNextFileNameW(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PWSTR LinkName)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubFindNextStreamW
// -------------------
// Implemented: 0%

BOOL WINAPI stubFindNextStreamW(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubGetCompressedFileSizeTransactedA
// ------------------------------------
// Implemented: 0%

DWORD WINAPI stubGetCompressedFileSizeTransactedA(IN LPCSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_FILE_SIZE;
}

// stubGetCompressedFileSizeTransactedW
// ------------------------------------
// Implemented: 0%

DWORD WINAPI stubGetCompressedFileSizeTransactedW(IN LPCWSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return INVALID_FILE_SIZE;
}

// stubGetFileAttributesTransactedA
// --------------------------------
// Implemented: 0%

BOOL WINAPI stubGetFileAttributesTransactedA(IN LPCSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubGetFileAttributesTransactedW
// --------------------------------
// Implemented: 0%

BOOL WINAPI stubGetFileAttributesTransactedW(IN LPCWSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubGetFileSizeEx
// -----------------
// Implemented: 100%

BOOL WINAPI stubGetFileSizeEx(IN HANDLE hFile, OUT PLARGE_INTEGER lpFileSize)
{
    LARGE_INTEGER FileSize;

    FileSize.LowPart = GetFileSize(hFile, (LPDWORD) &FileSize.HighPart);

    if ((FileSize.LowPart == INVALID_FILE_SIZE) && GetLastError()) return FALSE;

    lpFileSize->QuadPart = FileSize.QuadPart;

    return TRUE;
}

// stubGetFinalPathNameByHandleA
// -----------------------------
// Implemented: 0%

DWORD WINAPI stubGetFinalPathNameByHandleA(IN HANDLE hFile, OUT LPSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubGetFinalPathNameByHandleW
// -----------------------------
// Implemented: 0%

DWORD WINAPI stubGetFinalPathNameByHandleW(IN HANDLE hFile, OUT LPWSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubGetFullPathNameTransactedA
// ------------------------------
// Implemented: 0%

DWORD WINAPI stubGetFullPathNameTransactedA(IN LPCSTR lpFileName, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubGetFullPathNameTransactedW
// ------------------------------
// Implemented: 0%

DWORD WINAPI stubGetFullPathNameTransactedW(IN LPCWSTR lpFileName, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubGetLongPathNameTransactedA
// ------------------------------
// Implemented: 0%

DWORD WINAPI stubGetLongPathNameTransactedA(IN LPCSTR lpszShortPath, OUT LPSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubGetLongPathNameTransactedW
// ------------------------------
// Implemented: 0%

DWORD WINAPI stubGetLongPathNameTransactedW(IN LPCWSTR lpszShortPath, OUT LPWSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubGetMappedFileNameA
// ----------------------
// Implemented: 0%

DWORD WINAPI stubGetMappedFileNameA(IN HANDLE hProcess, IN LPVOID lpv, OUT LPSTR lpFilename, IN DWORD nSize)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubGetMappedFileNameW
// ----------------------
// Implemented: 0%

DWORD WINAPI stubGetMappedFileNameW(IN HANDLE hProcess, IN LPVOID lpv, OUT LPWSTR lpFilename, IN DWORD nSize)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubGetModuleFileNameExA
// ------------------------
// Implemented: 0%

DWORD WINAPI stubGetModuleFileNameExA(IN HANDLE hProcess, IN HMODULE hModule, OUT LPSTR lpFilename, IN DWORD nSize)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubGetModuleFileNameExW
// ------------------------
// Implemented: 0%

DWORD WINAPI stubGetModuleFileNameExW(IN HANDLE hProcess, IN HMODULE hModule, OUT LPWSTR lpFilename, IN DWORD nSize)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubMoveFileTransactedA
// -----------------------
// Implemented: 0%

BOOL WINAPI stubMoveFileTransactedA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubMoveFileTransactedW
// -----------------------
// Implemented: 0%

BOOL WINAPI stubMoveFileTransactedW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubMoveFileWithProgressA
// -------------------------
// Implemented: 0%

BOOL WINAPI stubMoveFileWithProgressA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubMoveFileWithProgressW
// -------------------------
// Implemented: 0%

BOOL WINAPI stubMoveFileWithProgressW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubRemoveDirectoryTransactedA
// ------------------------------
// Implemented: 0%

BOOL WINAPI stubRemoveDirectoryTransactedA(IN LPCSTR lpPathName, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubRemoveDirectoryTransactedW
// ------------------------------
// Implemented: 0%

BOOL WINAPI stubRemoveDirectoryTransactedW(IN LPCWSTR lpPathName, IN HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubReplaceFileA
// ----------------
// Implemented: 0%

BOOL WINAPI stubReplaceFileA(IN LPCSTR lpReplacedFileName, IN LPCSTR lpReplacementFileName, IN LPCSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubReplaceFileW
// ----------------
// Implemented: 0%

BOOL WINAPI stubReplaceFileW(IN LPCWSTR lpReplacedFileName, IN LPCWSTR lpReplacementFileName, IN LPCWSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubSearchPathA
// ---------------
// Implemented: 0%

DWORD WINAPI stubSearchPathA(IN LPCSTR lpPath, IN LPCSTR lpFileName, IN LPCSTR lpExtension, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubSearchPathW
// ---------------
// Implemented: 0%

DWORD WINAPI stubSearchPathW(IN LPCWSTR lpPath, IN LPCWSTR lpFileName, IN LPCWSTR lpExtension, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}

// stubSetFileAttributesTransactedA
// --------------------------------
// Implemented: 0%

BOOL WINAPI stubSetFileAttributesTransactedA(IN LPCSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubSetFileAttributesTransactedW
// --------------------------------
// Implemented: 0%

BOOL WINAPI stubSetFileAttributesTransactedW(IN LPCWSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubSetSearchPathMode
// ---------------------
// Implemented: 0%

BOOL WINAPI stubSetSearchPathMode(IN DWORD Flags)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubWow64DisableWow64FsRedirection
// ----------------------------------
// Implemented: 0%

BOOL WINAPI stubWow64DisableWow64FsRedirection(OUT PBOOL pbOldValue)
{
    if (pbOldValue)
        g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);
    else g_pRtlSetLastWin32Error(ERROR_NOACCESS);

    return FALSE;
}

// stubWow64EnableWow64FsRedirection
// ---------------------------------
// Implemented: 0%

BOOL WINAPI stubWow64EnableWow64FsRedirection(IN BOOL bWow64FsEnableRedirection)
{
    g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
}

// stubWow64RevertWow64FsRedirection
// ---------------------------------
// Implemented: 0%

BOOL WINAPI stubWow64RevertWow64FsRedirection(OUT PBOOL pbOldValue)
{
    if (pbOldValue)
        g_pRtlSetLastWin32Error(ERROR_CALL_NOT_IMPLEMENTED);
    else g_pRtlSetLastWin32Error(ERROR_NOACCESS);

    return FALSE;
}

/* +===========+ */
/* | DLL ENTRY | */
/* +===========+ */

// DllMain
// -------
// Implemented: 100%

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            #ifdef NTFS_I30_FILTER
            IsNTFSi30PathCheckEnabled = TRUE;
            #endif

            g_hProcessHeap = GetProcessHeap();

            HMODULE hDynamicLinkLibrary = GetModuleHandle(TEXT("NTDLL.DLL"));

            g_pRtlAnsiStringToUnicodeString = (pRtlAnsiStringToUnicodeString) GetProcAddress(hDynamicLinkLibrary, "RtlAnsiStringToUnicodeString");

            if (!g_pRtlAnsiStringToUnicodeString)
                g_pRtlAnsiStringToUnicodeString = (pRtlAnsiStringToUnicodeString) stubRtlAnsiStringToUnicodeString;

            g_pRtlGetLastWin32Error = (pRtlGetLastWin32Error) GetProcAddress(hDynamicLinkLibrary, "RtlGetLastWin32Error");

            if (!g_pRtlGetLastWin32Error)
                g_pRtlGetLastWin32Error = (pRtlGetLastWin32Error) GetLastError;

            g_pRtlNtStatusToDosError = (pRtlNtStatusToDosError) GetProcAddress(hDynamicLinkLibrary, "RtlNtStatusToDosError");

            if (!g_pRtlNtStatusToDosError)
                g_pRtlNtStatusToDosError = (pRtlNtStatusToDosError) stubRtlNtStatusToDosError;

            g_pRtlOemStringToUnicodeString = (pRtlOemStringToUnicodeString) GetProcAddress(hDynamicLinkLibrary, "RtlOemStringToUnicodeString");

            if (!g_pRtlOemStringToUnicodeString)
                g_pRtlOemStringToUnicodeString = (pRtlOemStringToUnicodeString) stubRtlOemStringToUnicodeString;

            g_pRtlSetLastWin32Error = (pRtlSetLastWin32Error) GetProcAddress(hDynamicLinkLibrary, "RtlSetLastWin32Error");

            if (!g_pRtlSetLastWin32Error)
                g_pRtlSetLastWin32Error = (pRtlSetLastWin32Error) SetLastError;

            g_pRtlUnicodeStringToAnsiString = (pRtlUnicodeStringToAnsiString) GetProcAddress(hDynamicLinkLibrary, "RtlUnicodeStringToAnsiString");

            if (!g_pRtlUnicodeStringToAnsiString)
                g_pRtlUnicodeStringToAnsiString = (pRtlUnicodeStringToAnsiString) stubRtlUnicodeStringToAnsiString;

            g_pRtlUnicodeStringToOemString = (pRtlUnicodeStringToOemString) GetProcAddress(hDynamicLinkLibrary, "RtlUnicodeStringToOemString");

            if (!g_pRtlUnicodeStringToOemString)
                g_pRtlUnicodeStringToOemString = (pRtlUnicodeStringToOemString) stubRtlUnicodeStringToOemString;

            hDynamicLinkLibrary = GetModuleHandle(TEXT("KERNEL32.DLL"));

            g_pCopyFile2 = (pCopyFile2) GetProcAddress(hDynamicLinkLibrary, "CopyFile2");

            if (!g_pCopyFile2)
                g_pCopyFile2 = (pCopyFile2) &stubCopyFile2;

            g_pCopyFileTransactedA = (pCopyFileTransactedA) GetProcAddress(hDynamicLinkLibrary, "CopyFileTransactedA");

            if (!g_pCopyFileTransactedA)
                g_pCopyFileTransactedA = (pCopyFileTransactedA) &stubCopyFileTransactedA;

            g_pCopyFileTransactedW = (pCopyFileTransactedW) GetProcAddress(hDynamicLinkLibrary, "CopyFileTransactedW");

            if (!g_pCopyFileTransactedW)
                g_pCopyFileTransactedW = (pCopyFileTransactedW) &stubCopyFileTransactedW;

            g_pCreateDirectoryTransactedA = (pCreateDirectoryTransactedA) GetProcAddress(hDynamicLinkLibrary, "CreateDirectoryTransactedA");

            if (!g_pCreateDirectoryTransactedA)
                g_pCreateDirectoryTransactedA = (pCreateDirectoryTransactedA) &stubCreateDirectoryTransactedA;

            g_pCreateDirectoryTransactedW = (pCreateDirectoryTransactedW) GetProcAddress(hDynamicLinkLibrary, "CreateDirectoryTransactedW");

            if (!g_pCreateDirectoryTransactedW)
                g_pCreateDirectoryTransactedW = (pCreateDirectoryTransactedW) &stubCreateDirectoryTransactedW;

            g_pCreateFile2 = (pCreateFile2) GetProcAddress(hDynamicLinkLibrary, "CreateFile2");

            if (!g_pCreateFile2)
                g_pCreateFile2 = (pCreateFile2) &stubCreateFile2;

            g_pCreateFileTransactedA = (pCreateFileTransactedA) GetProcAddress(hDynamicLinkLibrary, "CreateFileTransactedA");

            if (!g_pCreateFileTransactedA)
                g_pCreateFileTransactedA = (pCreateFileTransactedA) &stubCreateFileTransactedA;

            g_pCreateFileTransactedW = (pCreateFileTransactedW) GetProcAddress(hDynamicLinkLibrary, "CreateFileTransactedW");

            if (!g_pCreateFileTransactedW)
                g_pCreateFileTransactedW = (pCreateFileTransactedW) &stubCreateFileTransactedW;

            g_pCreateHardLinkTransactedA = (pCreateHardLinkTransactedA) GetProcAddress(hDynamicLinkLibrary, "CreateHardLinkTransactedA");

            if (!g_pCreateHardLinkTransactedA)
                g_pCreateHardLinkTransactedA = (pCreateHardLinkTransactedA) &stubCreateHardLinkTransactedA;

            g_pCreateHardLinkTransactedW = (pCreateHardLinkTransactedW) GetProcAddress(hDynamicLinkLibrary, "CreateHardLinkTransactedW");

            if (!g_pCreateHardLinkTransactedW)
                g_pCreateHardLinkTransactedW = (pCreateHardLinkTransactedW) &stubCreateHardLinkTransactedW;

            g_pCreateSymbolicLinkTransactedA = (pCreateSymbolicLinkTransactedA) GetProcAddress(hDynamicLinkLibrary, "CreateSymbolicLinkTransactedA");

            if (!g_pCreateSymbolicLinkTransactedA)
                g_pCreateSymbolicLinkTransactedA = (pCreateSymbolicLinkTransactedA) &stubCreateSymbolicLinkTransactedA;

            g_pCreateSymbolicLinkTransactedW = (pCreateSymbolicLinkTransactedW) GetProcAddress(hDynamicLinkLibrary, "CreateSymbolicLinkTransactedW");

            if (!g_pCreateSymbolicLinkTransactedW)
                g_pCreateSymbolicLinkTransactedW = (pCreateSymbolicLinkTransactedW) &stubCreateSymbolicLinkTransactedW;

            g_pCreateSymbolicLinkA = (pCreateSymbolicLinkA) GetProcAddress(hDynamicLinkLibrary, "CreateSymbolicLinkA");

            if (!g_pCreateSymbolicLinkA)
                g_pCreateSymbolicLinkA = (pCreateSymbolicLinkA) &stubCreateSymbolicLinkA;

            g_pCreateSymbolicLinkW = (pCreateSymbolicLinkW) GetProcAddress(hDynamicLinkLibrary, "CreateSymbolicLinkW");

            if (!g_pCreateSymbolicLinkW)
                g_pCreateSymbolicLinkW = (pCreateSymbolicLinkW) &stubCreateSymbolicLinkW;

            g_pDeleteFileTransactedA = (pDeleteFileTransactedA) GetProcAddress(hDynamicLinkLibrary, "DeleteFileTransactedA");

            if (!g_pDeleteFileTransactedA)
                g_pDeleteFileTransactedA = (pDeleteFileTransactedA) &stubDeleteFileTransactedA;

            g_pDeleteFileTransactedW = (pDeleteFileTransactedW) GetProcAddress(hDynamicLinkLibrary, "DeleteFileTransactedW");

            if (!g_pDeleteFileTransactedW)
                g_pDeleteFileTransactedW = (pDeleteFileTransactedW) &stubDeleteFileTransactedW;

            g_pFindFirstFileNameTransactedW = (pFindFirstFileNameTransactedW) GetProcAddress(hDynamicLinkLibrary, "FindFirstFileNameTransactedW");

            if (!g_pFindFirstFileNameTransactedW)
                g_pFindFirstFileNameTransactedW = (pFindFirstFileNameTransactedW) &stubFindFirstFileNameTransactedW;

            g_pFindFirstFileNameW = (pFindFirstFileNameW) GetProcAddress(hDynamicLinkLibrary, "FindFirstFileNameW");

            if (!g_pFindFirstFileNameW)
                g_pFindFirstFileNameW = (pFindFirstFileNameW) &stubFindFirstFileNameW;

            g_pFindFirstFileTransactedA = (pFindFirstFileTransactedA) GetProcAddress(hDynamicLinkLibrary, "FindFirstFileTransactedA");

            if (!g_pFindFirstFileTransactedA)
                g_pFindFirstFileTransactedA = (pFindFirstFileTransactedA) &stubFindFirstFileTransactedA;

            g_pFindFirstFileTransactedW = (pFindFirstFileTransactedW) GetProcAddress(hDynamicLinkLibrary, "FindFirstFileTransactedW");

            if (!g_pFindFirstFileTransactedW)
                g_pFindFirstFileTransactedW = (pFindFirstFileTransactedW) &stubFindFirstFileTransactedW;

            g_pFindFirstStreamTransactedW = (pFindFirstStreamTransactedW) GetProcAddress(hDynamicLinkLibrary, "FindFirstStreamTransactedW");

            if (!g_pFindFirstStreamTransactedW)
                g_pFindFirstStreamTransactedW = (pFindFirstStreamTransactedW) &stubFindFirstStreamTransactedW;

            g_pFindFirstStreamW = (pFindFirstStreamW) GetProcAddress(hDynamicLinkLibrary, "FindFirstStreamW");

            if (!g_pFindFirstStreamW)
                g_pFindFirstStreamW = (pFindFirstStreamW) &stubFindFirstStreamW;

            g_pFindNextFileNameW = (pFindNextFileNameW) GetProcAddress(hDynamicLinkLibrary, "FindNextFileNameW");

            if (!g_pFindNextFileNameW)
                g_pFindNextFileNameW = (pFindNextFileNameW) &stubFindNextFileNameW;

            g_pFindNextStreamW = (pFindNextStreamW) GetProcAddress(hDynamicLinkLibrary, "FindNextStreamW");

            if (!g_pFindNextStreamW)
                g_pFindNextStreamW = (pFindNextStreamW) &stubFindNextStreamW;

            g_pGetCompressedFileSizeTransactedA = (pGetCompressedFileSizeTransactedA) GetProcAddress(hDynamicLinkLibrary, "GetCompressedFileSizeTransactedA");

            if (!g_pGetCompressedFileSizeTransactedA)
                g_pGetCompressedFileSizeTransactedA = (pGetCompressedFileSizeTransactedA) &stubGetCompressedFileSizeTransactedA;

            g_pGetCompressedFileSizeTransactedW = (pGetCompressedFileSizeTransactedW) GetProcAddress(hDynamicLinkLibrary, "GetCompressedFileSizeTransactedW");

            if (!g_pGetCompressedFileSizeTransactedW)
                g_pGetCompressedFileSizeTransactedW = (pGetCompressedFileSizeTransactedW) &stubGetCompressedFileSizeTransactedW;

            g_pGetFileAttributesTransactedA = (pGetFileAttributesTransactedA) GetProcAddress(hDynamicLinkLibrary, "GetFileAttributesTransactedA");

            if (!g_pGetFileAttributesTransactedA)
                g_pGetFileAttributesTransactedA = (pGetFileAttributesTransactedA) &stubGetFileAttributesTransactedA;

            g_pGetFileAttributesTransactedW = (pGetFileAttributesTransactedW) GetProcAddress(hDynamicLinkLibrary, "GetFileAttributesTransactedW");

            if (!g_pGetFileAttributesTransactedW)
                g_pGetFileAttributesTransactedW = (pGetFileAttributesTransactedW) &stubGetFileAttributesTransactedW;

            g_pGetFileSizeEx = (pGetFileSizeEx) GetProcAddress(hDynamicLinkLibrary, "GetFileSizeEx");

            if (!g_pGetFileSizeEx)
                g_pGetFileSizeEx = (pGetFileSizeEx) &stubGetFileSizeEx;

            g_pGetFinalPathNameByHandleA = (pGetFinalPathNameByHandleA) GetProcAddress(hDynamicLinkLibrary, "GetFinalPathNameByHandleA");

            if (!g_pGetFinalPathNameByHandleA)
                g_pGetFinalPathNameByHandleA = (pGetFinalPathNameByHandleA) &stubGetFinalPathNameByHandleA;

            g_pGetFinalPathNameByHandleW = (pGetFinalPathNameByHandleW) GetProcAddress(hDynamicLinkLibrary, "GetFinalPathNameByHandleW");

            if (!g_pGetFinalPathNameByHandleW)
                g_pGetFinalPathNameByHandleW = (pGetFinalPathNameByHandleW) &stubGetFinalPathNameByHandleW;

            g_pGetFullPathNameTransactedA = (pGetFullPathNameTransactedA) GetProcAddress(hDynamicLinkLibrary, "GetFullPathNameTransactedA");

            if (!g_pGetFullPathNameTransactedA)
                g_pGetFullPathNameTransactedA = (pGetFullPathNameTransactedA) &stubGetFullPathNameTransactedA;

            g_pGetFullPathNameTransactedW = (pGetFullPathNameTransactedW) GetProcAddress(hDynamicLinkLibrary, "GetFullPathNameTransactedW");

            if (!g_pGetFullPathNameTransactedW)
                g_pGetFullPathNameTransactedW = (pGetFullPathNameTransactedW) &stubGetFullPathNameTransactedW;

            g_pGetLongPathNameTransactedA = (pGetLongPathNameTransactedA) GetProcAddress(hDynamicLinkLibrary, "GetLongPathNameTransactedA");

            if (!g_pGetLongPathNameTransactedA)
                g_pGetLongPathNameTransactedA = (pGetLongPathNameTransactedA) &stubGetLongPathNameTransactedA;

            g_pGetLongPathNameTransactedW = (pGetLongPathNameTransactedW) GetProcAddress(hDynamicLinkLibrary, "GetLongPathNameTransactedW");

            if (!g_pGetLongPathNameTransactedW)
                g_pGetLongPathNameTransactedW = (pGetLongPathNameTransactedW) &stubGetLongPathNameTransactedW;

            g_pMoveFileTransactedA = (pMoveFileTransactedA) GetProcAddress(hDynamicLinkLibrary, "MoveFileTransactedA");

            if (!g_pMoveFileTransactedA)
                g_pMoveFileTransactedA = (pMoveFileTransactedA) &stubMoveFileTransactedA;

            g_pMoveFileTransactedW = (pMoveFileTransactedW) GetProcAddress(hDynamicLinkLibrary, "MoveFileTransactedW");

            if (!g_pMoveFileTransactedW)
                g_pMoveFileTransactedW = (pMoveFileTransactedW) &stubMoveFileTransactedW;

            g_pMoveFileWithProgressA = (pMoveFileWithProgressA) GetProcAddress(hDynamicLinkLibrary, "MoveFileWithProgressA");

            if (!g_pMoveFileWithProgressA)
                g_pMoveFileWithProgressA = (pMoveFileWithProgressA) &stubMoveFileWithProgressA;

            g_pMoveFileWithProgressW = (pMoveFileWithProgressW) GetProcAddress(hDynamicLinkLibrary, "MoveFileWithProgressW");

            if (!g_pMoveFileWithProgressW)
                g_pMoveFileWithProgressW = (pMoveFileWithProgressW) &stubMoveFileWithProgressW;

            g_pRemoveDirectoryTransactedA = (pRemoveDirectoryTransactedA) GetProcAddress(hDynamicLinkLibrary, "RemoveDirectoryTransactedA");

            if (!g_pRemoveDirectoryTransactedA)
                g_pRemoveDirectoryTransactedA = (pRemoveDirectoryTransactedA) &stubRemoveDirectoryTransactedA;

            g_pRemoveDirectoryTransactedW = (pRemoveDirectoryTransactedW) GetProcAddress(hDynamicLinkLibrary, "RemoveDirectoryTransactedW");

            if (!g_pRemoveDirectoryTransactedW)
                g_pRemoveDirectoryTransactedW = (pRemoveDirectoryTransactedW) &stubRemoveDirectoryTransactedW;

            g_pReplaceFileA = (pReplaceFileA) GetProcAddress(hDynamicLinkLibrary, "ReplaceFileA");

            if (!g_pReplaceFileA)
                g_pReplaceFileA = (pReplaceFileA) &stubReplaceFileA;

            g_pReplaceFileW = (pReplaceFileW) GetProcAddress(hDynamicLinkLibrary, "ReplaceFileW");

            if (!g_pReplaceFileW)
                g_pReplaceFileW = (pReplaceFileW) &stubReplaceFileW;

            g_pSearchPathA = (pSearchPathA) GetProcAddress(hDynamicLinkLibrary, "SearchPathA");

            if (!g_pSearchPathA)
                g_pSearchPathA = (pSearchPathA) &stubSearchPathA;

            g_pSearchPathW = (pSearchPathW) GetProcAddress(hDynamicLinkLibrary, "SearchPathW");

            if (!g_pSearchPathW)
                g_pSearchPathW = (pSearchPathW) &stubSearchPathW;

            g_pSetFileAttributesTransactedA = (pSetFileAttributesTransactedA) GetProcAddress(hDynamicLinkLibrary, "SetFileAttributesTransactedA");

            if (!g_pSetFileAttributesTransactedA)
                g_pSetFileAttributesTransactedA = (pSetFileAttributesTransactedA) &stubSetFileAttributesTransactedA;

            g_pSetFileAttributesTransactedW = (pSetFileAttributesTransactedW) GetProcAddress(hDynamicLinkLibrary, "SetFileAttributesTransactedW");

            if (!g_pSetFileAttributesTransactedW)
                g_pSetFileAttributesTransactedW = (pSetFileAttributesTransactedW) &stubSetFileAttributesTransactedW;

            g_pSetSearchPathMode = (pSetSearchPathMode) GetProcAddress(hDynamicLinkLibrary, "SetSearchPathMode");

            if (!g_pSetSearchPathMode)
                g_pSetSearchPathMode = (pSetSearchPathMode) &stubSetSearchPathMode;

            g_pWow64DisableWow64FsRedirection = (pWow64DisableWow64FsRedirection) GetProcAddress(hDynamicLinkLibrary, "Wow64DisableWow64FsRedirection");

            if (!g_pWow64DisableWow64FsRedirection)
                g_pWow64DisableWow64FsRedirection = (pWow64DisableWow64FsRedirection) &stubWow64DisableWow64FsRedirection;

            g_pWow64EnableWow64FsRedirection = (pWow64EnableWow64FsRedirection) GetProcAddress(hDynamicLinkLibrary, "Wow64EnableWow64FsRedirection");

            if (!g_pWow64EnableWow64FsRedirection)
                g_pWow64EnableWow64FsRedirection = (pWow64EnableWow64FsRedirection) &stubWow64EnableWow64FsRedirection;

            g_pWow64RevertWow64FsRedirection = (pWow64RevertWow64FsRedirection) GetProcAddress(hDynamicLinkLibrary, "Wow64RevertWow64FsRedirection");

            if (!g_pWow64RevertWow64FsRedirection)
                g_pWow64RevertWow64FsRedirection = (pWow64RevertWow64FsRedirection) &stubWow64RevertWow64FsRedirection;

            break;
        }

        case DLL_PROCESS_DETACH:
        {
            if (IsPSAPIModuleManuallyLoaded && g_hModulePSAPI)
                FreeLibrary(g_hModulePSAPI);
            break;
        }
    }
    return TRUE;
}
