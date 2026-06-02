/*
 * Talking Prescription Lid — Firmware
 * ------------------------------------
 * An accessible bottle lid that reads medication information aloud
 * at the press of a button, for older / visually impaired users.
 *
 * Hardware:
 *   - Seeed Studio XIAO RP2040  (3.3 V logic)
 *   - DFRobot DFPlayer Mini      (MP3 playback from microSD)
 *   - Micro speaker
 *   - Momentary push button      (active-low, internal pull-up)
 *   - On-board common-anode RGB LED (status feedback)
 *
 * Controls:
 *   - Short press        -> play / advance to the next track
 *   - Long press (2 s)   -> stop playback
 *
 * LED status:
 *   - White              -> booting
 *   - Green (3 blinks)   -> ready
 *   - Blue / green pulse -> playing
 *   - Purple             -> stopped
 *   - Red (blinking)     -> error (check wiring / SD card)
 *
 * Dependencies: DFRobotDFPlayerMini library, Arduino-Pico board support.
 *
 * Team: Dhruv, Krish, Nick, Omer  |  Lead Engineer: Omer Khawar
 */

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

#define LED_R          PIN_LED_R
#define LED_G          PIN_LED_G
#define LED_B          PIN_LED_B
#define BUTTON_PIN     D1
#define DFP_SERIAL     Serial1
#define DEBOUNCE_MS    50
#define LONG_PRESS_MS  2000
#define LED_PULSE_MS   500

DFRobotDFPlayerMini dfPlayer;

int totalTracks = 0;
int currentTrack = 0;
bool isPlaying = false;

bool lastButtonState = HIGH;
bool buttonState = HIGH;
uint32_t lastDebounceTime = 0;
uint32_t pressStartTime = 0;
bool longPressHandled = false;

uint32_t lastLedToggle = 0;
bool ledOn = false;

// On the XIAO RP2040 the RGB LED is active-LOW, so a "true" channel is driven LOW.
void setLeds(bool red, bool green, bool blue) {
  digitalWrite(LED_R, red ? LOW : HIGH);
  digitalWrite(LED_G, green ? LOW : HIGH);
  digitalWrite(LED_B, blue ? LOW : HIGH);
}

void ledsOff() { setLeds(false, false, false); }

void playTrack(int track) {
  dfPlayer.play(track);
  isPlaying = true;
  currentTrack = track;
  setLeds(false, false, true);
  lastLedToggle = millis();
}

void stopPlayback() {
  dfPlayer.stop();
  isPlaying = false;
  ledsOff();
}

void playNextTrack() {
  if (totalTracks == 0) return;
  currentTrack = (currentTrack % totalTracks) + 1;
  playTrack(currentTrack);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  ledsOff();

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial1.begin(9600);
  delay(200);

  setLeds(true, true, true);  // white = booting

  if (!dfPlayer.begin(Serial1)) {
    Serial.println("DFPlayer not found! Check wiring / SD card.");
    while (true) {
      setLeds(true, false, false);  // red blink = error
      delay(150);
      ledsOff();
      delay(150);
    }
  }

  dfPlayer.volume(25);  // 0-30
  dfPlayer.EQ(DFPLAYER_EQ_NORMAL);
  dfPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  totalTracks = dfPlayer.readFileCounts();
  Serial.print("Tracks found: ");
  Serial.println(totalTracks);

  ledsOff();
  for (int i = 0; i < 3; i++) {
    setLeds(false, true, false);  // green blink = ready
    delay(120);
    ledsOff();
    delay(120);
  }
}

void loop() {
  // --- Button debounce ---
  bool reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) lastDebounceTime = millis();

  if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        pressStartTime = millis();
        longPressHandled = false;
      }
      if (buttonState == HIGH && !longPressHandled) {
        playNextTrack();  // short press = next track
      }
    }
  }

  // --- Long press = stop ---
  if (buttonState == LOW && !longPressHandled) {
    if ((millis() - pressStartTime) >= LONG_PRESS_MS) {
      longPressHandled = true;
      stopPlayback();
      setLeds(true, false, true);  // purple = stopped
      delay(400);
      ledsOff();
    }
  }

  lastButtonState = reading;

  // --- LED pulse while playing ---
  if (isPlaying) {
    uint32_t now = millis();
    if (now - lastLedToggle >= LED_PULSE_MS) {
      lastLedToggle = now;
      ledOn = !ledOn;
      setLeds(false, !ledOn, ledOn);  // alternate blue/green
    }
  }

  // --- DFPlayer events ---
  if (dfPlayer.available()) {
    uint8_t type = dfPlayer.readType();
    int value = dfPlayer.read();
    if (type == DFPlayerPlayFinished) {
      isPlaying = false;
      ledsOff();
    } else if (type == DFPlayerError) {
      Serial.print("Error: ");
      Serial.println(value);
      isPlaying = false;
      setLeds(true, false, false);
      delay(300);
      ledsOff();
    }
  }
}
