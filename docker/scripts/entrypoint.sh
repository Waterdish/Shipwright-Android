#!/usr/bin/env bash

###############################################################################

# Enable exit on any error
set -e

###############################################################################

# Setup Android SDK and NDK paths in project local.properties
echo "sdk.dir=${ANDROID_SDK_ROOT}" > /workspace/local.properties
echo "ndk.dir=${ANDROID_NDK_HOME}" >> /workspace/local.properties

# Execute any received command
exec "$@"

exit 0

###############################################################################
