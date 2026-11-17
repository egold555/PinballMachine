REPROGRAM THE MEGA
==================

Drop a .hex file in this folder and it is uploaded to the Arduino Mega
plugged into the Pi's USB port. Nothing else to do.

  - The game stops while uploading and starts again afterwards.
  - LOG.TXT shows what is happening, live. Open it again to refresh.
  - When it is done, the file moves to Flashed/ (it worked) or Failed/
    (it didn't, LOG.TXT says why). Nothing is ever uploaded twice.

Where to get the .hex file
--------------------------
Build the Mega code on your computer:

    cd mega
    pio run

The file is  mega/.pio/build/megaatmega2560/firmware.hex

To go back to the original all-in-one firmware, build Arduino/ the same way
and drop in its firmware.hex instead.

If an upload fails
------------------
Nothing is lost: the bootloader on the Mega is never touched. Drop the file
in again, or as a last resort plug a laptop into the Mega's USB and upload
from there like always.
