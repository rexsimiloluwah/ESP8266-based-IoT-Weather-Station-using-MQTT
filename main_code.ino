#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SFE_BMP180.h>
#include <Wire.h>

SFE_BMP180 pressure;

#define ALTITUDE 244
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11

const char* ssid = "TECNO SPARK 2";
const char* password = "acidicman";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.println("Command is : please print D in your phone for ");
  Serial.print(topic);
 char p=(char)payload[0];
 int chk = dht.read(DHTPIN);
  // if MQTT comes a 0 message, show humidity
  if(p=='D' ) 
  {
    Serial.println("to show humidity!]");
    Serial.print(" Humidity is: " );
    Serial.print(dht.readHumidity());
    Serial.println('%');
    Serial.println(" is to show temperature!] ");
    Serial.print(" Temp is: " );
    Serial.print(dht.readTemperature());
    Serial.println(' C');
  } 

} //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect("1ac9e73b-b30b-46ba-acf1-85fd8d5d2834","eee310group5","adetoyosi"))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("dht11");
      client.subscribe("dht11Humidity");
      client.subscribe("Pressure");
      client.subscribe("Altitude");
      client.subscribe("SeaLevelAltitude");
      client.subscribe("HeatIndex");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  int chk = dht.read(DHTPIN);
  Serial.println("DHT11 Test");
  dht.begin();
   if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
 }

void loop(){
  delay(2000);

  
  
   char status;
  double T,P,p0,a;
  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);


        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");
          
          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
  

   if (!client.connected()) {
    reconnect();
  }
  client.loop();

   client.loop();
  long now = millis();
  // read DHT11 sensor every 6 seconds
  if (now - lastMsg > 10000) {
     lastMsg = now;
     int chk = dht.read(DHTPIN);
     if(!(isnan(dht.readTemperature())|| isnan(dht.readHumidity()) ||isnan(dht.computeHeatIndex(dht.readTemperature(),dht.readHumidity(),false)))){
     String msg1="Temperature :- ";
     msg1= msg1 +dht.readTemperature();
     msg1 = msg1+ "◦C";
    
     
     String msg2 = "Humidity :-" ;
     msg2 =msg2+ (dht.readHumidity() - 18);
     msg2 = msg2 +"%";
     
     
     String msg3 = "Pressure :-";
     msg3 = msg3+ P;
     msg3 = msg3 + "Mb";
     
     
    String  msg4 = "Altitude :-";
     msg4 = msg4 + a;
     msg4 = msg4 + "Meters";
     
     
     String msg5 = "Sea-level Pressure:-";
     msg5 = msg5 + p0;
     msg5 = msg5 + "Mb";

     String msg6 = "Heat Index :-";
     msg6 = msg6 + dht.computeHeatIndex(dht.readTemperature(),dht.readHumidity(),false);
     msg6 = msg6 + "◦C";
     
     
     char message1[30];
     char message2[30];
     char message3[30];
     char message4[30];
     char message5[30];
     char message6[30];
     
     msg1.toCharArray(message1,30);
     msg2.toCharArray(message2, 30);
     msg3.toCharArray(message3, 30);
    msg4.toCharArray(message4, 30);
    msg5.toCharArray(message5, 30);
    msg6.toCharArray(message6, 30);
     Serial.println(message1);
     Serial.println(message2);
     Serial.println(message3);
     Serial.println(message4);
     Serial.println(message5);
     Serial.println(message6);
     //publish sensor data to MQTT broker
    client.publish("dht11",message1);
    client.publish("dht11Humidity",message2);
    client.publish("Pressure",message3);
    client.publish("Altitude",message4);
    client.publish("SeaLevelAltitude",message5);
    client.publish("HeatIndex", message6);
  }
  }
 

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) ||isnan(t) || isnan(f) )
  {
      Serial.println("Failed to Read from the DHT Sensor");
      return;
      
  }

  Serial.print("Humidity :");
  Serial.println(h);

  Serial.print("Temperature  :");
  Serial.print(t);
  Serial.println("oC");

  Serial.print("Temperature( in Fahenreits ):");
  Serial.print(h);
  Serial.println("oF");

 

  delay(5000);  // Pause for 5 seconds.
  
  
  
  
  
  
  
  }
