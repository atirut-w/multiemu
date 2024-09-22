# MultiEmu
An attempt at making a modular multi-machine emulator, written in C++.

## Goals
- Be easier to develop than QEMU. They don't even have an official guide on implementing a new machine.
- Be modular enough to mix and match devices, i.e. a Z80 computer with a PCI bus for some reason.
- Initial support for CP/M userland emulation.

## What I won't implement
- Cycle accuracy. QEMU isn't cycle accurate either, and it's good enough for most things. Don't expect Elite to run on this.

## Contributing
PRs are welcome, but I'd appreciate it if you could...
- Format your codes properly. I might make a CMake target for this.
- If it's not clear what a piece of code does, add a comment.
