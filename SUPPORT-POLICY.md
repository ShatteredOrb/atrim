# atrim: Support Policy
atrim natively supports and targets Win32/Win64, and is compiled as such. It is the primary target for the dependencies, engine and game executable code.

### Minimum Requirements & Specifications
The minimum requirements for running atrim are a machine running Windows 2000 (Service Pack 4). This is if you are running a 32-bit executable (`amnesia-Win32-Release.exe`).

If you are running a 64-bit executable (`amnesia-x64-Release.exe`), your machine needs to run a 64-bit edition of Windows Server 2003, or Windows XP x64 Edition. atrim currently does not support the Itanium processor architecture as it is specifically made for x86 and x86-64. However, you can fork the project and try compiling the dependencies, engine and game code for Itanium.

You must also have a graphics card that supports at least OpenGL 2.0/2.1+, as it utilizes GLSL shaders. For now, atrim has been tested on multiple graphics cards; the oldest graphics card that has been tested for atrim that still works (albeit with graphical issues) is an ATi Radeon 9550 XL with 256 MB of VRAM.

atrim typically requires 512 MB of system RAM for the base game, however it is recommended to have at least 1 GB or more of RAM to give the engine more headroom, especially if you are going to be playing mods with atrim.

### Linux/Other OS Support
At this time, atrim does not natively support Linux. Not only would testing native Linux binaries for every single desktop distribution be a major pain in the ass, but it would also take too much of my time, and I would rather have that time be spent on fixing the game and making it compatible for older hardware, instead of making binaries that have the likelihood of not working later due to [ABI-breaking changes that most Linux desktop distributions often introduce](https://www.youtube.com/watch?v=Pzl1B7nB9Kc).

To keep it short, if you are running any Linux distribution or other OS (BSD/Unix-like) that supports WINE/Proton, it is best recommended to run the Win32/Win64 binaries through that.

Keep in mind that in order to execute the game through WINE/Proton you will need the respective OpenAL library packages installed for both i386 (x86) and x86-64, depending on your distribution's application package manager. You will also need to install the Windows OpenAL libraries (https://openal.org/downloads/) in your WINE/Proton environment.
