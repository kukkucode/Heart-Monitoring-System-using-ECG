///// ************ myecg.ino ***************/////
//*********************************************//

void setup()
{
// initialize the serial communication:
Serial.begin(9600);
pinMode(14, INPUT); // Setup for leads off detection LO +
pinMode(12, INPUT); // Setup for leads off detection LO -
}

void loop() {
if((digitalRead(10) == 1)||(digitalRead(11) == 1)){
Serial.println('!');
}
else{
// send the value of analog input 0:
Serial.println(analogRead(A0));
}
//Wait for a bit to keep serial data from saturating
delay(1);
}


///// ********* Heart_Monitoring_System.ino ***********/////
//*******************************************************//

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define WIFISSID "Pooh" // WifiSSID
#define PASSWORD "hulkbuster" // wifi password
#define TOKEN "BBFF-8GERINm7dMjRnq1n2yvoELeRIwGL1R" // Ubidots' TOKEN
#define MQTT_CLIENT_NAME "My ECG" // MQTT client Name 
 
///// Define Constants
#define VARIABLE_LABEL "My ECG" // Assign the variable label
#define DEVICE_LABEL "204161012" // Assign the device label
#define SENSOR A0 // Set the A0 as SENSOR
char mqttBroker[] = "industrial.api.ubidots.com";
char payload[100];
char topic[150];
// Space to store values to send
char str_sensor[10];
///// Auxiliar Functions
WiFiClient ubidots;
PubSubClient client(ubidots);
void callback(char* topic, byte* payload, unsigned int length) {
 char p[length + 1];
 memcpy(p, payload, length);
 p[length] = NULL;
 Serial.write(payload, length);
 Serial.println(topic);
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.println("Attempting MQTT connection...");
 
 // Attemp to connect
 if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
 Serial.println("Connected");
 } else {
 Serial.print("Failed, rc=");
 Serial.print(client.state());
 Serial.println(" try again in 2 seconds");
 // Wait 2 seconds before retrying
 delay(2000);
 }
 }
} 


///// *********** Main Functions **********//////
//*********************************************//

void setup() {
 Serial.begin(115200);
 WiFi.begin(WIFISSID, PASSWORD);
 // Assign the pin as INPUT 
 pinMode(SENSOR, INPUT);
 Serial.println();
 Serial.print("Waiting for WiFi...");
 
 while (WiFi.status() != WL_CONNECTED) {
 Serial.print(".");
 delay(500);
 }
 Serial.println("");
 Serial.println("WiFi Connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
 client.setServer(mqttBroker, 1883);
 client.setCallback(callback); 
}

void loop() {
 if (!client.connected()) {
 reconnect();
 } 
 sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
 sprintf(payload, "%s", ""); // Cleans the payload
 sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adds the variable label
 float myecg = analogRead(SENSOR); 
 
 /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
 dtostrf(myecg, 4, 2, str_sensor);
 
 sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor); // Adds the value
 Serial.println("Data uploaded to Ubidots Cloud");
 client.publish(topic, payload);
 client.loop();
 delay(10);
}

/////********* END **********/////
//*****************************//