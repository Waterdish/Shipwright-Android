
# Container Support

An [OCI](https://opencontainers.org/) container image to generate an isolate environment that includes all the needed tools for building the project is available.

This container can be used from any OCI compatible container tool, such as Docker, Podman, etc.

The main purpose of this is to provide:

- An easy way to build the project from CLI (Command Line Interface), without the need of installing and using a full android-studio.
- A virtual container environment to use it for automatically building the project during CI (Continuous Integration) via Github actions.
