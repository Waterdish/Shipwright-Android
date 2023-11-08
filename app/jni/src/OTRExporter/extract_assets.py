#!/usr/bin/env python3

import os, sys, shutil
import shutil
from rom_info import Z64Rom
import rom_chooser
import struct
import subprocess
import argparse

def BuildOTR(xmlPath, rom, zapd_exe=None, genHeaders=None, customAssetsPath=None, customOtrFile=None, portVer=None):
    if not zapd_exe:
        zapd_exe = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPDTR/ZAPD.out"

    exec_cmd = [zapd_exe, "ed", "-i", xmlPath, "-b", rom, "-fl", "CFG/filelists",
                "-o", "placeholder", "-osf", "placeholder", "-rconf", "CFG/Config.xml"]

    # generate headers, but not otrs by excluding the otr exporter
    if genHeaders:
        exec_cmd.extend(["-gsf", "1"])
    else:
        # generate otrs, but not headers
        exec_cmd.extend(["-gsf", "0", "-se", "OTR", "--customAssetsPath", customAssetsPath,
                "--customOtrFile", customOtrFile, "--otrfile",
                "oot-mq.otr" if Z64Rom.isMqRom(rom) else "oot.otr"])

    if portVer:
        exec_cmd.extend(["--portVer", portVer])

    print(exec_cmd)
    exitValue = subprocess.call(exec_cmd)
    if exitValue != 0:
        print("\n")
        print("Error when building the OTR file...", file=os.sys.stderr)
        print("Aborting...", file=os.sys.stderr)
        print("\n")

def BuildCustomOtr(zapd_exe=None, assets_path=None, otrfile=None, portVer=None):
    if not zapd_exe:
        zapd_exe = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPDTR/ZAPD.out"

    if not assets_path or not otrfile:
        print("\n")
        print("Assets path or otrfile name not provided. Exiting...", file=os.sys.stderr)
        print("\n")
        return

    exec_cmd = [zapd_exe, "botr", "-se", "OTR", "--norom", "--customAssetsPath", assets_path, "--customOtrFile", otrfile]

    if portVer:
        exec_cmd.extend(["--portVer", portVer])

    print(exec_cmd)
    exitValue = subprocess.call(exec_cmd)
    if exitValue != 0:
        print("\n")
        print("Error when building custom otr file...", file=os.sys.stderr)
        print("Aborting...", file=os.sys.stderr)
        print("\n")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-z", "--zapd", help="Path to ZAPD executable", dest="zapd_exe", type=str)
    parser.add_argument("rom", help="Path to the rom", type=str, nargs="?")
    parser.add_argument("--non-interactive", help="Runs the script non-interactively for use in build scripts.", dest="non_interactive", action="store_true")
    parser.add_argument("-v", "--verbose", help="Display rom's header checksums and their corresponding xml folder", dest="verbose", action="store_true")
    parser.add_argument("--gen-headers", help="Generate source headers to be checked in", dest="gen_headers", action="store_true")
    parser.add_argument("--norom", help="Generate only custom otr to be bundled to the game", dest="norom", action="store_true")
    parser.add_argument("--xml-root", help="Root path for the rom xmls", dest="xml_root", type=str)
    parser.add_argument("--custom-assets-path", help="Path to custom assets for the custom otr file", dest="custom_assets_path", type=str)
    parser.add_argument("--custom-otr-file", help="Name for custom otr file", dest="custom_otr_file", type=str)
    parser.add_argument("--port-ver", help="Store the port version in the otr", dest="port_ver", type=str)

    args = parser.parse_args()

    if args.norom:
        BuildCustomOtr(args.zapd_exe, args.custom_assets_path, args.custom_otr_file, portVer=args.port_ver)
        return

    roms = [ Z64Rom(args.rom) ] if args.rom else rom_chooser.chooseROM(args.verbose, args.non_interactive)
    for rom in roms:
        BuildOTR(os.path.join(args.xml_root, rom.version.xml_ver), rom.file_path, zapd_exe=args.zapd_exe, genHeaders=args.gen_headers,
                 customAssetsPath=args.custom_assets_path, customOtrFile=args.custom_otr_file, portVer=args.port_ver)

if __name__ == "__main__":
    main()
