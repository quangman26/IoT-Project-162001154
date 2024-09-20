#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

#define LED 2

WebSocketsClient webSocket;

const char *ssid = "CTY SONG HONG TAN"; // Tên WiFi
const char *password = "SHT12345678@"; // Mật khẩu WiFi
const char *websocket_server = "192.168.99.54"; // Địa chỉ máy tính
const int websocket_port = 1880;
const char *url = "/esp32"; // Cập nhật URL cho ESP32

unsigned long last_time = 0;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) 
{
  switch (type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      webSocket.sendTXT("Connected");
      break;
    case WStype_TEXT:
      String message = "";
      for (size_t i = 0; i < length; i++) 
      {
        message += (char)payload[i];
      }

      if (message == "1") 
      {
        digitalWrite(LED, LOW);
      }

      if (message == "0") 
      {
        digitalWrite(LED, HIGH);
      }
      break;
  }
}

void sendToServer() 
{
  if (millis() - last_time > 3000) 
  {
    last_time = millis();
    if (webSocket.isConnected()) 
    {
      int co2 = random(500, 1000); // CO2 từ 500ppm đến 1000ppm
      int airQuality = random(50, 201); // Chất lượng không khí từ 50 đến 200 AQI

      // Hiển thị
      Serial.print("CO2: ");
      Serial.print(co2);
      Serial.print(" ppm, Air Quality: ");
      Serial.print(airQuality);
      Serial.println(" AQI");

      // Tạo chuỗi payload để gửi qua WebSocket
      String payload = "{\"co2\":";
      payload += co2;
      payload += ",\"airQuality\":";
      payload += airQuality;
      payload += "}";

      webSocket.sendTXT(payload.c_str());
    }
  }
}


void setup() 
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  Serial.print("Connecting to Wifi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wifi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Địa chỉ server, cổng và URL
  webSocket.begin(websocket_server, websocket_port, url);

  // Hàm xử lý sự kiện
  webSocket.onEvent(webSocketEvent);

  // Thử lại kết nối sau 5000ms nếu kết nối thất bại
  webSocket.setReconnectInterval(5000);
}

void loop() 
{
  webSocket.loop();
  sendToServer();
}
