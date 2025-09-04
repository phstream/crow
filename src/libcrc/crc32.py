#!/usr/bin/env python3
# ##############################################################################
# \\  __   
# \ \(o >  
# \/ ) |   
#  // /    
#   || CROW - Communicatio Retis Omni Via
#
# FILE:         crc32.py
# DESCRIPTION:  Python wrapper for C implemented CRC32 calculator.
#
# AUTHOR:       phstream
# LICENSE:      MIT
# VERSION:      1.0
# DATE:         July 04 2025
#
# NOTES:
# ##############################################################################
import ctypes
import os
from ctypes import c_void_p, c_size_t, c_uint32, c_int

class Crc32:
    def __init__(self) -> None:
        # Determine library name from OS type
        self.libname = "libcrc32.so" if os.name != "nt" else "crc32.dll"
        
        # Calc full path + libr name instead of LD_LIBRARY_PATH:
        _libpath = os.path.abspath(os.path.join(os.path.dirname(__file__), self.libname))
        
        try: # Load the shared lib
            self.crc = ctypes.CDLL(_libpath)
        except OSError:
            raise FileNotFoundError(f"Cannot load library {self.libname}")
        
        try: # Get library version 
            # crc32_version();
            self.crc.crc32_version.argtypes = []
            self.crc.crc32_version.restype = c_int
            self.verval = self.crc.crc32_version()
        except Exception as e:
            raise ImportError(f"Function get_version not found in {self.libname}: {e}")
            
        # Determine if correct version 1.NN.NNNN
        self.verstr = f"{self.verval >> 16}.{(self.verval >> 8) & 0xff}.{self.verval & 0xff}"
        if self.verval >= 0x20000:
            raise RuntimeError(f"Incompatible {self.libname} version: {self.verstr}, expected < 2.0.0")
        
        try: # Bind rest of library function
        
            # Function bindings:
            # uint32_t crc32_initialize();
            self.crc.crc32_initialize.argtypes = []
            self.crc.crc32_initialize.restype = c_uint32

            # uint32_t crc32_finalize(uint32_t crc_in);
            self.crc.crc32_finalize.argtypes = [c_uint32]
            self.crc.crc32_finalize.restype = c_uint32

            # uint32_t crc32_update(const uint8_t* p_data, size_t length, uint32_t crc_in);
            self.crc.crc32_update.argtypes = [c_void_p, c_size_t, c_uint32]
            self.crc.crc32_update.restype = c_uint32

            # uint32_t crc32_calc(const uint8_t* p_data, size_t length);
            self.crc.crc32_calc.argtypes = [c_void_p, c_size_t]
            self.crc.crc32_calc.restype = c_uint32

        except Exception as e:
            raise ImportError(f"Function bind mismatch in {self.libname}: {e}")        

    def version(self) -> int:
        return self.verval

    def version_str(self) -> str:
        return self.verstr
    
    # Expose Python functions
    def initialize(self) -> int:
        try:
            return self.crc.crc32_initialize()
        except Exception as e:
            raise RuntimeError(f"{self.libname} initialize() failed: {e}")
        
    def finalize(self, crc_in: int) -> int:
        try:
            return self.crc.crc32_finalize(crc_in)
        except Exception as e:
            raise RuntimeError(f"{self.libname} finalize() failed: {e}")

    def update(self, data: bytes, crc_in: int = None) -> int:
        if not isinstance(data, (bytes, bytearray)):
            raise TypeError(f"{self.libname} update(): data must be bytes or bytearray")
        
        if crc_in is None:
            crc_in = self.initialize()

        try:
            return self.crc.crc32_update(data, len(data), crc_in)
        except Exception as e:
            raise RuntimeError(f"{self.libname} update() failed: {e}")

    def calc(self, data: bytes) -> int:
        if not isinstance(data, (bytes, bytearray)):
            raise TypeError(f"{self.libname} calc(): data must be bytes or bytearray")
        
        try:
            return self.crc.crc32_calc(data, len(data))
        except Exception as e:
            raise RuntimeError(f"{self.libname} calc() failed: {e}")

