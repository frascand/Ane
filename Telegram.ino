#include<ESP8266WiFi.h>
#include "DHT.h"
#include <ThingSpeak.h>
#include "CTBot.h"



 char ssid[] = "Wi-Fi House 2,4G";
 char pass[] = "DC8987SC01071105";
 int status = WL_IDLE_STATUS;
 WiFiClient client;

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


//TELEGRAM
#define BOT_TOKEN "2056781967:AAFHDrCkS_3gzPqYPKTM3aM9NGeDFjnjK2Q"
 int64_t CHAT_ID = 831226200;
 CTBot myBot;
 
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

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid,pass);
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
  }
  Serial.println("WiFi connesso");
  Serial.println("indirizzo IP: ");
  Serial.println(WiFi.localIP());
  while (!Serial) continue;
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken (BOT_TOKEN);
  ThingSpeak.begin(client);
  pinMode(WIND_SPD_PIN, INPUT);     
  attachInterrupt(digitalPinToInterrupt(WIND_SPD_PIN), windTick, FALLING);
  if (myBot.testConnection())
Serial.println("\ntestConnection OK");
else
Serial.println("\ntestConnection NOK");

}

void loop() {
  // put your main code here, to run repeatedly:
  getDati();
  VisualizzaSeriale();
  InviaDati();
  vento=0;
  Serial.println("Vado a dormire per 10 minuti circa");
  ESP.deepSleep(6e8);
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
  Serial.println();  
}

void InviaDati(){
 Serial.println("Invio dati a Telegram");
String data = "Ciao, la Temperatura è di " + String(temperatura) + " Pa e l'Umidità è del " + String(umidita) + "%";
 myBot.sendMessage(CHAT_ID, data);
  Serial.println("Invio dati a Thingspeak");
  ThingSpeak.setField(1, (float)temperatura);
  ThingSpeak.setField(2, (float)umidita);
  ThingSpeak.setField(3, (float)vento);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  delay(updateThingSpeakInterval);
  
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
     
