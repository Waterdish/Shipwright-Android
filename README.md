# Ship of Harkinian Android Port
A port of Ship of Harkinian to Android. <br>

Original Repository: https://github.com/HarbourMasters/Shipwright <br>
<br>

NOTE: Controller only. No touch controls yet except for in the enhancements menu. <br>

Supported (probably): Android 4.3+ (OpenGL ES 3.0 + required) <br>
Tested On: Android 10 and Android 13 <br>

<h3>Installation With PC (Any Device) :</h3>
1. Install the apk from here: https://github.com/Waterdish/Shipwright-Android/releases (Use soh.storage.apk if on Android 11+ as scoped storage requires an extra permission) <br>
2. Run this version of the PC release: https://github.com/HarbourMasters/Shipwright/actions/runs/6539782569 to generate an oot.otr and/or oot-mq.otr file. After launching the game on PC, you will be able to find these files in the same directory as soh.exe or soh.appimage. On macOS, these files can be found in /Users/<username>/Library/Application Support/com.shipofharkinian.soh/ <br>
3. Transfer the oot.otr(and oot-mq.otr if using master quest) to Android/data/com.dishii.soh/files/ <br>
4. Launch the game <br>
<br>

<h3>Installation Without PC (Lower Compatibility):</h3>
1. Install the apk from here: https://github.com/Waterdish/Shipwright-Android/releases. Use the rompicker version if on Android 11+ <br>
2. Open the app once. It will generate the directory for your rom. <br>
3.<b>If using soh.apk:</b> Place your compatible "ZELOOTD.z64" rom (check here: https://ship.equipment/) at Android/data/com.dishii.soh/files/ <br>
<b>If using soh.storage.rompicker.apk:</b> Select your "ZELOOTD.z64" rom wherever you have it. <br>
4. Select "Yes" when prompted by the app if you would like to generate an OTR. Select "Yes" when it finds your rom. Your screen will go black for approximately 30 seconds. It could take longer depending on your device. <br>
5. When asked if you would like to extract another rom, select "No" to start the game. <br>
6. It should launch straight into the game the next time you open the app. <br>
<br>
  
Use Back/Select/- controller button to open Enhancements menu. Use touch controls to navigate menus. <br>


<h3>FAQ:</h3>
Q: Why does it say No Rom Found?<br>
  A: If you are on Android 10 or below, your rom file was not placed at Android/data/com.dishii.soh/files/. Check to make sure it is there next to soh.otr. If you are on Android 11+, you need to use the rompicker version. <br> <br>
  
Q: Why is it immediately crashing? <br>
  A: Try deleting and re-extracting the OTR file (oot.otr). <br> <br>

Q: Why are these changes not in the main Shipwright repository?<br>
  A: Working on it.<br> <br>

<h3>Build Instructions:</h3>
1.Edit the app/build.gradle file to point to your ndk folder. NDK 26+ tested as working.<br>
2.Open the project in android studio and build.<br>


