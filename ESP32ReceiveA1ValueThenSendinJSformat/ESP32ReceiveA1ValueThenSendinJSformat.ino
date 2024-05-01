// Part 2 of the firmware 
// This code was uploaded to the esp32 only 
// this code was written when the esp32 was connected to the arduino 
// serial 2 was to establsih a communication between the arduino and the esp32 
// the sensor was received and sent to the web page using a the POST and the GET methods 



#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "ADT";
const char *password = "adt@12345";
const char *server_address = "192.168.1.2";
const int serverPort = 5000;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 18, 19);

  // Connect to WiFi
  connectToWiFi();

  // Start server
  server.begin();
}

void loop() {
  handleWiFiClientRequests();
  handleArduinoData();
  handleSerialData();
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void handleWiFiClientRequests() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New WiFi client");

    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print("Received from client: ");
        Serial.println(line);

        // Forward data to Arduino via Serial2
        Serial2.print(line);

        // You can add additional logic here if needed
      }
    }

    client.stop();
    Serial.println("WiFi client disconnected");
  }
}

void handleArduinoData() {
  if (Serial2.available() > 0) {
    String receivedData = Serial2.readStringUntil('\n');
    Serial.println("Received from Arduino: " + receivedData);

    // Add logic to handle Arduino data if needed
  }
}

void handleSerialData() {
  if (Serial2.available() > 0) {
    String receivedData = Serial2.readStringUntil('\n');
    Serial.println(receivedData);

    if (receivedData.startsWith("Analog Value")) {
      int sensorValue = receivedData.substring(20).toInt();
      Serial.println("Sensor Value: " + String(sensorValue));

      // Send the sensor value to the HTTP server
      sendSensorValueToHTTPServer(sensorValue);

      // Send the sensor value over WiFi
      sendSensorValueOverWiFi(sensorValue);

      // Fetch emergency level over WiFi
      int emergencyLevel = fetchEmergencyLevelOverWiFi();
      Serial.println("Received Emergency Level: " + String(emergencyLevel));
      Serial2.println("Received Emergency Level: " + String(emergencyLevel));
      delay(1000);
    }
  }
}

void sendSensorValueToHTTPServer(int sensorValue) {
  HTTPClient http;
  String url = "/update";
  String postData = "value=" + String(sensorValue);

  http.begin(server_address, serverPort, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);

  if (httpCode == 200) {
    Serial.println("HTTP POST request successful");
    Serial.println(http.getString());
  } else {
    Serial.println("HTTP POST request failed");
    Serial.println("HTTP Code: " + String(httpCode));
  }

  http.end();
  delay(1000);
}

void sendSensorValueOverWiFi(int value) {
  WiFiClient client;
  String url = "/update?value=" + String(value);

  if (client.connect(server_address, serverPort)) {
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server_address + "\r\n" +
                 "Connection: close\r\n\r\n");

    delay(1000);

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }

    client.stop();
    Serial.println("Server disconnected");
  } else {
    Serial.println("Connection to server failed");
  }
}

int fetchEmergencyLevelOverWiFi() {
  WiFiClient client;
  String url = "/emergency";

  if (client.connect(server_address, serverPort)) {
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server_address + "\r\n" +
                 "Connection: close\r\n\r\n");

    delay(1000);

    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.print("read this: ");
      Serial.println(line);
      if (line.startsWith("{\"level\":")) {
        int emergencyLevel = line.substring(9).toInt(); // Adjust the substring index
        Serial.println("we are turning emergency level");
        Serial.write(emergencyLevel);
        return emergencyLevel;
      }
    }

    client.stop();
    Serial.println("Server disconnected");
  } else {
    Serial.println("Connection to server failed");
  }

  return 0; // Return an appropriate default value if the fetch fails
}
