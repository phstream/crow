# PCOM

A simple, low-level IPC (Inter-Process Communication) library written in C.  
Supports both **UNIX domain sockets on Linux** and **Named Pipes on Windows**.

You can build it as a shared library (`.so` / `.dll`), or include the source directly in your project.  
Wrappers for Python and C++ are planned.

---

## Concept

PCOM follows a classic **Server/Client** model.

- The server listens and blocks until a client connects.
- Both ends can `open`, `send`, `recv`, and `close` connections.
- The API is intentionally minimal for easy embedding.

---

## Credentials

For basic security and access control, PCOM includes a function to:
- Retrieve the connecting user's name
- Check if the user has administrative/root privileges
- List the user's group memberships

This allows servers to implement **group-based access filters**.

---

## Files

- `Makefile` – Targets to build the library and test tools
- `pcom.h` – Public C header
- `pcom.c` – Implementation
- `test_client.c`, `test_server.c` – Example programs
- `LICENSE` – MIT License

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
