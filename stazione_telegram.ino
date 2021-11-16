#include<ESP8266WiFi.h>
#include "DHT.h"
#include <ThingSpeak.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>


 / Wifi network station credentials
#define WIFI_SSID "Wi-Fi House 2,4G"                    //inserire SSID WIFI
#define WIFI_PASSWORD "DC8987SC01071105"                //inserire PASSWORD WIFI

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "2131179233:AAEe0IXFghY-Qxx8Lu2lTVFDer4GmddM-pE"                   //Inserire bot Telegram


const unsigned long BOT_MTBS = 1000; // mean time between scan messages
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

//ANEMOMETRO
#define WIND_SPD_PIN 14
volatile unsigned long timeSinceLastTick = 0;
volatile unsigned long lastTick = 0;    
long secsClock = 0;
float vento=0;


 //THINGSPEAK
 unsigned long myChannelNumber = 1512831 ;
 const char * myWriteAPIKey = "FOO4L91V7ENPY32V";
 const int updateThingSpeakInterval = 1 * 1000;

 #define DHTPIN 4
 #define DHTTYPE DHT22
 DHT dht(DHTPIN, DHTTYPE);

 
//Variabili
float temperatura = 0.0;
float umidita = 0.0;


//Prototipi funzioni
void getDati();
void VisualizzaSeriale();
void InviaDati();
float Vento();
void windTick(void)
    {
      timeSinceLastTick = millis() - lastTick;
      lastTick = millis();
    }
//pioggia
#define pinIngresso 5
int pioggia=0;
int misurazione=0;

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";


    if (text == "/status")
    {
      digitalWrite(temperatura); 
 }
    if (text == "/start")
    {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      
     welcome += "/status : Returns current status of LED\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
 
}
}

void setup() {
  pinMode(pinIngresso, INPUT);
   dht.begin();
  Serial.begin(115200);
 
 delay(5000);
  Serial.begin(115200);
  Serial.println();

   // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);

  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}
  ThingSpeak.begin(client);
  pinMode(WIND_SPD_PIN, INPUT);     
  attachInterrupt(digitalPinToInterrupt(WIND_SPD_PIN), windTick, FALLING);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  getDati();
  VisualizzaSeriale();
  LeggiPioggia();
  InviaDati();
  vento=0;
  Serial.println("Vado a dormire per 10 minuti circa");
  //ESP.deepSleep(6e8);
  {
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
}

void getDati(){
  temperatura = dht.readTemperature();
  umidita = dht.readHumidity();
  vento=Vento()*1.61;
}

void VisualizzaSeriale(){
  Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" °C");
  Serial.print("Umidita': "); Serial.print(umidita); Serial.println(" %");
  Serial.print("Velocita' vento: "); Serial.print(vento); Serial.println(" km/h"); 
}

void InviaDati(){
  Serial.println("Invio dati a Thingspeak");
  ThingSpeak.setField(1, (float)temperatura);
  ThingSpeak.setField(2, (float)umidita);
  ThingSpeak.setField(3, (float)vento);
  ThingSpeak.setField(4, (float)pioggia);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  delay(updateThingSpeakInterval);
}
void LeggiPioggia(){
 pioggia = digitalRead(pinIngresso);
  
  if(pioggia==1) 
    Serial.println("non piove");
  if(pioggia==0)
    Serial.println("piove");
  }
float Vento(){
      static unsigned long outLoopTimer = 0;
      static unsigned long wundergroundUpdateTimer = 0;
      static unsigned long clockTimer = 0;
      static unsigned long tempMSClock = 0;
      float windSpeed=0;
      tempMSClock += millis() - clockTimer;
      clockTimer = millis();
      while (tempMSClock >= 1000)
      {
        secsClock++;
        tempMSClock -= 1000;
      }
      
      // This is a once-per-second timer that calculates and prints off various
      //  values from the sensors attached to the system.
      if (millis() - outLoopTimer >= 2000)
      {
        outLoopTimer = millis();
        // Windspeed calculation, in mph. timeSinceLastTick gets updated by an
        //  interrupt when ticks come in from the wind speed sensor.
        if (timeSinceLastTick != 0) windSpeed = 1000.0/timeSinceLastTick;
      }
     return(windSpeed);
  }
      
