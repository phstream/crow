```
   \\  __
   \ \(o >
   \/ ) |
    // /
     || CROW Communicatio Retis Omni Via
```

# libcrc

CRC library for 8, 16 and 32 checksums.

You can build it as a shared library (`.so` / `.dll`), or include the source directly in your project.  
Wrappers for Python and C++ are planned.

---

## Concept

Simple CRC calculation

- Start calculation by calling initialize which gives you the starting sum.
- Update the checksum a block of data at a time.
- End the calculation with a finalize call.

Or just call calc to do it in one sweep.

---

## Files

- `Makefile` – Targets to build the library and test tools
- `crcNN.h` – Public C header
- `crcNN.c` – Implementation
- `crc32.py`- Python libcrc32 wrapper
- `crc32builder`- C++ libcrc32 wrapper
- `test_crcNN.c`, `test_crcNN.py` – Tests and examples

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for full legalese.

### Human version:
- Use it freely.
- Don’t sue me.
- If it breaks, it’s your screwdriver now.
- If you’re a company making money off it — you should at least buy your team a beer.
- This project was built to help, not harm. Please don’t be evil with it.

**P.S.** If this code saved your butt and we meet in person — first round’s on you.

---

## Contact

GitHub: [@phstream](https://github.com/phstream)
Issues and contributions welcome.
