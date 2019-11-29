#include "PubSubClient.h" //Knolleary
#include  <ESP8266WiFi.h> //ESP8266WiFi.h
//#include   <credentials.h> //This is a personal file containing web credentials

const char* ssid = "reginato";// this constant is defined in my credentials file
const char* password = "reginato";// ditto
//char* topic="channels/<channelID/publish/<channelAPI>
char* topic = "channels/921259/publish/BX9HBEYJD3G03PAT";
char* server = "mqtt.thingspeak.com";

unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 3000;//sampe 1s ;
unsigned long delay_time = 15000;
unsigned long lowpulseoccupancy = 0;
float mp25 = 0;
int pin = D2;
float concentration = 0;
float ratio = 0;

WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print("Connecting");
  Serial.println(ssid);

  pinMode(D2, INPUT);
  starttime = millis();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  String clientName = "ESP-Thingspeak";
  Serial.print("Connecting");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);

  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic");
    Serial.println(topic);
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
}

void loop() {

  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;


  if ((millis() - starttime) > sampletime_ms) //if the sampel time == 30s
  {
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0);
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;

    Serial.print("Ratio: ");
    Serial.print(ratio);
    Serial.print("   ");
    Serial.print("MP2.5: ");
    Serial.println(mp25);
    lowpulseoccupancy = 0;

    //convert sensor values to strings
    String dustStr = String(mp25);
    dustStr.replace(" ", "");

    delay(delay_time);
    starttime = millis();
    Serial.println(concentration);


    if (concentration < 1000) {

      for (int i = 0; i < 16; ++i)
      {

      }
      Serial.println("CLEAN");
    }
  }
  static int counter = 0;
  String payload = "field1=";
  payload += concentration;
  payload += "&status=MQTTPUBLISH";
  String clientName = "ESP-Thingspeak";
  if (client.connect((char*) clientName.c_str())) {
    Serial.print("Sendingoad: ");
    Serial.println(payload);

    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  }
  ++counter;
  delay(120000);
}
