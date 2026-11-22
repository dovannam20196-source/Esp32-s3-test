#include <Arduino.h>

// 3 chan GPIO dieu khien Damper 3 pha qua IC 18 chan
const int DAMPER_PINS[] = {5, 6, 7};

void quayDamper(bool moCua, int soVong) {
  for (int v = 0; v < soVong; v++) {
    for (int step = 0; step < 3; step++) {
      int idx = moCua ? step : (2 - step);
      for (int p = 0; p < 3; p++) {
        digitalWrite(DAMPER_PINS[p], (p == idx) ? HIGH : LOW);
      }
      delay(5); // Tre 5ms moi buoc cho banh rang keo khoe
    }
  }
  // Ngat dien hoan toan sau khi quay xong
  for (int i = 0; i < 3; i++) {
    digitalWrite(DAMPER_PINS[i], LOW);
  }
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) {
    pinMode(DAMPER_PINS[i], OUTPUT);
    digitalWrite(DAMPER_PINS[i], LOW);
  }

  delay(2000);
  // Test mo cua gio -> nghi 3 giay -> dong cua gio
  quayDamper(true, 180);
  delay(3000);
  quayDamper(false, 180);
}

void loop() {
  // Chu ky test lap lai moi 15 giay de ban quan sat la gio damper
  delay(15000);
  quayDamper(true, 180);
  delay(3000);
  quayDamper(false, 180);
}
