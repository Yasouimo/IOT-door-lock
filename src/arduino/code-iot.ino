/*
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

// Configuration des broches SPI pour ESP32
#define SS_PIN    5  // GPIO21 (adaptable selon ton câblage)
#define RST_PIN   27  // GPIO22
#define RELAY_PIN 21  // GPIO23 pour le relais
/*
// Configuration WiFi
const char* ssid     = "Wifi-ssid";
const char* password = "Your-Wifi-Pass";
*/
/*
// Configuration WiFi
const char* ssid     = "inwi Home 4G E2F379";
const char* password = "wifi-pass";
// Serveur
const char* serverUrl = "http://192.168.0.143/api.php";
;
const unsigned long timeout = 5000; // Timeout HTTP en ms

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Initialisation SPI et RFID
  SPI.begin();  // ESP32: SCK=18, MISO=19, MOSI=23 par défaut
  rfid.PCD_Init();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Connexion WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nErreur connexion WiFi!");
  } else {
    Serial.println("\nConnecté! IP: " + WiFi.localIP().toString());
  }
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(10);
    return;
  }

  // Lire UID
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    char buffer[3];
    sprintf(buffer, "%02X", rfid.uid.uidByte[i]);
    uid += buffer;
  }
  Serial.println("UID détecté: " + uid);

  // Envoi au serveur
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.setTimeout(timeout);

    int httpCode = http.POST("uid=" + uid);

    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      response.trim();
      Serial.println("Réponse serveur: " + response);

      if (response.equalsIgnoreCase("GRANTED")) {
        activateRelay(3000);
      }
    } else {
      Serial.printf("Erreur HTTP: %d\n", httpCode);
    }

    http.end();
  } else {
    Serial.println("Pas de connexion WiFi!");
  }

  // Reset RFID
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(200);
}

void activateRelay(unsigned int duration) 
{
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Relais ACTIVÉ");
  delay(duration);
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Relais DÉSACTIVÉ");
}
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

// Broches RFID (adaptées pour ESP32)
#define SS_PIN    5   // SDA
#define RST_PIN   27  // RST
#define RELAY_PIN 21  // Contrôle du relais

// Informations WiFi
const char* ssid     = "inwi Home 4G E2F379";
const char* password = "46389619";

// Adresse IP du serveur local (où se trouve api.php)
const char* serverUrl = "http://192.168.0.143/api.php";

const unsigned long timeout = 5000; // Timeout HTTP en ms

MFRC522 rfid(SS_PIN, RST_PIN); // Initialisation lecteur RFID

void setup() {
  Serial.begin(9600);
  delay(1000);

  SPI.begin();           // Initialisation SPI
  rfid.PCD_Init();       // Initialisation du module RFID

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Relais désactivé au début

  // Connexion WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nErreur de connexion WiFi !");
  } else {
    Serial.println("\nConnecté ! IP: " + WiFi.localIP().toString());
  }
}

void loop() {
  // Vérifie s'il y a une nouvelle carte
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(10);
    return;
  }

  // Lire l'UID de la carte
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    char buffer[3];
    sprintf(buffer, "%02X", rfid.uid.uidByte[i]);
    uid += buffer;
  }

  Serial.println("UID détecté: " + uid);

  // Envoyer l'UID au serveur
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // Construire l'URL avec le paramètre GET
    String fullUrl = String(serverUrl) + "?uid=" + uid;
    http.begin(client, fullUrl);
    http.setTimeout(timeout);

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      response.trim();
      Serial.println("Réponse serveur: " + response);

      if (response.equalsIgnoreCase("GRANTED")) {
        activateRelay(3000);  // Active le relais pendant 3 secondes
      } else {
        Serial.println("Accès refusé !");
      }
    } else {
      Serial.printf("Erreur HTTP: %d\n", httpCode);
    }

    http.end();
  } else {
    Serial.println("Pas de connexion WiFi !");
  }

  // Fin de communication avec la carte
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(200);
}

void activateRelay(unsigned int duration) {
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Relais ACTIVÉ");
  delay(duration);
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Relais DÉSACTIVÉ");
}
