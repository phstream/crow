/*****************************************************************************
 * \\  __   
 * \ \(o >  
 * \/ ) |   
 *  // /    
 *   || CROW - Communicatio Retis Omni Via
 * 
 * @file      pcom.h
 * @version   1.0
 * @author    phstream
 * @copyright Copyright (c) 2025 phstream - MIT License
 * @date      1 May 2025
 * @brief     Inter-Process Communication (IPC) library header file.
 ****************************************************************************/
/** @defgroup  PCOM
 * @brief     Inter-Process Communication Client/Server.
 * @details   Cross-platform client-server IPC functions using either
 *            UNIX domain sockets (Linux/macOS) or Named Pipes (Windows). 
 *            Can be statically linked or compiled as a shared library (.so/.dll).
 *
 * @pre       common/lib_defs.h
 * @bug       -
 * @warning   -
 * @todo      Windows currently supports only one PCOM server instance at a time.
 * @ingroup   Communication
 ****************************************************************************/
#ifndef PCOM_H
#define PCOM_H

#include <stddef.h>  // for size_t

#include "../common/lib_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCOM_USER_NAME_LEN (256)
#define PCOM_MAX_GROUP_COUNT (32)
#define PCOM_GROUP_NAME_BUFFER_LEN (1024)

typedef struct {
    int is_admin;                                  // 1 if admin, 0 if not
    int group_count;                               // Number of groups
    char username[PCOM_USER_NAME_LEN];             // Username of the client
    char* group_list[PCOM_MAX_GROUP_COUNT];           // Pointers to group names
    char group_name_buffer[PCOM_GROUP_NAME_BUFFER_LEN]; // Flat buffer for names
} pcom_user_info_t;

/* ---- Common functions -------------------------------------------------- */
/**
 * @brief      Get the version of the PCOM library
 * @return     Version number as a 24-bit integer (MAJOR << 16 | MINOR << 8 | PATCH)
 * @details    This function returns the version of the PCOM library.
 */
int 
pcom_version(void);

/**
 * @brief      Get error message from error code
 * @param      error_code    The error code
 * @param      p_text_buf    The buffer to store the error message
 * @param      buf_len       The maximum length of the buffer
 * @return     Length of the error message.
 * @details    This function retrieves the error message corresponding to the
 *             given error code.
 */
LIB_EXPORT int
pcom_error_text(int error_code, char* p_text_buf, size_t buf_len);

/* ---- Server functions -------------------------------------------------- */
/**
 * @brief      Open a PCOM server for IPC communication.
 * @param      name  The name of the connection (e.g., socket path or pipe name).
 * @return     Server handle or negative error code.
 * @details    This function initializes a server-side communication endpoint.
 *             On Windows, it prepares internal state for named pipe usage.
 */
LIB_EXPORT int
pcom_server_open(const char* name);

/**
 * @brief      Accept a client connection on the server.
 * @param      server_handle  The server handle obtained from pcom_server_open().
 * @return     Client handle or negative error code.
 * @details    This function blocks until a client connects.
 *             The returned handle is a single active client session
 *             and must be closed with pcom_client_close().
 */
LIB_EXPORT int 
pcom_server_accept(int server_handle);

/**
 * @brief Checks the user information of the connected client.
 * @param client_handle  The socket or pipe handle of the client.
 * @param info           Pointer to a pcom_user_info_t struct that will be filled.
 * @return 0 on success, negative error code on failure.
 */
LIB_EXPORT int
pcom_server_check_user(int client_handle, pcom_user_info_t *info);

/**
 * @brief      Send data to the client
 * @param      client_handle  The client file handle
 * @param      buf            The buffer to send
 * @param      len            The length of the buffer
 * @return     Number of bytes written, negative error code on failure.
 * @details    This function sends data to the client.
 */
LIB_EXPORT int
pcom_server_send(int client_handle, const void* buf, size_t len); 

/**
 * @brief      Receive data from the client
 * @param      client_handle  The client file handle
 * @param      buf            The buffer to receive data into
 * @param      len            The length of the buffer
 * @return     Number of bytes read, negative error code on failure.
 * @details    This function receives data from the client.
 */
LIB_EXPORT int 
pcom_server_recv(int client_handle, void* buf, size_t len);

/**
 * @brief      Close the server handle from pcom_server_open()
 * @param      server_handle  The server handle. (0 on Windows).
 * @details    Closes the server-side listening endpoint.
 *             On Windows, this function clears internal static state 
 *             and disables further use until pcom_server_open() is called again.
 */
LIB_EXPORT void 
pcom_server_close(int server_handle);

/* ---- Client functions -------------------------------------------------- */

/**
 * @brief      Open IPC client
 * @param      name  The connection name
 * @return     Client handle or -1 on error
 * @details    This function opens a client for communication.
 */
LIB_EXPORT int
pcom_client_open(const char* name);

/**
 * @brief      Send data to the server
 * @param      client_handle  The client handle
 * @param      buf            The buffer to send
 * @param      len            The length of the buffer
 * @return     Number of bytes written, negative error code on failure.
 * @details    This function sends data to the server.
 */
LIB_EXPORT int
pcom_client_send(int client_handle, const void* buf, size_t len);

/**
 * @brief      Receive data from the server
 * @param      client_handle  The client handle
 * @param      buf            The buffer to receive data into
 * @param      len            The length of the buffer
 * @return     Number of bytes read, negative error code on failure.
 * @details    This function receives data from the server.
 */
LIB_EXPORT int 
pcom_client_recv(int client_handle, void* buf, size_t len);

/**
 * @brief      Close the client handle from pcom_client_open() or pcom_server_accept()
 * @param      client_handle  The client handle
 * @details    This function closes an active client connection.
 */
LIB_EXPORT void
pcom_client_close(int client_handle);

#ifdef __cplusplus
}
#endif

#endif // PCOM_H
