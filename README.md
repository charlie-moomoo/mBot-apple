# mBot-apple
Play Bad Apple!! on an mBot or Arduino, provided you have a `Me LED Matrix`.

The Arduino versions have only been tested on Arduino UNO.

## Versions
This project has 3 versions in different folders.
- `mbot-serial`: Requires a computer to send Bad Apple frame data to the mBot via the serial port.
- `arduino-serial`: Requires a computer to send Bad Apple frame data to the Arduino via the serial port.
- `arduino-onboard`: Bad Apple runs completely on the Arduino and only requires power.

## Wiring
### mBot Wiring
Use an RJ25 cable to plug a `Me LED Matrix` into port 1.
### Arduino Wiring
1. Use an RJ25 cable to plug a `Me LED Matrix` into a `Me RJ25 Adapter` (or [cut an RJ25 cable](https://lioujj.blogspot.com/2015/10/mbotrj-25.html) if you want).
2. Plug the **S1** port on the `Me RJ25 Adapter` (or the cable you just cut), which is now connected to SCK on the LED matrix, into **A1** on the Arduino, and plug the **S2** port (DIN) into **A0**.
3. Power the LED matrix using the VCC and GND ports on the `Me RJ25 Adapter` (or the cable you just cut).

## Running
Use the Arduino IDE to upload the code for one of the three versions onto your mBot or Arduino.
### Serial version instructions
1. Run `npm i serialport` in the terminal.
2. Change the COM port number in `index.js`.
3. `node .` and watch Bad Apple.
### Onboard version instructions
Just power the device and watch Bad Apple!