#ifndef WIFI_CORE_H
#define WIFI_CORE_H

#include "WiFi.h"
#include "ESPAsyncWebServer.h"

class WifiCore {
public:
    AsyncWebServer server;
    char* ssid;
    char* password;

    WifiCore(char* ssid, char* password): server(80), ssid(ssid), password(password) {}

    void setupAP() {
        // Setting the ESP as an access point
        Serial.print("Setting AP (Access Point)…");

        // Remove the password parameter, if you want the AP (Access Point) to be open
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ssid, password);

        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);
    }

    void setupStation() {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        connectToAP();
    }

    void connectToAP() {
        Serial.println("Connecting");
        while(WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.print("Connected to WiFi network with IP Address: ");
        Serial.println(WiFi.localIP());
    }

    void checkConnection() {
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.reconnect();
            connectToAP();
        }
    }

    void setupServer() {
        // Start server
        server.begin();
    }
};

#endif // WIFI_CORE_H