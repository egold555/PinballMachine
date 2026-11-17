#!/bin/bash
# Sends a .hex file to the Pi's Reprogram folder and prints LOG.TXT until the
# upload to the Mega is done.
#
#   ./flash.sh firmware.hex [host]

set -euo pipefail
HEX=${1:?usage: $0 firmware.hex [host]}
HOST=${2:-pinballmachine}
DIR=/home/pi/PinballMachine/Reprogram

# Copy under a name the watcher ignores, then rename, so it never sees half a file.
scp -q "$HEX" "$HOST:$DIR/.upload.part"
ssh "$HOST" bash -s <<EOF
cd $DIR
: > LOG.TXT
mv .upload.part firmware.hex
exec 3< <(timeout 300 tail -n +1 -F LOG.TXT 2>/dev/null)
tail_pid=\$!
result=1
while IFS= read -r -u 3 line; do
    echo "\$line"
    case "\$line" in
        *DONE:*) result=0; break ;;
        *FAILED:*) break ;;
    esac
done
kill \$tail_pid 2>/dev/null || true
exit \$result
EOF
