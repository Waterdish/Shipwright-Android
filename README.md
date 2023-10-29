# Ship of Harkinian Android Port
A port of Ship of Harkinian to Android. <br>

Original Repository: https://github.com/HarbourMasters/Shipwright <br>
<br>

NOTE: Controller only. No touch controls yet except for in the enhancements menu. <br>

Supported (probably): Android 4.3+ (OpenGL ES 3.0+ required) <br>
Tested On: Android 10 and Android 13 <br>

<h3>Installation Without PC:</h3>
1. Install the apk from here: https://github.com/Waterdish/Shipwright-Android/releases. (soh.storage.apk is recommended for Android 11+, but isn't required). <br>
2. Open the app once. It will generate the directory for your rom. Allow all file permissions and then close and reopen the app.<br>
3. Select "Yes" when prompted by the app if you would like to generate an OTR. Select "Yes" when it asks to look for a rom. Navigate to your "ZELOOTD.z64" and select it. The extraction should start.<br>
4. When asked if you would like to extract another rom, select "Yes" to choose another rom or select "No" to start the game. <br>
5. It will launch straight into the game on subsequent plays. (If you would like to get the rom selection dialog back, delete the .otr files in Android/data/com.dishii.soh/files/) <br>
<br>

<h3>Installation With PC (Backup Method):</h3>
1. Install the apk from here: https://github.com/Waterdish/Shipwright-Android/releases (Use soh.storage.apk if on Android 11+ as scoped storage requires an extra permission) <br>
2. Run this version of the PC release: https://github.com/HarbourMasters/Shipwright/actions/runs/6539782569 (Github account required) to generate an oot.otr and/or oot-mq.otr file. After launching the game on PC, you will be able to find these files in the same directory as soh.exe or soh.appimage. On macOS, these files can be found in /Users/<username>/Library/Application Support/com.shipofharkinian.soh/ <br>
3. Transfer the oot.otr(and oot-mq.otr if using master quest) to Android/data/com.dishii.soh/files/ <br>
4. Launch the game <br>
<br>
  
Use Back/Select/- controller button to open Enhancements menu. Use touch controls to navigate menus. <br>


<h3>FAQ:</h3>
Q: Why is my save file grayed out when trying to play Master Quest?<br>
  A:Extract the otr files for both the non-Master Quest and Master Quest roms. Known Bug. <br> <br>

Q: I selected the rom file, but even after 5 minutes it is still at a black screen extracting.<br>
  A: Open an issue with your Android version and device name. As a workaround, use the installation with PC method.<br> <br>
  
Q: Why is it immediately crashing? <br>
  A: Try deleting and re-extracting the OTR file (oot.otr). <br> <br>

Q: Why are these changes not in the main Shipwright repository?<br>
  A: Working on it.<br> <br>

<b>Known Bugs</b>:<br>
Both oot.otr and oot-mq.otr are required to play Master Quest<br>
Orientation Lock does not work. https://github.com/libsdl-org/SDL/issues/6090<br>


<h3>Build Instructions:</h3>
1.Edit the app/build.gradle file to point to your ndk folder. NDK 26+ tested as working.<br>
2.Open the project in android studio and build.<br>


