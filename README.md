# BigBaseV2
A mod menu base for Grand Theft Auto V.
Strictly for educational purposes.
[Get the original source here ](https://bitbucket.org/gir489/bigbasev2-fix/src/master/)

## Features
* ImGui–based user interface
* Unloading at runtime
* Log console
* Script fibers
* Fiber pool
* Access to YSC script globals
* scrProgram–based native hook

## Building
To build BigBaseV2 you need:
1. Visual Studio 2019
2. [Premake 5.0](https://premake.github.io/download.html) in your PATH , to set up the build environment


#### Create a new folder 
* Go to the new folder created with the cmd terminal 
* Run the following commands in the terminal:
```dos
git clone https://github.com/KennnyTroll/BBV2_PPM.git --recurse-submodules
cd BBV2_PPM
.\GenerateProjects.bat
```

#### Or 
* Unzip BBV2_PPM-main.zip  
* Delete vendor folder 
* Run Ze_Open_cmd_In_This_Dir.bat
* Run the following commands in the terminal:
```dos
git init
git submodule add https://github.com/fmtlib/fmt vendor/fmtlib
git submodule add https://github.com/nlohmann/json vendor/json
git submodule add https://github.com/TsudaKageyu/minhook vendor/MinHook
git submodule add https://github.com/ocornut/imgui vendor/ImGui
git submodule add https://github.com/KjellKod/g3log.git vendor/g3log
GenerateProjects.bat
```

## End
Now, you will be able to open the solution, and simply build it in Visual Studio.
