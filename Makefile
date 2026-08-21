PREFIX ?= $(HOME)/.local

.PHONY: debug release install

debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

release:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(PREFIX)
	cmake --build build

install: release
	cmake --install build
	cp ./fw-fanctrl-tray.service $(HOME)/.config/systemd/user/fw-fanctrl-tray.service