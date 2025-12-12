# AppImage Packaging Guide

This tutorial describes the ways to create AppImage for the supported platforms.

## Linux

Linux is the primary target for AppImage executables

## Overview

There are several tools to create an AppImage from the binaries of an application.
For a detailed description on what tools are available and how they can be used have a look at
[Welcome to the AppImage documentation](https://docs.appimage.org).
For the specification see [Packaging Guide](https://docs.appimage.org/packaging-guide/index.html).

## Our procedure

We describe here our current procedure to create from our sources the AppImages that we release. We use a mix of several tools to obtain what we have found to be a reliable way to built these packages. 

This is still work in progress, all corrections and improvements are welcome

* **Create build directory structure**

In an empty directory, create directories "AppDir" and "build" and "tools"

### Download AppImage tools

* Download *linuxdeploy-x86_64.AppImage* from : https://github.com/linuxdeploy/linuxdeploy/releases/  
* Download *linuxdeploy-plugin-qt-x86_64.AppImage* from https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases  
* Download *appimagetool-x86_64.AppImage* from https://github.com/AppImage/appimagetool/releases


* *~/tools/linuxdeploy-x86_64.AppImage*  
* *~/tools/linuxdeploy-plugin-qt-x86_64.AppImage*  
* *~/tools/appimagetool-x86_64.AppImage*  


### Compile from source

* Get the source

`git clone https://codeberg.org/xCAD/FreeCAD11.git`  
rename the downloaded directory to FreeCAD11_src  

* Set BUILD options

Software version is set in the file :  
*~/CMakeLists.txt*

Build options are set in the file :  
*~/cMake/FreeCAD_Helpers/InitializeFreeCADBuildOptions.cmake*

* Configure

```
cd ~/build  

cmake ../FreeCAD11_src              \  
    -DCMAKE_BUILD_TYPE=Release      \  
    -DCMAKE_INSTALL_PREFIX=/usr     \  
    -DBUILD_ASSEMBLY=OFF            \  
    -DBUILD_TEST=OFF                \  
    -DENABLE_DEVELOPER_TESTS=OFF  
```

* Compile

If cmake didn't finish with errors, compile :  
`make -j$(nproc --ignore=2)`

* Test

Check that the compiled executable works as expected by running :

`./bin/FreeCAD`


### Install

* Install into *AppDir*

`DESTDIR=../AppDir make install`

* move everything from `~/AppDir/usr/lib/x86_64-linux-gnu` to `~/AppDir/usr/lib` , because with the install target it may put all your shared libraries into the directory lib/x86_64-linux-gnu. In order to get a working image you have to move them one level up so that they are directly inside lib.


[//]: # (not sure about the following :  )
[//]: # (cd ..  )
[//]: # (mkdir ~/AppDir/usr/lib/qml)



### Make metadata

* create / rename (use only lowercase names !)  

_./AppDir/usr/share/applications/freecad.desktop_

* set 

`Icon=freecad`

* create / copy

./AppDir/usr/share/icons/hicolor/scalable/apps/freecad.svg  
./AppDir/usr/share/icons/hicolor/128x128/apps/freecad.png  
./AppDir/freecad.svg  


### run *linuxdeploy* and *linuxdeploy-plugin-qt*

[//]: # (export QML_SOURCES_PATHS=./AppDir/usr/lib/qml)

```
export LD_LIBRARY_PATH=./build/lib/:./AppDir/usr/lib/

./tools/linuxdeploy-x86_64.AppImage                            \
    --appdir AppDir                                            \
    -d AppDir/usr/share/applications/freecad.desktop           \
    -i AppDir/usr/share/icons/hicolor/128x128/apps/freecad.png \
    --output appimage

./tools/linuxdeploy-plugin-qt-x86_64.AppImage --appdir AppDir
```

Copy Python distribution files
------------------------------

* copy /usr/lib/python3.11 to ./AppImageDir/usr/lib  
* copy /usr/lib/python3 to ./AppImageDir/usr/lib  
* remove all `*.pyc` files and `__pycache__` directories  
* saves 200Mb  

```
cd AppDir/usr/lib
find -name *.pyc | xargs rm
find -name __pycache__ | xargs rmdir
```

* go back : `cd ../..`

* run linuxdeploy again

```
./tools/linuxdeploy-x86_64.AppImage                            \
    --appdir AppDir                                            \
    -d AppDir/usr/share/applications/freecad.desktop           \
    -i AppDir/usr/share/icons/hicolor/128x128/apps/freecad.png \
    --output appimage
```

* check that it starts

`./FreeCAD-x86_64.AppImage`


### Adjust manually

* extract the content of the _AppImage_ (will create a directory _squashfs-root_)

`./FreeCAD-x86_64.AppImage --appimage-extract`

* add to _./squashfs-root/AppRun_

```
export PREFIX=${this_dir}/usr
export LD_LIBRARY_PATH=${this_dir}/usr/lib
export QT_QPA_PLATFORM=xcb
export PYTHONHOME=${this_dir}/usr
export PATH_TO_FREECAD_LIBDIR=${this_dir}/usr/lib
export FONTCONFIG_FILE=/etc/fonts/fonts.conf
export FONTCONFIG_PATH=/etc/fonts
```

* remove unwanted/problematic libraries

`rm squashfs-root/usr/lib/libheif.so.1`

* add matplotlibrc

```
mkdir -p squashfs-root/usr/lib/python3/dist-packages/matplotlib/mpl-data
cp /usr/share/matplotlib/mpl-data/matplotlibrc  squashfs-root/usr/lib/python3/dist-packages/matplotlib/mpl-data
```

[//]: # (Include Assembly4 if wanted :)
[//]: # (copy Assembly4 files to ./squashfs-root/usr/Mod)
[//]: # (remove "Ressources/Tutorial1-2-3" folders)
[//]: # (remove "Ressources/media" folder ?)


### Build AppImage

`./tools/appimagetool-x86_64.AppImage -v squashfs-root`

Will make _FreeCAD-x86_64.AppImage_

### Test and enjoy

run  

`./FreeCAD-x86_64.AppImage`



