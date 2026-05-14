#define BLYNK_TEMPLATE_ID "TMPL3XzQ2ZHff"
#define BLYNK_TEMPLATE_NAME "Breathing monitoring"
#define BLYNK_AUTH_TOKEN "uELpyvSdvKd06X0Uu4nsE6r2K5lgr8Qy"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Redmi 11 Prime 5G";
char pass[] = "12345678";

int sensorPin = 32;

float filtered = 0;
float alpha = 0.1;

int prev = 0;
bool rising = false;

int breathCount = 0;

unsigned long lastBreathTime = 0;
unsigned long startTime;

void setup() {
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  startTime = millis();
}

void loop() {
  Blynk.run();

  int raw = analogRead(sensorPin);

  // smoothing
  filtered = alpha * raw + (1 - alpha) * filtered;
  int value = filtered;

  // peak detection
  if (value > prev) {
    rising = true;
  } 
  else if (value < prev && rising) {
    if (millis() - lastBreathTime > 1100) {
      breathCount++;
      lastBreathTime = millis();
    }
    rising = false;
  }

  prev = value;

  // 10 sec BPM calculation
  if (millis() - startTime >= 10000) {
    int bpm = breathCount * 6;

    Serial.print("BPM: ");
    Serial.println(bpm);

    // send to Blynk
    Blynk.virtualWrite(V0, bpm);

    // HIGH BPM alert
    if (bpm > 15) {
      Blynk.logEvent("high_bpm", "Your BPM is high. Please slow your breathing.");
    }

    // LOW BPM alert
    if (bpm < 8) {
      Blynk.logEvent("low_bpm", "Your BPM is low. Breathe properly.");
    }

    // reset
    breathCount = 0;
    startTime = millis();
  }

  delay(50);
}