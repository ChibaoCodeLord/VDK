#include "HX711.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ===== WiFi info =====
const char* ssid = "NGOC HOA";
const char* password = "home1234";

// ===== Flask servers =====
String weightServer = "http://192.168.1.22:5000/weight"; // Flask 1
String objectServer = "http://192.168.1.18:5001/object"; // Flask 2

// ===== HX711 pins =====
#define DOUT D5
#define CLK D6
HX711 scale;

// ===== LCD =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== Calibration =====
float calibration_factor = 425.24;

// ===== Biến cho lọc và thời gian =====
float filtered = 0.0;
float alpha = 0.25; // tốc độ phản ứng (0.2–0.4 là mượt)
unsigned long lastSend = 0;
String fruitName = "Unknown";
float lastDisplayed = -1;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Connecting WiFi");

  // --- WiFi ---
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi Connected");
  delay(600);
  lcd.clear();

  // --- Loadcell ---
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  scale.tare();
  Serial.println("✅ Loadcell Ready");
  lcd.print("Loadcell Ready");
  delay(600);
  lcd.clear();
}

void loop() {
  // ===== Kiểm tra WiFi =====
  if (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("WiFi Lost!   ");
    WiFi.reconnect();
    delay(500);
    return;
  }

  // ===== Đọc loadcell liên tục =====
  if (scale.is_ready()) {
    float raw = scale.get_units(1) / 1000.0;
    if (abs(raw) < 0.01) raw = 0;
    filtered = alpha * raw + (1 - alpha) * filtered;
  }

  // ===== Hiển thị LCD nếu thay đổi đáng kể =====
  if (abs(filtered - lastDisplayed) > 0.005) { // 5g thay đổi mới update
    lastDisplayed = filtered;

    lcd.setCursor(0, 0);
    lcd.print("W:");
    lcd.print(abs(filtered), 3);
    lcd.print("kg   "); // xóa ký tự thừa
  }

  lcd.setCursor(0, 1);
  lcd.print("Obj:");
  lcd.print(fruitName + "   ");

  // ===== Gửi Flask mỗi 1 giây =====
  unsigned long now = millis();
  if (now - lastSend >= 1000) {
    lastSend = now;

    WiFiClient client;
    HTTPClient http;

    // --- Gửi cân nặng ---
    http.begin(client, weightServer);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"weight\":" + String(abs(filtered), 3) + "}";
    int code = http.POST(payload);
    http.end();

    if (code > 0) Serial.printf("📤 Sent weight %.3f kg\n", filtered);

    // --- Lấy tên quả ---
    HTTPClient http2;
    http2.begin(client, objectServer);
    int fruitCode = http2.GET();
    if (fruitCode == 200) {
      String response = http2.getString();
      int start = response.indexOf("\"object\"");
      if (start != -1) {
        start = response.indexOf(":", start) + 1;
        int end = response.indexOf("}", start);
        fruitName = response.substring(start, end);
        fruitName.replace("\"", "");
        fruitName.trim();
      }
      Serial.println("🍎 Object: " + fruitName);
    }
    http2.end();
  }
}
