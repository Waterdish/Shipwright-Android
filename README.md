# Ship of Harkinian Android Port

A port of Ship of Harkinian to Android.

Original Repository: [HarbourMasters/Shipwright](https://github.com/HarbourMasters/Shipwright)

Supported (probably): Android 7.0+ (OpenGL ES 3.0+ required)

Tested On: Android 15

## Installation

Installation instructions:

1. Install the apk [from here](https://github.com/Waterdish/Shipwright-Android/releases).

2. Open the app and allow all file permissions. It will ask to set up files. Let it do its thing.

3. Select "Yes" when prompted by the app if you would like to generate an OTR. Select "Yes" when it asks to look for a rom. Navigate to your rom and select it. The extraction should start.

4. When asked if you would like to extract another rom, select "Yes" to choose another rom or select "No" to start the game.

5. It will launch straight into the game on subsequent plays. (If you would like to get the rom selection dialog back, delete the .otr files in the *SOH* folder at the root of your device storage.)

Use *Back/Select/-* controller button to open **Enhancements menu**. Use touch controls or controller to navigate menus.

## FAQ

**Q: How to add mods?**

A: "SOH" folder at root of device storage.

**Q: Why is it immediately crashing?**

A: Try deleting the "SOH" folder and having it set up the files again. Be patient at the black screens.

**Q: The game opened once, but now it's just a black screen.**

A: Delete the imgui.ini file and don't raise MSAA above 1.

**Q: Some buttons on my controller won't map.**

A: Unfortunately the only option for now is to use a different controller. Some controllers have issues with this app.

**Q: My controller is not doing anything.**

A: Have your controller plugged in before opening the app. Don't disconnect it mid-game.

## Known Bugs

- If a controller is reconnected, it will not open the Enhancements Menu.

## Build

### Build Tools

- [Ubuntu Noble Numbat | 24.04.2 LTS](https://releases.ubuntu.com/noble/)
- [CMake 3.31.5](https://github.com/Kitware/CMake/releases)
- [OpenJDK 17 | Java Development Kit](https://jdk.java.net/archive/)
- [Android Studio Koala | 2024.1.1 Patch 2 August 8, 2024](https://developer.android.com/studio/archive)
- Android SDK 31 | Android 12
- Android SDK Build-Tools 30.0.2
- Android SDK Command-Line Tools 17.0
- Android NDK 26.0.10792818
- Android Gradle Plugin (AGP) 7.0.3

**Note:** These are the tools and versions that has been tested and are supported to build the project directly. If you want to use different tools/versions, note that you would need to figure out by yourself how to setup and configure the tools/project to build with them.

### Build Instructions

1. Use a Linux base OS system (Windows build support seems to be unsupported. For Windows users, you can use WSL or a full VM).

2. Download and install OpenJDK:

    ```bash
    cd ~
    wget -O openjdk.tar.gz https://download.java.net/java/GA/jdk17.0.2/dfd4a8d0985749f896bed50d7138ee7f/8/GPL/openjdk-17.0.2_linux-x64_bin.tar.gz
    tar -xvzf openjdk.tar.gz && rm -f openjdk.tar.gz
    sudo mv jdk* /opt/jdk_17
    sudo rm -f /etc/profile.d/jdk.sh
    echo "export JAVA_HOME=/opt/jdk_17" | sudo tee /etc/profile.d/jdk.sh
    echo "export PATH=/opt/jdk_17/bin:\$PATH" | sudo tee -a /etc/profile.d/jdk.sh
    sudo chmod +x /etc/profile.d/jdk.sh
    source /etc/profile.d/jdk.sh
    ```

3. Download and install Android Studio:

    ```bash
    cd ~
    wget -O android-studio.tar.gz https://redirector.gvt1.com/edgedl/android/studio/ide-zips/2024.1.1.13/android-studio-2024.1.1.13-linux.tar.gz
    tar -xvzf android-studio.tar.gz && rm -f android-studio.tar.gz
    sudo mv android-studio /opt/android-studio
    sudo rm -f /usr/local/bin/android-studio
    sudo ln -s /opt/android-studio/bin/studio.sh /usr/local/bin/android-studio
    ```

4. Execute Android Studio:

    ```bash
    android-studio
    ```

5. Go to "Tools/SDK Manager" and install the next tools:

    - Android SDK 31 | Android 12
    - Android SDK Build-Tools 30.0.2
    - Android NDK 26.0.10792818
    - Android SDK Command-Line Tools 17.0
    - Android SDK Platform-Tools 35.0.2
    - CMake 3.31.5

6. Sync and Build the Project.
