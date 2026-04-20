# Overview

This module contains the following:

- A full audio device module implementation
- A template audio device module implementation with intentionally blank functions to be filled in by workshop attendees/openDAQ students

# How to build

Each module folder contains a CMakeLists.txt that serves as the root of the CMake project. Each of those projects should be built separately. 

The modules/CMake projects have a dependency to openDAQ 3.31.0, the openDAQ packages used at this workshop can be found at: https://github.com/openDAQ/audio-device-workshop-binaries
It is recommended to first install the binaries if a version matching your complier of choice is available. Otherwise, openDAQ will be fetched and built from source.

## Requirements

- CMake version 3.25 or newer.
- A C++ compiler (MSVC, gcc > 7.3, clang > 9)
- (optional) Installed openDAQ package version 3.31.0
- (optional, if using installed openDAQ) The openDAQ installation folder is in the system path variable/LD_LIBRARY_PATH.
- (optional) openDAQ Python package v3.31.0dev0 that can be installed via: `py -m pip install -i https://test.pypi.org/simple/ opendaq==3.31.0dev`

## Environment setup guide

### Windows

To follow along the workshop, lightweight installation is recommended (smaller download size, faster installation). A full installation of Visual Studio with the C++ workload is also valid.

Instructions for the lightweight setup:
1. Download "Build Tools for Visual Studio 2026" [directly](https://aka.ms/vs/stable/vs_BuildTools.exe) or follow the steps:
  - Go to [visualstudio.microsoft.com/downloads](visualstudio.microsoft.com/downloads)
  - Scroll down to the **All Downloads** section expand "Tools for Visual Studio"
  - Download "Build Tools for Visual Studio 2026"
4. Install the build tools:
  - Run the downloaded installer
  - In the Workloads tab select "Desktop development with C++"
  - On the right hand side in the "Installation details" checklist, select two optional features: **MSVC Build Tools** and **C++ CMake Tools** (918MB download, 3.29GB disk space) and uncheck all others.
5. Download [Visual Studio Code](https://code.visualstudio.com/docs/?dv=win64user) and install it.
6. Open `audio_device_module_template` folder in Visual Studio Code and install the recommended extensions (accept the prompt):
	- C/C++ by Microsoft
	- CMake Tools by Microsoft
