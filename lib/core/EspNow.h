#ifndef ESP_NOW_H
#define ESP_NOW_H

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>


uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

template<typename T>
class EspNow {

esp_now_peer_info_t peerInfo;
uint8_t channel = 7;  // default channel

public:
    T data;

    void setup(esp_now_recv_cb_t OnDataRecv, esp_now_send_cb_t OnDataSent, uint8_t wifi_channel = 7)
    {
        // Set device as a Wi-Fi Station
        WiFi.mode(WIFI_STA);

        // Set WiFi channel
        channel = wifi_channel;
        esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

        // Init ESP-NOW
        if (esp_now_init() != ESP_OK)
        {
            Serial.println("Error initializing ESP-NOW");
            return;
        }
        esp_now_register_send_cb(OnDataSent);
        esp_now_register_recv_cb(OnDataRecv);
    }

    void setupBroadcast()
    {
        setupPeer(broadcastAddress);
    }

    void setupPeer(uint8_t peer[])
    {
        // Register peer
        memcpy(peerInfo.peer_addr, peer, 6);
        peerInfo.channel = channel;  // Use the same channel as WiFi
        peerInfo.encrypt = false;

        // Add peer
        if (esp_now_add_peer(&peerInfo) != ESP_OK)
        {
            Serial.println("Failed to add peer");
            return;
        }
    }

    void send()
    {
        // you need to fill up data before calling send
        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&data, sizeof(data));

        if (result == ESP_OK)
        {
            // Serial.println("Sent with success");
        }
        else
        {
            // Serial.println("Error sending the data");
        }
    }

    // // callback when data is sent
    // void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    // {
    //     Serial.print("\r\nLast Packet Send Status:\t");
    //     Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    // }

    // void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
    // {
    //     memcpy(&myData, incomingData, sizeof(myData));
    //     Serial.print("Bytes received: ");
    //     Serial.println(len);
    //     Serial.println(data.a);
    // }
};

#endif // ESP_NOW_H