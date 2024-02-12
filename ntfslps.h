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

#ifdef LPWIN32_FIND_STREAM_DATA
#undef LPWIN32_FIND_STREAM_DATA
#endif

#ifdef PWIN32_FIND_STREAM_DATA
#undef PWIN32_FIND_STREAM_DATA
#endif

#ifdef WIN32_FIND_STREAM_DATA
#undef WIN32_FIND_STREAM_DATA
#endif

#ifdef UNICODE
#define WIN32_FIND_STREAM_DATA WIN32_FIND_STREAM_DATAW
#define PWIN32_FIND_STREAM_DATA *WIN32_FIND_STREAM_DATAW
#define LPWIN32_FIND_STREAM_DATA *WIN32_FIND_STREAM_DATAW
#else
#define WIN32_FIND_STREAM_DATA WIN32_FIND_STREAM_DATAA
#define PWIN32_FIND_STREAM_DATA *WIN32_FIND_STREAM_DATAA
#define LPWIN32_FIND_STREAM_DATA *WIN32_FIND_STREAM_DATAA
#endif

#ifdef NTFSLPS_EXPORTS
#define NTFSLPSAPI
#else
#define NTFSLPSAPI __declspec(dllimport)
#endif

NTFSLPSAPI UINT WINAPI NTFSLPS_AddPathEndingBackslashA(IN OUT LPSTR lpPathBuffer, IN UINT cchBuffer);
NTFSLPSAPI UINT WINAPI NTFSLPS_AddPathEndingBackslashW(IN OUT LPWSTR lpPathBuffer, IN UINT cchBuffer);
NTFSLPSAPI UINT WINAPI NTFSLPS_AppendPathComponentA(IN OUT LPSTR lpPathBuffer, IN UINT cchBuffer, IN LPCSTR lpComponent);
NTFSLPSAPI UINT WINAPI NTFSLPS_AppendPathComponentW(IN OUT LPWSTR lpPathBuffer, IN UINT cchBuffer, IN LPCWSTR lpComponent);
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
NTFSLPSAPI UINT WINAPI NTFSLPS_RemovePathEndingSlashA(IN OUT LPSTR lpPathBuffer, IN UINT cchBuffer);
NTFSLPSAPI UINT WINAPI NTFSLPS_RemovePathEndingSlashW(IN OUT LPWSTR lpPathBuffer, IN UINT cchBuffer);
NTFSLPSAPI BOOL WINAPI NTFSLPS_ReplaceFileA(IN LPCSTR lpReplacedFileName, IN LPCSTR lpReplacementFileName, IN LPCSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved);
NTFSLPSAPI DWORD WINAPI NTFSLPS_ReplaceFilePathExtensionA(IN OUT LPSTR lpObjectPath, IN DWORD nSize, LPCSTR lpExtension);
NTFSLPSAPI DWORD WINAPI NTFSLPS_ReplaceFilePathExtensionW(IN OUT LPWSTR lpObjectPath, IN DWORD nSize, LPCWSTR lpExtension);
NTFSLPSAPI BOOL WINAPI NTFSLPS_ReplaceFileW(IN LPCWSTR lpReplacedFileName, IN LPCWSTR lpReplacementFileName, IN LPCWSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved);
NTFSLPSAPI DWORD WINAPI NTFSLPS_SearchPathA(IN LPCSTR lpPath, IN LPCSTR lpFileName, IN LPCSTR lpExtension, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart);
NTFSLPSAPI DWORD WINAPI NTFSLPS_SearchPathW(IN LPCWSTR lpPath, IN LPCWSTR lpFileName, IN LPCWSTR lpExtension, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart);
NTFSLPSAPI BOOL WINAPI NTFSLPS_SetCurrentDirectoryA(IN LPCSTR lpPathName);
NTFSLPSAPI BOOL WINAPI NTFSLPS_SetCurrentDirectoryW(IN LPCWSTR lpPathName);
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

#ifndef NTFSLPS_EXPORTS

#define AddPathEndingBackslashA NTFSLPS_AddPathEndingBackslashA
#define AddPathEndingBackslashW NTFSLPS_AddPathEndingBackslashW

#define AppendPathComponentA NTFSLPS_AppendPathComponentA
#define AppendPathComponentW NTFSLPS_AppendPathComponentW

#ifdef CopyFile2
#undef CopyFile2
#endif
#define CopyFile2 NTFSLPS_CopyFile2

#ifdef CopyFileA
#undef CopyFileA
#endif
#define CopyFileA NTFSLPS_CopyFileA

#ifdef CopyFileExA
#undef CopyFileExA
#endif
#define CopyFileExA NTFSLPS_CopyFileExA

#ifdef CopyFileExW
#undef CopyFileExW
#endif
#define CopyFileExW NTFSLPS_CopyFileExW

#ifdef CopyFileTransactedA
#undef CopyFileTransactedA
#endif
#define CopyFileTransactedA NTFSLPS_CopyFileTransactedA

#ifdef CopyFileTransactedW
#undef CopyFileTransactedW
#endif
#define CopyFileTransactedW NTFSLPS_CopyFileTransactedW

#ifdef CopyFileW
#undef CopyFileW
#endif
#define CopyFileW NTFSLPS_CopyFileW

#ifdef CreateDirectoryA
#undef CreateDirectoryA
#endif
#define CreateDirectoryA NTFSLPS_CreateDirectoryA

#ifdef CreateDirectoryExA
#undef CreateDirectoryExA
#endif
#define CreateDirectoryExA NTFSLPS_CreateDirectoryExA

#ifdef CreateDirectoryExW
#undef CreateDirectoryExW
#endif
#define CreateDirectoryExW NTFSLPS_CreateDirectoryExW

#ifdef CreateDirectoryTransactedA
#undef CreateDirectoryTransactedA
#endif
#define CreateDirectoryTransactedA NTFSLPS_CreateDirectoryTransactedA

#ifdef CreateDirectoryTransactedW
#undef CreateDirectoryTransactedW
#endif
#define CreateDirectoryTransactedW NTFSLPS_CreateDirectoryTransactedW

#ifdef CreateDirectoryW
#undef CreateDirectoryW
#endif
#define CreateDirectoryW NTFSLPS_CreateDirectoryW

#ifdef CreateFile2
#undef CreateFile2
#endif
#define CreateFile2 NTFSLPS_CreateFile2

#ifdef CreateFileA
#undef CreateFileA
#endif
#define CreateFileA NTFSLPS_CreateFileA

#define CreateFileBackupA NTFSLPS_CreateFileBackupA
#define CreateFileBackupExA NTFSLPS_CreateFileBackupExA
#define CreateFileBackupExW NTFSLPS_CreateFileBackupExW
#define CreateFileBackupW NTFSLPS_CreateFileBackupW

#ifdef CreateFileTransactedA
#undef CreateFileTransactedA
#endif
#define CreateFileTransactedA NTFSLPS_CreateFileTransactedA

#ifdef CreateFileTransactedW
#undef CreateFileTransactedW
#endif
#define CreateFileTransactedW NTFSLPS_CreateFileTransactedW

#ifdef CreateFileW
#undef CreateFileW
#endif
#define CreateFileW NTFSLPS_CreateFileW

#define CreateFolderA NTFSLPS_CreateFolderA
#define CreateFolderW NTFSLPS_CreateFolderW

#ifdef CreateHardLinkA
#undef CreateHardLinkA
#endif
#define CreateHardLinkA NTFSLPS_CreateHardLinkA

#ifdef CreateHardLinkTransactedA
#undef CreateHardLinkTransactedA
#endif
#define CreateHardLinkTransactedA NTFSLPS_CreateHardLinkTransactedA

#ifdef CreateHardLinkTransactedW
#undef CreateHardLinkTransactedW
#endif
#define CreateHardLinkTransactedW NTFSLPS_CreateHardLinkTransactedW

#ifdef CreateHardLinkW
#undef CreateHardLinkW
#endif
#define CreateHardLinkW NTFSLPS_CreateHardLinkW

#ifdef CreateSymbolicLinkA
#undef CreateSymbolicLinkA
#endif
#define CreateSymbolicLinkA NTFSLPS_CreateSymbolicLinkA

#ifdef CreateSymbolicLinkTransactedA
#undef CreateSymbolicLinkTransactedA
#endif
#define CreateSymbolicLinkTransactedA NTFSLPS_CreateSymbolicLinkTransactedA

#ifdef CreateSymbolicLinkTransactedW
#undef CreateSymbolicLinkTransactedW
#endif
#define CreateSymbolicLinkTransactedW NTFSLPS_CreateSymbolicLinkTransactedW

#ifdef CreateSymbolicLinkW
#undef CreateSymbolicLinkW
#endif
#define CreateSymbolicLinkW NTFSLPS_CreateSymbolicLinkW

#ifdef DeleteFileA
#undef DeleteFileA
#endif
#define DeleteFileA NTFSLPS_DeleteFileA

#ifdef DeleteFileTransactedA
#undef DeleteFileTransactedA
#endif
#define DeleteFileTransactedA NTFSLPS_DeleteFileTransactedA

#ifdef DeleteFileTransactedW
#undef DeleteFileTransactedW
#endif
#define DeleteFileTransactedW NTFSLPS_DeleteFileTransactedW

#ifdef DeleteFileW
#undef DeleteFileW
#endif
#define DeleteFileW NTFSLPS_DeleteFileW

#define DeleteFolderA NTFSLPS_DeleteFolderA
#define DeleteFolderW NTFSLPS_DeleteFolderW

#define DumpToFileA NTFSLPS_DumpToFileA
#define DumpToFileW NTFSLPS_DumpToFileW

#ifdef FindFirstFileA
#undef FindFirstFileA
#endif
#define FindFirstFileA NTFSLPS_FindFirstFileA

#ifdef FindFirstFileExA
#undef FindFirstFileExA
#endif
#define FindFirstFileExA NTFSLPS_FindFirstFileExA

#ifdef FindFirstFileExW
#undef FindFirstFileExW
#endif
#define FindFirstFileExW NTFSLPS_FindFirstFileExW

#define FindFirstFileNameA NTFSLPS_FindFirstFileNameA

#define FindFirstFileNameTransactedA NTFSLPS_FindFirstFileNameTransactedA

#ifdef FindFirstFileNameTransactedW
#undef FindFirstFileNameTransactedW
#endif
#define FindFirstFileNameTransactedW NTFSLPS_FindFirstFileNameTransactedW

#ifdef FindFirstFileNameW
#undef FindFirstFileNameW
#endif
#define FindFirstFileNameW NTFSLPS_FindFirstFileNameW

#ifdef FindFirstFileTransactedA
#undef FindFirstFileTransactedA
#endif
#define FindFirstFileTransactedA NTFSLPS_FindFirstFileTransactedA

#ifdef FindFirstFileTransactedW
#undef FindFirstFileTransactedW
#endif
#define FindFirstFileTransactedW NTFSLPS_FindFirstFileTransactedW

#ifdef FindFirstFileW
#undef FindFirstFileW
#endif
#define FindFirstFileW NTFSLPS_FindFirstFileW

#define FindFirstStreamA NTFSLPS_FindFirstStreamA

#define FindFirstStreamTransactedA NTFSLPS_FindFirstStreamTransactedA

#ifdef FindFirstStreamTransactedW
#undef FindFirstStreamTransactedW
#endif
#define FindFirstStreamTransactedW NTFSLPS_FindFirstStreamTransactedW

#ifdef FindFirstStreamW
#undef FindFirstStreamW
#endif
#define FindFirstStreamW NTFSLPS_FindFirstStreamW

#ifdef FindNextFileA
#undef FindNextFileA
#endif
#define FindNextFileA NTFSLPS_FindNextFileA

#define FindNextFileNameA NTFSLPS_FindNextFileNameA

#ifdef FindNextFileNameW
#undef FindNextFileNameW
#endif
#define FindNextFileNameW NTFSLPS_FindNextFileNameW

#ifdef FindNextFileW
#undef FindNextFileW
#endif
#define FindNextFileW NTFSLPS_FindNextFileW

#define FindNextStreamA NTFSLPS_FindNextStreamA

#ifdef FindNextStreamW
#undef FindNextStreamW
#endif
#define FindNextStreamW NTFSLPS_FindNextStreamW

#ifdef GetCompressedFileSizeA
#undef GetCompressedFileSizeA
#endif
#define GetCompressedFileSizeA NTFSLPS_GetCompressedFileSizeA

#ifdef GetCompressedFileSizeTransactedA
#undef GetCompressedFileSizeTransactedA
#endif
#define GetCompressedFileSizeTransactedA NTFSLPS_GetCompressedFileSizeTransactedA

#ifdef GetCompressedFileSizeTransactedW
#undef GetCompressedFileSizeTransactedW
#endif
#define GetCompressedFileSizeTransactedW NTFSLPS_GetCompressedFileSizeTransactedW

#ifdef GetCompressedFileSizeW
#undef GetCompressedFileSizeW
#endif
#define GetCompressedFileSizeW NTFSLPS_GetCompressedFileSizeW

#ifdef GetFileAttributesA
#undef GetFileAttributesA
#endif
#define GetFileAttributesA NTFSLPS_GetFileAttributesA

#ifdef GetFileAttributesExA
#undef GetFileAttributesExA
#endif
#define GetFileAttributesExA NTFSLPS_GetFileAttributesExA

#ifdef GetFileAttributesExW
#undef GetFileAttributesExW
#endif
#define GetFileAttributesExW NTFSLPS_GetFileAttributesExW

#ifdef GetFileAttributesTransactedA
#undef GetFileAttributesTransactedA
#endif
#define GetFileAttributesTransactedA NTFSLPS_GetFileAttributesTransactedA

#ifdef GetFileAttributesTransactedW
#undef GetFileAttributesTransactedW
#endif
#define GetFileAttributesTransactedW NTFSLPS_GetFileAttributesTransactedW

#ifdef GetFileAttributesW
#undef GetFileAttributesW
#endif
#define GetFileAttributesW NTFSLPS_GetFileAttributesW

#ifdef GetFileSizeEx
#undef GetFileSizeEx
#endif
#define GetFileSizeEx NTFSLPS_GetFileSizeEx

#ifdef GetFinalPathNameByHandleA
#undef GetFinalPathNameByHandleA
#endif
#define GetFinalPathNameByHandleA NTFSLPS_GetFinalPathNameByHandleA

#ifdef GetFinalPathNameByHandleW
#undef GetFinalPathNameByHandleW
#endif
#define GetFinalPathNameByHandleW NTFSLPS_GetFinalPathNameByHandleW

#define GetFullPathA NTFSLPS_GetFullPathA
#define GetFullPathW NTFSLPS_GetFullPathW

#ifdef GetFullPathNameA
#undef GetFullPathNameA
#endif
#define GetFullPathNameA NTFSLPS_GetFullPathNameA

#ifdef GetFullPathNameTransactedA
#undef GetFullPathNameTransactedA
#endif
#define GetFullPathNameTransactedA NTFSLPS_GetFullPathNameTransactedA

#ifdef GetFullPathNameTransactedW
#undef GetFullPathNameTransactedW
#endif
#define GetFullPathNameTransactedW NTFSLPS_GetFullPathNameTransactedW

#ifdef GetFullPathNameW
#undef GetFullPathNameW
#endif
#define GetFullPathNameW NTFSLPS_GetFullPathNameW

#ifdef GetLongPathNameA
#undef GetLongPathNameA
#endif
#define GetLongPathNameA NTFSLPS_GetLongPathNameA

#ifdef GetLongPathNameTransactedA
#undef GetLongPathNameTransactedA
#endif
#define GetLongPathNameTransactedA NTFSLPS_GetLongPathNameTransactedA

#ifdef GetLongPathNameTransactedW
#undef GetLongPathNameTransactedW
#endif
#define GetLongPathNameTransactedW NTFSLPS_GetLongPathNameTransactedW

#ifdef GetLongPathNameW
#undef GetLongPathNameW
#endif
#define GetLongPathNameW NTFSLPS_GetLongPathNameW

#ifdef GetMappedFileNameA
#undef GetMappedFileNameA
#endif
#define GetMappedFileNameA NTFSLPS_GetMappedFileNameA

#ifdef GetMappedFileNameW
#undef GetMappedFileNameW
#endif
#define GetMappedFileNameW NTFSLPS_GetMappedFileNameW

#ifdef GetModuleFileNameA
#undef GetModuleFileNameA
#endif
#define GetModuleFileNameA NTFSLPS_GetModuleFileNameA

#ifdef GetModuleFileNameExA
#undef GetModuleFileNameExA
#endif
#define GetModuleFileNameExA NTFSLPS_GetModuleFileNameExA

#ifdef GetModuleFileNameExW
#undef GetModuleFileNameExW
#endif
#define GetModuleFileNameExW NTFSLPS_GetModuleFileNameExW

#ifdef GetModuleFileNameW
#undef GetModuleFileNameW
#endif
#define GetModuleFileNameW NTFSLPS_GetModuleFileNameW

#define IsPathExistA NTFSLPS_IsPathExistA
#define IsPathExistW NTFSLPS_IsPathExistW

#ifdef MoveFileA
#undef MoveFileA
#endif
#define MoveFileA NTFSLPS_MoveFileA

#ifdef MoveFileExA
#undef MoveFileExA
#endif
#define MoveFileExA NTFSLPS_MoveFileExA

#ifdef MoveFileExW
#undef MoveFileExW
#endif
#define MoveFileExW NTFSLPS_MoveFileExW

#ifdef MoveFileTransactedA
#undef MoveFileTransactedA
#endif
#define MoveFileTransactedA NTFSLPS_MoveFileTransactedA

#ifdef MoveFileTransactedW
#undef MoveFileTransactedW
#endif
#define MoveFileTransactedW NTFSLPS_MoveFileTransactedW

#ifdef MoveFileW
#undef MoveFileW
#endif
#define MoveFileW NTFSLPS_MoveFileW

#ifdef MoveFileWithProgressA
#undef MoveFileWithProgressA
#endif
#define MoveFileWithProgressA NTFSLPS_MoveFileWithProgressA

#ifdef MoveFileWithProgressW
#undef MoveFileWithProgressW
#endif
#define MoveFileWithProgressW NTFSLPS_MoveFileWithProgressW

#define NewFileA NTFSLPS_NewFileA
#define NewFileW NTFSLPS_NewFileW

#ifdef RemoveDirectoryA
#undef RemoveDirectoryA
#endif
#define RemoveDirectoryA NTFSLPS_RemoveDirectoryA

#ifdef RemoveDirectoryTransactedA
#undef RemoveDirectoryTransactedA
#endif
#define RemoveDirectoryTransactedA NTFSLPS_RemoveDirectoryTransactedA

#ifdef RemoveDirectoryTransactedW
#undef RemoveDirectoryTransactedW
#endif
#define RemoveDirectoryTransactedW NTFSLPS_RemoveDirectoryTransactedW

#ifdef RemoveDirectoryW
#undef RemoveDirectoryW
#endif
#define RemoveDirectoryW NTFSLPS_RemoveDirectoryW

#define RemovePathEndingSlashA NTFSLPS_RemovePathEndingSlashA
#define RemovePathEndingSlashW NTFSLPS_RemovePathEndingSlashW

#ifdef ReplaceFileA
#undef ReplaceFileA
#endif
#define ReplaceFileA NTFSLPS_ReplaceFileA

#define ReplaceFilePathExtensionA NTFSLPS_ReplaceFilePathExtensionA
#define ReplaceFilePathExtensionW NTFSLPS_ReplaceFilePathExtensionW

#ifdef ReplaceFileW
#undef ReplaceFileW
#endif
#define ReplaceFileW NTFSLPS_ReplaceFileW

#ifdef SearchPathA
#undef SearchPathA
#endif
#define SearchPathA NTFSLPS_SearchPathA

#ifdef SearchPathW
#undef SearchPathW
#endif
#define SearchPathW NTFSLPS_SearchPathW

#ifdef SetCurrentDirectoryA
#undef SetCurrentDirectoryA
#endif
#define SetCurrentDirectoryA NTFSLPS_SetCurrentDirectoryA

#ifdef SetCurrentDirectoryW
#undef SetCurrentDirectoryW
#endif
#define SetCurrentDirectoryW NTFSLPS_SetCurrentDirectoryW

#ifdef SetFileAttributesA
#undef SetFileAttributesA
#endif
#define SetFileAttributesA NTFSLPS_SetFileAttributesA

#ifdef SetFileAttributesTransactedA
#undef SetFileAttributesTransactedA
#endif
#define SetFileAttributesTransactedA NTFSLPS_SetFileAttributesTransactedA

#ifdef SetFileAttributesTransactedW
#undef SetFileAttributesTransactedW
#endif
#define SetFileAttributesTransactedW NTFSLPS_SetFileAttributesTransactedW

#ifdef SetFileAttributesW
#undef SetFileAttributesW
#endif
#define SetFileAttributesW NTFSLPS_SetFileAttributesW

#ifdef SetSearchPathMode
#undef SetSearchPathMode
#endif
#define SetSearchPathMode NTFSLPS_SetSearchPathMode

#ifdef Wow64DisableWow64FsRedirection
#undef Wow64DisableWow64FsRedirection
#endif
#define Wow64DisableWow64FsRedirection NTFSLPS_Wow64DisableWow64FsRedirection

#ifdef Wow64EnableWow64FsRedirection
#undef Wow64EnableWow64FsRedirection
#endif
#define Wow64EnableWow64FsRedirection NTFSLPS_Wow64EnableWow64FsRedirection

#ifdef Wow64RevertWow64FsRedirection
#undef Wow64RevertWow64FsRedirection
#endif
#define Wow64RevertWow64FsRedirection NTFSLPS_Wow64RevertWow64FsRedirection

#ifdef _lcreat
#undef _lcreat
#endif
#define _lcreat NTFSLPS_lcreat

#ifdef _lopen
#undef _lopen
#endif
#define _lopen NTFSLPS_lopen

#ifdef _mkdir
#undef _mkdir
#endif
#define _mkdir NTFSLPS_mkdir

#ifdef _remove
#undef _remove
#endif
#define _remove NTFSLPS_remove

#ifdef _rmdir
#undef _rmdir
#endif
#define _rmdir NTFSLPS_rmdir

#ifdef _wmkdir
#undef _wmkdir
#endif
#define _wmkdir NTFSLPS_wmkdir

#ifdef _wremove
#undef _wremove
#endif
#define _wremove NTFSLPS_wremove

#ifdef _wrmdir
#undef _wrmdir
#endif
#define _wrmdir NTFSLPS_wrmdir

#ifdef CopyFile
#undef CopyFile
#endif

#ifdef CopyFileEx
#undef CopyFileEx
#endif

#ifdef CopyFileTransacted
#undef CopyFileTransacted
#endif

#ifdef CreateDirectoryTransacted
#undef CreateDirectoryTransacted
#endif

#ifdef CreateDirectoryEx
#undef CreateDirectoryEx
#endif

#ifdef CreateDirectory
#undef CreateDirectory
#endif

#ifdef CreateFile
#undef CreateFile
#endif

#ifdef CreateFileTransacted
#undef CreateFileTransacted
#endif

#ifdef CreateHardLink
#undef CreateHardLink
#endif

#ifdef CreateHardLinkTransacted
#undef CreateHardLinkTransacted
#endif

#ifdef CreateSymbolicLink
#undef CreateSymbolicLink
#endif

#ifdef CreateSymbolicLinkTransacted
#undef CreateSymbolicLinkTransacted
#endif

#ifdef DeleteFile
#undef DeleteFile
#endif

#ifdef DeleteFileTransacted
#undef DeleteFileTransacted
#endif

#ifdef FindFirstFile
#undef FindFirstFile
#endif

#ifdef FindFirstFileEx
#undef FindFirstFileEx
#endif

#ifdef FindFirstFileName
#undef FindFirstFileName
#endif

#ifdef FindFirstFileNameTransacted
#undef FindFirstFileNameTransacted
#endif

#ifdef FindFirstStream
#undef FindFirstStream
#endif

#ifdef FindFirstStreamTransacted
#undef FindFirstStreamTransacted
#endif

#ifdef FindNextFile
#undef FindNextFile
#endif

#ifdef FindNextFileName
#undef FindNextFileName
#endif

#ifdef FindNextStream
#undef FindNextStream
#endif

#ifdef GetCompressedFileSize
#undef GetCompressedFileSize
#endif

#ifdef GetCompressedFileSizeTransacted
#undef GetCompressedFileSizeTransacted
#endif

#ifdef GetFileAttributes
#undef GetFileAttributes
#endif

#ifdef GetFileAttributesEx
#undef GetFileAttributesEx
#endif

#ifdef GetFileAttributesTransacted
#undef GetFileAttributesTransacted
#endif

#ifdef GetFinalPathNameByHandle
#undef GetFinalPathNameByHandle
#endif

#ifdef GetFullPathName
#undef GetFullPathName
#endif

#ifdef GetFullPathNameTransacted
#undef GetFullPathNameTransacted
#endif

#ifdef GetLongPathName
#undef GetLongPathName
#endif

#ifdef GetLongPathNameTransacted
#undef GetLongPathNameTransacted
#endif

#ifdef GetMappedFileName
#undef GetMappedFileName
#endif

#ifdef GetModuleFileName
#undef GetModuleFileName
#endif

#ifdef GetModuleFileNameEx
#undef GetModuleFileNameEx
#endif

#ifdef MoveFile
#undef MoveFile
#endif

#ifdef MoveFileEx
#undef MoveFileEx
#endif

#ifdef MoveFileTransacted
#undef MoveFileTransacted
#endif

#ifdef MoveFileWithProgress
#undef MoveFileWithProgress
#endif

#ifdef RemoveDirectory
#undef RemoveDirectory
#endif

#ifdef RemoveDirectoryTransacted
#undef RemoveDirectoryTransacted
#endif

#ifdef ReplaceFile
#undef ReplaceFile
#endif

#ifdef SearchPath
#undef SearchPath
#endif

#ifdef SetCurrentDirectory
#undef SetCurrentDirectory
#endif

#ifdef SetFileAttributes
#undef SetFileAttributes
#endif

#ifdef SetFileAttributesTransacted
#undef SetFileAttributesTransacted
#endif

#endif

#ifdef UNICODE

#define NTFSLPS_AddPathEndingBackslash NTFSLPS_AddPathEndingBackslashW
#define NTFSLPS_AppendPathComponent NTFSLPS_AppendPathComponentW
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
#define NTFSLPS_GetFileAttributes NTFSLPS_GetFileAttributesW
#define NTFSLPS_GetFileAttributesEx NTFSLPS_GetFileAttributesExW
#define NTFSLPS_GetFileAttributesTransacted NTFSLPS_GetFileAttributesTransactedW
#define NTFSLPS_GetFinalPathNameByHandle NTFSLPS_GetFinalPathNameByHandleW
#define NTFSLPS_GetFullPath NTFSLPS_GetFullPathW
#define NTFSLPS_GetFullPathName NTFSLPS_GetFullPathNameW
#define NTFSLPS_GetFullPathNameTransacted NTFSLPS_GetFullPathNameTransactedW
#define NTFSLPS_GetLongPathName NTFSLPS_GetLongPathNameW
#define NTFSLPS_GetLongPathNameTransacted NTFSLPS_GetLongPathNameTransactedW
#define NTFSLPS_GetMappedFileName NTFSLPS_GetMappedFileNameW
#define NTFSLPS_GetModuleFileName NTFSLPS_GetModuleFileNameW
#define NTFSLPS_GetModuleFileNameEx NTFSLPS_GetModuleFileNameExW
#define NTFSLPS_IsPathExist NTFSLPS_IsPathExistW
#define NTFSLPS_MoveFile NTFSLPS_MoveFileW
#define NTFSLPS_MoveFileEx NTFSLPS_MoveFileExW
#define NTFSLPS_MoveFileTransacted NTFSLPS_MoveFileTransactedW
#define NTFSLPS_MoveFileWithProgress NTFSLPS_MoveFileWithProgressW
#define NTFSLPS_NewFile NTFSLPS_NewFileW
#define NTFSLPS_RemoveDirectory NTFSLPS_RemoveDirectoryW
#define NTFSLPS_RemoveDirectoryTransacted NTFSLPS_RemoveDirectoryTransactedW
#define NTFSLPS_RemovePathEndingSlash NTFSLPS_RemovePathEndingSlashW
#define NTFSLPS_ReplaceFile NTFSLPS_ReplaceFileW
#define NTFSLPS_ReplaceFilePathExtension NTFSLPS_ReplaceFilePathExtensionW
#define NTFSLPS_SearchPath NTFSLPS_SearchPathW
#define NTFSLPS_SetCurrentDirectory NTFSLPS_SetCurrentDirectoryW
#define NTFSLPS_SetFileAttributes NTFSLPS_SetFileAttributesW
#define NTFSLPS_SetFileAttributesTransacted NTFSLPS_SetFileAttributesTransactedW

#ifndef NTFSLPS_EXPORTS

#define AddPathEndingBackslash NTFSLPS_AddPathEndingBackslashW
#define AppendPathComponent NTFSLPS_AppendPathComponentW
#define CopyFile NTFSLPS_CopyFileW
#define CopyFileEx NTFSLPS_CopyFileExW
#define CopyFileTransacted NTFSLPS_CopyFileTransactedW
#define CreateDirectoryTransacted NTFSLPS_CreateDirectoryTransactedW
#define CreateDirectoryEx NTFSLPS_CreateDirectoryExW
#define CreateDirectory NTFSLPS_CreateDirectoryW
#define CreateFile NTFSLPS_CreateFileW
#define CreateFileBackup NTFSLPS_CreateFileBackupW
#define CreateFileBackupEx NTFSLPS_CreateFileBackupExW
#define CreateFileTransacted NTFSLPS_CreateFileTransactedW
#define CreateFolder NTFSLPS_CreateFolderW
#define CreateHardLink NTFSLPS_CreateHardLinkW
#define CreateHardLinkTransacted NTFSLPS_CreateHardLinkTransactedW
#define CreateSymbolicLink NTFSLPS_CreateSymbolicLinkW
#define CreateSymbolicLinkTransacted NTFSLPS_CreateSymbolicLinkTransactedW
#define DeleteFile NTFSLPS_DeleteFileW
#define DeleteFileTransacted NTFSLPS_DeleteFileTransactedW
#define DeleteFolder NTFSLPS_DeleteFolderW
#define DumpToFile NTFSLPS_DumpToFileW
#define FindFirstFile NTFSLPS_FindFirstFileW
#define FindFirstFileEx NTFSLPS_FindFirstFileExW
#define FindFirstFileName NTFSLPS_FindFirstFileNameW
#define FindFirstFileNameTransacted NTFSLPS_FindFirstFileNameTransactedW
#define FindFirstStream NTFSLPS_FindFirstStreamW
#define FindFirstStreamTransacted NTFSLPS_FindFirstStreamTransactedW
#define FindNextFile NTFSLPS_FindNextFileW
#define FindNextFileName NTFSLPS_FindNextFileNameW
#define FindNextStream NTFSLPS_FindNextStreamW
#define GetCompressedFileSize NTFSLPS_GetCompressedFileSizeW
#define GetCompressedFileSizeTransacted NTFSLPS_GetCompressedFileSizeTransactedW
#define GetFileAttributes NTFSLPS_GetFileAttributesW
#define GetFileAttributesEx NTFSLPS_GetFileAttributesExW
#define GetFileAttributesTransacted NTFSLPS_GetFileAttributesTransactedW
#define GetFinalPathNameByHandle NTFSLPS_GetFinalPathNameByHandleW
#define GetFullPath NTFSLPS_GetFullPathW
#define GetFullPathName NTFSLPS_GetFullPathNameW
#define GetFullPathNameTransacted NTFSLPS_GetFullPathNameTransactedW
#define GetLongPathName NTFSLPS_GetLongPathNameW
#define GetLongPathNameTransacted NTFSLPS_GetLongPathNameTransactedW
#define GetMappedFileName NTFSLPS_GetMappedFileNameW
#define GetModuleFileName NTFSLPS_GetModuleFileNameW
#define GetModuleFileNameEx NTFSLPS_GetModuleFileNameExW
#define IsPathExist NTFSLPS_IsPathExistW
#define MoveFile NTFSLPS_MoveFileW
#define MoveFileEx NTFSLPS_MoveFileExW
#define MoveFileTransacted NTFSLPS_MoveFileTransactedW
#define MoveFileWithProgress NTFSLPS_MoveFileWithProgressW
#define NewFile NTFSLPS_NewFileW
#define RemoveDirectory NTFSLPS_RemoveDirectoryW
#define RemoveDirectoryTransacted NTFSLPS_RemoveDirectoryTransactedW
#define RemovePathEndingSlash NTFSLPS_RemovePathEndingSlashW
#define ReplaceFile NTFSLPS_ReplaceFileW
#define ReplaceFilePathExtension NTFSLPS_ReplaceFilePathExtensionW
#define SearchPath NTFSLPS_SearchPathW
#define SetCurrentDirectory NTFSLPS_SetCurrentDirectoryW
#define SetFileAttributes NTFSLPS_SetFileAttributesW
#define SetFileAttributesTransacted NTFSLPS_SetFileAttributesTransactedW

#endif

#else

#define NTFSLPS_AddPathEndingBackslash NTFSLPS_AddPathEndingBackslashA
#define NTFSLPS_AppendPathComponent NTFSLPS_AppendPathComponentA
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
#define NTFSLPS_GetFileAttributes NTFSLPS_GetFileAttributesA
#define NTFSLPS_GetFileAttributesEx NTFSLPS_GetFileAttributesExA
#define NTFSLPS_GetFileAttributesTransacted NTFSLPS_GetFileAttributesTransactedA
#define NTFSLPS_GetFinalPathNameByHandle NTFSLPS_GetFinalPathNameByHandleA
#define NTFSLPS_GetFullPath NTFSLPS_GetFullPathA
#define NTFSLPS_GetFullPathName NTFSLPS_GetFullPathNameA
#define NTFSLPS_GetFullPathNameTransacted NTFSLPS_GetFullPathNameTransactedA
#define NTFSLPS_GetLongPathName NTFSLPS_GetLongPathNameA
#define NTFSLPS_GetLongPathNameTransacted NTFSLPS_GetLongPathNameTransactedA
#define NTFSLPS_GetMappedFileName NTFSLPS_GetMappedFileNameA
#define NTFSLPS_GetModuleFileName NTFSLPS_GetModuleFileNameA
#define NTFSLPS_GetModuleFileNameEx NTFSLPS_GetModuleFileNameExA
#define NTFSLPS_IsPathExist NTFSLPS_IsPathExistA
#define NTFSLPS_MoveFile NTFSLPS_MoveFileA
#define NTFSLPS_MoveFileEx NTFSLPS_MoveFileExA
#define NTFSLPS_MoveFileTransacted NTFSLPS_MoveFileTransactedA
#define NTFSLPS_MoveFileWithProgress NTFSLPS_MoveFileWithProgressA
#define NTFSLPS_NewFile NTFSLPS_NewFileA
#define NTFSLPS_RemoveDirectory NTFSLPS_RemoveDirectoryA
#define NTFSLPS_RemoveDirectoryTransacted NTFSLPS_RemoveDirectoryTransactedA
#define NTFSLPS_RemovePathEndingSlash NTFSLPS_RemovePathEndingSlashA
#define NTFSLPS_ReplaceFile NTFSLPS_ReplaceFileA
#define NTFSLPS_ReplaceFilePathExtension NTFSLPS_ReplaceFilePathExtensionA
#define NTFSLPS_SearchPath NTFSLPS_SearchPathA
#define NTFSLPS_SetCurrentDirectory NTFSLPS_SetCurrentDirectoryA
#define NTFSLPS_SetFileAttributes NTFSLPS_SetFileAttributesA
#define NTFSLPS_SetFileAttributesTransacted NTFSLPS_SetFileAttributesTransactedA

#ifndef NTFSLPS_EXPORTS

#define AddPathEndingBackslash NTFSLPS_AddPathEndingBackslashA
#define AppendPathComponent NTFSLPS_AppendPathComponentA
#define CopyFile NTFSLPS_CopyFileA
#define CopyFileEx NTFSLPS_CopyFileExA
#define CopyFileTransacted NTFSLPS_CopyFileTransactedA
#define CreateDirectory NTFSLPS_CreateDirectoryA
#define CreateDirectoryEx NTFSLPS_CreateDirectoryExA
#define CreateDirectoryTransacted NTFSLPS_CreateDirectoryTransactedA
#define CreateFile NTFSLPS_CreateFileA
#define CreateFileBackup NTFSLPS_CreateFileBackupA
#define CreateFileBackupEx NTFSLPS_CreateFileBackupExA
#define CreateFileTransacted NTFSLPS_CreateFileTransactedA
#define CreateFolder NTFSLPS_CreateFolderA
#define CreateHardLink NTFSLPS_CreateHardLinkA
#define CreateHardLinkTransacted NTFSLPS_CreateHardLinkTransactedA
#define CreateSymbolicLink NTFSLPS_CreateSymbolicLinkA
#define CreateSymbolicLinkTransacted NTFSLPS_CreateSymbolicLinkTransactedA
#define DeleteFile NTFSLPS_DeleteFileA
#define DeleteFileTransacted NTFSLPS_DeleteFileTransactedA
#define DeleteFolder NTFSLPS_DeleteFolderA
#define DumpToFile NTFSLPS_DumpToFileA
#define FindFirstFile NTFSLPS_FindFirstFileA
#define FindFirstFileEx NTFSLPS_FindFirstFileExA
#define FindFirstFileName NTFSLPS_FindFirstFileNameA
#define FindFirstFileNameTransacted NTFSLPS_FindFirstFileNameTransactedA
#define FindFirstStream NTFSLPS_FindFirstStreamA
#define FindFirstStreamTransacted NTFSLPS_FindFirstStreamTransactedA
#define FindNextFile NTFSLPS_FindNextFileA
#define FindNextFileName NTFSLPS_FindNextFileNameA
#define FindNextStream NTFSLPS_FindNextStreamA
#define GetCompressedFileSize NTFSLPS_GetCompressedFileSizeA
#define GetCompressedFileSizeTransacted NTFSLPS_GetCompressedFileSizeTransactedA
#define GetFileAttributes NTFSLPS_GetFileAttributesA
#define GetFileAttributesEx NTFSLPS_GetFileAttributesExA
#define GetFileAttributesTransacted NTFSLPS_GetFileAttributesTransactedA
#define GetFinalPathNameByHandle NTFSLPS_GetFinalPathNameByHandleA
#define GetFullPath NTFSLPS_GetFullPathA
#define GetFullPathName NTFSLPS_GetFullPathNameA
#define GetFullPathNameTransacted NTFSLPS_GetFullPathNameTransactedA
#define GetLongPathName NTFSLPS_GetLongPathNameA
#define GetLongPathNameTransacted NTFSLPS_GetLongPathNameTransactedA
#define GetMappedFileName NTFSLPS_GetMappedFileNameA
#define GetModuleFileName NTFSLPS_GetModuleFileNameA
#define GetModuleFileNameEx NTFSLPS_GetModuleFileNameExA
#define IsPathExist NTFSLPS_IsPathExistA
#define MoveFile NTFSLPS_MoveFileA
#define MoveFileEx NTFSLPS_MoveFileExA
#define MoveFileTransacted NTFSLPS_MoveFileTransactedA
#define MoveFileWithProgress NTFSLPS_MoveFileWithProgressA
#define NewFile NTFSLPS_NewFileA
#define RemoveDirectory NTFSLPS_RemoveDirectoryA
#define RemoveDirectoryTransacted NTFSLPS_RemoveDirectoryTransactedA
#define RemovePathEndingSlash NTFSLPS_RemovePathEndingSlashA
#define ReplaceFile NTFSLPS_ReplaceFileA
#define ReplaceFilePathExtension NTFSLPS_ReplaceFilePathExtensionA
#define SearchPath NTFSLPS_SearchPathA
#define SetCurrentDirectory NTFSLPS_SetCurrentDirectoryA
#define SetFileAttributes NTFSLPS_SetFileAttributesA
#define SetFileAttributesTransacted NTFSLPS_SetFileAttributesTransactedA

#endif

#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef __cplusplus
}
#endif

#endif /* __NTFSLPS_H__ */
