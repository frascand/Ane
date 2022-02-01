#include <ESP8266WiFi.h>
#include "DHT.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Wire.h>
#include <BMx280I2C.h>



#define WIFI_SSID "Wi-Fi House 2,4G_EXT"                                     //inserire SSID WIFI
#define WIFI_PASSWORD ""                                               //inserire PASSWORD WIFI
 

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN ""                                                                       //Inserire bot Telegram
#define chat_ID  "" //INSERIRE ID CHAT
#define chat_ID2 ""

const unsigned long BOT_MTBS = 1000; // mean time between scan messages
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

long momentoprima = 0;
#define intervallo 900000

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
BMx280I2C bmx280(0x76);

const byte   interruptPin = 14; // Or other pins that support an interrupt
unsigned int Debounce_Timer, Current_Event_Time, Last_Event_Time, Event_Counter;
float        vento = 0.0; 
float        rugiada;
const double r = 2.733018;

 
//Variabili
float temperatura = 0.0;
float umidita = 0.0;
float pressure = 0.0;

//Prototipi funzioni
void getDati();
void VisualizzaSeriale();
float Pressione();
float Puntor();



void setup() {
  Serial.begin(115200);
 pinMode(interruptPin, INPUT_PULLUP);
 dht.begin();
 Wire.begin();
 bmx280.begin();
 bmx280.resetToDefaults();

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
  
 bmx280.writeOversamplingPressure(BMx280I2C::OSRS_P_x16);
 bmx280.writeOversamplingTemperature(BMx280I2C::OSRS_T_x16);
 

  noInterrupts();                                // Disable interrupts during set-up
  attachInterrupt(digitalPinToInterrupt(interruptPin), WSpeed_ISR, FALLING); //Respond to a LOW on the interrupt pin and goto WSpeed_ISR
  timer0_isr_init();                             // Initialise Timer-0
  timer0_attachInterrupt(Timer_ISR);             // Goto the Timer_ISR function when an interrupt occurs
  timer0_write(ESP.getCycleCount() + 80000000L); // Pre-load Timer-0 with a time value of 1-second
  interrupts();        
}

void loop() {
  Timer_ISR();
  VisualizzaSeriale();
 /*/ if (millis() - momentoprima > intervallo ) {
    momentoprima = millis();
      Pressione();
      getDati();
      Puntor();
      VisualizzaSeriale();
       
  delay(5000);
 ESP.deepSleep(9e8);
  {
  }/*/
  delay(5000);
}



void getDati(){
  temperatura = bmx280.getTemperature();
  umidita = dht.readHumidity();
  pressure = bmx280.getPressure()/100;
    }
void VisualizzaSeriale(){
  Serial.println("I dati esterni sono :");
  Serial.print("Temperatura: ");        Serial.print(temperatura);     Serial.println(" Ã‚Â°C");
  Serial.print("Umidita': ");           Serial.print(umidita);         Serial.println(" %");
  Serial.print("Velocita' vento: ");    Serial.print(vento);           Serial.println(" km/h"); 
  Serial.print("Pressione Atmosferica "); Serial.print(pressure); Serial.println(" hPa");
  Serial.print("Punto di rugiada: "); Serial.print(rugiada); Serial.println(" C");
  String data = "<b>ESTERNO</b>:  \n"; 
         data +=  "Temperatura: "    +String(temperatura)+   " C \n";
         data +=  "Umidità: "       +String(umidita)+       " % \n";
         data +=  "Vento: " +String(vento)+         " km/h \n";
         data +=  "Pressione: " +String(pressure) +     " hPa \n";
         data += "Punto di rugiada: " +String(rugiada)+    " C \n";
         
  bot.sendMessage(chat_ID, data, "HTML");
  bot.sendMessage(chat_ID2, data, "HTML");
}

float Pressione(){
 bmx280.measure();
 do {
 delay(100);
 }  while(!bmx280.hasValue());

 }

float Puntor() {
  double pow(double x, double y), log(double x), Es, E, Dp;
  Es = 6.11 * pow(10, (7.5 * temperatura)/(237.7 + temperatura));
  E = ( umidita * Es ) / 100;
  Dp = (-430.22 + 237.7 * log(E))/(-log(E) + 19.08);
  rugiada = Dp;

}

 ICACHE_RAM_ATTR void WSpeed_ISR (void) {
  if (!(millis() - Debounce_Timer) < 5) {
    Debounce_Timer = millis();                                        // Set debouncer to prevent false triggering
    Event_Counter++;
  }
}

void Timer_ISR (void) {                                                       // Timer reached zero, now re-load it to repeat
  timer0_write(ESP.getCycleCount() + 80000000L);    // Reset the timer, do this first for timing accuracy
    vento = (Event_Counter*r);
  //Serial.println(vento);
  Event_Counter =0;
}
