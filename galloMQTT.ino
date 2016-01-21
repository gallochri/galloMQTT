#include <ESP8266WiFi.h>  //For ESP8266
#include <PubSubClient.h> //For MQTT
#include <ESP8266mDNS.h>  //For OTA
#include <WiFiUdp.h>      //For OTA

//WIFI configuration
#define wifi_ssid         "XXXXXXXX"
#define wifi_password     "XXXXXXXX"

//MQTT configuration
#define mqtt_server       "XXXXXXXX"
#define mqtt_user         "XXXXXXXX"
#define mqtt_password     "XXXXXXXX"
String mqtt_client_id =   "ESP8266-";   //This text is concatenated with ChipId to get unique client_id

//MQTT Topic configuration
String mqtt_base_topic="/sensor/"+mqtt_client_id+"/data";
#define lux_topic "/lux"

//MQTT client
WiFiClient espClient;
PubSubClient mqtt_client(espClient);

int sensorPin = A0;

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.print(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("OK");
  Serial.print("   IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() { 
  Serial.begin(115200);
  Serial.println("\r\nBooting...");
  
  setup_wifi();

  Serial.println("Configuring MQTT server...");
  mqtt_client_id=mqtt_client_id+ESP.getChipId();
  mqtt_base_topic="/sensor/"+mqtt_client_id+"/data";
  mqtt_client.setServer(mqtt_server, 1883);
  Serial.printf("   Server IP: %s\r\n",mqtt_server);  
  Serial.printf("   Username:  %s\r\n",mqtt_user);
  Serial.println("   Cliend Id: "+mqtt_client_id);  
  Serial.println("   MQTT configured!");

  Serial.println("Setup completed! Running app...");
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {    
    if (mqtt_client.connect(mqtt_client_id.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
   
  if (!mqtt_client.connected()) {
    mqtt_reconnect();
  }
  mqtt_client.loop();
  int sensorValue = analogRead(sensorPin);
  delay (5000);
  int sensorValueNow = analogRead(sensorPin);
  if (sensorValueNow != sensorValue) {
    Serial.print("Lux value changed from " +String(sensorValue)+ " to "  );
  Serial.print(String(sensorValueNow).c_str());
  Serial.println(". Sent to "+mqtt_base_topic+lux_topic);
  mqtt_client.publish((mqtt_base_topic+lux_topic).c_str(), String(sensorValue).c_str(), true);
}
  }
  
