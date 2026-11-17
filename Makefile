# Everyday jobs for the pinball machine. Run `make` for the list.
#
# The Pi is reached as ssh host "pinballmachine" (see pi/README.md).
# Override with  make deploy PI=otherhost

PI ?= pinballmachine
PIO ?= $(shell command -v pio || echo $(HOME)/.platformio/penv/bin/pio)

MEGA_HEX = mega/.pio/build/megaatmega2560/firmware.hex
ORIGINAL_HEX = Arduino/.pio/build/megaatmega2560/firmware.hex

.PHONY: help mega flash flash-original deploy test logs console setup-pi

help:
	@echo "make mega            build the Mega I/O firmware ($(MEGA_HEX))"
	@echo "make flash           build it and upload it to the Mega through the Pi"
	@echo "make flash-original  upload the original all-in-one Arduino/ firmware instead"
	@echo "make deploy          build the game and install it on the Pi"
	@echo "make test            run the game's tests"
	@echo "make logs            follow the game's output on the Pi"
	@echo "make console         stop the game and drive the Mega by hand (Ctrl+D to quit)"
	@echo "make setup-pi        rerun the Pi setup after changing pi/system/ (asks for the sudo password)"

mega:
	cd mega && $(PIO) run

flash: mega
	pi/flash.sh $(MEGA_HEX) $(PI)

flash-original:
	cd Arduino && $(PIO) run
	pi/flash.sh $(ORIGINAL_HEX) $(PI)

deploy:
	pi/deploy.sh $(PI)

test:
	cd pi && npm test

logs:
	ssh $(PI) journalctl -fu pinball-gameplay

console:
	ssh -t $(PI) 'sudo -n systemctl stop pinball-gameplay; cd PinballMachine/Gameplay && node dist/console.js; sudo -n systemctl start pinball-gameplay'

setup-pi:
	rsync -a --delete pi/system/ $(PI):pinball-system/
	ssh -t $(PI) sudo bash pinball-system/setup.sh
