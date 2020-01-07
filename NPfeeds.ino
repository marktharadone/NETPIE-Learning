#include "MicroGear.h"
#include "WiFi.h"

const char* ssid     = "RRT";
const char* password = "99887766";

#define APPID   "NetpieLab"
#define KEY     "fUSEzACtZV2yVIP"
#define SECRET  "V6pYHfkHnJA17LHVUchGYYTEU"
#define ALIAS   "npapp"

#define FEEDS_ID  "npfeeds"
#define FEEDS_KEY "nuzPM42ZABuoITXiVdNXEq6YG1JgrpzU"

#define led 17
#define LDR 36

WiFiClient client;
int timer = 0;
MicroGear microgear(client);

int readLight() 
  {
  int light = 0;
  light = map(analogRead(LDR), 0, 1023, 100, 0);
  return light;
  }

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{
  Serial.print("Topic --> ");
  Serial.println((char *)topic); 
  
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);

  char strMsg[msglen];
  for (int i = 0; i < msglen; i++) 
  {
    strMsg[i] = (char)msg[i];
  }

  String msgStr = String(strMsg).substring(0, msglen);
  String strTopic = (char *)topic;

  Serial.print("Topic: ");
  Serial.println(strTopic);
  Serial.print("Msg: ");
  Serial.println(msgStr);

    if (msgStr == "ON") 
    {
      Serial.println("ON");
      digitalWrite(led, 0);
    }
    
    if (msgStr == "OFF") 
    {
      Serial.println("OFF");   
      digitalWrite(led, 1);
    }
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) 
{
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) 
{
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}


void setup() 
{
  microgear.on(MESSAGE, onMsghandler);
  microgear.on(PRESENT, onFoundgear);
  microgear.on(ABSENT, onLostgear);
  microgear.on(CONNECTED, onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");

  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

  if (WiFi.begin(ssid, password)) 
  {
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  microgear.init(KEY, SECRET, ALIAS);

  microgear.connect(APPID);
  microgear.subscribe("/gearname/npapp/data/$/command");
}

void loop() 
{
  if (microgear.connected()) 
  {
    microgear.loop();

    if (timer >= 5000) 
    {
      Serial.println("Publish...");

      microgear.chat(ALIAS, "Hello");
      microgear.publish("/gearname/npapp/light", analogRead(36));
      String dataFeeds = "light:" + String(readLight());
      microgear.writeFeed(FEEDS_ID, dataFeeds, FEEDS_KEY);
      timer = 0;
    } 
    else timer += 100;
  }
  else 
  {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) 
    {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }
  delay(10);
}
