# Installing Condition Zero

## Prerequisites

Condition Zero is currently available only on Windows.

System requirements:
* Windows 7 or newer

You will need to install the following dependencies before you can install the mods themselves:

1. Install the Visual Studio 2019 re-distributable. You can find it [here](https://docs.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170). Make sure to install the x86 version. The re-distributable for 2019 is also used for several other versions, this should be noted on the download page.

2. Set up dotnet script. You can find out how to do that here: [Setting up and using dotnet script](/docs/tutorials/setting-up-and-using-dotnet-script.md)

3. Install [J.A.C.K](https://store.steampowered.com/app/496450/JACK/). This is required to compile the map sources, as the compiled maps are **not** available on the repo.

## Installing Condition Zero

1. Install the Condtion Zero: Deleted Scenes.

2. Download the latest version of this repository (no releases yet)

3. Extract the archive and copy `game_build` to your Half-Life directory and rename it to `csz`.

4. Compile the JMF map sources from `csz/mapsrc`.

5. Restart Steam so the game is added to the list of games.

## Running the game

You can run the game by either running it through Steam or by launching Half-Life with the command-line parameter `-game csz`.
