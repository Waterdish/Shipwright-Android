# Ship of Harkinian Android Port

A port of Ship of Harkinian to Android.

Original Repository: [HarbourMasters/Shipwright](https://github.com/HarbourMasters/Shipwright)

Supported (probably): Android 4.3+ (OpenGL ES 3.0+ required)

Tested On: Android 10 and Android 13

## Installation

Installation instructions:

1. Install the apk [from here](https://github.com/Waterdish/Shipwright-Android/releases).

2. Open the app once. It will generate the directory for your rom. Allow all file permissions and then close and reopen the app.

3. Select "Yes" when prompted by the app if you would like to generate an OTR. Select "Yes" when it asks to look for a rom. Navigate to your *"ZELOOTD.z64"* and select it. The extraction should start.

4. When asked if you would like to extract another rom, select "Yes" to choose another rom or select "No" to start the game.

5. It will launch straight into the game on subsequent plays. (If you would like to get the rom selection dialog back, delete the .otr files in *Android/data/com.dishii.soh/files/*)

Use *Back/Select/-* controller button, or the *Android back button* (swipe left if using gesture controls) to open **Enhancements menu**. Use touch controls to navigate menus.

## FAQ

**Q: Why is it immediately crashing?**

A: Try deleting and re-extracting the OTR file (oot.otr).

**Q: The game opened once, but now it's just a black screen.**

A: Reinstall and change MSAA to 1 in Settings->Graphics

**Q: I can't map the C buttons because the window is cut off.**

A: Go to Settings->Graphics->ImGui Menu Scale and change it from X-Large to a smaller value.

**Q: Some buttons on my controller won't map.**

A: Unfortunately the only option for now is to use a different controller. Some controllers have issues with this app.

**Q: My controller is not doing anything.**

A: Close the Enhancements Menu. If the Enhancements Menu is not open, open it with the Android back button and check if it is detected in Settings->Controller->Controller Mapping. If it is, press refresh.

**Q: Why are these changes not in the main Shipwright repository?**

A: Working on it.

## Known Bugs

- [Orientation Lock does not work.](https://github.com/libsdl-org/SDL/issues/6090)
- Near-plane clipping when the camera is close to walls.

## Build Instructions

1. Edit the *app/build.gradle* file to point to your ndk folder. *NDK 26+* tested as working.
2. Open the project in android studio and build.
