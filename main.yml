#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* AP_SSID = "TuLanh-Samsung-Test";
const char* AP_PASS = "12345678";

WebServer server(80);

// 4 chan GPIO dieu khien du 4 pha Damper qua IC 18 chan
const int DAMPER_PINS[] = {4, 5, 6, 7};
String trangThai = "Sẵn sàng test";

void quayDamper(bool moCua, int soVong) {
  for (int v = 0; v < soVong; v++) {
    for (int step = 0; step < 4; step++) {
      int idx = moCua ? step : (3 - step);
      for (int p = 0; p < 4; p++) {
        digitalWrite(DAMPER_PINS[p], (p == idx) ? HIGH : LOW);
      }
      delay(5);
    }
  }
  // Ngắt điện hoàn toàn cả 4 cuộn dây sau khi quay xong
  for (int i = 0; i < 4; i++) {
    digitalWrite(DAMPER_PINS[i], LOW);
  }
  trangThai = moCua ? "Đã MỞ cửa gió" : "Đã ĐÓNG cửa gió";
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Test Damper 4 Pha</title></head><body style='font-family:sans-serif;text-align:center;padding:30px;'>";
  html += "<h2>SAMSUNG RS62R - DAMPER 4 PHA</h2>";
  html += "<p>Trạng thái: <b>" + trangThai + "</b></p>";
  html += "<p><a href='/mo'><button style='padding:15px 30px;font-size:18px;background:#28a745;color:#fff;border:none;border-radius:8px;'>MỞ CỬA GIÓ</button></a></p>";
  html += "<p><a href='/dong'><button style='padding:15px 30px;font-size:18px;background:#dc3545;color:#fff;border:none;border-radius:8px;'>ĐÓNG CỬA GIÓ</button></a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(DAMPER_PINS[i], OUTPUT);
    digitalWrite(DAMPER_PINS[i], LOW);
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);

  server.on("/", handleRoot);
  server.on("/mo", []() {
    quayDamper(true, 160);
    server.sendHeader("Location", "/");
    server.send(303);
  });
  server.on("/dong", []() {
    quayDamper(false, 160);
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
