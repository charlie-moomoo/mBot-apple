const fs = require("fs");
const frames = fs.readFileSync("serial.txt").toString().split("\n");

const { SerialPort } = require("serialport");
const port = new SerialPort({ path: "\\\\.\\COM6", baudRate: 115200 });

setTimeout(() => {
  frames.forEach((frame, index) => {
    setTimeout(() => {
      port.write(`${frame}\n`);
    }, index * (1000 / 30));
  });
}, 5000);
