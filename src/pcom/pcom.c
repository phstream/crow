/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      pcom.c
 * @version   1.0
 * @author    phstream
 * @copyright Copyright (c) 2025 phstream- MIT License
 * @date      1 May 2025
 * @brief     Inter-Process Communication (IPC) library source file.
 ****************************************************************************/
#define PCOM_VERSION_MAJOR 1
#define PCOM_VERSION_MINOR 0
#define PCOM_VERSION_PATCH 0

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <sddl.h>
#include <lm.h>
#include <string.h>

#endif

#include <stdio.h>

#include "pcom.h"

/* ---- Private definintions and functions -------------------------------- */

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

static inline int 
pcom_errno_from(int err) { return (err > 0) ? -err : -1;}

#define FULL_NAME_MAX (107) // Max length of struct sockaddr_un.sun_path

/** Convert server name to full path or pipe name */
static int 
build_full_name(const char* name, char* out, size_t outlen) {
    if (snprintf(out, outlen, "/var/tmp/%s.sock", name) < (int)outlen) return 0;
    out[0] = 0;
    return -ENAMETOOLONG;
}

#elif defined(_WIN32) || defined(_WIN64)

static inline int 
pcom_errno_from(DWORD err) { return (err > 0) ? -((int)err) : -1; }

#define FULL_NAME_MAX (256)

/** 
 * A Fix for the future. For now only one instance of PCOM on Windows.
 * This static full_name buffer holds the current pipe name. 
 */
static char full_name[FULL_NAME_MAX]  = { 0 };

/** Convert server name to full path or pipe name */
static int 
build_full_name(const char* name, char* out, size_t outlen) {
    if (snprintf(out, outlen, "\\\\.\\pipe\\%s", name) < (int)outlen) return 0;
    out[0] = 0;
    return -ERROR_FILENAME_EXCED_RANGE;
}

#endif

/* ---- Public functions -------------------------------------------------- */


int 
pcom_version(void) {
    return (PCOM_VERSION_MAJOR << 16) | (PCOM_VERSION_MINOR << 8) | PCOM_VERSION_PATCH;
}

int 
pcom_error_text(int error_code, char* p_text_buf, size_t buf_len)
{
    if (!p_text_buf || buf_len == 0) return -1;

    error_code = error_code < 0 ? -error_code : error_code;

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

#ifdef __GLIBC__
    // GNU strerror_r
    char* msg = strerror_r(error_code, p_text_buf, buf_len);
    if (msg) {
        if (msg != p_text_buf) {
            strncpy(p_text_buf, msg, buf_len - 1);
            p_text_buf[buf_len - 1] = '\0';
        }
        return (int)strlen(p_text_buf);
    }

#else
    // POSIX strerror_r
    if (strerror_r(error_code, p_text_buf, buf_len) == 0)
        return (int)strlen(p_text_buf);
#endif

    // Fallback
    strncpy(p_text_buf, "Unknown error", buf_len - 1);
    p_text_buf[buf_len - 1] = '\0';
    return (int)strlen(p_text_buf);

#elif defined(_WIN32) || defined(_WIN64)
    DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    DWORD len = FormatMessageA(flags, NULL, (DWORD)error_code, 0,
                               p_text_buf, (DWORD)buf_len,NULL);

    if (len > 0) {
        // Strip trailing newline
        while (len > 0 && (p_text_buf[len - 1] == '\n' || p_text_buf[len - 1] == '\r')) {
            p_text_buf[--len] = '\0';
        }
        return (int)len;
    }

    // If FormatMessage failed, set the buffer to "Unknown error"
    strncpy(p_text_buf, "Unknown error", buf_len - 1);
    p_text_buf[buf_len - 1] = '\0';
    return (int)strlen(p_text_buf);

#else
    // Unsupported platform
    int n = snprintf(p_text_buf, buf_len, "PCOM not supported on platform!");
    if (n > (int)buf_len -1) return (int) buf_len -1;
    return n;

#endif
}

int
pcom_server_open(const char* name) 
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    int result;
    int server_handle = -1;    
    char full_name[FULL_NAME_MAX];

    // Build the full connection name
    result = build_full_name(name, full_name, sizeof(full_name));

    if (result) return result;

    // Remove if already exists
    unlink(full_name);  
    
    // Create the socket address structure
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, full_name, sizeof(addr.sun_path) - 1);

    // Create the socket
    server_handle = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_handle < 0) return pcom_errno_from(errno);
    
    // Bind socket to address
    if (bind(server_handle, (struct sockaddr*)&addr, sizeof(addr)) < 0) 
        { result = errno; close(server_handle); return pcom_errno_from(result); }

    // Set socket to listen for incoming connections    
    if (listen(server_handle, 5) < 0) 
        { result = errno; close(server_handle); return pcom_errno_from(result); }
    
    return server_handle;

#elif defined(_WIN32) || defined(_WIN64)   

    // Build the full connection name
    return build_full_name(name, full_name, sizeof(full_name));
#else
    return -1;
#endif
}

int
pcom_server_accept(int server_handle)
{
    int client_handle = -1;

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        
    // Accept a client connection
    client_handle = accept(server_handle, NULL, NULL);
    if (client_handle < 0) return pcom_errno_from(errno);
    
#elif defined(_WIN32) || defined(_WIN64)

    DWORD err;
    
    // The Server handle is just a success code from pcom_server_open()
    if (server_handle != 0) return pcom_errno_from(INVALID_HANDLE_VALUE);

    // Create the named pipe
    HANDLE hPipe = CreateNamedPipeA( 
        full_name,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        4096, 4096,
        0,
        NULL
    );
    if (hPipe == INVALID_HANDLE_VALUE) return pcom_errno_from(GetLastError());

    // Wait for a client to connect
    BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE :
                     (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!connected) { err = GetLastError(); CloseHandle(hPipe); pcom_errno_from(err); }

    client_handle = (int)(intptr_t)hPipe;

#endif
    return client_handle;
}

int 
pcom_server_check_user(int client_handle, pcom_user_info_t* p_info) 
{

    /* Failed attempt info */
    strcpy(p_info->username, "unknown");
    p_info->group_count = 0;
    p_info->is_admin = 0;

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    struct ucred credentials;
    socklen_t len = sizeof(struct ucred);
    int result;

    if (getsockopt(client_handle, SOL_SOCKET, SO_PEERCRED, &credentials, &len) < 0) {
        result = errno;
        perror("getsockopt(SO_PEERCRED) failed");
        return pcom_errno_from(result);
    }

    // Lookup username
    struct passwd* p_pw = getpwuid(credentials.uid);
    if (!p_pw) {
        result = errno;
        perror("getpwuid() failed");
        return pcom_errno_from(result);
    }
    strncpy(p_info->username, p_pw->pw_name, sizeof(p_info->username) - 1);

    // Check if root (admin)
    p_info->is_admin = (credentials.uid == 0) ? 1 : 0;

    // --- Get all group memberships ---
    gid_t groups[PCOM_MAX_GROUP_COUNT];
    int ngroups = PCOM_MAX_GROUP_COUNT;

    if (getgrouplist(p_pw->pw_name, credentials.gid, groups, &ngroups) == -1) {
        result = errno;
        perror("getgrouplist() failed");
        return pcom_errno_from(result);
    }

    // Populate the buffer with group names
    p_info->group_count = 0;
    char* p_buf = p_info->group_name_buffer;
    size_t buf_len = sizeof(p_info->group_name_buffer);

    for (int i = 0; i < ngroups; ++i) {
        struct group* p_grp = getgrgid(groups[i]);
        if (p_grp) {
            size_t name_len = strlen(p_grp->gr_name) + 1;
            if (name_len < buf_len) {
                strncpy(p_buf, p_grp->gr_name, buf_len);
                p_info->group_list[p_info->group_count++] = p_buf;
                p_buf += name_len;
                buf_len -= name_len;
            } else {
                perror ("Group list overflow");
                return pcom_errno_from(EOVERFLOW);
            }
        }
    }

#elif defined(_WIN32) || defined(_WIN64)

    HANDLE hToken = NULL;
    BOOL result;
    DWORD errval, retval, size, total_size;
    TOKEN_USER* p_token_user;
    LOCALGROUP_USERS_INFO_0* p_grp_info;
    char* p_buf;
    size_t buf_len, name_len;
    char name[UNLEN + 1];

    // Impersonate the client
    if (!ImpersonateNamedPipeClient((HANDLE)(intptr_t)client_handle)) {
        errval = GetLastError();
        printf("ImpersonateNamedPipeClient failed. Error: %lu\n", errval);
        return pcom_errno_from(errval);
    }

    // Open the access token for the impersonated client
    if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken)) {
        errval = GetLastError();
        printf("OpenThreadToken failed. Error: %lu\n", errval);
        RevertToSelf();
        return pcom_errno_from(errval);
    }

    // Get the user info
    size = 0;
    GetTokenInformation(hToken, TokenUser, NULL, 0, &size);
    
    p_token_user = (TOKEN_USER *)malloc(size);
    if (!p_token_user) {
        printf("Memory allocation for TOKEN_USER failed.\n");
        RevertToSelf();
        CloseHandle(hToken);
        return pcom_errno_from(ERROR_NOT_ENOUGH_MEMORY);
    }

    result = GetTokenInformation(hToken, TokenUser, p_token_user, size, &size);
    if (!result) {
        errval = GetLastError();
        RevertToSelf();
        CloseHandle(hToken);
        free(p_token_user);
        return pcom_errno_from(errval);
    }
    
    size = sizeof(name);
    
    result = LookupAccountSidA(NULL, p_token_user->User.Sid, name, &size, NULL, 0, NULL);
    
    free(p_token_user);
    RevertToSelf();
    CloseHandle(hToken);

    if (!result)
    {
        errval = GetLastError();
        printf("LookupAccountSidA failed. Error: %lu\n", GetLastError());
        return pcom_errno_from(errval);
    }
    
    strncpy(p_info->username, name, sizeof(p_info->username));

    // Now fetch the list of local groups
    size = 0;
    total_size = 0;
    
    retval = NetUserGetLocalGroups(NULL, name, 0, LG_INCLUDE_INDIRECT, (LPBYTE*)&p_grp_info, MAX_PREFERRED_LENGTH, &size, &total_size);
    if (retval != NERR_Success) return pcom_errno_from(retval);

    p_buf = p_info->group_name_buffer;
    buf_len = sizeof(p_info->group_name_buffer);

    for (DWORD i = 0; i < size; ++i) {
        name_len = wcslen(p_grp_info[i].lgrui0_name) + 1;
        if (name_len <= buf_len) {
            wcstombs(p_buf, p_grp_info[i].lgrui0_name, name_len);
            p_info->group_list[p_info->group_count++] = p_buf;
            p_buf += name_len;
            buf_len -= name_len;
        } else  {
            NetApiBufferFree(p_grp_info);
            return pcom_errno_from(ERROR_NOT_ENOUGH_MEMORY);
        }
    }
    NetApiBufferFree(p_grp_info);
                
    // --- Check for Administrator and SOQ membership ---
    for (int i = 0; i < p_info->group_count; ++i) {
        if (strcmp(p_info->group_list[i], "Administrators") == 0) { p_info->is_admin = 1; break; }
    }

#endif

    return 0;
}



int 
pcom_server_send(int client_handle, const void* buf, size_t len) 
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

    ssize_t result;
    // Send data to the client
    result = write(client_handle, buf, len);
    if (result < 0) return pcom_errno_from(errno);
    
    return result;

#elif defined(_WIN32) || defined(_WIN64)

    DWORD written;
    
    // Send data to the client
    if (!WriteFile((HANDLE)(intptr_t)client_handle, buf, (DWORD)len, &written, NULL))
        return pcom_errno_from(GetLastError());

    return (int)written;

#else
    
    return -1;

#endif
}

int
pcom_server_recv(int client_handle, void* buf, size_t len)
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

    // Receive data from the client
    ssize_t result = read(client_handle, buf, len);
    if (result < 0) return pcom_errno_from(errno);

    return (int)result;

#elif defined(_WIN32) || defined(_WIN64)
    
    DWORD read;
    
    // Receive data from the client
    if (!ReadFile((HANDLE)(intptr_t)client_handle, buf, (DWORD)len, &read, NULL))
        return pcom_errno_from(GetLastError());

    return (int)read;

#else
    
    return -1;

#endif
}

void 
pcom_server_close(int server_handle)
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

    // Close the server handle
    close(server_handle);

#elif defined(_WIN32) || defined(_WIN64)

    // Kill the pipe name. No Server to close
    memset(full_name, 0, sizeof(full_name));

#endif
}

int
pcom_client_open(const char* name) 
{
    int client_handle = -1, result;
    char full_name[FULL_NAME_MAX];

    // Build the full connection name
    result = build_full_name(name, full_name, sizeof(full_name));
    if (result) return result;

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

    // Create the socket address structure
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, full_name, sizeof(addr.sun_path) - 1);    
    
    // Create the socket
    client_handle = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_handle < 0) return pcom_errno_from(errno);

    // Connect to the server
    if (connect(client_handle, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        { result = errno; close(client_handle); pcom_errno_from(result); }

#elif defined(_WIN32) || defined(_WIN64)
    
    // Create the named pipe
    HANDLE hPipe = CreateFileA(
        full_name,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (hPipe == INVALID_HANDLE_VALUE) pcom_errno_from(GetLastError());

    client_handle = (int)(intptr_t)hPipe;

#endif

    return client_handle;
}

int 
pcom_client_send(int client_handle, const void* buf, size_t len)
{
    // Reuse the server send function. 
    // Both handles are treated equivalently.
    return pcom_server_send(client_handle, buf, len);
}

int 
pcom_client_recv(int client_handle, void* buf, size_t len)
{
    // Reuse the server receive function
    // Both handles are treated equivalently.
    return pcom_server_recv(client_handle, buf, len);
}

void 
pcom_client_close(int client_handle) 
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)

    // Close the client
    close(client_handle);

#elif defined(_WIN32) || defined(_WIN64)

    // Close the client
    CloseHandle((HANDLE)(intptr_t)client_handle);

#endif
}
