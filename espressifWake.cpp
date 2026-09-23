#include <WiFi.h>
#include <WebServer.h>
#include <USB.h>
#include <USBHIDKeyboard.h>

// Initialize USB HID Keyboard object
USBHIDKeyboard Keyboard;

// Wi-Fi AP Credentials
const char* ssid = "Deck_Wake_Bridge";
const char* password = "securepassword"; // Optional: leave blank or set 8+ chars

WebServer server(80);

// Pin assigned to pulse the Steam Deck power switch or transistor relay
const int POWER_PIN = 4; 

void handleRoot() {
  server.send(200, "text/html", 
    "<html><body>"
    "<h2>Steam Deck Wake & Launch Bridge</h2>"
    "<form action=\"/trigger\" method=\"POST\">"
    "<button type=\"submit\" style=\"font-size:24px;padding:20px;\">WAKE & LAUNCH</button>"
    "</form>"
    "</body></html>"
  );
}

void handleTrigger() {
  server.send(200, "text/plain", "Trigger received. Waking system and executing macro...");

  // 1. Pulse the power pin to wake the Steam Deck (adjust delay as needed for your relay)
  digitalWrite(POWER_PIN, HIGH);
  delay(500);
  digitalWrite(POWER_PIN, LOW);

  // 2. Wait for the Steam Deck to boot up and reach the desktop / game mode
  delay(6000); 

  // 3. Send a keystroke macro to launch your application or Sunshine session
  // Example: Ctrl+Alt+T to open terminal, or a custom desktop shortcut key combination
  Keyboard.press(KEY_LEFT_GUI); // Windows key / Steam menu equivalent
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  
  delay(500);
  
  // Type command or shortcut string if needed
  Keyboard.println("flatpak run night.code.sunshine"); // Placeholder command example
}

void setup() {
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);

  // Start Wi-Fi Access Point
  WiFi.softAP(ssid, password);
  
  // Setup Web Server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/trigger", HTTP_POST, handleTrigger);
  server.begin();

  // Start Native USB & HID Stack
  Keyboard.begin();
  USB.begin();
}

void loop() {
  server.handleClient();
}
