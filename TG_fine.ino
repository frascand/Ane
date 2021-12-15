#include <ESP8266WiFi.h>
#include "DHT.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Wire.h>
#include <BMx280I2C.h>



#define WIFI_SSID "Wi-Fi House 2,4G"                    //inserire SSID WIFI
#define WIFI_PASSWORD "zz"                //inserire PASSWORD WIFI
 

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "2zz"                   //Inserire bot Telegram
#define chat_ID  "zz"                                                                 //INSERIRE ID CHAT


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

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

BMx280I2C bmx280(0x76);
 
//Variabili
float temperatura = 0.0;
float umidita = 0.0;
float pressure = 0.0;

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




void setup() {
  
   dht.begin();
 Wire.begin();
 bmx280.begin();
  Serial.begin(115200);

 delay(5000);
    Serial.println();
  
  configTime(0, 0, "pool.ntp.org");                 // get UTC time via NTP
  secured_client.setTrustAnchors(&cert);            // Add root certificate for api.telegram.org
  
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
 
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  
 bmx280.resetToDefaults();
 bmx280.writeOversamplingPressure(BMx280I2C::OSRS_P_x16);
 pinMode(WIND_SPD_PIN, INPUT);
 attachInterrupt(digitalPinToInterrupt(WIND_SPD_PIN), windTick, FALLING);
}

void loop() {
  getDati();
  Pressione();
  vento=0;
  VisualizzaSeriale();
 
  Serial.println("Vado a dormire per 10 minuti circa");
  //ESP.deepSleep(6e8);
  {
  }
}



void getDati(){
  temperatura = dht.readTemperature();
  umidita = dht.readHumidity();
  vento = Vento()*1.61;
pressure = bmx280.getPressure()/100;
}
void VisualizzaSeriale(){
  Serial.print("Temperatura: ");        Serial.print(temperatura);     Serial.println(" Â°C");
  Serial.print("Umidita': ");           Serial.print(umidita);         Serial.println(" %");
  Serial.print("Velocita' vento: ");    Serial.print(vento);           Serial.println(" km/h"); 
  Serial.print("Pressione Atmosferica "); Serial.print(bmx280.getPressure()/100); Serial.println(" mPa");
  String data =   "Temperatura: "    +String(temperatura)+   "Â°C \n";
         data +=  "UmiditÃ  : "       +String(umidita)+       " % \n";
         data +=  "VelocitÃ  vento: " +String(vento)+         " km/h \n";
         data +=  "La pressione è: " +String(pressure) +     " mPa \n";
         
  bot.sendMessage(chat_ID, data, "");
}

void Pressione(){
 bmx280.measure();
 do {
 delay(100);
}while(!bmx280.hasValue());

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
