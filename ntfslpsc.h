/* +============================================================================================+ */
/* | ##    ## ######## ######## ######## ##       ######## ########          © Mikhail Subbotin | */
/* | ###   ##    ##    ##       ##    ## ##       ##    ## ##    ##                             | */
/* | ####  ##    ##    ##       ##       ##       ##    ## ##                                   | */
/* | ## ## ##    ##    ######## ######## ##       ######## ########                             | */
/* | ##  ####    ##    ##             ## ##       ##             ##                             | */
/* | ##   ###    ##    ##       ##    ## ##       ##       ##    ##                             | */
/* | ##    ##    ##    ##       ######## ######## ##       ########                             | */
/* +============================================================================================+ */
/* | ntfslpsc.h                                                                                 | */
/* +--------------------------------------------------------------------------------------------+ */

#ifndef __NTFSLPSC_H__
#define __NTFSLPSC_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX_PATH16
#define MAX_PATH16 255
#endif

#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif

#ifndef NTAPI
#define NTAPI __stdcall
#endif

#ifndef STATUS_NOT_IMPLEMENTED
#define STATUS_NOT_IMPLEMENTED 0xC0000002
#endif

#ifndef STATUS_OBJECT_PATH_SYNTAX_BAD
#define STATUS_OBJECT_PATH_SYNTAX_BAD 0xC000003B
#endif

/*

#ifndef _COPYFILE2_MESSAGE_TYPE
typedef enum _COPYFILE2_MESSAGE_TYPE {
    COPYFILE2_CALLBACK_NONE = 0,
    COPYFILE2_CALLBACK_CHUNK_STARTED,
    COPYFILE2_CALLBACK_CHUNK_FINISHED,
    COPYFILE2_CALLBACK_STREAM_STARTED,
    COPYFILE2_CALLBACK_STREAM_FINISHED,
    COPYFILE2_CALLBACK_POLL_CONTINUE,
    COPYFILE2_CALLBACK_ERROR,
    COPYFILE2_CALLBACK_MAX,
} COPYFILE2_MESSAGE_TYPE;
#endif // COPYFILE2_MESSAGE_TYPE

#ifndef COPYFILE2_MESSAGE_ACTION
typedef enum _COPYFILE2_MESSAGE_ACTION {
    COPYFILE2_PROGRESS_CONTINUE = 0,
    COPYFILE2_PROGRESS_CANCEL,
    COPYFILE2_PROGRESS_STOP,
    COPYFILE2_PROGRESS_QUIET,
    COPYFILE2_PROGRESS_PAUSE,
} COPYFILE2_MESSAGE_ACTION;
#endif // COPYFILE2_MESSAGE_ACTION

#ifndef COPYFILE2_COPY_PHASE
typedef enum _COPYFILE2_COPY_PHASE {
    COPYFILE2_PHASE_NONE = 0,
    COPYFILE2_PHASE_PREPARE_SOURCE,
    COPYFILE2_PHASE_PREPARE_DEST,
    COPYFILE2_PHASE_READ_SOURCE,
    COPYFILE2_PHASE_WRITE_DESTINATION,
    COPYFILE2_PHASE_SERVER_COPY,
    COPYFILE2_PHASE_NAMEGRAFT_COPY,
    COPYFILE2_PHASE_MAX,
} COPYFILE2_COPY_PHASE;
#endif // COPYFILE2_COPY_PHASE

#define COPYFILE2_MESSAGE_COPY_OFFLOAD (0x00000001L)

#ifndef COPYFILE2_MESSAGE
typedef struct COPYFILE2_MESSAGE {
    COPYFILE2_MESSAGE_TYPE Type;
    DWORD dwPadding;
    union {
        struct {
            DWORD dwStreamNumber;
            DWORD dwReserved;
            HANDLE hSourceFile;
            HANDLE hDestinationFile;
            ULARGE_INTEGER uliChunkNumber;
            ULARGE_INTEGER uliChunkSize;
            ULARGE_INTEGER uliStreamSize;
            ULARGE_INTEGER uliTotalFileSize;
        } ChunkStarted;
        struct {
            DWORD dwStreamNumber;
            DWORD dwFlags;
            HANDLE hSourceFile;
            HANDLE hDestinationFile;
            ULARGE_INTEGER uliChunkNumber;
            ULARGE_INTEGER uliChunkSize;
            ULARGE_INTEGER uliStreamSize;
            ULARGE_INTEGER uliStreamBytesTransferred;
            ULARGE_INTEGER uliTotalFileSize;
            ULARGE_INTEGER uliTotalBytesTransferred;
        } ChunkFinished;
        struct {
            DWORD dwStreamNumber;
            DWORD dwReserved;
            HANDLE hSourceFile;
            HANDLE hDestinationFile;
            ULARGE_INTEGER uliStreamSize;
            ULARGE_INTEGER uliTotalFileSize;
        } StreamStarted;
        struct {
            DWORD dwStreamNumber;
            DWORD dwReserved;
            HANDLE hSourceFile;
            HANDLE hDestinationFile;
            ULARGE_INTEGER uliStreamSize;
            ULARGE_INTEGER uliStreamBytesTransferred;
            ULARGE_INTEGER uliTotalFileSize;
            ULARGE_INTEGER uliTotalBytesTransferred;
        } StreamFinished;
        struct {
            DWORD dwReserved;
        } PollContinue;
        struct {
            COPYFILE2_COPY_PHASE CopyPhase;
            DWORD dwStreamNumber;
            HRESULT hrFailure;
            DWORD dwReserved;
            ULARGE_INTEGER uliChunkNumber;
            ULARGE_INTEGER uliStreamSize;
            ULARGE_INTEGER uliStreamBytesTransferred;
            ULARGE_INTEGER uliTotalFileSize;
            ULARGE_INTEGER uliTotalBytesTransferred;
        } Error;
    } Info;
} COPYFILE2_MESSAGE;
#endif // COPYFILE2_MESSAGE

#ifndef PCOPYFILE2_PROGRESS_ROUTINE
typedef COPYFILE2_MESSAGE_ACTION(CALLBACK *PCOPYFILE2_PROGRESS_ROUTINE)(IN const COPYFILE2_MESSAGE *pMessage, IN PVOID pvCallbackContext);
#endif // PCOPYFILE2_PROGRESS_ROUTINE

#ifndef COPYFILE2_EXTENDED_PARAMETERS
typedef struct COPYFILE2_EXTENDED_PARAMETERS {
    DWORD dwSize;
    DWORD dwCopyFlags;
    BOOL *pfCancel;
    PCOPYFILE2_PROGRESS_ROUTINE pProgressRoutine;
    PVOID pvCallbackContext;
} COPYFILE2_EXTENDED_PARAMETERS;
#endif // COPYFILE2_EXTENDED_PARAMETERS

#ifndef CREATEFILE2_EXTENDED_PARAMETERS
typedef struct _CREATEFILE2_EXTENDED_PARAMETERS {
    DWORD dwSize;
    DWORD dwFileAttributes;
    DWORD dwFileFlags;
    DWORD dwSecurityQosFlags;
    LPSECURITY_ATTRIBUTES lpSecurityAttributes;
    HANDLE hTemplateFile;
} CREATEFILE2_EXTENDED_PARAMETERS, *PCREATEFILE2_EXTENDED_PARAMETERS, *LPCREATEFILE2_EXTENDED_PARAMETERS;
#endif // CREATEFILE2_EXTENDED_PARAMETERS

*/

#ifdef __cplusplus
}
#endif

#endif /* __NTFSLPSC_H__ */
