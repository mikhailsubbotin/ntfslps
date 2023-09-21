/* +============================================================================================+ */
/* | ##    ## ######## ######## ######## ##       ######## ########          © Mikhail Subbotin | */
/* | ###   ##    ##    ##       ##    ## ##       ##    ## ##    ##                             | */
/* | ####  ##    ##    ##       ##       ##       ##    ## ##                                   | */
/* | ## ## ##    ##    ######## ######## ##       ######## ########                             | */
/* | ##  ####    ##    ##             ## ##       ##             ##                             | */
/* | ##   ###    ##    ##       ##    ## ##       ##       ##    ##                             | */
/* | ##    ##    ##    ##       ######## ######## ##       ########                             | */
/* +============================================================================================+ */
/* | ntfslps.h                                                                                  | */
/* +--------------------------------------------------------------------------------------------+ */

#ifndef __NTFSLPS_H__
#define __NTFSLPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

#include <stddef.h>
#include <strsafe.h>
#include <windows.h>
#include <winternl.h>

#define NTFSLPS_MAX_FS_PATH_LENGTH 32767
#define NTFSLPS_FS_PATH_MEMORY_LENGTH (NTFSLPS_MAX_FS_PATH_LENGTH + 1) * sizeof(TCHAR)

#define FS_PATH_MEMORY_LENGTH NTFSLPS_FS_PATH_MEMORY_LENGTH
#define MAX_FS_PATH_LENGTH NTFSLPS_MAX_FS_PATH_LENGTH

#define DFS_SUCCESS 0
#define DFS_PARTIALLY_MISSED 1
#define DFS_ERROR -1

typedef struct _WIN32_FIND_STREAM_DATAA {
    LARGE_INTEGER StreamSize;
    CHAR cStreamName[MAX_PATH + 36];
} WIN32_FIND_STREAM_DATAA, *PWIN32_FIND_STREAM_DATAA, *LPWIN32_FIND_STREAM_DATAA;

typedef struct _WIN32_FIND_STREAM_DATAW {
    LARGE_INTEGER StreamSize;
    WCHAR cStreamName[MAX_PATH + 36];
} WIN32_FIND_STREAM_DATAW, *PWIN32_FIND_STREAM_DATAW, *LPWIN32_FIND_STREAM_DATAW;

#ifdef NTFSLPS_EXPORTS
#define NTFSLPSAPI
#else
#define NTFSLPSAPI __declspec(dllimport)
#endif

NTFSLPSAPI BOOL WINAPI NTFSLPS_AreFileApisANSI();
NTFSLPSAPI HRESULT WINAPI NTFSLPS_CopyFile2(IN PCWSTR pwszExistingFileName, IN PCWSTR pwszNewFileName, IN COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CopyFileA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN BOOL bFailIfExists);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CopyFileExA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CopyFileExW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CopyFileTransactedA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CopyFileTransactedW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CopyFileW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN BOOL bFailIfExists);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateDirectoryA(IN LPCSTR lpPathName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateDirectoryExA(IN LPCSTR lpTemplateDirectory, IN LPCSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateDirectoryExW(IN LPCWSTR lpTemplateDirectory, IN LPCWSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateDirectoryTransactedA(IN LPCSTR lpTemplateDirectory, IN LPCSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateDirectoryTransactedW(IN LPCWSTR lpTemplateDirectory, IN LPCWSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateDirectoryW(IN LPCWSTR lpPathName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_CreateFile2(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN DWORD dwCreationDisposition, IN LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_CreateFileA(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile);
NTFSLPSAPI DWORD WINAPI NTFSLPS_CreateFileBackupA(IN LPCSTR lpExistingFileName);
NTFSLPSAPI UINT WINAPI NTFSLPS_CreateFileBackupExA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN BOOL bNumerate);
NTFSLPSAPI UINT WINAPI NTFSLPS_CreateFileBackupExW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN BOOL bNumerate);
NTFSLPSAPI DWORD WINAPI NTFSLPS_CreateFileBackupW(IN LPCWSTR lpExistingFileName);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_CreateFileTransactedA(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_CreateFileTransactedW(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_CreateFileW(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateFolderA(IN LPCSTR lpPathName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateFolderW(IN LPCWSTR lpPathName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateHardLinkA(IN LPCSTR lpFileName, IN LPCSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateHardLinkTransactedA(IN LPCSTR lpFileName, IN LPCSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateHardLinkTransactedW(IN LPCWSTR lpFileName, IN LPCWSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateHardLinkW(IN LPCWSTR lpFileName, IN LPCWSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateSymbolicLinkA(IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateSymbolicLinkTransactedA(IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateSymbolicLinkTransactedW(IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_CreateSymbolicLinkW(IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags);
NTFSLPSAPI BOOL WINAPI NTFSLPS_DeleteFileA(IN LPCSTR lpFileName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_DeleteFileTransactedA(IN LPCSTR lpFileName, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_DeleteFileTransactedW(IN LPCWSTR lpFileName, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_DeleteFileW(IN LPCWSTR lpFileName);
NTFSLPSAPI UINT WINAPI NTFSLPS_DeleteFolderA(IN LPCSTR lpPathName);
NTFSLPSAPI UINT WINAPI NTFSLPS_DeleteFolderW(IN LPCWSTR lpPathName);
NTFSLPSAPI DWORD WINAPI NTFSLPS_DumpToFileA(IN LPCSTR lpFileName, IN LPVOID lpMemory, IN UINT nLength, OUT LPUINT lpNumberOfBytesProcessed);
NTFSLPSAPI DWORD WINAPI NTFSLPS_DumpToFileW(IN LPCWSTR lpFileName, IN LPVOID lpMemory, IN UINT nLength, OUT LPUINT lpNumberOfBytesProcessed);
NTFSLPSAPI BOOL WINAPI NTFSLPS_FindClose(IN OUT HANDLE hFindFile);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileA(IN LPCSTR lpFileName, OUT LPWIN32_FIND_DATAA lpFindFileData);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileExA(IN LPCSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAA lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileExW(IN LPCWSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAW lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileNameA(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileNameTransactedA(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName, IN HANDLE hTransaction);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileNameTransactedW(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName, IN HANDLE hTransaction);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileNameW(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileTransactedA(IN LPCSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAA lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileTransactedW(IN LPCWSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAW lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstFileW(IN LPCWSTR lpFileName, OUT LPWIN32_FIND_DATAW lpFindFileData);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstStreamA(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData, DWORD dwFlags);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstStreamTransactedA(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstStreamTransactedW(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_FindFirstStreamW(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData, DWORD dwFlags);
NTFSLPSAPI BOOL WINAPI NTFSLPS_FindNextFileA(IN HANDLE hFindFile, OUT LPWIN32_FIND_DATAA lpFindFileData);
NTFSLPSAPI BOOL WINAPI NTFSLPS_FindNextFileNameA(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PSTR LinkName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_FindNextFileNameW(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PWSTR LinkName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_FindNextFileW(IN HANDLE hFindFile, OUT LPWIN32_FIND_DATAW lpFindFileData);
NTFSLPSAPI BOOL WINAPI NTFSLPS_FindNextStreamA(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData);
NTFSLPSAPI BOOL WINAPI NTFSLPS_FindNextStreamW(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetCompressedFileSizeA(IN LPCSTR lpFileName, OUT LPDWORD lpFileSizeHigh);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetCompressedFileSizeTransactedA(IN LPCSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetCompressedFileSizeTransactedW(IN LPCWSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetCompressedFileSizeW(IN LPCWSTR lpFileName, OUT LPDWORD lpFileSizeHigh);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetCurrentDirectoryA(IN DWORD nBufferLength, OUT LPSTR lpBuffer);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetCurrentDirectoryW(IN DWORD nBufferLength, OUT LPWSTR lpBuffer);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFileAttributesA(IN LPCSTR lpFileName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_GetFileAttributesExA(IN LPCSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation);
NTFSLPSAPI BOOL WINAPI NTFSLPS_GetFileAttributesExW(IN LPCWSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation);
NTFSLPSAPI BOOL WINAPI NTFSLPS_GetFileAttributesTransactedA(IN LPCSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_GetFileAttributesTransactedW(IN LPCWSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFileAttributesW(IN LPCWSTR lpFileName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_GetFileSizeEx(IN HANDLE hFile, OUT PLARGE_INTEGER lpFileSize);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFinalPathNameByHandleA(IN HANDLE hFile, OUT LPSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFinalPathNameByHandleW(IN HANDLE hFile, OUT LPWSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFullPathA(IN LPCSTR lpObjectPath, OUT LPSTR lpBuffer, IN DWORD nBufferLength);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFullPathNameA(IN LPCSTR lpFileName, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFullPathNameTransactedA(IN LPCSTR lpFileName, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart, IN HANDLE hTransaction);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFullPathNameTransactedW(IN LPCWSTR lpFileName, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart, IN HANDLE hTransaction);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFullPathNameW(IN LPCWSTR lpFileName, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetFullPathW(IN LPCWSTR lpObjectPath, OUT LPWSTR lpBuffer, IN DWORD nBufferLength);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetLongPathNameA(IN LPCSTR lpszShortPath, OUT LPSTR lpszLongPath, IN DWORD cchBuffer);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetLongPathNameTransactedA(IN LPCSTR lpszShortPath, OUT LPSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetLongPathNameTransactedW(IN LPCWSTR lpszShortPath, OUT LPWSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetLongPathNameW(IN LPCWSTR lpszShortPath, OUT LPWSTR lpszLongPath, IN DWORD cchBuffer);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetMappedFileNameA(IN HANDLE hProcess, IN LPVOID lpv, OUT LPSTR lpFilename, IN DWORD nSize);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetMappedFileNameW(IN HANDLE hProcess, IN LPVOID lpv, OUT LPWSTR lpFilename, IN DWORD nSize);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetModuleFileNameA(IN HMODULE hModule, OUT LPSTR lpFilename, IN DWORD nSize);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetModuleFileNameExA(IN HANDLE hProcess, IN HMODULE hModule, OUT LPSTR lpFilename, IN DWORD nSize);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetModuleFileNameExW(IN HANDLE hProcess, IN HMODULE hModule, OUT LPWSTR lpFilename, IN DWORD nSize);
NTFSLPSAPI DWORD WINAPI NTFSLPS_GetModuleFileNameW(IN HMODULE hModule, OUT LPWSTR lpFilename, IN DWORD nSize);
NTFSLPSAPI UINT WINAPI NTFSLPS_GetSystemDirectoryA(OUT LPSTR lpBuffer, IN UINT uSize);
NTFSLPSAPI UINT WINAPI NTFSLPS_GetSystemDirectoryW(OUT LPWSTR lpBuffer, IN UINT uSize);
NTFSLPSAPI UINT WINAPI NTFSLPS_GetSystemWindowsDirectoryA(OUT LPSTR lpBuffer, IN UINT uSize);
NTFSLPSAPI UINT WINAPI NTFSLPS_GetSystemWindowsDirectoryW(OUT LPWSTR lpBuffer, IN UINT uSize);
NTFSLPSAPI UINT WINAPI NTFSLPS_GetWindowsDirectoryA(OUT LPSTR lpBuffer, IN UINT uSize);
NTFSLPSAPI UINT WINAPI NTFSLPS_GetWindowsDirectoryW(OUT LPWSTR lpBuffer, IN UINT uSize);
NTFSLPSAPI BOOL WINAPI NTFSLPS_IsPathExistA(LPCSTR lpObjectPath);
NTFSLPSAPI BOOL WINAPI NTFSLPS_IsPathExistW(LPCWSTR lpObjectPath);
NTFSLPSAPI BOOL WINAPI NTFSLPS_MoveFileA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_MoveFileExA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN DWORD dwFlags);
NTFSLPSAPI BOOL WINAPI NTFSLPS_MoveFileExW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN DWORD dwFlags);
NTFSLPSAPI BOOL WINAPI NTFSLPS_MoveFileTransactedA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_MoveFileTransactedW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_MoveFileW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_MoveFileWithProgressA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags);
NTFSLPSAPI BOOL WINAPI NTFSLPS_MoveFileWithProgressW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_NewFileA(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN BOOL bCreateAlways, IN DWORD dwFlagsAndAttributes);
NTFSLPSAPI HANDLE WINAPI NTFSLPS_NewFileW(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN BOOL bCreateAlways, IN DWORD dwFlagsAndAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_RemoveDirectoryA(IN LPCSTR lpPathName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_RemoveDirectoryTransactedA(IN LPCSTR lpPathName, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_RemoveDirectoryTransactedW(IN LPCWSTR lpPathName, IN HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_RemoveDirectoryW(IN LPCWSTR lpPathName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_ReplaceFileA(IN LPCSTR lpReplacedFileName, IN LPCSTR lpReplacementFileName, IN LPCSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved);
NTFSLPSAPI DWORD WINAPI NTFSLPS_ReplaceFilePathExtensionA(IN OUT LPSTR lpObjectPath, IN DWORD nSize, LPCSTR lpExtension);
NTFSLPSAPI DWORD WINAPI NTFSLPS_ReplaceFilePathExtensionW(IN OUT LPWSTR lpObjectPath, IN DWORD nSize, LPCWSTR lpExtension);
NTFSLPSAPI BOOL WINAPI NTFSLPS_ReplaceFileW(IN LPCWSTR lpReplacedFileName, IN LPCWSTR lpReplacementFileName, IN LPCWSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved);
NTFSLPSAPI DWORD WINAPI NTFSLPS_SearchPathA(IN LPCSTR lpPath, IN LPCSTR lpFileName, IN LPCSTR lpExtension, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart);
NTFSLPSAPI DWORD WINAPI NTFSLPS_SearchPathW(IN LPCWSTR lpPath, IN LPCWSTR lpFileName, IN LPCWSTR lpExtension, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart);
NTFSLPSAPI BOOL WINAPI NTFSLPS_SetCurrentDirectoryA(IN LPCSTR lpPathName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_SetCurrentDirectoryW(IN LPCWSTR lpPathName);
NTFSLPSAPI VOID WINAPI NTFSLPS_SetFileApisToANSI();
NTFSLPSAPI VOID WINAPI NTFSLPS_SetFileApisToOEM();
NTFSLPSAPI BOOL WINAPI NTFSLPS_SetFileAttributesA(IN LPCSTR lpFileName, IN DWORD dwFileAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_SetFileAttributesTransactedA(IN LPCSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_SetFileAttributesTransactedW(IN LPCWSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction);
NTFSLPSAPI BOOL WINAPI NTFSLPS_SetFileAttributesW(IN LPCWSTR lpFileName, IN DWORD dwFileAttributes);
NTFSLPSAPI BOOL WINAPI NTFSLPS_SetSearchPathMode(IN DWORD Flags);
NTFSLPSAPI BOOL WINAPI NTFSLPS_Wow64DisableWow64FsRedirection(OUT PBOOL pbOldValue);
NTFSLPSAPI BOOL WINAPI NTFSLPS_Wow64EnableWow64FsRedirection(IN BOOL bWow64FsEnableRedirection);
NTFSLPSAPI BOOL WINAPI NTFSLPS_Wow64RevertWow64FsRedirection(OUT PBOOL pbOldValue);
NTFSLPSAPI HFILE WINAPI NTFSLPS_lcreat(IN LPCSTR lpPathName, IN INT iAttribute);
NTFSLPSAPI HFILE WINAPI NTFSLPS_lopen(IN LPCSTR lpPathName, IN INT iReadWrite);
NTFSLPSAPI int __cdecl NTFSLPS_mkdir(IN const char *dirname);
NTFSLPSAPI int __cdecl NTFSLPS_remove(IN const char *filename);
NTFSLPSAPI int __cdecl NTFSLPS_rmdir(const char *path);
NTFSLPSAPI int __cdecl NTFSLPS_wmkdir(IN const wchar_t *dirname);
NTFSLPSAPI int __cdecl NTFSLPS_wremove(IN const wchar_t *filename);
NTFSLPSAPI int __cdecl NTFSLPS_wrmdir(const wchar_t *path);

#ifdef UNICODE
#define NTFSLPS_CopyFile NTFSLPS_CopyFileW
#define NTFSLPS_CopyFileEx NTFSLPS_CopyFileExW
#define NTFSLPS_CopyFileTransacted NTFSLPS_CopyFileTransactedW
#define NTFSLPS_CreateDirectoryTransacted NTFSLPS_CreateDirectoryTransactedW
#define NTFSLPS_CreateDirectoryEx NTFSLPS_CreateDirectoryExW
#define NTFSLPS_CreateDirectory NTFSLPS_CreateDirectoryW
#define NTFSLPS_CreateFile NTFSLPS_CreateFileW
#define NTFSLPS_CreateFileBackup NTFSLPS_CreateFileBackupW
#define NTFSLPS_CreateFileBackupEx NTFSLPS_CreateFileBackupExW
#define NTFSLPS_CreateFileTransacted NTFSLPS_CreateFileTransactedW
#define NTFSLPS_CreateFolder NTFSLPS_CreateFolderW
#define NTFSLPS_CreateHardLink NTFSLPS_CreateHardLinkW
#define NTFSLPS_CreateHardLinkTransacted NTFSLPS_CreateHardLinkTransactedW
#define NTFSLPS_CreateSymbolicLink NTFSLPS_CreateSymbolicLinkW
#define NTFSLPS_CreateSymbolicLinkTransacted NTFSLPS_CreateSymbolicLinkTransactedW
#define NTFSLPS_DeleteFile NTFSLPS_DeleteFileW
#define NTFSLPS_DeleteFileTransacted NTFSLPS_DeleteFileTransactedW
#define NTFSLPS_DeleteFolder NTFSLPS_DeleteFolderW
#define NTFSLPS_DumpToFile NTFSLPS_DumpToFileW
#define NTFSLPS_FindFirstFile NTFSLPS_FindFirstFileW
#define NTFSLPS_FindFirstFileEx NTFSLPS_FindFirstFileExW
#define NTFSLPS_FindFirstFileName NTFSLPS_FindFirstFileNameW
#define NTFSLPS_FindFirstFileNameTransacted NTFSLPS_FindFirstFileNameTransactedW
#define NTFSLPS_FindFirstStream NTFSLPS_FindFirstStreamW
#define NTFSLPS_FindFirstStreamTransacted NTFSLPS_FindFirstStreamTransactedW
#define NTFSLPS_FindNextFile NTFSLPS_FindNextFileW
#define NTFSLPS_FindNextFileName NTFSLPS_FindNextFileNameW
#define NTFSLPS_FindNextStream NTFSLPS_FindNextStreamW
#define NTFSLPS_GetCompressedFileSize NTFSLPS_GetCompressedFileSizeW
#define NTFSLPS_GetCompressedFileSizeTransacted NTFSLPS_GetCompressedFileSizeTransactedW
#define NTFSLPS_GetCurrentDirectory NTFSLPS_GetCurrentDirectoryW
#define NTFSLPS_GetFileAttributes NTFSLPS_GetFileAttributesW
#define NTFSLPS_GetFileAttributesEx NTFSLPS_GetFileAttributesExW
#define NTFSLPS_GetFileAttributesTransacted NTFSLPS_GetFileAttributesTransactedW
#define NTFSLPS_GetFileSystemObjectAttributes NTFSLPS_GetFileSystemObjectAttributesW
#define NTFSLPS_GetFinalPathNameByHandle NTFSLPS_GetFinalPathNameByHandleW
#define NTFSLPS_GetFullPath NTFSLPS_GetFullPathW
#define NTFSLPS_GetFullPathName NTFSLPS_GetFullPathNameW
#define NTFSLPS_GetFullPathNameTransacted NTFSLPS_GetFullPathNameTransactedW
#define NTFSLPS_GetLongPathName NTFSLPS_GetLongPathNameW
#define NTFSLPS_GetLongPathNameTransacted NTFSLPS_GetLongPathNameTransactedW
#define NTFSLPS_GetMappedFileName NTFSLPS_GetMappedFileNameW
#define NTFSLPS_GetModuleFileName NTFSLPS_GetModuleFileNameW
#define NTFSLPS_GetModuleFileNameEx NTFSLPS_GetModuleFileNameExW
#define NTFSLPS_GetSystemDirectory NTFSLPS_GetSystemDirectoryW
#define NTFSLPS_GetSystemWindowsDirectory NTFSLPS_GetSystemWindowsDirectoryW
#define NTFSLPS_GetWindowsDirectory NTFSLPS_GetWindowsDirectoryW
#define NTFSLPS_IsPathExist NTFSLPS_IsPathExistW
#define NTFSLPS_MoveFile NTFSLPS_MoveFileW
#define NTFSLPS_MoveFileEx NTFSLPS_MoveFileExW
#define NTFSLPS_MoveFileTransacted NTFSLPS_MoveFileTransactedW
#define NTFSLPS_MoveFileWithProgress NTFSLPS_MoveFileWithProgressW
#define NTFSLPS_NewFile NTFSLPS_NewFileW
#define NTFSLPS_RemoveDirectory NTFSLPS_RemoveDirectoryW
#define NTFSLPS_RemoveDirectoryTransacted NTFSLPS_RemoveDirectoryTransactedW
#define NTFSLPS_ReplaceFile NTFSLPS_ReplaceFileW
#define NTFSLPS_ReplaceFilePathExtension NTFSLPS_ReplaceFilePathExtensionW
#define NTFSLPS_SearchPath NTFSLPS_SearchPathW
#define NTFSLPS_SetCurrentDirectory NTFSLPS_SetCurrentDirectoryW
#define NTFSLPS_SetFileAttributes NTFSLPS_SetFileAttributesW
#define NTFSLPS_SetFileAttributesTransacted NTFSLPS_SetFileAttributesTransactedW
#else
#define NTFSLPS_CopyFile NTFSLPS_CopyFileA
#define NTFSLPS_CopyFileEx NTFSLPS_CopyFileExA
#define NTFSLPS_CopyFileTransacted NTFSLPS_CopyFileTransactedA
#define NTFSLPS_CreateDirectory NTFSLPS_CreateDirectoryA
#define NTFSLPS_CreateDirectoryEx NTFSLPS_CreateDirectoryExA
#define NTFSLPS_CreateDirectoryTransacted NTFSLPS_CreateDirectoryTransactedA
#define NTFSLPS_CreateFile NTFSLPS_CreateFileA
#define NTFSLPS_CreateFileBackup NTFSLPS_CreateFileBackupA
#define NTFSLPS_CreateFileBackupEx NTFSLPS_CreateFileBackupExA
#define NTFSLPS_CreateFileTransacted NTFSLPS_CreateFileTransactedA
#define NTFSLPS_CreateFolder NTFSLPS_CreateFolderA
#define NTFSLPS_CreateHardLink NTFSLPS_CreateHardLinkA
#define NTFSLPS_CreateHardLinkTransacted NTFSLPS_CreateHardLinkTransactedA
#define NTFSLPS_CreateSymbolicLink NTFSLPS_CreateSymbolicLinkA
#define NTFSLPS_CreateSymbolicLinkTransacted NTFSLPS_CreateSymbolicLinkTransactedA
#define NTFSLPS_DeleteFile NTFSLPS_DeleteFileA
#define NTFSLPS_DeleteFileTransacted NTFSLPS_DeleteFileTransactedA
#define NTFSLPS_DeleteFolder NTFSLPS_DeleteFolderA
#define NTFSLPS_DumpToFile NTFSLPS_DumpToFileA
#define NTFSLPS_FindFirstFile NTFSLPS_FindFirstFileA
#define NTFSLPS_FindFirstFileEx NTFSLPS_FindFirstFileExA
#define NTFSLPS_FindFirstFileName NTFSLPS_FindFirstFileNameA
#define NTFSLPS_FindFirstFileNameTransacted NTFSLPS_FindFirstFileNameTransactedA
#define NTFSLPS_FindFirstStream NTFSLPS_FindFirstStreamA
#define NTFSLPS_FindFirstStreamTransacted NTFSLPS_FindFirstStreamTransactedA
#define NTFSLPS_FindNextFile NTFSLPS_FindNextFileA
#define NTFSLPS_FindNextFileName NTFSLPS_FindNextFileNameA
#define NTFSLPS_FindNextStream NTFSLPS_FindNextStreamA
#define NTFSLPS_GetCompressedFileSize NTFSLPS_GetCompressedFileSizeA
#define NTFSLPS_GetCompressedFileSizeTransacted NTFSLPS_GetCompressedFileSizeTransactedA
#define NTFSLPS_GetCurrentDirectory NTFSLPS_GetCurrentDirectoryA
#define NTFSLPS_GetFileAttributes NTFSLPS_GetFileAttributesA
#define NTFSLPS_GetFileAttributesEx NTFSLPS_GetFileAttributesExA
#define NTFSLPS_GetFileAttributesTransacted NTFSLPS_GetFileAttributesTransactedA
#define NTFSLPS_GetFileSystemObjectAttributes NTFSLPS_GetFileSystemObjectAttributesA
#define NTFSLPS_GetFinalPathNameByHandle NTFSLPS_GetFinalPathNameByHandleA
#define NTFSLPS_GetFullPath NTFSLPS_GetFullPathA
#define NTFSLPS_GetFullPathName NTFSLPS_GetFullPathNameA
#define NTFSLPS_GetFullPathNameTransacted NTFSLPS_GetFullPathNameTransactedA
#define NTFSLPS_GetLongPathName NTFSLPS_GetLongPathNameA
#define NTFSLPS_GetLongPathNameTransacted NTFSLPS_GetLongPathNameTransactedA
#define NTFSLPS_GetMappedFileName NTFSLPS_GetMappedFileNameA
#define NTFSLPS_GetModuleFileName NTFSLPS_GetModuleFileNameA
#define NTFSLPS_GetModuleFileNameEx NTFSLPS_GetModuleFileNameExA
#define NTFSLPS_GetSystemDirectory NTFSLPS_GetSystemDirectoryA
#define NTFSLPS_GetSystemWindowsDirectory NTFSLPS_GetSystemWindowsDirectoryA
#define NTFSLPS_GetWindowsDirectory NTFSLPS_GetWindowsDirectoryA
#define NTFSLPS_IsPathExist NTFSLPS_IsPathExistA
#define NTFSLPS_MoveFile NTFSLPS_MoveFileA
#define NTFSLPS_MoveFileEx NTFSLPS_MoveFileExA
#define NTFSLPS_MoveFileTransacted NTFSLPS_MoveFileTransactedA
#define NTFSLPS_MoveFileWithProgress NTFSLPS_MoveFileWithProgressA
#define NTFSLPS_NewFile NTFSLPS_NewFileW
#define NTFSLPS_RemoveDirectory NTFSLPS_RemoveDirectoryA
#define NTFSLPS_RemoveDirectoryTransacted NTFSLPS_RemoveDirectoryTransactedA
#define NTFSLPS_ReplaceFile NTFSLPS_ReplaceFileA
#define NTFSLPS_ReplaceFilePathExtension NTFSLPS_ReplaceFilePathExtensionA
#define NTFSLPS_SearchPath NTFSLPS_SearchPathA
#define NTFSLPS_SetCurrentDirectory NTFSLPS_SetCurrentDirectoryA
#define NTFSLPS_SetFileAttributes NTFSLPS_SetFileAttributesA
#define NTFSLPS_SetFileAttributesTransacted NTFSLPS_SetFileAttributesTransactedA
#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef __cplusplus
}
#endif

#endif /* __NTFSLPS_H__ */
