class Player {
  constructor(x, y) {
    this.x = x;
    this.y = y;
    this.size = 30;
    this.baseSpeed = 1;
  }

  update(data)   {
    if (!data) return;

    const accelX = data.ax || 0;
    const accelY = data.ay || 0;

    let moveX = 0;
    let moveY = 0;

    const speedX = this.baseSpeed + Math.abs(accelX) * 2.5;
    const speedY = this.baseSpeed + Math.abs(accelY) * 2.5;

    if (data.roll_dir === "Tilted Left") {
      moveX = -speedX;
    } else if (data.roll_dir === "Tilted Right") {
      moveX = speedX;
    }

    if (data.pitch_dir === "Tilted Forward") {
      moveY = -speedY;
    } else if (data.pitch_dir === "Tilted Backward") {
      moveY = speedY;
    }

    this.x += moveX;
    this.y += moveY;

    this.x = constrain(this.x, 0, width - this.size);
    this.y = constrain(this.y, 0, height - this.size);
  }

  draw() {
    fill(50, 100, 255);
    rect(this.x, this.y, this.size, this.size);
  }
}