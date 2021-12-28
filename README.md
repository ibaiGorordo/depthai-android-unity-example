# depthai-android-unity-example
 Unity project (built with Unity 2020.3.25f1) with an example on how to run the depthai library in Android.

![!depthai unity example android](https://github.com/ibaiGorordo/depthai-android-unity-example/blob/main/doc/img/depthai_unity_android_example.gif)

# Important 
- This example is a simple example to show how to use the depthai library in Android with Unity. For a proper support of the library in Unity, check the official **depthai unity** library (link below).
- Known issue: it is necessary to allow the app to acces the MyriadX (multiple times) every time the app launches. If you have any solutions, please submit a PR.

# Repository Info
The repository contains both the Unity project to build the app and the Android Studio project to build the Android library (.aar) for Unity.
- **Unity Depthai Android Example**: Unity project with the example scene to build the Android app. **Important** - Switch to Android before building.
- **depthai-android-api**: Android Studio project to buid the Android library (.aar) for Unity. The project contains and empty app (ignore it) and an Android library **depthai-android-api**. Press the hammer (Make project) in Android Studio to build the library. The compiled library (.aar) will be automatically copied into the Plugins folder of the Unity project.

# Android JNI Example
- A similar example but using Android JNI instead of Unity: https://github.com/ibaiGorordo/depthai-android-jni-example

# Dynamic Library
The [arm64 dynamic library](https://github.com/ibaiGorordo/depthai-android-jni-example/blob/main/app/src/main/libs/depthai/arm64-v8a/libdepthai-core.so) (depthai-core) was compiled in Ubuntu 20.04 using the *xlink_device_search_improvements* branch. If you are interested in compiling it yourself, please ask in the Discord channel referenced below.

# License
I have copied directly some of the include folders of the dependency libraries for the Android library. Therefore, all the code except the ones inside the include folder of the Android project are free to use. I will try to fix it adding submodules in the future.

# References:
- *depthai-core*: https://github.com/luxonis/depthai-core. To build the depthai library for Android, particularly, the *xlink_device_search_improvements* branch.
- *depthai unity*: https://github.com/luxonis/depthai-unity. Official Unity plugin for depthai.
- *depthai-android-jni-example*:https://github.com/ibaiGorordo/depthai-android-jni-example. Similar example using the Android JNI.
- *Luxonis Community Discord*: https://discord.gg/tPczgVdk. Check there to get updates and other information about the products from Luxonis.
- *Colormap function*: https://www.particleincell.com/2014/colormap. Used to draw the disparity by modifying the Short Rainbow example.


