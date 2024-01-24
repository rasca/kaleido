#ifndef WIFI_CORE_H
#define WIFI_CORE_H

#include "WiFi.h"
#include <HTTPClient.h>
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

    int POST(String path, String encodedData) {
        WiFiClient client;
        HTTPClient http;
        // Your Domain name with URL path or IP address with path
        http.begin(client, "http://192.168.4.1" + path);

        // Specify content-type header
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Send HTTP POST request
        int httpResponseCode = http.POST(encodedData);
        http.end();

        return httpResponseCode;
    }

    // Sample usage:
    //
    // Server:
    //
    // void Project::initialize(Framework<NUM_LEDS>& framework) {
    //     // Add any additional initialization logic here
    //     wifi.setupAP();
    //     wifi.server.on("/gyro", HTTP_POST, [this](AsyncWebServerRequest *request) {
    //         this->processGyroData(request);
    //     });
    //     wifi.setupServer();
    // }
    //
    // void Project::processGyroData(AsyncWebServerRequest *request)
    // {
    //     int params = request->params();
    //     for (int i = 0; i < params; i++)
    //     {
    //         AsyncWebParameter *p = request->getParam(i);
    //         // Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    //     }
    //     request->send_P(200, "text/plain", "OK");
    // }
    //
    //
    // Client:
    // void Project::initialize(Framework<0>& framework) {
    //     wifi.setupStation();
    //     gyro.initialize();
    // }
    // void Project::tick() override {
    //     wifi.checkConnection();
    //     gyro.tick();

    //     // Data to send with HTTP POST
    //     String gyroData = ("a_x=" + String(gyro.a.acceleration.x) +
    //                       "&t=" + String(gyro.temp.temperature)
    //                       );
    //     wifi.POST("/gyro", gyroData);
    // }


};

#endif // WIFI_CORE_H