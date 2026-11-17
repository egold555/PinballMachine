#!/bin/bash
# Builds the game and copies it to the Pi's Gameplay folder, then restarts it.
#
#   ./deploy.sh          to ssh host "pinballmachine"
#   ./deploy.sh myhost   somewhere else
#
# High scores (Gameplay/data/) are left alone.

set -euo pipefail
cd "$(dirname "$0")"
HOST=${1:-pinballmachine}
DEST=/home/pi/PinballMachine/Gameplay

npm run build
rsync -a --delete dist public package.json package-lock.json "$HOST:$DEST/"
ssh "$HOST" "cd $DEST && npm ci --omit=dev --no-audit --no-fund && sudo -n systemctl restart pinball-gameplay"
echo "Deployed. Logs: ssh $HOST journalctl -fu pinball-gameplay"
