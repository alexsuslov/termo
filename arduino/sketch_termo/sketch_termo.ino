#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <config.h>

#define mqtt_port 1883
#define ONE_WIRE_BUS 4
const int sleepTimeS  = 5 * 60;  // 5 m

WiFiClient espClient;
PubSubClient client(espClient);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temp;
String buff;
char msg[50];

void setup(void) {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Start wifi termo");
  Serial.print("ChipId:");
  Serial.println(ChipId);
  setup_wifi();
  setup_mqtt();
  sensors.begin();
  send_temp();
  client.loop();
  ESP.deepSleep(sleepTimeS * 1000000);
}

void setup_mqtt(){
  IPAddress mqtt_server(192, 168, 127, 1);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();  
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
//      client.publish("outTopic","hello world");
//      client.publish( "ESP/connect", String(ChipId) ); 
//      client.subscribe(ledTopic); // resubscribe
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void send_temp(){
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(temp);
  buff = String( String( ChipId ) + ":" + String( temp, 4));
  buff.toCharArray( msg, sizeof(buff) + 5);
  client.publish("temp", msg);
  Serial.println("Temperature send DONE");
}

void loop(void) {
//  Serial.println("loop start");
//  client.loop();
//  delay(5000);
//  send_temp();
}
