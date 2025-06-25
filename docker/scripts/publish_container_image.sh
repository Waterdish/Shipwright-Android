#!/usr/bin/env bash

###############################################################################

# Actual script directory path
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

# Load Container IMAGE_NAME and IMAGE_TAG variables
source ${DIR}/set_container_vars.sh

###############################################################################

# Header
echo ""
echo "-----------------------------------------------------------"
echo "-- Publishing Container Image                            --"
echo "-----------------------------------------------------------"

# Check if the image is available
if ! docker image inspect "${IMAGE_NAME}:${IMAGE_TAG}" >/dev/null 2>&1; then
    echo "Error: ${IMAGE_NAME}:${IMAGE_TAG} not found."
    echo "Note: The image need to be build first."
    exit 1
fi

# Login to the remote registry
docker login
if [[ $? -ne 0 ]]; then
    echo "Error: Login to registry fail."
    exit 1
fi

# Tag Image for remote registry
docker tag "${IMAGE_NAME}:${IMAGE_TAG}" "${REGISTRY_REPO}:${IMAGE_TAG}"

# Publish the image
echo "Publishing container image ${REGISTRY_REPO}:${IMAGE_TAG} to registry..."
docker push "${REGISTRY_REPO}:${IMAGE_TAG}"
if [[ $? -ne 0 ]]; then
    echo "Error: Image publish fail."
    exit 1
fi
echo ""
echo "Image published successfully."

exit 0

###############################################################################
