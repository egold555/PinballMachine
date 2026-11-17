#!/bin/bash
# Uploads any .hex file dropped in the Reprogram folder to the Mega.
#
# Installed as /usr/local/bin/pinball-reprogram and run by
# pinball-reprogram.service. Output goes to LOG.TXT next to the file as it
# happens. Afterwards the file moves to Flashed/ or Failed/, so it is never
# uploaded twice.

set -u
DIR=/home/pi/PinballMachine/Reprogram
LOG=$DIR/LOG.TXT
GAME=pinball-gameplay

log() { echo "$(date '+%F %T')  $*" >> "$LOG"; }

find_port() {
    local p
    for p in /dev/serial/by-id/*rduino* /dev/ttyACM* /dev/ttyUSB*; do
        [ -e "$p" ] && { echo "$p"; return 0; }
    done
    return 1
}

# Moves the file into Flashed/ or Failed/ with the time in front of its name.
file_as() {
    mkdir -p "$DIR/$2"
    mv -f "$1" "$DIR/$2/$(date '+%F_%H%M%S')_$(basename "$1")"
}

flash() {
    local hex=$1 name size last=-1 port game_on=no
    name=$(basename "$hex")
    : > "$LOG" # every upload starts a fresh log
    log "Got $name"

    # Wait until the copy has finished: same size for 2 seconds.
    while size=$(stat -c %s "$hex" 2>/dev/null) && [ "$size" != "$last" ]; do
        last=$size
        sleep 2
    done
    if [ ! -e "$hex" ]; then
        log "The file went away, nothing to do."
        return
    fi

    if [ "$(head -c 1 "$hex")" != ":" ]; then
        log "FAILED: $name is not an Intel HEX file. Use firmware.hex from mega/.pio/build/megaatmega2560/"
        file_as "$hex" Failed
        return
    fi

    if ! port=$(find_port); then
        log "FAILED: no Mega found on USB. Is it plugged in?"
        file_as "$hex" Failed
        return
    fi
    log "Mega is on $port"

    # The game holds the port, so it has to stop while uploading.
    if systemctl is-active --quiet "$GAME" || systemctl is-enabled --quiet "$GAME"; then
        game_on=yes
        log "Stopping the game"
        sudo -n systemctl stop "$GAME" >> "$LOG" 2>&1
    fi

    log "Uploading ($size bytes)..."
    if avrdude -p atmega2560 -c wiring -P "$port" -b 115200 -D -U "flash:w:$hex:i" >> "$LOG" 2>&1; then
        log "DONE: the Mega is running $name"
        file_as "$hex" Flashed
    else
        log "FAILED: avrdude could not upload (see above). The Mega is unchanged or half written; drop the file in again to retry."
        file_as "$hex" Failed
    fi

    if [ "$game_on" = yes ]; then
        log "Starting the game"
        sudo -n systemctl start "$GAME" >> "$LOG" 2>&1
    fi
}

mkdir -p "$DIR"
shopt -s nullglob nocaseglob

# Anything dropped in while this wasn't running
for f in "$DIR"/*.hex; do flash "$f"; done

inotifywait -m -q -e close_write -e moved_to --format '%f' "$DIR" | while read -r name; do
    case "${name,,}" in
        *.hex) [ -e "$DIR/$name" ] && flash "$DIR/$name" ;;
    esac
done
