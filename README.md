![스크린샷](https://github.com/akahyulim/DiveGames/blob/master/Assets/image.png)

## Build Instructions

1. Clone the repository with submodules:<br>git clone --recursive https://github.com/akahyulim/DiveGames DiveGames
2. Update all submodules to their latest remote versions:<br>git submodule update --remote
3. Checkout the docking branch for ImGui:<br>git checkout -b docking origin/docking
4. Set the runtime library of all submodule projects to /MT or /MTd depending on your build configuration (Release/Debug).
5. Build all submodules and link their generated .lib files to the main project.
6. Build the DiveGames projects.