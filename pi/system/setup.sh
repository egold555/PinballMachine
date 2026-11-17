#!/bin/bash
# One-time setup of the Raspberry Pi. Safe to run again after changing
# anything in this folder.
#
#   sudo bash setup.sh
#
# Installs Node, avrdude, Samba and vsftpd; creates /home/pi/PinballMachine
# with Reprogram/ and Gameplay/; shares it over SMB (public, no login) and FTP
# (login as pi); installs the reprogram watcher and the game service.

set -euo pipefail
[ "$(id -u)" = 0 ] || { echo "Run with sudo: sudo bash $0"; exit 1; }

HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=/home/pi/PinballMachine

echo "== Packages"
install -d -m 0755 /etc/apt/keyrings
if [ ! -f /etc/apt/keyrings/nodesource.gpg ]; then
    curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg
fi
echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_24.x nodistro main" \
    > /etc/apt/sources.list.d/nodesource.list
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y nodejs avrdude inotify-tools rsync samba vsftpd

echo "== Folders"
install -d -o pi -g pi -m 0775 "$ROOT" "$ROOT/Reprogram" "$ROOT/Gameplay"
install -o pi -g pi -m 0664 "$HERE/README.TXT" "$ROOT/Reprogram/README.TXT"

echo "== Services"
install -m 0755 "$HERE/reprogram.sh" /usr/local/bin/pinball-reprogram
install -m 0644 "$HERE/pinball-reprogram.service" "$HERE/pinball-gameplay.service" /etc/systemd/system/
install -m 0440 "$HERE/sudoers" /etc/sudoers.d/pinball
visudo -cf /etc/sudoers.d/pinball
systemctl daemon-reload
systemctl enable pinball-reprogram pinball-gameplay
systemctl restart pinball-reprogram
# The game starts once deploy.sh has put the code in Gameplay/.
[ -f "$ROOT/Gameplay/dist/main.js" ] && systemctl restart pinball-gameplay

echo "== SMB"
install -m 0644 "$HERE/smb-share.conf" /etc/samba/pinball-share.conf
grep -q pinball-share.conf /etc/samba/smb.conf ||
    printf '\ninclude = /etc/samba/pinball-share.conf\n' >> /etc/samba/smb.conf
# Guests need this in [global]. Debian's default smb.conf already has it.
testparm -s 2>/dev/null | grep -qi 'map to guest = bad user' ||
    sed -i 's/^\[global\]$/[global]\n   map to guest = bad user/' /etc/samba/smb.conf
testparm -s > /dev/null
systemctl restart smbd

echo "== FTP"
[ -f /etc/vsftpd.conf.orig ] || cp /etc/vsftpd.conf /etc/vsftpd.conf.orig
install -m 0644 "$HERE/vsftpd.conf" /etc/vsftpd.conf
echo pi > /etc/vsftpd.userlist
systemctl restart vsftpd

echo
echo "Done. Node $(node --version), $(avrdude -? 2>&1 | grep -io 'version [0-9.]*' | head -1)"
echo "SMB:  smb://$(hostname -I | awk '{print $1}')/PinballMachine   (no login)"
echo "FTP:  ftp://$(hostname -I | awk '{print $1}')                   (login: pi)"
