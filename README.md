# Ship of Harkinian Android Port
A port of Ship of Harkinian to Android. <br>

Original Repository: https://github.com/HarbourMasters/Shipwright <br>
<br>

NOTE: Controller only. No touch controls yet except for in the enhancements menu. <br>


<h3>Installation:</h3>
1. Install the apk from here: [Releases](https://github.com/Waterdish/Shipwright-Android/releases) (Use soh.storage.apk if on Android 13 as scoped storage requires an extra permission) <br>
2. Run one of the PC releases to generate an oot.otr and/or oot-mq.otr file. After launching the game on PC, you will be able to find these files in the same directory as soh.exe or soh.appimage. On macOS, these files can be found in /Users/<username>/Library/Application Support/com.shipofharkinian.soh/ <br>
3. Transfer the oot.otr(and/or oot-mq.otr) and soh.otr files to Android/data/com.dishii.soh/files/. <br>
4. Launch the game <br>
<br>

Use Back/Select/- controller button to open Enhancements menu. Use touch controls to navigate menus. <br>

<h3>Build Instructions:</h3><br>
1.Edit the app/build.gradle file to point to your ndk folder. NDK 26+ tested as working.<br>
2.Open the project in android studio and build.<br>
