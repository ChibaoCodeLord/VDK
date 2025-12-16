#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// ================== WiFi ==================
const char* ssid     = "<3";           // Tên Wi-Fi 2.4 GHz
const char* password = "baophan123";   // Mật khẩu Wi-Fi

// ================== Server ==================
const char* serverUrl = "http://172.20.10.7:5000/upload"; // Địa chỉ Flask server

// ================== ESP32-CAM AI Thinker Pinout ==================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


// ================== Setup ==================
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\nBooting...");

  // ---- Connect Wi-Fi ----
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n❌ WiFi connection failed");
    return;
  }
  Serial.printf("\n✅ WiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());

  // ---- Camera Config ----
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // ---- Safe Settings ----
  if (psramFound()) {
    config.frame_size   = FRAMESIZE_QVGA;
    config.jpeg_quality = 15;              // giảm chất lượng
    config.fb_count     = 2;               // 2 buffer
    config.fb_location  = CAMERA_FB_IN_PSRAM;
} else {
    config.frame_size   = FRAMESIZE_QQVGA;
    config.jpeg_quality = 15;
    config.fb_count     = 2;
    config.fb_location  = CAMERA_FB_IN_DRAM;
}
config.xclk_freq_hz = 16000000;            // giảm xung


  // ---- Init Camera ----
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("❌ Camera init failed: 0x%x\n", err);
    return;
  }
  Serial.println("✅ Camera initialized");
  Serial.printf("Free heap: %d  |  Free PSRAM: %d\n", ESP.getFreeHeap(), ESP.getFreePsram());
}

// ================== Loop ==================
void loop() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("❌ Camera capture failed");
    delay(5000);
    return;
  }

  // ---- Upload to server ----
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "image/jpeg");
  int code = http.POST(fb->buf, fb->len);
  if (code > 0) {
    Serial.printf("✅ Upload success, HTTP %d\n", code);
  } else {
    Serial.printf("❌ Upload failed, error: %s\n", http.errorToString(code).c_str());
  }
  http.end();

  esp_camera_fb_return(fb);
  delay(5000);   // Chụp và gửi ảnh mỗi 5 giây
}
