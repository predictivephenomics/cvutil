@echo off
set CONDA_PREFIX=%PREFIX%
mkdir build\conda
cd build\conda
cmake -DCMAKE_INSTALL_PREFIX=%PREFIX% ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_TOOLCHAIN_FILE=%CONDA_PREFIX%\lib\cmake\Qt6\qt.toolchain.cmake ^
      -G "MinGW Makefiles" ^
      ..\..
cmake --build . --target install
cmake --install . --prefix %PREFIX%
