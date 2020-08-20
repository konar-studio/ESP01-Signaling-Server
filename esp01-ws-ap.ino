
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
//#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer serverHTTP(80);

WebSocketsServer webSocket = WebSocketsServer(81);

const char *mdnsName = "surface-scanner"; // Domain name for the mDNS responder

String inputString = ""; // a String to hold incoming data

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{

    switch (type)
    {
    case WStype_DISCONNECTED:
        //        Serial.printf("[%u] Disconnected!\n", num);
        digitalWrite(2, LOW);
        break;
    case WStype_CONNECTED:
    {
        digitalWrite(2, HIGH);

        //        IPAddress ip = webSocket.remoteIP(num);
        //        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        //        webSocket.sendTXT(num, "Connected");
    }
    break;
    case WStype_TEXT:
        //        Serial.printf("[%u] get Text: %s\n", num, payload);

        // send message to client
        // webSocket.sendTXT(num, "message here");

        // send data to all connected clients
        // webSocket.broadcastTXT("message here");
        break;
    case WStype_BIN:
        //        Serial.printf("[%u] get binary length: %u\n", num, length);
        //        hexdump(payload, length);

        // send message to client
        // webSocket.sendBIN(num, payload, length);
        break;
    }
}

void startMDNS()
{
    MDNS.begin(mdnsName); // start the multicast domain name server
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
    Serial.print("mDNS responder started: http://");
    Serial.print(mdnsName);
    Serial.println(".local");
}
void startServer()
{

    // handle index

    serverHTTP.on("/", []() {
        // send index.html
        serverHTTP.send(200, "text/html", "<html><head> </head><body>Hello from http</body></html>");
    });

    serverHTTP.begin();
    Serial.print("Web server Started");
}
void setup()
{
    Serial.begin(115200);

    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
    Serial.println();

    for (uint8_t t = 4; t > 0; t--)
    {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFiMulti.addAP("Honor 8A", "1234567890");

    while (WiFiMulti.run() != WL_CONNECTED)
    {
        delay(100);
    }
    startMDNS();
    delay(100);

    startServer();
    delay(100);

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    // reserve 200 bytes for the inputString:
    inputString.reserve(200);
}

void loop()
{
    byte ch;

    webSocket.loop();
    MDNS.update();
    serverHTTP.handleClient();
    if (Serial.available())
    {
        ch = Serial.read();
        inputString += (char)ch;
        if (ch == '\r')
        { // Command recevied and ready.
            inputString.trim();
            webSocket.broadcastTXT(inputString);
            inputString = ""; // Clear the string ready for the next command.
        }
    }
}
