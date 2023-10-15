# Ship of Harkinian Android Port
A port of Ship of Harkinian to Android. <br>

Original Repository: https://github.com/HarbourMasters/Shipwright <br>
<br>

NOTE: Controller only. No touch controls yet except for in the enhancements menu. <br>

Supported (probably): Android 4.3+ (OpenGL ES 3.0 + required) <br>
Tested On: Android 10 and Android 13 <br>

<h3>Installation:</h3>
1. Install the apk from here: [Releases](https://github.com/Waterdish/Shipwright-Android/releases/) (Use soh.storage.apk if on Android 13 as scoped storage requires an extra permission) <br>
2. Run one of the PC releases to generate an oot.otr and/or oot-mq.otr file. After launching the game on PC, you will be able to find these files in the same directory as soh.exe or soh.appimage. On macOS, these files can be found in /Users/<username>/Library/Application Support/com.shipofharkinian.soh/ <br>
3. Transfer the oot.otr(and/or oot-mq.otr) and soh.otr files to Android/data/com.dishii.soh/files/. <br>
4. Launch the game <br>
<br>

Use Back/Select/- controller button to open Enhancements menu. Use touch controls to navigate menus. <br>

<h3>FAQ:</h3>
Q: Why does it say no assets/extractor folder exists?<br>
  A: The app is checking for a rom file to extract the assets of. This currently does not work. If you see this message then you have not placed oot.otr and/or soh.otr in Android/data/com.dishii.soh/files/. If you are on Android 13, make sure you are using the soh.storage.apk file. <br> <br>

Q: Why are these changes not in the main Shipwright repository?<br>
  A: This started as just an experiment that ended up working. This code is not in a state to be merged with the main repository, but I welcome anybody to take code from this repo to help add the port to the main repository. The main changes made were pointing the app to search in the ExternalDataDir and editing the libultraship opengles backend to support opengl es 3.<br> <br>

<h3>Build Instructions:</h3><br>
1.Edit the app/build.gradle file to point to your ndk folder. NDK 26+ tested as working.<br>
2.Open the project in android studio and build.<br>
