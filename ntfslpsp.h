/* +============================================================================================+ */
/* | ##    ## ######## ######## ######## ##       ######## ########          © Mikhail Subbotin | */
/* | ###   ##    ##    ##       ##    ## ##       ##    ## ##    ##                             | */
/* | ####  ##    ##    ##       ##       ##       ##    ## ##                                   | */
/* | ## ## ##    ##    ######## ######## ##       ######## ########                             | */
/* | ##  ####    ##    ##             ## ##       ##             ##                             | */
/* | ##   ###    ##    ##       ##    ## ##       ##       ##    ##                             | */
/* | ##    ##    ##    ##       ######## ######## ##       ########                             | */
/* +============================================================================================+ */
/* | ntfslpsp.h                                                                                 | */
/* +--------------------------------------------------------------------------------------------+ */

#ifndef __NTFSLPSP_H__
#define __NTFSLPSP_H__

#include <errno.h>
#include "ntfslps.h"
#include "ntfslpsc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _UNICODE_PATH_STRING {
    USHORT Length;
    USHORT MaximumLength;
    LPWSTR TemporaryBuffer;
    LPWSTR Buffer;
} UNICODE_PATH_STRING, *PUNICODE_PATH_STRING;

PVOID NTAPI RtlAllocateHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN SIZE_T Size);
BOOLEAN NTAPI RtlFreeHeap(IN HANDLE HeapHandle, IN ULONG Flags, IN PVOID Pointer);

typedef DWORD (NTAPI *pRtlGetLastWin32Error)();
typedef ULONG (NTAPI *pRtlNtStatusToDosError)(IN NTSTATUS Status);
typedef VOID (NTAPI *pRtlSetLastWin32Error)(IN DWORD Win32ErrorCode);

typedef NTSTATUS (NTAPI *pRtlAnsiStringToUnicodeString)(OUT PUNICODE_STRING DestinationString, IN PANSI_STRING SourceString, IN BOOL AllocateDestinationString);
typedef NTSTATUS (NTAPI *pRtlOemStringToUnicodeString)(OUT PUNICODE_STRING DestinationString, IN PCOEM_STRING SourceString, IN BOOL AllocateDestinationString);
typedef NTSTATUS (NTAPI *pRtlUnicodeStringToAnsiString)(OUT PANSI_STRING DestinationString, IN PCUNICODE_STRING SourceString, IN BOOL AllocateDestinationString);
typedef NTSTATUS (NTAPI *pRtlUnicodeStringToOemString)(OUT POEM_STRING DestinationString, IN PCUNICODE_STRING SourceString, IN BOOL AllocateDestinationString);

typedef NTSTATUS (NTAPI *pCurrentCodePageStringToUnicodeString)(OUT PUNICODE_STRING DestinationString, IN PVOID SourceString, IN BOOL AllocateDestinationString);
typedef NTSTATUS (NTAPI *pUnicodeStringToCurrentCodePageString)(OUT PVOID DestinationString, IN PCUNICODE_STRING SourceString, IN BOOL AllocateDestinationString);

typedef HRESULT (WINAPI *pCopyFile2)(IN PCWSTR pwszExistingFileName, IN PCWSTR pwszNewFileName, IN COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters);
typedef BOOL (WINAPI *pCopyFileTransactedA)(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pCopyFileTransactedW)(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pCreateDirectoryTransactedA)(IN LPCSTR lpTemplateDirectory, IN LPCSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pCreateDirectoryTransactedW)(IN LPCWSTR lpTemplateDirectory, IN LPCWSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
typedef HANDLE (WINAPI *pCreateFile2)(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN DWORD dwCreationDisposition, IN LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams);
typedef HANDLE (WINAPI *pCreateFileTransactedA)(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter);
typedef HANDLE (WINAPI *pCreateFileTransactedW)(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter);
typedef BOOL (WINAPI *pCreateHardLinkTransactedA)(IN LPCSTR lpFileName, IN LPCSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pCreateHardLinkTransactedW)(IN LPCWSTR lpFileName, IN LPCWSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pCreateSymbolicLinkTransactedA)(IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pCreateSymbolicLinkTransactedW)(IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pCreateSymbolicLinkA)(IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags);
typedef BOOL (WINAPI *pCreateSymbolicLinkW)(IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags);
typedef BOOL (WINAPI *pDeleteFileTransactedA)(IN LPCSTR lpFileName, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pDeleteFileTransactedW)(IN LPCWSTR lpFileName, IN HANDLE hTransaction);
typedef HANDLE (WINAPI *pFindFirstFileNameA)(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName);
typedef HANDLE (WINAPI *pFindFirstFileNameTransactedA)(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName, IN HANDLE hTransaction);
typedef HANDLE (WINAPI *pFindFirstFileNameTransactedW)(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName, IN HANDLE hTransaction);
typedef HANDLE (WINAPI *pFindFirstFileNameW)(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName);
typedef HANDLE (WINAPI *pFindFirstFileTransactedA)(IN LPCSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAA lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction);
typedef HANDLE (WINAPI *pFindFirstFileTransactedW)(IN LPCWSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPWIN32_FIND_DATAW lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction);
typedef HANDLE (WINAPI *pFindFirstStreamA)(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData, DWORD dwFlags);
typedef HANDLE (WINAPI *pFindFirstStreamTransactedA)(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction);
typedef HANDLE (WINAPI *pFindFirstStreamTransactedW)(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction);
typedef HANDLE (WINAPI *pFindFirstStreamW)(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData, DWORD dwFlags);
typedef BOOL (WINAPI *pFindNextFileNameA)(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PSTR LinkName);
typedef BOOL (WINAPI *pFindNextFileNameW)(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PWSTR LinkName);
typedef BOOL (WINAPI *pFindNextStreamA)(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData);
typedef BOOL (WINAPI *pFindNextStreamW)(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData);
typedef DWORD (WINAPI *pGetCompressedFileSizeTransactedA)(IN LPCSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction);
typedef DWORD (WINAPI *pGetCompressedFileSizeTransactedW)(IN LPCWSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pGetFileAttributesTransactedA)(IN LPCSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pGetFileAttributesTransactedW)(IN LPCWSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction);
typedef DWORD (WINAPI *pGetFinalPathNameByHandleA)(IN HANDLE hFile, OUT LPSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags);
typedef DWORD (WINAPI *pGetFinalPathNameByHandleW)(IN HANDLE hFile, OUT LPWSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags);
typedef DWORD (WINAPI *pGetFullPathNameTransactedA)(IN LPCSTR lpFileName, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart, IN HANDLE hTransaction);
typedef DWORD (WINAPI *pGetFullPathNameTransactedW)(IN LPCWSTR lpFileName, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart, IN HANDLE hTransaction);
typedef DWORD (WINAPI *pGetLongPathNameTransactedA)(IN LPCSTR lpszShortPath, OUT LPSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction);
typedef DWORD (WINAPI *pGetLongPathNameTransactedW)(IN LPCWSTR lpszShortPath, OUT LPWSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pMoveFileTransactedA)(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pMoveFileTransactedW)(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pMoveFileWithProgressA)(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags);
typedef BOOL (WINAPI *pMoveFileWithProgressW)(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags);
typedef BOOL (WINAPI *pRemoveDirectoryTransactedA)(IN LPCSTR lpPathName, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pRemoveDirectoryTransactedW)(IN LPCWSTR lpPathName, IN HANDLE hTransaction);
typedef BOOL (WINAPI *pReplaceFileA)(IN LPCSTR lpReplacedFileName, IN LPCSTR lpReplacementFileName, IN LPCSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved);
typedef BOOL (WINAPI *pReplaceFileW)(IN LPCWSTR lpReplacedFileName, IN LPCWSTR lpReplacementFileName, IN LPCWSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved);
typedef DWORD (WINAPI *pSearchPathA)(IN LPCSTR lpPath, IN LPCSTR lpFileName, IN LPCSTR lpExtension, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart);
typedef DWORD (WINAPI *pSearchPathW)(IN LPCWSTR lpPath, IN LPCWSTR lpFileName, IN LPCWSTR lpExtension, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart);
typedef BOOL (WINAPI *pSetFileAttributesTransactedA)(IN LPCSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction);
typedef BOOL (WINAPI *pSetFileAttributesTransactedW)(IN LPCWSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction);
typedef BOOL (WINAPI *pSetSearchPathMode)(IN DWORD Flags);
typedef BOOL (WINAPI *pWow64DisableWow64FsRedirection)(OUT PBOOL pbOldValue);
typedef BOOL (WINAPI *pWow64EnableWow64FsRedirection)(IN BOOL bWow64FsEnableRedirection);
typedef BOOL (WINAPI *pWow64RevertWow64FsRedirection)(OUT PBOOL pbOldValue);

LPWSTR WINAPI AllocateUnicodeString(IN LPCSTR lpString);
UINT WINAPI AnsiStringToUnicodeString(IN LPCSTR lpAnsiString, OUT LPWSTR lpUnicodeStringBuffer, IN UINT nBufferLength);
UINT WINAPI UnicodeStringToAnsiString(IN LPCWSTR lpUnicodeString, OUT LPSTR lpAnsiStringBuffer, IN UINT nBufferLength);
DWORD WINAPI GetFullPathAW(IN LPCSTR lpObjectPath, OUT LPWSTR lpBuffer, IN DWORD nBufferLength);
DWORD WINAPI GetFullPathExAW(IN LPCSTR lpObjectPath, OUT LPWSTR lpBuffer, IN DWORD nBufferLength, BOOL bForcedLongPathPrefix);
DWORD WINAPI GetFullPathExW(IN LPCWSTR lpObjectPath, OUT LPWSTR lpBuffer, IN DWORD nBufferLength, BOOL bForcedLongPathPrefix);
LPWSTR WINAPI GetFullObjectPathA(IN LPCSTR lpObjectPath);
LPWSTR WINAPI GetFullObjectPathExA(IN LPCSTR lpObjectPath, BOOL bForcedLongPathPrefix);
LPWSTR WINAPI GetFullObjectPathExW(IN LPCWSTR lpObjectPath, BOOL bForcedLongPathPrefix);
LPWSTR WINAPI GetFullObjectPathW(IN LPCWSTR lpObjectPath);
BOOL WINAPI FreeFullObjectPathBuffer(IN PVOID lpFullObjectPathBuffer);
int __cdecl getwinerrnocode(IN unsigned long oserrno);

UINT WINAPI DeleteSubFolderW(IN LPCWSTR lpPathName);

ULONG NTAPI alternateRtlNtStatusToDosError(IN NTSTATUS Status);

NTSTATUS NTAPI alternateRtlAnsiStringToUnicodeString(OUT PUNICODE_STRING DestinationString, IN PANSI_STRING SourceString, IN BOOL AllocateDestinationString);
NTSTATUS NTAPI alternateRtlOemStringToUnicodeString(OUT PUNICODE_STRING DestinationString, IN PCOEM_STRING SourceString, IN BOOL AllocateDestinationString);
NTSTATUS NTAPI alternateRtlUnicodeStringToAnsiString(OUT PANSI_STRING DestinationString, IN PCUNICODE_STRING SourceString, IN BOOL AllocateDestinationString);
NTSTATUS NTAPI alternateRtlUnicodeStringToOemString(OUT POEM_STRING DestinationString, IN PCUNICODE_STRING SourceString, IN BOOL AllocateDestinationString);

HRESULT WINAPI stubCopyFile2(IN PCWSTR pwszExistingFileName, IN PCWSTR pwszNewFileName, IN COPYFILE2_EXTENDED_PARAMETERS *pExtendedParameters);
BOOL WINAPI stubCopyFileTransactedA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction);
BOOL WINAPI stubCopyFileTransactedW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN LPBOOL pbCancel, IN DWORD dwCopyFlags, IN HANDLE hTransaction);
BOOL WINAPI stubCreateDirectoryTransactedA(IN LPCSTR lpTemplateDirectory, IN LPCSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
BOOL WINAPI stubCreateDirectoryTransactedW(IN LPCWSTR lpTemplateDirectory, IN LPCWSTR lpNewDirectory, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
HANDLE WINAPI stubCreateFile2(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN DWORD dwCreationDisposition, IN LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams);
HANDLE WINAPI stubCreateFileTransactedA(IN LPCSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter);
HANDLE WINAPI stubCreateFileTransactedW(IN LPCWSTR lpFileName, IN DWORD dwDesiredAccess, IN DWORD dwShareMode, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN DWORD dwCreationDisposition, IN DWORD dwFlagsAndAttributes, IN HANDLE hTemplateFile, IN HANDLE hTransaction, IN PUSHORT pusMiniVersion, IN PVOID lpExtendedParameter);
BOOL WINAPI stubCreateHardLinkTransactedA(IN LPCSTR lpFileName, IN LPCSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
BOOL WINAPI stubCreateHardLinkTransactedW(IN LPCWSTR lpFileName, IN LPCWSTR lpExistingFileName, IN LPSECURITY_ATTRIBUTES lpSecurityAttributes, IN HANDLE hTransaction);
BOOL WINAPI stubCreateSymbolicLinkA (IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags);
BOOL WINAPI stubCreateSymbolicLinkTransactedA(IN LPCSTR lpSymlinkFileName, IN LPCSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction);
BOOL WINAPI stubCreateSymbolicLinkTransactedW(IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags, IN HANDLE hTransaction);
BOOL WINAPI stubCreateSymbolicLinkW (IN LPCWSTR lpSymlinkFileName, IN LPCWSTR lpTargetFileName, IN DWORD dwFlags);
BOOL WINAPI stubDeleteFileTransactedA(IN LPCSTR lpFileName, IN HANDLE hTransaction);
BOOL WINAPI stubDeleteFileTransactedW(IN LPCWSTR lpFileName, IN HANDLE hTransaction);
HANDLE WINAPI stubFindFirstFileNameA(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName);
HANDLE WINAPI stubFindFirstFileNameTransactedA(IN LPCSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PSTR LinkName, IN HANDLE hTransaction);
HANDLE WINAPI stubFindFirstFileNameTransactedW(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName, IN HANDLE hTransaction);
HANDLE WINAPI stubFindFirstFileNameW(IN LPCWSTR lpFileName, IN DWORD dwFlags, OUT LPDWORD StringLength, OUT PWSTR LinkName);
HANDLE WINAPI stubFindFirstFileTransactedA(IN LPCSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction);
HANDLE WINAPI stubFindFirstFileTransactedW(IN LPCWSTR lpFileName, IN FINDEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFindFileData, IN FINDEX_SEARCH_OPS fSearchOp, IN LPVOID lpSearchFilter, IN DWORD dwAdditionalFlags, IN HANDLE hTransaction);
HANDLE WINAPI stubFindFirstStreamA(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPVOID lpFindStreamData, DWORD dwFlags);
HANDLE WINAPI stubFindFirstStreamTransactedA(IN LPCSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPVOID lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction);
HANDLE WINAPI stubFindFirstStreamTransactedW(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPVOID lpFindStreamData, IN DWORD dwFlags, IN HANDLE hTransaction);
HANDLE WINAPI stubFindFirstStreamW(IN LPCWSTR lpFileName, IN STREAM_INFO_LEVELS InfoLevel, OUT LPVOID lpFindStreamData, DWORD dwFlags);
BOOL WINAPI stubFindNextFileNameA(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PSTR LinkName);
BOOL WINAPI stubFindNextFileNameW(IN HANDLE hFindStream, OUT LPDWORD StringLength, OUT PWSTR LinkName);
BOOL WINAPI stubFindNextStreamA(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAA lpFindStreamData);
BOOL WINAPI stubFindNextStreamW(IN HANDLE hFindStream, OUT LPWIN32_FIND_STREAM_DATAW lpFindStreamData);
DWORD WINAPI stubGetCompressedFileSizeTransactedA(IN LPCSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction);
DWORD WINAPI stubGetCompressedFileSizeTransactedW(IN LPCWSTR lpFileName, IN LPDWORD lpFileSizeHigh, IN HANDLE hTransaction);
BOOL WINAPI stubGetFileAttributesTransactedA(IN LPCSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction);
BOOL WINAPI stubGetFileAttributesTransactedW(IN LPCWSTR lpFileName, IN GET_FILEEX_INFO_LEVELS fInfoLevelId, OUT LPVOID lpFileInformation, IN HANDLE hTransaction);
DWORD WINAPI stubGetFinalPathNameByHandleA(IN HANDLE hFile, OUT LPSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags);
DWORD WINAPI stubGetFinalPathNameByHandleW(IN HANDLE hFile, OUT LPWSTR lpszFilePath, IN DWORD cchFilePath, IN DWORD dwFlags);
DWORD WINAPI stubGetFullPathNameTransactedA(IN LPCSTR lpFileName, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart, IN HANDLE hTransaction);
DWORD WINAPI stubGetFullPathNameTransactedW(IN LPCWSTR lpFileName, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart, IN HANDLE hTransaction);
DWORD WINAPI stubGetLongPathNameTransactedA(IN LPCSTR lpszShortPath, OUT LPSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction);
DWORD WINAPI stubGetLongPathNameTransactedW(IN LPCWSTR lpszShortPath, OUT LPWSTR lpszLongPath, IN DWORD cchBuffer, IN HANDLE hTransaction);
BOOL WINAPI stubMoveFileTransactedA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction);
BOOL WINAPI stubMoveFileTransactedW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags, IN HANDLE hTransaction);
BOOL WINAPI stubMoveFileWithProgressA(IN LPCSTR lpExistingFileName, IN LPCSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags);
BOOL WINAPI stubMoveFileWithProgressW(IN LPCWSTR lpExistingFileName, IN LPCWSTR lpNewFileName, IN LPPROGRESS_ROUTINE lpProgressRoutine, IN LPVOID lpData, IN DWORD dwFlags);
BOOL WINAPI stubRemoveDirectoryTransactedA(IN LPCSTR lpPathName, IN HANDLE hTransaction);
BOOL WINAPI stubRemoveDirectoryTransactedW(IN LPCWSTR lpPathName, IN HANDLE hTransaction);
BOOL WINAPI stubReplaceFileA(IN LPCSTR lpReplacedFileName, IN LPCSTR lpReplacementFileName, IN LPCSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved);
BOOL WINAPI stubReplaceFileW(IN LPCWSTR lpReplacedFileName, IN LPCWSTR lpReplacementFileName, IN LPCWSTR lpBackupFileName, IN DWORD dwReplaceFlags, IN LPVOID lpExclude, IN LPVOID lpReserved);
DWORD WINAPI stubSearchPathA(IN LPCSTR lpPath, IN LPCSTR lpFileName, IN LPCSTR lpExtension, IN DWORD nBufferLength, OUT LPSTR lpBuffer, OUT LPSTR *lpFilePart);
DWORD WINAPI stubSearchPathW(IN LPCWSTR lpPath, IN LPCWSTR lpFileName, IN LPCWSTR lpExtension, IN DWORD nBufferLength, OUT LPWSTR lpBuffer, OUT LPWSTR *lpFilePart);
BOOL WINAPI stubSetFileAttributesTransactedA(IN LPCSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction);
BOOL WINAPI stubSetFileAttributesTransactedW(IN LPCWSTR lpFileName, IN DWORD dwFileAttributes, HANDLE hTransaction);
BOOL WINAPI stubSetSearchPathMode(IN DWORD Flags);
BOOL WINAPI stubWow64DisableWow64FsRedirection(OUT PBOOL pbOldValue);
BOOL WINAPI stubWow64EnableWow64FsRedirection(IN BOOL bWow64FsEnableRedirection);
BOOL WINAPI stubWow64RevertWow64FsRedirection(OUT PBOOL pbOldValue);

#ifdef __cplusplus
}
#endif

#endif /* __NTFSLPSP_H__ */
