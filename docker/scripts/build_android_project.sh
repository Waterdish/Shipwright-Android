#!/usr/bin/env bash

###############################################################################

# Actual script directory path
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# Load Container IMAGE_NAME and IMAGE_TAG variables
source ${DIR}/set_container_vars.sh

# Setup
IMAGE_NAME="${REGISTRY_REPO}:${IMAGE_TAG}"
CONTAINER_NAME="android_builder_soh"
PROJECT_PATH=$(realpath "${DIR}/../../")

###############################################################################

# Check if only release build flag was provided
ONLY_RELEASE_BUILD="false"
if [[ "$1" == "--release" ]] || [[ "$1" == "-r" ]]; then
    ONLY_RELEASE_BUILD="true"
fi

# Set type of build
GRADLEW_BUILD_TYPE="build"
if [[ "$ONLY_RELEASE_BUILD" == "true" ]]; then
    GRADLEW_BUILD_TYPE="assembleRelease"
fi

# Header
echo ""
echo "-----------------------------------------------------------"
echo "-- Android Project Build                                 --"
echo "-----------------------------------------------------------"
echo "Building project '${PROJECT_PATH}'..."
echo ""

# Check if the project path exists
if [ ! -d "$PROJECT_PATH" ]; then
    echo "Error: Project path not found"
    echo ""
    exit 1
fi

# Run Container to build
docker run --network="host" --rm \
    --name $CONTAINER_NAME \
    -v "$PROJECT_PATH":/workspace \
    -w /workspace \
    $IMAGE_NAME \
    bash -c "cp -a local.properties Android/ && cd Android && ./gradlew ${GRADLEW_BUILD_TYPE}"

BUILD_RESULT=$?

# Verify the result
echo ""
if [ $BUILD_RESULT -ne 0 ]; then
    echo "Build Fail (error code ${BUILD_RESULT})"
    exit $BUILD_RESULT
else
    echo "Build success"
fi

exit 0

###############################################################################
