#include "HX711.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ===== WiFi info =====
const char* ssid = "<3";
const char* password = "baophansayhi";

// ===== Flask server =====
String serverName = "http://172.20.10.13:5000/weight";


// ===== HX711 pins =====
#define DOUT D5
#define CLK D6
HX711 scale;

// ===== LCD (địa chỉ 0x27 hoặc 0x3F) =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ======= Calibration =======
float calibration_factor = 425.24;

void setup() {
  Serial.begin(115200);

  // --- LCD ---
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  // --- Kết nối WiFi ---
  WiFi.begin(ssid, password);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    lcd.print(".");
    Serial.print(".");
    retry++;
  }

  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
  } else {
    Serial.println("\n❌ WiFi Failed!");
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!");
  }
  delay(1000);
  lcd.clear();

  // --- Loadcell ---
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  scale.tare();
  Serial.println("✅ Loadcell Ready");
  lcd.setCursor(0, 0);
  lcd.print("Loadcell Ready");
  delay(1000);
  lcd.clear();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    if (scale.is_ready()) {
      float weight = scale.get_units(3)/1000;
      if (abs(weight) < 0.05) weight = 0;

      // --- Hiển thị LCD (mượt, xóa dòng cũ trước khi in) ---
      lcd.setCursor(0, 0);
      lcd.print("Weight:        "); 
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(abs(weight), 2);
      lcd.print(" kg");

      // --- Serial log ---
      Serial.print("⚖️  Weight: ");
      Serial.println(abs(weight), 2);

      // --- Gửi Flask ---
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");
      String payload = "{\"weight\":" + String(abs(weight), 2) + "}";
      int httpResponseCode = http.POST(payload);

      if (httpResponseCode > 0) {
        Serial.print("📤 HTTP code: ");
        Serial.println(httpResponseCode);
        lcd.setCursor(0, 0);
        lcd.print("Sent to server ");
      } else {
        Serial.print("⚠️ Send error: ");
        Serial.println(httpResponseCode);
        lcd.setCursor(0, 0);
        lcd.print("Send error!    ");
      }

      http.end();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("HX711 Error!");
      Serial.println("❌ HX711 not ready!");
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Lost!");
    Serial.println("❌ WiFi Lost!");
    WiFi.reconnect();
  }

  delay(200);
}
