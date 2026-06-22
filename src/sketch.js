let port;
let reader;
let decoder = new TextDecoder();
let serial;
let serialBuffer = "";
let player;

let connectButton;
let latestLine = "No data yet";
let statusMessage = "Not connected";

function setup() {
  createCanvas(400, 400);
  connectButton = createButton('Connect Arduino');
  connectButton.position(20, 20);
  connectButton.mousePressed(connectSerial);
  serial = new Serial();
  player = new Player(width / 2, height / 2);
}

function draw() {
  background(220);

  if (serial) {
    player.update(serial);
  }
  player.draw();

  fill(0);
  textSize(12);
  text("Status: " + statusMessage, 20, height - 20);

  // console.log(latestLine);
  // let dataText =
  //   "ax=" + serial.ax.toFixed(3) +
  //   "  ay=" + serial.ay.toFixed(3) +
  //   "  az=" + serial.az.toFixed(3) +
  //   "  gx=" + serial.gx.toFixed(3) +
  //   "  gy=" + serial.gy.toFixed(3) +
  //   "  gz=" + serial.gz.toFixed(3) +
  //   "  roll=" + serial.roll_deg.toFixed(2) +
  //   "  pitch=" + serial.pitch_deg.toFixed(2) +
  //   "  rollDir=" + serial.roll_dir +
  //   "  pitchDir=" + serial.pitch_dir;
  // text(dataText, 20, 60, 360, 300);
}

async function connectSerial() {
  if (!("serial" in navigator)) {
    statusMessage = "Web Serial is not supported in this browser";
    return;
  }
  try {
    statusMessage = "Connecting to Arduino...";
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: 115200 });
    reader = port.readable.getReader();
    statusMessage = "Connected to Arduino";
    readData();
  } catch (err) {
    statusMessage = "Connection failed";
    console.error(err);
  }
}

async function readData() {
  while (true) {
    const { value, done } = await reader.read();
    if (done) {
      break;
    }

    serialBuffer += decoder.decode(value, { stream: true });
    const lines = serialBuffer.split(/\r?\n/);
    serialBuffer = lines.pop() || "";

    for (const line of lines) {
      const trimmed = line.trim();
      if (!trimmed) continue;

      latestLine = trimmed;
      if (serial) {
        serial.translate(trimmed);
      }
    }
  }
}