## fw-fanctrl-tray

Simple tray widget created using C/C++ and Qt to switch between [fw-fanctrl](https://github.com/TamtamHero/fw-fanctrl) states/strategies.

### Installation

Clone the repo and install:
```bash
git clone https://github.com/tnrenzo/fw-fanctrl-tray.git
cd fw-fanctrl-tray
chmod u+x ./install.sh
./install.sh
```

This will compile and install to `~/.local/bin` and create a systemd service `~/.config/systemd/user/fw-fanctrl-tray.service`

### Troubleshooting

If the program instantly crashes on start, the service might not find the Qt plugin path on your system


1. Find the plugin path on your system
```bash
find /usr -name libqwayland.so 2>/dev/null
```
2. Then add the returned directory to the systemd service at `~/.config/systemd/user/fw-fanctrl-tray.service`


This might look like this:
```
[Service]
Type=simple
ExecStart=%h/.local/bin/fw-fanctrl-tray
Environment=QT_PLUGIN_PATH=/usr/lib64/qt6/plugins
Restart=on-failure
RestartSec=2
```
> [!NOTE]
> On Wayland, you might also want to add `Environment=QT_QPA_PLATFORM=wayland` to the same file