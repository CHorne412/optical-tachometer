# Optical Tachometer

[![build](https://github.com/CHorne412/optical-tachometer/actions/workflows/build.yml/badge.svg)](https://github.com/CHorne412/optical-tachometer/actions/workflows/build.yml)

Measures the speed of a spinning motor shaft in real time using a TCRT5000
reflective optical sensor and an Arduino, and reports RPM over the serial
console.

Built for a CSEE course project, March 2026.

## How it works

A reflective marker on the rotating shaft passes the sensor once per
revolution segment. The TCRT5000 returns a lower analog reading when the
marker reflects its infrared beam back, so a reading below the threshold means
"marker in view."

The part that matters is counting each pass **once**. The sensor is polled
thousands of times per second, so a marker sitting in front of it for a few
milliseconds would otherwise register hundreds of pulses. A state flag
(`objectDetected`) latches on the falling edge and doesn't release until the
reading rises back above the threshold — so one pass is one pulse, regardless
of how long it lingers.

Every sampling window, the accumulated pulse count converts to RPM and the
counter resets.

## Hardware

| Component | Detail |
|---|---|
| Board | Arduino (any with an analog input) |
| Sensor | TCRT5000 reflective optical sensor |
| Wiring | Sensor analog output → `A0`; VCC and GND to the board |
| Target | Motor shaft with reflective marker(s) |

## Calibration

The two constants in the sketch were tuned empirically against a stroboscope
rather than derived on paper:

- `threshold = 900` — the analog reading that separates reflective from
  non-reflective surface. Depends on your sensor distance, marker material,
  and ambient light, so expect to re-tune it for a different rig.
- `sampleInterval = 800` (ms) — the measurement window. Together with the
  `pulses * 60.0 / 2` conversion, this was adjusted until readings matched the
  stroboscope reference.

Validated against the stroboscope up to **5,000 RPM**.

Because the constants encode the behaviour of this specific rig, porting this
to different hardware means repeating the calibration, not just recompiling.

## Usage

1. Open `optical-tachometer/optical-tachometer.ino` in the Arduino IDE.
2. Wire the sensor's analog output to `A0`.
3. Upload, then open the Serial Monitor at **9600 baud**.

Output:

```
Pulses: 42 | RPM: 1260.00 | Raw: 873
```

The raw reading is printed alongside the result so you can watch the signal
while adjusting sensor position or threshold.

## Known limitations

- **Polling, not interrupts.** The sketch reads the sensor in `loop()`. The
  `volatile` qualifiers and the `noInterrupts()` / `interrupts()` pair are
  left over from an earlier interrupt-driven design and aren't doing anything
  here. Moving the pulse counting to a hardware interrupt on a digital pin
  would be the right change for higher speeds or a busier main loop.
- **The conversion assumes a fixed number of marker passes per revolution**
  (`/ 2` in the sketch). A different marker arrangement needs a different
  divisor.
- **No averaging.** Each reading reflects a single window, so the output
  fluctuates with motor jitter. A rolling average would steady it.
