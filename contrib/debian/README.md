
Debian
====================
This directory contains files used to package chratosd/chratos-qt
for Debian-based Linux systems. If you compile chratosd/chratos-qt yourself, there are some useful files here.

## chratos: URI support ##


chratos-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install chratos-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your chratos-qt binary to `/usr/bin`
and the `../../share/pixmaps/chratos128.png` to `/usr/share/pixmaps`

chratos-qt.protocol (KDE)

