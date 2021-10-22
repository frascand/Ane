#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include "DHT.h"

//WIFI
char ssid[] = "Wi-Fi House 2,4G"; //
char pass[] = "DC8987SC01071105";
int status = WL_IDLE_STATUS;
WiFiClient  client;

//THINKSPEAK
unsigned long myChannelNumber = 1512831 ;
const char * myWriteAPIKey = "FOO4L91V7ENPY32V";
const int updateThingSpeakInterval = 1* 1000;  
 
//Costanti
#define sensore 12 //collegato a D7 
#define DHTPIN 4   //Pin a cui è connesso il sensore
#define DHTTYPE DHT22   //Tipo di sensore che stiamo utilizzando (DHT22)
DHT dht(DHTPIN, DHTTYPE); 

//Variabili
  int chk;
float hum;  //Variabile in cui verrà inserita la % di umidità
float temp; //Variabile in cui verrà inserita la temperatura
float momentoprecedente = 0;  // momento precedente impulso - previus pulse istant
int gialetto            = 0;  // semaforo rilevazione - switch to separe different pulses
float tempo             = 0;  // tempo intercorrente tra un due impulsi validi - time between two valid pulses
float impulsimetrosec   = 0.85; // impulsi al secondo per una velocita' del vento di un metro al secondo
// pulses/second for a one meter/second wind speed 
int velocita            = 0;  // velocita' del vento in metri al secondo
int impulso             = 0;  


void setup() {
Serial.begin(9600);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }
  
  dht.begin();
  pinMode (sensore, INPUT);
  momentoprecedente = millis ();

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");
  Serial.println("Running Anemometro");
  Serial.println("-------------------------------------");
   int i;
  int connection_success = 0;
  
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  for (int i = 0; i <= 5; i++) {
    if (WiFi.status() != WL_CONNECTED) {
      delay(2000);
      Serial.print("Trial: ");
      Serial.print(i);
    } else {
      Serial.print("Connected to WiFi");
      connection_success = 1;
      break;
    }
  }

  if (connection_success == 0) {
    exit(0);
  }
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
}

void loop() {

delay(2000);  //Ritardo di 2 secondi. (E' possibile leggere dal sensore massimo una volta ogni 2 secondi)

  //Leggi i dati e salvali nelle variabili hum e temp

  hum = dht.readHumidity();
  temp= dht.readTemperature();
  {
  impulso = digitalRead (sensore);
  Serial.println (impulso);
  if ((impulso == 1) && (gialetto == 0))
  {
Serial.println ("pulse");
    gialetto = 1;                           // semaforo doppie letture - switch to avoid multiple reading
    tempo = millis () - momentoprecedente;  // tempo dal precedente evento
    momentoprecedente = millis ();          // memorizza momento evento - store current time
    velocita = ((1000 / tempo) / impulsimetrosec);  //calcolo della velocita' in metri al secondo - compute wind speed
       
  }
  if ((impulso == 0) && (gialetto == 1))  
  {
    gialetto = 2;
  }
  if ((impulso == 1) && (gialetto == 2))
  {
    gialetto = 3;
  }
  if ((impulso == 0) && (gialetto == 3))
  {
    gialetto = 0;
  }
  //Stampa umidità temperatura e vento tramite monitor seriale
  Serial.print("Umidità: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  Serial.print (velocita);
  Serial.println (" metri/sec  ");
  Serial.print("Sending data to ThingSpeak");
  Serial.println();

  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, hum);
  ThingSpeak.setField(3, velocita);
  
  // Write the fields that you've set all at once.
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  delay(1000);} 
}
