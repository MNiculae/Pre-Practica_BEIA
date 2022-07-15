#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define PIN 2  
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//wiFi
const char* ssid = "LANCOMBEIA"; // Enter your WiFi name
const char* password = "beialancom"; // Enter WiFi password

//MQTT broker
const char* mqtt_server = "mqtt.beia-telemetrie.ro";
const char* topic = "training/esp8266/NiculaeMaria"; 
WiFiClient espClient;
PubSubClient client(espClient);
StaticJsonDocument<512> doc;


unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
String msgStr = "";
char mess[512];
int counter;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.print(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("outTopic", "Reconnected!");
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds
      delay(5000);
    }
  }
}

void setup() {
  // Set software serial baud to 115200;
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
void mqtt_pub_sub(float i , char* k)
{
  doc["sensor"] = k;
  doc["value"] = i;

  serializeJson(doc, mess);
  client.publish(topic, mess);
  client.subscribe(topic);
  msgStr = "";
  delay(50);

}
void sensor(){
  float h = dht.readHumidity(); //humidity
  float t = dht.readTemperature(); //temperature
  //check if any reads failed and exit to try again
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.println(F("------------------------------------"));
  
  // Print humidity sensor details.
  Serial.print(F(" Humidity: "));
  Serial.print(h);
 
  // Print temperature sensor details.
  Serial.print(F("%  Temperature: "));
  Serial.print(t);

  doc["sensor"] ="Temperatura";
  doc["TC"] = t;
  doc["sensor"] = "Humidity";
  doc["HU"] = h;

  serializeJson(doc, mess);
  client.publish(topic, mess);
  client.subscribe(topic);
  msgStr = "";
  delay(50);
  Serial.println();
  delay(500);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  sensor();
  delay(50);
 
}
