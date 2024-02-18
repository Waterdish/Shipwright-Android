# Ship of Harkinian Android Port
A port of Ship of Harkinian to Android. <br>

Original Repository: https://github.com/HarbourMasters/Shipwright <br>
<br>

NOTE: Controller only. No touch controls yet except for in the enhancements menu. <br>

Supported (probably): Android 4.3+ (OpenGL ES 3.0+ required) <br>
Tested On: Android 10 and Android 13 <br>

<h3>Installation:</h3>
1. Install the apk from here: https://github.com/Waterdish/Shipwright-Android/releases. <br>
2. Open the app once. It will generate the directory for your rom. Allow all file permissions and then close and reopen the app.<br>
3. Select "Yes" when prompted by the app if you would like to generate an OTR. Select "Yes" when it asks to look for a rom. Navigate to your "ZELOOTD.z64" and select it. The extraction should start.<br>
4. When asked if you would like to extract another rom, select "Yes" to choose another rom or select "No" to start the game. <br>
5. It will launch straight into the game on subsequent plays. (If you would like to get the rom selection dialog back, delete the .otr files in Android/data/com.dishii.soh/files/) <br>
<br>
  
Use Back/Select/- controller button, or the Android back button (swipe left if using gesture controls) to open Enhancements menu. Use touch controls to navigate menus. <br>


<h3>FAQ:</h3>
Q: Why is it immediately crashing? <br>
  A: Try deleting and re-extracting the OTR file (oot.otr). <br> <br>

Q: The game opened once, but now it's just a black screen. <br>
  A: Reinstall and change MSAA to 1 in Settings->Graphics <br><br>

Q: I can't map the C buttons. <br>
  A: Go to Settings->Graphics->ImGui Menu Scale and change it from X-Large to a smaller value. <br><br>

Q: My controller is not doing anything. <br>
  A: Close the Enhancements Menu. If the Enhancements Menu is not open, open it with the Android back button and check if it is detected in Settings->Controller->Controller Mapping. If it is, press refresh. <br><br>

Q: Why are these changes not in the main Shipwright repository?<br>
  A: Working on it.<br> <br>

<b>Known Bugs</b>:<br>
Orientation Lock does not work. https://github.com/libsdl-org/SDL/issues/6090<br>
Near-plane clipping when the camera is close to walls.<br>

<h3>Build Instructions:</h3>
1. Edit the app/build.gradle file to point to your ndk folder. NDK 26+ tested as working.<br>
2. Open the project in android studio and build.<br>


