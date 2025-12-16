#include <esp32cam.h>
#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "<3";
const char* WIFI_PASS = "baophansayhi";

WebServer server(80);

// Không còn .value()
static const auto loRes  = esp32cam::Resolution::find(320, 240);
static const auto midRes = esp32cam::Resolution::find(350, 530);
static const auto hiRes  = esp32cam::Resolution::find(800, 600);

void serveJpg() {
  auto frame = esp32cam::capture();
  if (!frame) {
    Serial.println("❌ CAPTURE FAIL");
    server.send(503, "text/plain", "Capture failed");
    return;
  }

  Serial.printf("✅ CAPTURE OK %dx%d %d bytes\n",
                frame->getWidth(),
                frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpgLo() {
  if (!esp32cam::Camera.changeResolution(loRes))
    Serial.println("⚠️ SET-LO-RES FAIL");
  serveJpg();
}

void handleJpgMid() {
  if (!esp32cam::Camera.changeResolution(midRes))
    Serial.println("⚠️ SET-MID-RES FAIL");
  serveJpg();
}

void handleJpgHi() {
  if (!esp32cam::Camera.changeResolution(hiRes))
    Serial.println("⚠️ SET-HI-RES FAIL");
  serveJpg();
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  using namespace esp32cam;
  Config cfg;
  cfg.setPins(pins::AiThinker);
  cfg.setResolution(hiRes);
  cfg.setBufferCount(2);
  cfg.setJpeg(80);

  if (!Camera.begin(cfg)) {
    Serial.println("❌ CAMERA INIT FAIL");
    for (;;);
  }
  Serial.println("✅ CAMERA OK");

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("✅ WiFi connected, IP: ");
  Serial.println(WiFi.localIP());

  server.on("/cam-lo.jpg",  handleJpgLo);
  server.on("/cam-mid.jpg", handleJpgMid);
  server.on("/cam-hi.jpg",  handleJpgHi);
  server.begin();

  Serial.println("Endpoints:");
  Serial.println("  /cam-lo.jpg");
  Serial.println("  /cam-mid.jpg");
  Serial.println("  /cam-hi.jpg");
}

void loop() {
  server.handleClient();
}
