# MultiEmu
An attempt at making a modular multi-machine emulator, written in C++.

## Goals
- Be easier to develop than QEMU. They don't even have an official guide on implementing a new machine.
- Be modular enough to mix and match devices, i.e. a Z80 computer with a PCI bus for some reason.
- Initial support for the ZX Spectrum and the Zeal 8-bot Computer. This provides the groundwork for modularity.

## What I won't implement
- Cycle accuracy. QEMU isn't cycle accurate either, and it's good enough for most things. Don't expect Elite to run on this.
