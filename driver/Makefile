ifeq ($(OS),Windows_NT)
ifeq (,$(findstring MSYS,$(MSYSTEM)))
	.SHELLFLAGS := sh -c
	SHELL := busybox.exe
	SUFFIX := .exe
endif
endif


VCPKG_ROOT := $(subst \,/,${VCPKG_ROOT})


all: build


clean:
	rm -rf out


build:
	@mkdir -p out
	cmake -S . --preset x64-debug
	cmake --build out/build/x64-debug
	cmake --install out/build/x64-debug --prefix out/install/x64-debug
	# cmake -B out/build -S . -G Ninja -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=$(VCPKG_ROOT)/scripts/buildsystems/vcpkg.cmake -DVCPKG_APPLOCAL_DEPS=OFF -DVCPKG_TARGET_TRIPLET=x64-windows-static
