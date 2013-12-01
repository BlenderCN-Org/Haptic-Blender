OpenColorIO 1.0.7 built by Antony Riakiotakis under MSYS using

cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=/c/src/lib/mingw32/boost -DCMAKE_INSTALL_PREFIX=/c/src/lib/mingw32/opencolorio ../
make install

"patch" from MSYS extensions was also needed.
Also, the CMakefile was altered slightly to account correctly for MinGW, specifically if(WIN32) was substituted by if(WIN32 AND NOT CMAKE_COMPILER_IS_GNUCXX) in a few cases.

Also there has been a compile error complaining about missing alloca definition. Including <malloc.h> in the file solves the issue.