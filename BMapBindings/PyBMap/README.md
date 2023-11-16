# PyBMap

The real scripts are placed in sub PyBMap folder. This folder is served for testbench scripts placing.  
Place any testbench files (e.g. `testbench.py`) in there what you want and don't sumbit them.  
The native BMap library should be placed in sub PyBMap folder, and I have used gitignore file to filter them.  
The native BMap library must be named as `BMap.dll` (in Windows), `BMap.so` (in Linux or BSD), or `BMap.dylib` (in macOS). If you still can not load BMap or your system is not listed above, you should name it as `BMap.bin`.
