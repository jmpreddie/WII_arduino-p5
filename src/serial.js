class Serial {
    constructor() {
        this.ax = 0;
        this.ay = 0;
        this.az = 0;
        this.gx = 0;
        this.gy = 0;
        this.gz = 0;
        this.roll_deg = 0;
        this.pitch_deg = 0;
        this.roll_dir = "";
        this.pitch_dir = "";
    }

    translate(data) {
        if (!data) return;

        const line = String(data).trim();
        if (!line) return;

        const values = line.split(",");
        if (values.length < 10) return;

        const [ax, ay, az, gx, gy, gz, roll_deg, pitch_deg, roll_dir, pitch_dir] = values;

        this.ax = parseFloat(ax);
        this.ay = parseFloat(ay);
        this.az = parseFloat(az);
        this.gx = parseFloat(gx);
        this.gy = parseFloat(gy);
        this.gz = parseFloat(gz);
        this.roll_deg = parseFloat(roll_deg);
        this.pitch_deg = parseFloat(pitch_deg);
        this.roll_dir = roll_dir.trim();
        this.pitch_dir = pitch_dir.trim();
    }
}