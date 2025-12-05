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