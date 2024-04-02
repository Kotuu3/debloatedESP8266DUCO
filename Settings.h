// Settings.h
#ifndef SETTINGS_H
#define SETTINGS_H

// ---------------------- General settings ---------------------- //
// Change the part in brackets to your Duino-Coin username
extern const char DUCO_USER[] = "Kotuu3";
// Change the part in brackets to your mining key (if you have set it in the wallet)
extern const char MINER_KEY[] = "None";
// Change the part in brackets if you want to set a custom miner name
// Use Auto to autogenerate, None for no custom identifier
extern const char RIG_IDENTIFIER[] = "ESP";
// Change the part in brackets to your WiFi name
extern const char SSID[] = "";
// Change the part in brackets to your WiFi password
extern const char PASSWORD[] = "";

#define LED_HASHRATEWARNING

// Comment out the line below if you wish to disable LED blinking
#define LED_BLINKING

// Comment out the line below if you wish to disable Serial printing
#define SERIAL_PRINTING

#if defined(ESP8266)
    #define LED_WARNING 6

#if defined(ESP8266)
    // ESP8266
    #define LED_BUILTIN 5
#elif defined(CONFIG_FREERTOS_UNICORE) 
    // ESP32-S2
    #define LED_BUILTIN 15
#else
    // ESP32
    #define LED_BUILTIN 2
#endif

#define BLINK_SETUP_COMPLETE 2
#define BLINK_CLIENT_CONNECT 5

#define SOFTWARE_VERSION "4.0"
extern unsigned int hashrate = 0;
extern unsigned int difficulty = 0;
extern unsigned long share_count = 0;
extern String node_id = "";/
extern String WALLET_ID = "";


#endif  // End of SETTINGS_H