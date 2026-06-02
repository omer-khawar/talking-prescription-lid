# Talking Prescription Lid 💊🔊

An accessible, low-cost smart bottle lid that **reads medication information out loud at the press of a button** — designed for older adults and visually impaired users who struggle to read standard prescription labels.

> 🎓 Senior capstone for the Piscataway High School Engineering Academy (PLTW *Engineering Design & Development*).
> **My role:** Lead Engineer · **Team:** Dhruv, Krish, Nick, Omer

---

## The Problem

Prescription labels pack critical information — name, dosage, expiration, warnings — into tiny, dense print.

- **76–81%** of older adults don't meet the visual requirements to read a standard label.
- **~18%** of individuals already experience medication errors tied to readability.
- Existing fixes (talking RFID labels, caregivers, pillboxes) are **expensive, hard to access, or require tech the user may not have.**

## The Solution

A replacement bottle lid with a single button. Press it, and the device **speaks the medication's name, dosage, and instructions aloud.** No app, no internet, no pharmacy participation required — it works entirely on the device.

## How It Works

```
[ Button ] ──► [ XIAO RP2040 ] ──► [ DFPlayer Mini ] ──► [ Speaker ]
                     │
                     └──► [ RGB status LED ]  (color-coded feedback)
```

1. Audio clips for each medication live on a **microSD card** inside the lid.
2. A press on the button tells the **RP2040** microcontroller to advance to and play the next clip.
3. The **DFPlayer Mini** decodes the MP3 and drives the **speaker**.
4. The **RGB LED** gives at-a-glance status so the user (or a caregiver) always knows what the device is doing.

| Button action | Result |
| --- | --- |
| Short press | Play / advance to the next medication clip |
| Long press (2s) | Stop playback |

| LED color | Meaning |
| --- | --- |
| ⚪ White | Booting |
| 🟢 Green (3 blinks) | Ready |
| 🔵 / 🟢 Pulsing | Currently playing |
| 🟣 Purple | Stopped |
| 🔴 Red (blinking) | Error — check SD card / wiring |

## Bill of Materials

| Component | Purpose |
| --- | --- |
| Seeed Studio **XIAO RP2040** | Microcontroller (note: 3.3 V logic — see lessons learned) |
| DFRobot **DFPlayer Mini** | MP3 playback from microSD |
| Micro speaker (8 Ω) | Audio output |
| Momentary push button | User input |
| Common-anode RGB LED | Status feedback |
| microSD card | Stores medication audio clips |
| 3D-printed enclosure | Custom housing, modeled in Onshape |

## Firmware

The full firmware lives in [`/firmware`](./firmware/talking_prescription_lid.ino). It's written in C++ for the Arduino framework and handles button debouncing, short/long-press logic, DFPlayer control, and the RGB status states described above.

**Dependencies:** [DFRobotDFPlayerMini](https://github.com/DFRobot/DFRobotDFPlayerMini) library · Arduino-Pico (RP2040) board support.

## Repository Structure

```
talking-prescription-lid/
├── README.md            ← this file
├── firmware/
│   └── talking_prescription_lid.ino
├── docs/
│   └── Project_Documentation.pdf   ← full PLTW design portfolio
└── media/
    └── (build photos, CAD screenshots, demo clips)
```

## Engineering Process

Built end-to-end following the PLTW design process:

- **Problem identification** — research + 5 consumer interviews (including elderly users) and surveys.
- **Prior-art analysis** — evaluated ScriptTalk RFID labels, caregivers, pillboxes, and related patents.
- **Requirements & specs** — accessibility, cost, usability, efficiency, and development-time criteria.
- **Concept generation & selection** — 20-solution decision matrix narrowed to a final concept.
- **CAD & circuit design** — enclosure in Onshape, circuit prototyped in TinkerCAD/Multisim.
- **Prototype construction** — soldered the circuit, 3D-printed the housing, and built a testable unit.

## Lessons Learned

- The **XIAO RP2040 runs at 3.3 V**, not 5 V — we had to rewire the circuit after initially powering components at 5 V.
- We swapped to the correct **DFPlayer Mini** variant after the first module didn't behave.
- Added small vent holes above the speaker so the audio is loud enough through the enclosure.
- Soldering tight tolerances by hand cost real time — clean joints matter.

## Acknowledgments

Thanks to Mr. Hamrani (materials + 3D printing), Ms. Layachi, and the PHS Engineering Academy for support throughout the build.

---

*This project does not replace professional medical guidance. It is an academic engineering prototype.*
