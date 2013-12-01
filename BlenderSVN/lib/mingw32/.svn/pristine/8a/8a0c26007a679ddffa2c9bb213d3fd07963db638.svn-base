cmake ..\oiio\src\ -G "MinGW Makefiles" ^
	-DCMAKE_CXX_FLAGS="-march=i686" ^
	-DBUILDSTATIC=1 ^
	-DBOOST_CUSTOM=0 ^
	-DBOOST_ROOT=C:\src\lib\mingw32\boost\ ^
	-DLINKSTATIC=ON ^
	-DILMBASE_HALF_LIBRARIES=..\lib\mingw32\openexr\lib\libHalf.a ^
	-DILMBASE_IMATH_LIBRARIES=..\lib\mingw32\openexr\lib\libImath.a ^
	-DILMBASE_ILMTHREAD_LIBRARIES=..\lib\mingw32\openexr\lib\libIlmThread.a ^
	-DILMBASE_IEX_LIBRARIES=..\lib\mingw32\openexr\lib\libIex.a ^
	-DOPENEXR_ILMIMF_LIBRARIES=..\lib\mingw32\openexr\lib\libIlmImf.a ^
	-DILMBASE_INCLUDE_DIR=..\lib\mingw32\openexr\include ^
	-DOPENEXR_INCLUDE_DIR=..\lib\mingw32\openexr\include ^
	-DZLIB_INCLUDE_DIR=..\lib\mingw32\zlib\include ^
	-DZLIB_LIBRARY=..\lib\mingw32\zlib\lib\libz.a ^
	-DPNG_LIBRARY=..\lib\mingw32\png\lib\libpng.a ^
	-DPNG_PNG_INCLUDE_DIR=..\lib\mingw32\png\include ^
	-DJPEG_LIBRARY=..\lib\mingw32\lib\libjpeg.lib ^
	-DJPEG_INCLUDE_DIR=..\lib\mingw32\jpeg\include ^
	-DTIFF_LIBRARY=..\lib\mingw32\tiff\lib\libtiff.lib ^
	-DTIFF_INCLUDE_DIR=..\lib\mingw32\tiff\include ^
	-DCMAKE_INSTALL_PREFIX=..\lib\mingw32\openimageio ^
	-DUSE_OPENGL=OFF ^
	-DUSE_TBB=OFF ^
	-DUSE_FIELD3D=OFF ^
	-DUSE_QT=OFF ^
	-DUSE_PYTHON=OFF ^
	-DCMAKE_BUILD_TYPE=Release
	
mingw32-make install