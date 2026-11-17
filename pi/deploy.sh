#!/bin/bash
# Builds the game and copies it to the Pi's Gameplay folder, then restarts it.
#
#   ./deploy.sh          to ssh host "pinballmachine"
#   ./deploy.sh myhost   somewhere else
#
# Sounds in sounds/ overwrite the ones on the Pi (Gameplay/data/sounds/).
# Extra files added there by hand are kept. High scores are left alone.

set -euo pipefail
cd "$(dirname "$0")"
HOST=${1:-pinballmachine}
DEST=/home/pi/PinballMachine/Gameplay

npm run build
rsync -a --delete dist public package.json package-lock.json "$HOST:$DEST/"
rsync -a --mkpath sounds/ "$HOST:$DEST/data/sounds/"
ssh "$HOST" "cd $DEST && npm ci --omit=dev --no-audit --no-fund && sudo -n systemctl restart pinball-gameplay"
echo "Deployed. Logs: ssh $HOST journalctl -fu pinball-gameplay"
