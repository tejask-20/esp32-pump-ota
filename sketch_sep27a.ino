#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

// 1. Credentials for the REMOTE WiFi the ESP32 will connect to
const char* ssid = "Mauli";
const char* password = "Khedekar@123";

// 2. Update this manually in the code every time you make a change!
const String FIRMWARE_VERSION = "1.0"; 

// 3. The raw internet links to your hosted files
const char* version_url = "http://raw.githubusercontent.com/tejask-20/esp32-pump-ota/refs/heads/main/version.txt";
const char* firmware_url = "https://raw.githubusercontent.com/tejask-20/esp32-pump-ota/refs/heads/main/firmware.bin";

const int ledPin = 2; // Built-in blue LED

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  connectToWiFi();
  
  // Check the internet for a new version immediately on boot
  checkForUpdates();
}

void loop() {
  // Your normal automation code (e.g., dual-tank sensors, relays) goes here
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  
  // Flash the blue LED 5 times rapidly to confirm connection
  for(int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH); delay(150);
    digitalWrite(ledPin, LOW); delay(150);
  }
}

void checkForUpdates() {
  Serial.println("Checking for OTA updates...");
  
  WiFiClientSecure client;
  client.setInsecure(); // Bypass SSL verification for simple HTTPS downloads

  HTTPClient http;
  http.begin(client, version_url);
  int httpCode = http.GET();

  // If we successfully read the online text file
  if (httpCode == HTTP_CODE_OK) {
    String latest_version = http.getString();
    latest_version.trim(); // Remove any accidental spaces or newlines
    
    Serial.println("Current ESP32 Version: " + FIRMWARE_VERSION);
    Serial.println("Latest Online Version: " + latest_version);

    // Compare the versions
    if (latest_version != FIRMWARE_VERSION && latest_version != "") {
      Serial.println("New version detected! Downloading and installing...");
      
      // This line handles the download, writing to memory, and rebooting
      t_httpUpdate_return ret = httpUpdate.update(client, firmware_url);
      
      if (ret == HTTP_UPDATE_FAILED) {
        Serial.printf("Update Failed Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      }
    } else {
      Serial.println("ESP32 is already running the latest version.");
    }
  } else {
    Serial.println("Failed to reach version.txt. HTTP Code: " + String(httpCode));
  }
  http.end();
}