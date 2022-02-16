#include <DHT.h>  // Including library for dht

#include <ESP8266WiFi.h>

String apiKey = "S1TL6WF664JV3ESM";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "MUNDAKKOTTIL";     // replace with your wifi ssid and wpa2 key
const char *pass =  "45813171";
const char* server = "api.thingspeak.com";

const int total_height = 30;
const int hold_height = 25;

const int trigger = D7;
const int echo = D8;
long Time;
int x;
int i;
int distanceCM;
int resultCM;
int wtr_lvl = 0;
int sensr_to_wtr = 0;

#define DHTPIN D2          //pin where the dht11 is connected

DHT dht(DHTPIN, DHT11);

WiFiClient client;

void setup()
{
  Serial.begin(115200);
  delay(10);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  dht.begin();
  

  Serial.println("Connecting to ");
  Serial.println(ssid);



  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}

void measure()
{
  delay(100);
  
  sensr_to_wtr = total_height - hold_height;
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  Time = pulseIn(echo, HIGH);
  distanceCM = Time * 0.034;
  resultCM = distanceCM / 2;

  wtr_lvl = map(resultCM, sensr_to_wtr, total_height, 100, 0);
}

void loop()
{

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  measure();


  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
  {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += int(wtr_lvl);

    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(h);


    Serial.print("River Water Level:");
    Serial.print(wtr_lvl);
    Serial.println("%");
    Serial.println("%. Send to Thingspeak.");

  }
  client.stop();

  Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
}
