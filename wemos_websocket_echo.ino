#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <WebSocketsClient.h>
WebSocketsClient webSocketClient;

//=================================================================================================================================

// Enter your WiFi setup here:
const char *SSID = "weke-weke";
const char *PASSWORD = "password";

char *WEB_IP_ADDRESS = "echo.websocket.org"; 
//================================================================================================================================

bool isConnected = false;

int IRInput1 = 5;  //D1
int IRInput2 = 4;  //D2
int IRInput3 = 0;  //D3

int out1 = 1, out2 = 1, out3 = 1;

void webSocketClientEvent(WStype_t type, uint8_t * payload, size_t length) {

   String param = (char*)payload;

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      
      isConnected = true;
      Serial.printf("[WSc] Connected to url: %s\n", payload);      
    }
      break;
    case WStype_TEXT:
      {
        Serial.printf("Received by websocket server: %s\n", payload);
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocketClient.sendBIN(payload, length);
      break;
        case WStype_PING:
            // pong will be send automatically
            Serial.printf("[WSc] get ping\n");
            break;
        case WStype_PONG:
            // answer to a ping we send
            Serial.printf("Websocket sent a pong\n");
            break;
    }

}

void setup() {
  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial);      // Do nothing if no serial port is opened 

  pinMode(IRInput1,INPUT);
  pinMode(IRInput2,INPUT);
  pinMode(IRInput3,INPUT);

  Serial.println("Disconnecting Wifi connection");
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);

  WiFi.begin(SSID, PASSWORD);

  // Try forever
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting to WiFi");
    delay(1000);
  }
  Serial.print("Wifi Connected, IP Address: ");
  Serial.println(WiFi.localIP());

  connectSocket();

}

void connectSocket() {
  
  webSocketClient.begin((String)WEB_IP_ADDRESS, 80, "/");

  // event handler
  webSocketClient.onEvent(webSocketClientEvent);

  // try ever 5000 again if connection has failed
  webSocketClient.setReconnectInterval(5000);
  
  webSocketClient.enableHeartbeat(15000, 3000, 2);
  
  
}


void loop() {  
   webSocketClient.loop();
 
  if (isConnected) {
        static int lastEcho = 0;
        if (millis()-lastEcho > 5000) {
            lastEcho = millis();
            out1 = analogRead(IRInput1); delay(10);
            out2 = analogRead(IRInput2); delay(10);
            out3 = analogRead(IRInput3); delay(10);
            Serial.println(" ");
            Serial.printf("IR1: %d | IR2: %d | IR3: %d", out1, out2, out3); 
            Serial.println(" ");
            delay(50);
            Serial.println("You sent a ping!");
            webSocketClient.sendPing();
            Serial.println("You sent => Echo:" + String(out1) + "|" + String(out2) + "|" + String(out3) + "|" + String(lastEcho));
            webSocketClient.sendTXT("Echo:" + String(out1) + "|" + String(out2) + "|" + String(out3) + "|" + String(lastEcho));
        }          
  }
}
