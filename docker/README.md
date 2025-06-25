
# Container Support

An [OCI](https://opencontainers.org/) container image to generate an isolate environment that includes all the needed tools for building the project is available.

This container can be used from any OCI compatible container tool, such as Docker, Podman, etc.

The main purpose of this is to provide:

- An easy way to build the project from CLI (Command Line Interface), without the need of installing and using a full android-studio.
- A virtual container environment to use it for automatically building the project during CI (Continuous Integration) via Github actions.

## Usage

The next files are provided to ease the usage of the container:

- **Makefile:** An UNIX Makefile to abstract and simplify the usage of the container technology.

- **Containerfile:** The container definition file that can be used to build and create the container image.

The **Makefile** offers the next commands:

```bash
make setup - Download and install the container image environment needed for project build.

make build - Run the container to build the project.

make create_container - Build the container image from the Containerfile (no needed if you have install it via make setup).
```

The **Containerfile** has been used to create the corresponding container image and then this has been published to [Docker Hub](https://hub.docker.com/repository/docker/jriosbyte/android-builder-soh) registry, so you just need to install the container from the remote registry via `make setup`.

## Project Build

First time that you get the repository, you need to download and install the container:

```bash
make setup
```

Then, you can build the project via:

```bash
make build
```

The resulting image will be located at Android/app/build/outputs/apk/release/app-release.apk

## Publishing a new container image

In case some modifications needs to be done in the image and a new image version need to be created, follows the next steps:

1. Do the modifications in the Containerfile.
2. Increase the *IMAGE_TAG* version at *scripts/set_container_vars.sh*.
3. Create the new image version via `make create_container`.
4. Publish the image to the registry via `scripts/publish_container_image.sh`.

**Note:** you need to have access to the registry repository as owner or collaborator.
