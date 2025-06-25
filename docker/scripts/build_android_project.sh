#!/usr/bin/env bash

###############################################################################

# Actual script directory path
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# Load Container IMAGE_NAME and IMAGE_TAG variables
source ${DIR}/set_container_vars.sh

# Setup
IMAGE_NAME="${REGISTRY_REPO}:${IMAGE_TAG}"
CONTAINER_NAME="android_builder_soh"
PROJECT_PATH="${DIR}/../../"

###############################################################################

# Check for required arguments
if [ -z "$1" ]; then
    echo "Usage: $0 /path/to/project"
    exit 1
fi

PROJECT_PATH=$(realpath "${PROJECT_PATH}")

# Check if the project path exists
if [ ! -d "$PROJECT_PATH" ]; then
    echo "Error: Project path not found"
    echo "  $PROJECT_PATH"
    exit 1
fi

# Header
echo ""
echo "-----------------------------------------------------------"
echo "-- Android Project Build                                 --"
echo "-----------------------------------------------------------"
echo "Building project '${PROJECT_PATH}'..."
echo ""

# Run Container to build
docker run --network="host" --rm -it \
    --name $CONTAINER_NAME \
    -v "$PROJECT_PATH":/workspace \
    -w /workspace \
    $IMAGE_NAME \
    bash -c "cp -a local.properties Android/ && cd Android && ./gradlew build"

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
