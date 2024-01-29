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
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start server
  server.begin();
}
void loop() {
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

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
      Serial.println(String());
      if (line.startsWith("level: ")) {
        int emergencyLevel = line.substring(8).toInt(); // Adjust the substring index
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
