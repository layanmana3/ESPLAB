# Systems Programming in C on Linux

Low-level programming in C and x86 assembly on Linux: a Unix shell, an ELF loader and analyzer, direct system calls, and binary file tools covering processes, memory, and the ELF format.

## Labs

| Lab | Topic | What it does |
|---|---|---|
| [LAB 1](LAB%201) | Function pointers | Menu-driven program that maps functions (encrypt, decrypt, print hex) over a character array using an array of function pointers. |
| [LAB 2](LAB%202) | Unix shell | A shell in C with process creation (`fork`, `execvp`, `waitpid`), I/O redirection (`dup2`), `cd`, and process control signals (`SIGCONT`, `SIGTERM`). Includes a separate pipe demo (`pipe`, `fork`). |
| [LAB 3](LAB%203) | System calls without libc | Programs that call the Linux kernel directly from x86 assembly (`int 0x80`) for `open`, `read`, `write`, `lseek`, and `exit`, without the C standard library. |
| [LAB 4](LAB%204) | Hex editor | Menu-driven tool that loads file data into memory, displays it in hexadecimal or decimal with configurable unit sizes, modifies memory, and saves changes back to the file. |
| [LAB 5](LAB%205) | ELF loader | Loads a 32-bit ELF executable by reading its program headers, mapping each loadable segment into memory with `mmap`, and starting it through a custom assembly startup and linker script. |
| [LAB B](LAB%20B) | Antivirus scanner | Loads virus signatures from a binary file into a dynamic linked list, scans a suspected file for them, and neutralizes detected viruses by patching the file. |
| [LAB E](LAB%20E) | ELF analyzer | Maps ELF files into memory with `mmap`, prints the ELF header, section names, and symbol tables, and checks and merges two relocatable object files. |

## Tech

C, x86 assembly (NASM), Linux system calls, ELF, `mmap`, processes and signals, Makefiles.

## Build

Each lab has its own `makefile`. Build and run from inside the lab's folder:

```bash
cd "LAB 2"
make
```

The labs target 32-bit Linux (i386), so building them on a 64-bit system requires the 32-bit toolchain (for example `gcc-multilib`).

---

*Developed as part of the Extended Systems Programming Lab course at Ben-Gurion University.*
