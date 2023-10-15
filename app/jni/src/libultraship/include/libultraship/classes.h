#pragma once
#ifdef __cplusplus

#ifndef _LIBULTRASHIP_CLASSES_H
#define _LIBULTRASHIP_CLASSES_H

#include "resource/Archive.h"
#include "resource/ResourceManager.h"
#include "Context.h"
#include "window/Window.h"
#include "debug/Console.h"
#include "debug/CrashHandler.h"
#include "config/ConsoleVariable.h"
#include "config/Config.h"
#include "window/gui/ConsoleWindow.h"
#include "window/gui/GameOverlay.h"
#include "window/gui/Gui.h"
#include "window/gui/GuiMenuBar.h"
#include "window/gui/GuiElement.h"
#include "window/gui/GuiWindow.h"
#include "window/gui/InputEditorWindow.h"
#include "window/gui/StatsWindow.h"
#include "controller/Controller.h"
#include "controller/SDLController.h"
#include "controller/ControlDeck.h"
#include "controller/KeyboardController.h"
#include "controller/KeyboardScancodes.h"
#include "controller/DummyController.h"
#include "utils/binarytools/BinaryReader.h"
#include "utils/binarytools/MemoryStream.h"
#include "utils/binarytools/BinaryWriter.h"
#include "audio/Audio.h"
#include "audio/AudioPlayer.h"
#if defined(__linux__) || defined(__BSD__)
#include "audio/PulseAudioPlayer.h"
#elif defined(_WIN32)
#include "audio/WasapiAudioPlayer.h"
#endif
#include "audio/SDLAudioPlayer.h"
#ifdef __APPLE__
#include "utils/OSXFolderManager.h"
#endif
#ifdef __SWITCH__
#include "port/switch/SwitchImpl.h"
#endif
#ifdef __WIIU__
#include "port/wiiu/WiiUImpl.h"
#include "port/wiiu/WiiUController.h"
#include "port/wiiu/WiiUGamepad.h"
#endif
#endif
#endif
