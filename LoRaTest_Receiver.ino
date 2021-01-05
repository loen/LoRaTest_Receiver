
#include "heltec.h" 
#include "WiFi.h"
#include "PubSubClient.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6
String rssi = "RSSI --";
String packSize = "--";
String packet ;

const String ssid = "polsreberka";
const String password = "Argentyna2014";
const String mqtt_server = "192.168.0.107";
const String usr = "mqtt";
const String pass = "Polki1234!";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid.c_str());

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Heltec.display->drawString(0, 0, "WiFi connected SSID:");
  Heltec.display->drawString(0, 10, ssid); 
  Heltec.display->drawString(0, 20, "IP:" + WiFi.localIP().toString());
  Heltec.display->display();
  delay(2000);
  Heltec.display->clear();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "LoRa-Test";
    // Attempt to connect
    if (client.connect(clientId.c_str(),usr.c_str(),pass.c_str())) {
      Serial.println("connected");
      Heltec.display->drawString(0, 50, "MQTT Connected"); 
      Heltec.display->display();
      delay(2000);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      Heltec.display->drawString(0, 50, "MQTT - Try to reconnect in 5 seconds"); 
      Heltec.display->display();
      delay(5000);
    }
  }
}


void LoRaData(){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0 , 15 , "Received "+ packSize + " bytes");
  Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet);
  Heltec.display->drawString(0, 0, rssi);  
  Heltec.display->display();
}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  LoRaData();
  String p = packet + "/" + rssi;
  client.publish("heltec", p.c_str() );
  
}

void setup() { 
   //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
 
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  //delay(1500);
  setup_wifi();
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->drawString(0, 10, "Wait for incoming data...");
  Heltec.display->display();
  delay(1000);
  LoRa.receive();
  client.setServer(mqtt_server.c_str(), 1883);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize);  }
  delay(10);
}
