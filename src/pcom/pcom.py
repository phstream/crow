#!/usr/bin/env python3
# #############################################################################
## \\  __   
## \ \(o >  
## \/ ) |   
##  // /    
##   || CROW - Communicatio Retis Omni Via
##
## @file      pcom.py
## @author    phstream
## @copyright -
## @date      1 May 2025
## @version   1.0
##
## @defgroup  PCOM
## @brief     Inter-Process Communication Client/Server Python wrapper.
## @details   Uses libpcom.so for low level communication. Has a python to
##            C interface for the PCOM library.
##
## @todo      -
## @pre       Needs libpcom.so (or libpcom.dll) to be installed in the same directory.
## @bug       -
## @warning   -
## @ingroup   Communication
# #############################################################################
import ctypes

# Load the shared library
lib = ctypes.CDLL('./libpcom.so') # Adjust the path or library type as needed

# Define constants for PcomUserInfo. must match the C definition in pcom.h
PCOM_USER_NAME_LEN = 256
PCOM_MAX_GROUP_COUNT = 32
PCOM_GROUP_NAME_BUFFER_LEN = 1024

# Define the pcom_user_info_t structure in Python
class PcomUserInfo(ctypes.Structure):
    _fields_ = [
        ("is_admin", ctypes.c_int),                  
        ("group_count", ctypes.c_int),               
        ("username", ctypes.c_char * PCOM_USER_NAME_LEN),           
        ("group_list", ctypes.c_char_p * PCOM_MAX_GROUP_COUNT),
        ("group_name_buffer", ctypes.c_char * PCOM_GROUP_NAME_BUFFER_LEN)   
    ]

# Define the function signature for pcom_init
# int pcom_version(void)
lib.pcom_version.argtypes = []
lib.pcom_version.restype = ctypes.c_int

# Define the function signature for pcom_error_text
# int pcom_error_text(int error_code, char* p_text_buf, size_t buf_len)
lib.pcom_error_text.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_size_t]
lib.pcom_error_text.restype = ctypes.c_int

# Define the function signature for pcom_server_open
# int pcom_server_open(const char* name)
lib.pcom_server_open.argtypes = [ctypes.c_char_p]
lib.pcom_server_open.restype = ctypes.c_int
                                
# Define the function signature for pcom_server_accept
# int pcom_server_accept(int server_handle)
lib.pcom_server_accept.argtypes = [ctypes.c_int]
lib.pcom_server_accept.restype = ctypes.c_int

# Define the function signature for pcom_server_check_user
# int pcom_server_check_user(int client_handle, pcom_user_info_t *info)
lib.pcom_server_check_user.argtypes = [ctypes.c_int, ctypes.POINTER(PcomUserInfo)]
lib.pcom_server_check_user.restype = ctypes.c_int

# Define the function signature for pcom_server_send
# int pcom_server_send(int client_handle, const void* buf, size_t len)
lib.pcom_server_send.argtypes = [ctypes.c_int, ctypes.c_void_p, ctypes.c_size_t]
lib.pcom_server_send.restype = ctypes.c_int

# Define the function signature for pcom_server_recv
# int pcom_server_recv(int client_handle, void* buf, size_t len)
lib.pcom_server_recv.argtypes = [ctypes.c_int, ctypes.c_void_p, ctypes.c_size_t]
lib.pcom_server_recv.restype = ctypes.c_int

# Define the function signature for pcom_server_close
# void pcom_server_close(int server_handle)
lib.pcom_server_close.argtypes = [ctypes.c_int]
lib.pcom_server_close.restype = None

# Define the function signature for pcom_client_open
# int pcom_client_open(const char* name)
lib.pcom_client_open.argtypes = [ctypes.c_char_p]
lib.pcom_client_open.restype = ctypes.c_int

# Define the function signature for pcom_client_send
# int pcom_client_send(int client_handle, const void* buf, size_t len)
lib.pcom_client_send.argtypes = [ctypes.c_int, ctypes.c_void_p, ctypes.c_size_t]
lib.pcom_client_send.restype = ctypes.c_int

# Define the function signature for pcom_client_recv
# int pcom_client_recv(int client_handle, void* buf, size_t len)
lib.pcom_client_recv.argtypes = [ctypes.c_int, ctypes.c_void_p, ctypes.c_size_t]
lib.pcom_client_recv.restype = ctypes.c_int

# Define the function signature for pcom_client_close
# void pcom_client_close(int client_handle)
lib.pcom_client_close.argtypes = [ctypes.c_int]
lib.pcom_client_close.restype = None


class PcomException(Exception):
    """ Exception class for PCOM errors.
    It is used to handle errors that occur in the PCOM library.

    Args:
        Exception (str): The error message.
    """
    pass

class Pcom_Common:
    """ Common Base class and functions for the PCOM library """

    @staticmethod
    def _check_version():
        """ Check the version of the PCOM library """
        version = lib.pcom_version()
        version_str = f"{version>>24}.{(version >> 16) & 0xFF}.{(version >> 8) & 0xFF}"
        if version != 0x10000:
            raise PcomException(f"Wrong PCOM library version {version_str}. Version 1.0.0 expected")
    
    @staticmethod
    def _get_error(error_code: int) -> str:
        """ Get the error message for a given error code """
        # Create a writable char buffer
        buf = ctypes.create_string_buffer(256)  
        lib.pcom_error_text(error_code, buf, ctypes.sizeof(buf))  
        return buf.value.decode('utf-8')
    
    @staticmethod
    def _raise_error(error_text: str, error_code: int):
        """ Raise a PcomException with the error message and code """
        error_str = Pcom_Common._get_error(error_code)
        raise PcomException(f"{error_text}: PCOM Error Code {error_code}, {error_str}", error_text, error_code, error_str)


class PcomClient(Pcom_Common):
    """ PCOM Client class for sending and receiving data """
    def __init__(self, name: str, handle: int = None, credentials: dict = None):
        """ Initialize the PCOM client with connection name.
            Optional handle and credentials is for server transfer to new connection. """
        # Check the version of the PCOM library is correct
        self._check_version()
        
        # Set the connection name
        self.name = name
        
        # If no handle is provided, open a new client connection
        # Otherwise, use the provided handle
        if handle is None:
            c_name = self.name.encode('utf-8')
            handle = self.client_handle = lib.pcom_client_open(c_name)
            if handle < 0:
                self._raise_error("fFailed to open client as '{self.name}'", handle)
        self.client_handle = handle
        
        # If no credentials are provided, set default credentials
        # Otherwise, use the provided credentials
        if credentials is None:
            credentials = {"user": "", "admin": False, "groups": []}
        self.credentials = credentials

    def send(self, data: bytes):
        """ Send data to the server """
        # Send data buffer over connection
        result = lib.pcom_client_send(self.client_handle, data, len(data))
        if result < 0:
            self._raise_error(f"Failed to send data as client '{self.name}'", result)

    def recv(self, length: int) -> bytes:
        """ Receive data from the server """
        # Create a receive buffer for incomming data
        buffer = ctypes.create_string_buffer(length)

        # Receive data buffer over connection
        result = lib.pcom_client_recv(self.client_handle, buffer, length)
        if result < 0:
            self._raise_error(f"Failed to receive data as client '{self.name}'", result)
        
        # Return the received data as bytes
        return buffer.raw

    def close(self):
        """ Close the client connection """
        lib.pcom_client_close(self.client_handle)
        self.client_handle = None


class PcomServer(Pcom_Common):
    """ PCOM Server class for accepting client connections """

    def __init__(self, name: str):
        """ Initialize the PCOM server with connection name """

        # Check the version of the PCOM library is correct
        self._check_version()

        # Set the connection name
        self.name = name
        
        # Open the server connection
        c_name = self.name.encode('utf-8')
        self.server_handle = lib.pcom_server_open(c_name)
        if self.server_handle < 0:
            self._raise_error(f"Failed to open server '{self.name}'", self.server_handle)
     
    def _get_credentials(self, handle) -> dict:
        """ Query the client for user credentials """

        # Create a PcomUserInfo structure to hold user information
        info = PcomUserInfo()
        
        # Query the client for user information
        result = lib.pcom_server_check_user(handle, ctypes.byref(info))
        if result < 0:
            self._raise_error(f"Server '{self.name}' failed to get user info", result)
        
        #Assemble a dictionary with user credentials and return it
        return {
            "user": info.username.decode('utf-8').rstrip('\0'),
            "admin": bool(info.is_admin),
            "groups": [
                info.group_list[i].decode()
                for i in range(info.group_count)
                if info.group_list[i]
            ]
        }

    def accept(self):
        """ Accept and handle incoming client connections"""

        # Wait for and accept a client connections """
        client_handle = lib.pcom_server_accept(self.server_handle)
        if client_handle < 0:
            self._raise_error(f"Server '{self.name}' Failed to accept client", self.server_handle)
        
        # Create a client object to handle further communication
        return PcomClient(self.name, client_handle, self._get_credentials(client_handle))

    def send(self, data: bytes):
        """ Send data to the client """

        # Send data buffer over connection
        result = lib.pcom_server_send(self.server_handle, data, len(data))
        if result < 0:
            self._raise_error(f"Server '{self.name}' Failed to send data", result)

        return result
    
    def recv(self, length: int) -> bytes:
        """ Receive data from the client """
        
        # Create a receive buffer for incomming data
        buffer = ctypes.create_string_buffer(length)
        
        # Wait and receive data from connection
        result = lib.pcom_server_recv(self.server_handle, buffer, length)
        if result < 0:
            self._raise_error(f"Server '{self.name}' Failed to receive data", result)
        
        # Return the received data as bytes
        return buffer.raw
    
    def close(self):
        """ Close the server """
        lib.pcom_server_close(self.server_handle)
        self.server_handle = None

if __name__ == "__main__":
    # Example usage
    server = PcomServer("pcomtest")
    print(f"Server '{server.name}' opened with handle {server.server_handle}")

    client = server.accept()
    print(f"Client '{client.name}' accepted with credentials: {client.credentials}")

    # Send and receive data
    client.send(b"Hello from python server")
    data = client.recv(1024)
    print(f"Client received: {data.decode('utf-8')}")

    # Close connections
    client.close()
    server.close()