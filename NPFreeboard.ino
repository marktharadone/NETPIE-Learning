#include "MicroGear.h"
#include "WiFi.h"

const char* ssid     = "XXXXXXXXX";
const char* password = "XXXXXXXXX";

#define APPID   "XXXXXXXXX"
#define KEY     "XXXXXXXXX"
#define SECRET  "XXXXXXXXX"
#define ALIAS   "npapp"

#define led 17

WiFiClient client;
int timer = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{ 
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
