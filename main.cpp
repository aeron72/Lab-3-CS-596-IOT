#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DFRobot_DHT20.h"

// ----------------------- Wi-Fi Credentials ------------------------
#define WIFI_SSID "WifiName"      // Replace with your Wi-Fi network name (SSID)
#define WIFI_PASSWORD "WifiPass"  // Replace with your Wi-Fi password

// ----------------------- EC2 Flask Server -------------------------
const char* serverUrl = "http://18.212.5.0:5000/update";  // Replace with your EC2 server's public IP and port

// ----------------------- DHT20 Sensor Setup -----------------------
DFRobot_DHT20 dht20;               // Create DHT20 sensor object
float humidity, temperature;       // Variables to store sensor readings

void sendDataToEC2();              // Function prototype to send data to EC2

// ----------------------- Arduino Setup Function -------------------
void setup() {
    Serial.begin(115200);         // Initialize serial communication for debugging
    delay(1000);                  // Short delay to ensure Serial is ready

    // Connect to Wi-Fi
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Begin Wi-Fi connection

    // Retry connection up to 20 times (approximately 20 seconds)
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 20) {
        delay(1000);
        Serial.print(".");
        attempt++;
    }

    // Check if connected to Wi-Fi
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to Wi-Fi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());     // Print local IP address
    } else {
        Serial.println("\nFailed to connect to Wi-Fi. Restarting...");
        ESP.restart();                      // Restart the microcontroller if connection fails
    }

    // Initialize the DHT20 sensor
    dht20.begin();
}

// ----------------------- Main Loop --------------------------------
void loop() {
    // Read humidity and temperature from the DHT20 sensor
    humidity = dht20.getHumidity();
    temperature = dht20.getTemperature();

    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT20 sensor!");
        return;  // Skip sending if sensor data is invalid
    }

    // Print sensor readings to Serial Monitor
    Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%\n", temperature, humidity);

    // Send the sensor data to EC2 server
    sendDataToEC2();

    // Wait for 10 seconds before next reading
    delay(10000);
}

// ----------------------- Send Data to EC2 -------------------------
void sendDataToEC2() {
    // Check if Wi-Fi is still connected
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected! Reconnecting...");
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Reconnect to Wi-Fi
        return;
    }

    HTTPClient http;  // Create HTTP client object

    // Construct the GET request URL with temperature and humidity as parameters
    String url = String(serverUrl) + "?temp=" + String(temperature) + "&humidity=" + String(humidity);

    Serial.print("Sending GET request: ");
    Serial.println(url);

    // Initialize the GET request
    http.begin(url);
    int httpResponseCode = http.GET();  // Send the request

    // Print the response code
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // If the response is OK, print server's response
    if (httpResponseCode > 0) {
        Serial.println("Server Response: " + http.getString());
    } else {
        Serial.println("Failed to send data. Check AWS security settings.");
    }

    http.end();  // Close the HTTP connection
}
