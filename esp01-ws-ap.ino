/*
 * WebSocketServer.ino
 *
 *  Created on: 22.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
//#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
ESP8266WebServerSecure  serverHTTPS(443);
//BearSSL::ESP8266WebServerSecure serverHTTPS(443);
ESP8266WebServer serverHTTP(80);

static const char serverCert[] PROGMEM = R"EOF(
MIIC/DCCAmWgAwIBAgIUQM97TVG/lHx2FKXttR92yyPG0oYwDQYJKoZIhvcNAQEL
BQAwfzELMAkGA1UEBhMCUk8xCjAIBgNVBAgMAUIxDzANBgNVBAcMBlRlaHJhbjEb
MBkGA1UECgwST25lVHJhbnNpc3RvciBbUk9dMRYwFAYDVQQLDA1PbmVUcmFuc2lz
dG9yMR4wHAYDVQQDDBVzdXJmYWNlLXNjYW5uZXIubG9jYWwwHhcNMjAwODEzMDU1
NTU5WhcNMjEwODEzMDU1NTU5WjB/MQswCQYDVQQGEwJSTzEKMAgGA1UECAwBQjEP
MA0GA1UEBwwGVGVocmFuMRswGQYDVQQKDBJPbmVUcmFuc2lzdG9yIFtST10xFjAU
BgNVBAsMDU9uZVRyYW5zaXN0b3IxHjAcBgNVBAMMFXN1cmZhY2Utc2Nhbm5lci5s
b2NhbDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAw4t5EvPLuokfVX58W60N
ZWYz51PAU3mMogfqawKUmDFGRYrYRpsEeONbSzXDUXKUVSYNKWGEfTGX1d2iacdm
DDUIsu5o+El+4Qlw084YLxEfQrdRjrgxeGeW8ZL91NAxeZoS5Km+0HhQSDKn72F6
tVW7lNYhRJI+dW+ESH7MlCMCAwEAAaN1MHMwHQYDVR0OBBYEFFtXSKRKcp87jtUx
/hXedGbThHkrMB8GA1UdIwQYMBaAFFtXSKRKcp87jtUx/hXedGbThHkrMA8GA1Ud
EwEB/wQFMAMBAf8wIAYDVR0RBBkwF4IVc3VyZmFjZS1zY2FubmVyLmxvY2FsMA0G
CSqGSIb3DQEBCwUAA4GBAGJeimHrS7SbdB8SklBVMcvJNb7gb94PhHK1phakcdxT
nAnIX9sPlnsVC6xQPfVyZPLDTk3Z5fSDqNkjXstdwc6NbuR1ZqE2rrZpII684DOK
VOY7IGEDwGBuKkVLzCf4jwO5GblsoPwgKfEsKW4XrwOYvUe6tMtTcdlzjQ0X2soL
)EOF";

static const char serverKey[] PROGMEM = R"EOF(
MIICeAIBADANBgkqhkiG9w0BAQEFAASCAmIwggJeAgEAAoGBAMOLeRLzy7qJH1V+
fFutDWVmM+dTwFN5jKIH6msClJgxRkWK2EabBHjjW0s1w1FylFUmDSlhhH0xl9Xd
omnHZgw1CLLuaPhJfuEJcNPOGC8RH0K3UY64MXhnlvGS/dTQMXmaEuSpvtB4UEgy
p+9herVVu5TWIUSSPnVvhEh+zJQjAgMBAAECgYBwkHMr48edI54z+tHffCi//CtY
uK933fqEs4XP//ZF/edz1M9XJZlXdZlyHArJcenNclAVSBuabdf3ttPtjGlDDkba
owl1aHy9AqTCR/sPDK2S49MOdo21+Cb0LURDJMb9/lJ8r+PFYfEdUAJFkBFUNHOH
aPcaba+fxDyOxo0WYQJBAPvUn767svODb4LKxjmkfPnAbCkmj2Q6Dqmr5+NhbsLl
MxBWd4B2P2BqASmelI+DCN/pxQgMh/4ZwcogqzJtYMkCQQDGyEiXS3+ZT0PEVTb0
WlnBgz1rC5BQv7LFca3KpHiOdRfwIUv9Z0+uM3NI01tEB72TT/XNTc8gKR2/0wQw
s0+LAkEAsbIi1k9TwoioEr5KiToMBZ9FsvPGko508jye7888fMGF+q1IvZiyt7GV
CG0hnvJzPKyTn05YV3CNUbcjlf2gIQJBAIja2x9Om0qxh7Zj9LHGgoT4FhDcpT7y
u+GzojMhiN6LPy/TvL15oyILyhMZcHwl2B7Tribqqu85X4HxhbEBwoECQQDmiJbf
Es6vwT8zBXxICmqgI5sJZnM+38SL6l3lz33of/rG9oI1JRZq8QGnJLYm3KX8DBVZ
oOWh9eQOnN/3wpSe
)EOF";

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
    MDNS.addService("https", "tcp", 443);
    MDNS.addService("ws", "tcp", 81);
    Serial.print("mDNS responder started: http://");
    Serial.print(mdnsName);
    Serial.println(".local");
}
//void secureRedirect()
//{
//    serverHTTP.sendHeader("Location", String("https://surface-scanner.local"), true);
//    serverHTTP.send(301, "text/plain", "");
//}
void startServer()
{
//    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    // handle index

    serverHTTP.on("/", []() {
        // send index.html
        serverHTTP.send(200, "text/html", "<html><head> </head><body>Hello from http</body></html>");
    });

    serverHTTP.begin();
    // https
    serverHTTPS.getServer().setRSACert(new X509List(serverCert), new PrivateKey(serverKey));
//    serverHTTPS.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
    serverHTTPS.on("/", []() {
        // send index.html
        serverHTTPS.send(200, "text/html", "<html><head> </head><body>Hello from https</body></html>");
    });
    serverHTTPS.begin();
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
    serverHTTPS.handleClient();

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
