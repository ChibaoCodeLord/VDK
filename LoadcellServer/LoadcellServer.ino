/* esp8266_hx711_post.ino
   ESP8266 (NodeMCU/Wemos) + HX711 example
   - Read HX711 periodically
   - Use moving average and stddev to decide "stable"
   - POST JSON to server via HTTPS
   - If offline, queue JSON strings to LittleFS (one file per entry)
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <LittleFS.h>
#include "HX711.h"
#include <ArduinoJson.h>
#include <Ticker.h>

const char* WIFI_SSID = "<3";
const char* WIFI_PASS = "baophan123" 

const char* SERVER_HOST = "172.20.10.7";  
const int   SERVER_PORT = 8000
const char* WEIGHT_ENDPOINT = "/api/weight";
const char* API_KEY = "123456";

const int DOUT_PIN = D2; // HX711 DT
const int SCK_PIN  = D3; // HX711 SCK

HX711 scale;

// parameters for stability detection
const int SAMPLE_WINDOW = 8;      // number of samples in sliding window
const unsigned long READ_INTERVAL_MS = 200; // sample period
const float STABLE_STDDEV_THRESHOLD = 3.0; // grams, tune this

// calibration
float calibration_factor = -7050.0; // set yours (negative for some modules)
float offset = 0.0;

float samples[SAMPLE_WINDOW];
int sample_idx = 0;
int samples_count = 0;

unsigned long last_read_ms = 0;
unsigned long seq = 0;

Ticker wifiTicker;

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
  } else {
    Serial.println("WiFi connect failed");
  }
}

// helper: compute mean & stddev
void push_sample(float v) {
  samples[sample_idx] = v;
  sample_idx = (sample_idx + 1) % SAMPLE_WINDOW;
  if (samples_count < SAMPLE_WINDOW) samples_count++;
}

float mean_samples() {
  float s = 0;
  for (int i = 0; i < samples_count; ++i) s += samples[i];
  return s / max(1, samples_count);
}

float stddev_samples(float mean) {
  float s = 0;
  for (int i = 0; i < samples_count; ++i) {
    float d = samples[i] - mean;
    s += d * d;
  }
  return sqrt(s / max(1, samples_count));
}

// LittleFS queue helpers
void initStorage() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
  }
}

String makeWeightJson(float weight, bool stable) {
  StaticJsonDocument<256> doc;
  doc["device_id"] = "scale-01";
  // timestamp: use millis since boot plus server can assign real time; better if you implement NTP
  doc["timestamp_ms"] = millis();
  doc["seq"] = ++seq;
  doc["weight_g"] = weight;
  doc["stable"] = stable;
  doc["battery_v"] = (ESP.getVcc() / 1024.0); // requires ADC configured on some boards; may be 0
  String out;
  serializeJson(doc, out);
  return out;
}

void enqueueJson(const String &json) {
  // store as one file per entry to /queue/seq.json
  String fname = "/queue/" + String((unsigned long)millis()) + "_" + String(seq) + ".json";
  File f = LittleFS.open(fname, "w");
  if (f) {
    f.print(json);
    f.close();
    Serial.println("Queued to " + fname);
  } else {
    Serial.println("Failed to queue");
  }
}

void flushQueue(); // forward declare

bool postJsonToServer(const String &json) {
  connectWiFi();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No WiFi, will queue");
    return false;
  }

  // create secure client (no cert pin here; optional)
  BearSSL::WiFiClientSecure client;
  client.setInsecure(); // WARNING: for production, use fingerprint or root CA
  Serial.printf("Connecting to %s:%d\n", SERVER_HOST, SERVER_PORT);
  if (!client.connect(SERVER_HOST, SERVER_PORT)) {
    Serial.println("Connection failed");
    return false;
  }

  // prepare HTTP request
  String payload = json;
  String req = String("POST ") + WEIGHT_ENDPOINT + " HTTP/1.1\r\n" +
               "Host: " + SERVER_HOST + "\r\n" +
               "User-Agent: esp8266-hx711/1.0\r\n" +
               "Authorization: Bearer " + API_KEY + "\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + String(payload.length()) + "\r\n\r\n" +
               payload;

  client.print(req);

  // wait for response
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  // read status line
  String status = client.readStringUntil('\n');
  Serial.println("Server status: " + status);
  // simple check for 200/201
  if (status.indexOf("200") == -1 && status.indexOf("201") == -1) {
    // still read rest
    while (client.available()) client.readString();
    client.stop();
    return false;
  }

  // read headers & body (optional)
  while (client.available()) {
    String line = client.readStringUntil('\n');
    // optionally parse body
  }
  client.stop();
  return true;
}

void flushQueue() {
  if (!LittleFS.exists("/queue")) {
    // nothing
    return;
  }
  Dir dir = LittleFS.openDir("/queue");
  while (dir.next()) {
    String path = dir.fileName();
    File f = LittleFS.open(path, "r");
    if (!f) continue;
    String json = f.readString();
    f.close();
    Serial.println("Trying to flush " + path);
    if (postJsonToServer(json)) {
      LittleFS.remove(path);
      Serial.println("Flushed & removed " + path);
    } else {
      Serial.println("Flush failed for " + path);
      // stop trying further to prevent infinite loops
      break;
    }
    delay(200);
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 HX711 Starting...");
  initStorage();

  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); // reset zero

  connectWiFi();

  // ensure queue directory exists
  if (!LittleFS.exists("/queue")) {
    LittleFS.mkdir("/queue");
  }
}

void loop() {
  unsigned long now = millis();
  if (now - last_read_ms >= READ_INTERVAL_MS) {
    last_read_ms = now;
    // read value
    if (!scale.is_ready()) {
      Serial.println("HX711 not ready");
      delay(10);
      return;
    }
    long raw = scale.read();
    float weight = scale.get_units(1); // get_units returns adjusted weight in kg or g depending on calibration; treat as grams? assume calibration gives grams
    // if your calibration set returns kg, multiply accordingly
    // here we treat as grams already
    push_sample(weight);
    float m = mean_samples();
    float sd = stddev_samples(m);

    bool stable = (samples_count >= SAMPLE_WINDOW) && (sd < STABLE_STDDEV_THRESHOLD);

    Serial.printf("w=%.2f mean=%.2f sd=%.2f stable=%d\n", weight, m, sd, stable);

    String j = makeWeightJson(weight, stable);
    // first try flush queued
    flushQueue();
    // try post current
    if (!postJsonToServer(j)) {
      // queue if fail
      enqueueJson(j);
    }
  }
}
