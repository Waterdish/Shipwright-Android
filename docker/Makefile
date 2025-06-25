
###############################################################################

SHELL = /bin/bash
SCRIPTS="./scripts"

.PHONY: all help setup build

###############################################################################

### Main Targets ###

# make help
help:
	@ echo ""
	@ echo "Usage:"
	@ echo "  make setup - Install container environment"
	@ echo "  make build - Build project through the container"
	@ echo "  make build_release - Build project release through the container"
	@ echo "  make create_container - Local build the container"
	@ echo ""

# make setup (Install container/docker image)
setup: scripts_exec_permission
	@ $(SCRIPTS)/install_container_image.sh

# make build
build: scripts_exec_permission
	@ $(SCRIPTS)/build_android_project.sh

# make build_release
build_release: scripts_exec_permission
	@ $(SCRIPTS)/build_android_project.sh --release

# make create_container
create_container: scripts_exec_permission
	@ $(SCRIPTS)/build_container_image.sh

###############################################################################

### Auxiliary Targets ###

scripts_exec_permission:
	@ chmod +x $(SCRIPTS)/*.sh

###############################################################################
