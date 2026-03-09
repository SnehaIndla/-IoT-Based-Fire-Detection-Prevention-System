#include <WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define FLAME_PIN 4
#define GAS_PIN 34

// 🔵 ENTER YOUR WIFI DETAILS HERE
const char* ssid = "OnePlus12R";
const char* password = "saketharam";

// 🔵 ENTER YOUR THINGSPEAK DETAILS HERE
unsigned long channelID = 3279214;
const char* writeAPIKey = "LDTT803N4TGPYD3O";

WiFiClient client;

void setup() {
  Serial.begin(115200);

  pinMode(FLAME_PIN, INPUT_PULLUP);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");

  ThingSpeak.begin(client);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while(true);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {

  int gasValue = analogRead(GAS_PIN);
  int flameStatus = digitalRead(FLAME_PIN);

  String risk = "SAFE";

  if (gasValue > 3000 || flameStatus == LOW) {
    risk = "DANGER";
  }
  else if (gasValue > 1500) {
    risk = "WARNING";
  }

  // OLED Display
  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(0,0);
  display.print("Gas: ");
  display.println(gasValue);

  display.setCursor(0,20);
  display.print("Flame: ");
  display.println(flameStatus == LOW ? "YES" : "NO");

  display.setCursor(0,40);
  display.print("Risk: ");
  display.println(risk);

  display.display();

  // Upload to ThingSpeak
  ThingSpeak.setField(1, gasValue);
  ThingSpeak.setField(2, flameStatus == LOW ? 1 : 0);

  int response = ThingSpeak.writeFields(channelID, writeAPIKey);

  if(response == 200){
    Serial.println("Data uploaded to ThingSpeak");
  } else {
    Serial.println("Upload failed");
  }

  delay(15000);  // ThingSpeak requires 15 seconds
}