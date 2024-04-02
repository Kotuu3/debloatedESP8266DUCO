/* If optimizations cause problems, change them to -O0 (the default) */
#pragma GCC optimize("-Ofast")

#include <ArduinoJson.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>
#include <Ticker.h>

#include "MiningJob.h"
#include "Settings.h"

namespace {
    MiningConfig *configuration = new MiningConfig(
        DUCO_USER,
        RIG_IDENTIFIER,
        MINER_KEY
    );

    void UpdateHostPort(String input) {
        DynamicJsonDocument doc(256);
        deserializeJson(doc, input);
        const char *name = doc["name"];

        configuration->host = doc["ip"].as<String>().c_str();
        configuration->port = doc["port"].as<int>();
        node_id = String(name);

        #if defined(SERIAL_PRINTING)
          Serial.println("Poolpicker selected the best mining node: " + node_id);
        #endif
    }

    String httpGetString(String URL) {
        String payload = "";
        HTTPClient http;

        if (http.begin(URL)) {
          int httpCode = http.GET();

          if (httpCode == HTTP_CODE_OK)
            payload = http.getString();
          else
            #if defined(SERIAL_PRINTING)
               Serial.printf("Error fetching node from poolpicker: %s\n", http.errorToString(httpCode).c_str());
            #endif

          http.end();
        }
        return payload;
    }

    void SelectNode() {
        String input = "";
        int waitTime = 1;
        int poolIndex = 0;

        while (input == "") {
            #if defined(SERIAL_PRINTING)
              Serial.println("Fetching mining node from the poolpicker in " + String(waitTime) + "s");
            #endif
            input = httpGetString("https://server.duinocoin.com/getPool");
            
            delay(waitTime * 1000);
            waitTime *= 2;
            if (waitTime > 32)
                  waitTime = 32;
        }

        UpdateHostPort(input);
    }

    void SetupWifi() {
        #if defined(SERIAL_PRINTING)
          Serial.println("Connecting to: " + String(SSID));
        #endif
        WiFi.mode(WIFI_STA);
        WiFi.begin(SSID, PASSWORD);

        int wait_passes = 0;
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            delay(500);
            #if defined(SERIAL_PRINTING)
              Serial.print(".");
            #endif
            if (++wait_passes >= 10) {
                WiFi.begin(SSID, PASSWORD);
                wait_passes = 0;
            }
        }

        #if defined(SERIAL_PRINTING)
          Serial.println("\n\nSuccessfully connected to WiFi");
          Serial.println("Local IP address: " + WiFi.localIP().toString());
          Serial.println("Rig name: " + String(RIG_IDENTIFIER));
          Serial.println();
        #endif

        SelectNode();
    }

    void SetupOTA() {
        ArduinoOTA.onStart([]()
                           { 
                             #if defined(SERIAL_PRINTING)
                               Serial.println("Start"); 
                             #endif
                           });
        ArduinoOTA.onEnd([]()
                         { 
                            #if defined(SERIAL_PRINTING)
                              Serial.println("\nEnd"); 
                            #endif
                         });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                              { 
                                 #if defined(SERIAL_PRINTING)
                                   Serial.printf("Progress: %u%%\r", (progress / (total / 100))); 
                                 #endif
                              });
        ArduinoOTA.onError([](ota_error_t error)
                           {
                                Serial.printf("Error[%u]: ", error);
                                #if defined(SERIAL_PRINTING)
                                  if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
                                  else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
                                  else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
                                  else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
                                  else if (error == OTA_END_ERROR) Serial.println("End Failed");
                                #endif
                          });

        ArduinoOTA.setHostname(RIG_IDENTIFIER); 
        ArduinoOTA.begin();
    }

    void VerifyWifi() {
        while (WiFi.status() != WL_CONNECTED || WiFi.localIP() == IPAddress(0, 0, 0, 0))
            WiFi.reconnect();
    }

    void handleSystemEvents(void) {
        VerifyWifi();
        ArduinoOTA.handle();
        yield();
    }

    MiningJob *job[CORE];

    #if CORE == 2
      EasyFreeRTOS32 task1, task2;
    #endif

    void task1_func(void *) {
        #if defined(ESP32) && CORE == 2
          VOID SETUP() { }

          VOID LOOP() {
            job[0]->mine();
          }
        #endif
    }

    void task2_func(void *) {
        #if defined(ESP32) && CORE == 2
          VOID SETUP() {
            job[1] = new MiningJob(1, configuration);
          }

          VOID LOOP() {
            job[1]->mine();
          }
        #endif
    }

    void setup() {
        delay(500);

        #if defined(SERIAL_PRINTING)
          Serial.begin(500000);
          Serial.println("\n\nDuino-Coin " + String(configuration->MINER_VER));
        #endif
        pinMode(LED_BUILTIN, OUTPUT);

        assert(CORE == 1 || CORE == 2);
        WALLET_ID = String(random(0, 2811)); 
        job[0] = new MiningJob(0, configuration);

        SetupWifi();
        SetupOTA();

        job[0]->blink(BLINK_SETUP_COMPLETE);

        #if CORE == 2 && defined(ESP32)
            task1.start(task1_func);
            task2.start(task2_func);
        #endif
    }

    void loopOneCore() {
        job[0]->mine();

        #if defined(ESP8266)
            system_update_cpu_freq(160);
        #else
            setCpuFrequencyMhz(240);
        #endif

        VerifyWifi();
        ArduinoOTA.handle();
    }

    void loop() {
        #if CORE == 1
            loopOneCore();
        #endif
    }
}
