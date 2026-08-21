#!/usr/bin/bash
set -eu pipefail

make install

mkdir -p "$HOME/.config/systemd/user"
cp ./fw-fanctrl-tray.service $HOME/.config/systemd/user/fw-fanctrl-tray.service

systemctl --user daemon-reload
systemctl --user enable --now fw-fanctrl-tray.service
systemctl --user status fw-fanctrl-tray.service

echo
echo Done.
echo