file_name := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT   := $(abspath $(dir $(file_name)))
MAKEDIR   := $(PROJECT)/make
BUILDDIR  := $(PROJECT)/build
SCRIPTS   := $(PROJECT)/scripts
