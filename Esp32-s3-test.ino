#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

int dmP0 = 4, dmP1 = 5, dmP2 = 6, dmP3 = 7;
#define PIN_LIGHT_R  17
#define PIN_LIGHT_F  18
#define PIN_FAN      20
#define PIN_DEFROST  21

WebServer server(80);
int damperMode = 1;
bool dispSwap = false;
long dispBaud = 9600;
String lastRxHex = "Chua co";

void applyDamperMode(int mode) {
  damperMode = mode;
  if (mode == 1) { dmP0 = 4; dmP1 = 5; dmP2 = 6; dmP3 = 7; }
  if (mode == 2) { dmP0 = 4; dmP1 = 6; dmP2 = 5; dmP3 = 7; }
  if (mode == 3) { dmP0 = 4; dmP1 = 6; dmP2 = 7; dmP3 = 5; }
  pinMode(dmP0, OUTPUT); digitalWrite(dmP0, 0);
  pinMode(dmP1, OUTPUT); digitalWrite(dmP1, 0);
  pinMode(dmP2, OUTPUT); digitalWrite(dmP2, 0);
  pinMode(dmP3, OUTPUT); digitalWrite(dmP3, 0);
}

void runDamper(bool openCmd) {
  for (int step = 0; step < 500; step++) {
    int idx = openCmd ? (step % 4) : (3 - (step % 4));
    digitalWrite(dmP0, (idx == 0 || idx == 3) ? 1 : 0);
    digitalWrite(dmP1, (idx == 0 || idx == 1) ? 1 : 0);
    digitalWrite(dmP2, (idx == 1 || idx == 2) ? 1 : 0);
    digitalWrite(dmP3, (idx == 2 || idx == 3) ? 1 : 0);
    delay(8);
  }
  digitalWrite(dmP0, 0); digitalWrite(dmP1, 0); digitalWrite(dmP2, 0); digitalWrite(dmP3, 0);
}

void restartDisplayUart() {
  Serial1.end();
  delay(50);
  Serial1.begin(dispBaud, SERIAL_8N1, dispSwap ? 15 : 16, dispSwap ? 16 : 15);
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<style>body{font-family:sans-serif;background:#111;color:#fff;padding:12px;}.card{background:#222;padding:12px;border-radius:8px;margin-bottom:10px;}button{padding:10px 14px;font-size:15px;margin:4px;border-radius:6px;border:none;background:#0284c7;color:#fff;}.act{background:#16a34a;}</style></head><body>";
  html += "<h3>TEST SAMSUNG S3-N16R8</h3>";
  html += "<div class='card'><b>1. Damper (4,5,6,7) - Kieu " + String(damperMode) + "</b><br>";
  html += "<a href='/dm?m=1'><button class='" + String(damperMode==1?"act":"") + "'>Pha 1</button></a>";
  html += "<a href='/dm?m=2'><button class='" + String(damperMode==2?"act":"") + "'>Pha 2</button></a>";
  html += "<a href='/dm?m=3'><button class='" + String(damperMode==3?"act":"") + "'>Pha 3</button></a><br>";
  html += "<a href='/t?d=open'><button>MO DAMPER</button></a><a href='/t?d=close'><button>DONG DAMPER</button></a></div>";
  html += "<div class='card'><b>2. Bo Khien (15,16) - Baud " + String(dispBaud) + "</b><br>Nhan: " + lastRxHex + "<br>";
  html += "<a href='/u?s=1'><button>Dao TX/RX</button></a><a href='/u?b=2400'><button>2400</button></a><a href='/u?b=4800'><button>4800</button></a><a href='/u?b=9600'><button>9600</button></a><a href='/t?d=beep'><button>Test Beep</button></a></div>";
  html += "<div class='card'><b>3. Tai Khac</b><br><a href='/t?d=fan'><button>Quat 20</button></a><a href='/t?d=def'><button>Xa Da 21</button></a><a href='/t?d=lr'><button>Den Mat 17</button></a><a href='/t?d=lf'><button>Den Da 18</button></a></div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  pinMode(PIN_FAN, OUTPUT); digitalWrite(PIN_FAN, 0);
  pinMode(PIN_DEFROST, OUTPUT); digitalWrite(PIN_DEFROST, 0);
  pinMode(PIN_LIGHT_R, OUTPUT); digitalWrite(PIN_LIGHT_R, 0);
  pinMode(PIN_LIGHT_F, OUTPUT); digitalWrite(PIN_LIGHT_F, 0);
  applyDamperMode(1);
  restartDisplayUart();
  WiFi.softAP("Samsung_TuLanh_S3", "12345678");
  server.on("/", handleRoot);
  server.on("/dm", [](){ if(server.hasArg("m")) applyDamperMode(server.arg("m").toInt()); server.sendHeader("Location","/"); server.send(303); });
  server.on("/u", [](){ if(server.hasArg("s")) dispSwap=!dispSwap; if(server.hasArg("b")) dispBaud=server.arg("b").toInt(); restartDisplayUart(); server.sendHeader("Location","/"); server.send(303); });
  server.on("/t", [](){
    String d = server.arg("d");
    if(d=="open") runDamper(true);
    if(d=="close") runDamper(false);
    if(d=="fan") digitalWrite(PIN_FAN, !digitalRead(PIN_FAN));
    if(d=="def") digitalWrite(PIN_DEFROST, !digitalRead(PIN_DEFROST));
    if(d=="lr") digitalWrite(PIN_LIGHT_R, !digitalRead(PIN_LIGHT_R));
    if(d=="lf") digitalWrite(PIN_LIGHT_F, !digitalRead(PIN_LIGHT_F));
    if(d=="beep"){ for(int i=0;i<3;i++){ Serial1.write(0xAA); Serial1.write(0x04); Serial1.write(0x01); Serial1.write(0xAF); delay(100); } }
    server.sendHeader("Location","/"); server.send(303);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  if (Serial1.available()) {
    lastRxHex = "";
    while (Serial1.available()) {
      uint8_t b = Serial1.read();
      if (b < 16) lastRxHex += "0";
      lastRxHex += String(b, HEX) + " ";
    }
  }
}
