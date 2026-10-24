# atrim
**A legal, unmodified and DRM-free copy of *Amnesia: The Dark Descent* is required.**

This is atrim, a custom client for *Amnesia: The Dark Descent* which attempts to port the game to very old and weak machines, and make it compatible with operating systems as early as Windows 2000.

The client is based on version 1.4.3 of the game and it also brings out new enhancements and features from *Amnesia: A Machine for Pigs*, as well as fixes and improvements to bugs and glitches in the game and engine.

atrim is currently being maintained by one person ([@RoozerXC](https://github.com/roozerxc)). If you want to contribute to atrim, feel free to fork and submit your own pull requests, as long as everything follows the [Code of Conduct (or Anti-CoC)](CODE-OF-CONDUCT.md).

> [!IMPORTANT]
> - **This port is based on the official *Amnesia: The Dark Descent* version 1.4.3 (1.41b) source code release from October 12th, 2020.**
> - **This port now works with mods made for version 1.5, and mods installed from the Steam Workshop!**

See instructions below on how to install the atrim client and installing Steam Workshop mods for it.

For a list of changes, read [`CHANGELOG.md`](CHANGELOG.md). Special thanks are in [`THANKS.md`](THANKS.md).

## Installation Guide
### Normal Installation
1. Install *Amnesia: The Dark Descent* from DVD, Steam, GOG.com, Epic Games, or any source, provided it has the 1.2 *Justine* update.
- **Tip: You can check this after installation by searching for `ptest` in your game folder.**
2. Copy the atrim client files (found in Releases) into your *Amnesia: The Dark Descent* game folder.
3. Run `amnesia-Win32-Release.exe` for legacy systems, or `amnesia-x64-Release.exe` for modern systems.

### Installing *Amnesia: The Dark Descent - Remastered*
Installing [*Amnesia: The Dark Descent - Remastered*](https://archive.org/details/the-dark-descent-remaster-mod) is also possible with this client and it is a very easy 3-step process.
1. Backup your *Amnesia: The Dark Descent* game folder before installing *Amnesia: The Dark Descent - Remastered*.
2. Copy the *Amnesia: The Dark Descent - Remastered* files into your *Amnesia: The Dark Descent* game folder, overwrite all files.
3. Copy the atrim client files (i.e. from `v1.4.6-beta.zip`) into the game folder, overwrite all files.

### Installing Mods from the Steam Workshop
1. Go to this path: `steamapps/workshop/content/57300`
2. Copy all of the Workshop mod folders.
3. Go to the `custom_stories` folder in your game directory and paste all of them.

## Building & Debugging
### Prerequisites
- [*Creative Labs OpenAL 1.1 Core SDK*](https://openal.org/downloads)
- [*Microsoft DirectX SDK February 2010*](https://archive.org/download/dxsdk_feb10/DXSDK_Feb10.exe)
- *Microsoft Visual Studio 2005*
- [*Microsoft Visual Studio 2005 Service Pack 1*](https://archive.org/download/vs80sp1-all-langs/SP1/)
- [*Microsoft Visual Studio 2005 + Service Pack 1 Updates*](https://archive.org/download/vs80sp1-all-langs/sp1-updates/)

You will also need to [configure the *DirectX SDK* in your *Visual C++* directories](https://stackoverflow.com/a/46762539).

### Steps
#### Dependencies
> [!IMPORTANT]
> **You must compile the dependencies first before building `atrim.sln`**
1. `git clone` the repository or download it from the **Code** button.
2. Open the `HPL2/dependencies` folder and open the `dependencies.sln` solution file.
3. Press `F7` to build the solution. This will compile the dependencies needed for the `HPL2` project.

#### Engine and Game
1. Open the `atrim.sln` solution file.
2. Right click on the `amnesia` project and in **Properties** change the **Working Directory** to your *Amnesia: The Dark Descent* install folder.
3. Press `F5` to debug the solution. This will build the `HPL2` project first, then `amnesia`, and then launch the game after building.

## License
atrim is licensed under Version 3 of the GNU General Public License (GNU GPL).

Read the license information via the **GPL-3.0 license** tab on the top, or open the [`LICENSE.md`](LICENSE.md) file.

© 2009-2010 Frictional Games. Frictional, Amnesia: The Dark Descent, and the HPL Engine software are all registered trademarks of Frictional Games. All rights reserved.
