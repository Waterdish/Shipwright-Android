#!/usr/bin/env bash

###############################################################################

# Actual script directory path
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# Load Container IMAGE_NAME and IMAGE_TAG variables
source ${DIR}/set_container_vars.sh

# Setup
IMAGE_NAME="${IMAGE_NAME}:${IMAGE_TAG}"
DOCKER_FILE="Containerfile"

###############################################################################

# Build the image
echo ""
echo "-----------------------------------------------------------"
echo "-- Container Image Build                                 --"
echo "-----------------------------------------------------------"
echo "Building '${IMAGE_NAME}'..."
docker build --network="host" -t ${IMAGE_NAME} --file $DOCKER_FILE .
build_result=$?
if [[ $build_result -ne 0 ]]; then
    echo ""
    echo "ERROR: Image build fail."
    exit 1
fi
echo ""
echo "Image '${IMAGE_NAME}' build success."
echo ""
echo "Clean-up build cache..."
docker builder prune -f
echo "Done"
echo ""

exit 0

###############################################################################
