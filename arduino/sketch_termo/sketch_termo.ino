#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <dev-config.h>

#define ONE_WIRE_BUS 14
//#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const int sleepTime  = 5 * 60 * 1000000;  // 5 m
float temp;
int retry = 5;
uint16_t Vcc;
char msg[50];
ADC_MODE(ADC_VCC);


void setup(void) {
  Vcc = ESP.getVcc();
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Start wifi thermometer");
  Serial.print("ChipId:");
  Serial.println(ChipId);
  setup_wifi();
  setup_mqtt();
  sensors.begin();
  send_temp();
  client.loop();
  ESP.deepSleep( sleepTime );
}

void setup_mqtt(){
  Serial.print("mqtt_server");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(mqtt_port);

  client.setServer(mqtt_server, mqtt_port);
  if (!client.connected()) {
    reconnect();
  }
  sprintf (msg, "%d-11", ChipId);
  client.publish("register", msg);
  client.loop();
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      if( retry-- ){
        delay(5000);
        } else {
          ESP.deepSleep( sleepTime );
        }
    }
  }
}


void send_temp(){
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  Serial.println("Temperature for the device 1 (index 0) is: ");
  sprintf(msg, "%d %d %d",ChipId, int( temp * 100 ), Vcc  );
  client.publish("temp", msg);
  Serial.println(msg);
//  Serial.println(Vcc);
}

void loop(void) {
}
