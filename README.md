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
* Visual Studio 2019
* [Premake 5.0](https://premake.github.io/download.html) in your PATH
* To set up the build environment


## create a new folder 
* run the following commands in the terminal:
```dos
git clone https://github.com/KennnyTroll/BBV2-Fix.git --recurse-submodules
GenerateProjects.bat
```

## or 
* unzip BBV2_PPM-main.zip  
* delete vendor folder 
* run Ze_Open_cmd_In_This_Dir.bat
* run the following commands in the terminal:
```dos
git init
git submodule add https://github.com/fmtlib/fmt vendor/fmtlib
git submodule add https://github.com/nlohmann/json vendor/json
git submodule add https://github.com/TsudaKageyu/minhook vendor/MinHook
git submodule add https://github.com/ocornut/imgui vendor/ImGui
git submodule add https://github.com/KjellKod/g3log.git vendor/g3log
GenerateProjects.bat
```

Now, you will be able to open the solution, and simply build it in Visual Studio.
