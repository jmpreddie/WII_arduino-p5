class Player {
  constructor(x, y) {
    this.x = x;
    this.y = y;
    this.size = 30;
    this.baseSpeed = 1.1;
    this.middleSpeed = 2.1;
    this.maxSpeed = 3.6;
    this.vx = 0;
    this.vy = 0;
    this.smoothing = 0.16;
  }

  update(data) {
    if (!data) return;

    let targetX = 0;
    let targetY = 0;

    if (joystickMode) {
      const rawX = Number(data.jx) || 0;
      const rawY = Number(data.jy) || 0;
      const magnitude = Math.hypot(rawX, rawY);
      let speed = 0;

      if (magnitude <= 10) {
        speed = 0;
      } else if (magnitude <= 60) {
        speed = this.middleSpeed;
      } else {
        speed = this.maxSpeed;
      }

      if (speed > 0) {
        const angle = Math.atan2(rawY, rawX);
        targetX = Math.cos(angle) * speed;
        targetY = Math.sin(angle) * speed;
      }
    } else {
      const rollDeg = Number(data.roll_deg) || 0;
      const pitchDeg = Number(data.pitch_deg) || 0;

      targetX = constrain(rollDeg / 60, -1, 1) * this.middleSpeed;
      targetY = constrain(pitchDeg / 60, -1, 1) * this.middleSpeed;
    }

    this.vx = lerp(this.vx, targetX, this.smoothing);
    this.vy = lerp(this.vy, targetY, this.smoothing);

    this.x += this.vx;
    this.y += this.vy;

    this.x = constrain(this.x, 0, width - this.size);
    this.y = constrain(this.y, 0, height - this.size);
  }

  draw() {
    fill(50, 100, 255);
    rect(this.x, this.y, this.size, this.size);
  }
}