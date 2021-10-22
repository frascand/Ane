#include <ESP8266WiFi.h>
#define sensore 5 // l'anenmometro sulla porta 5 - anemometer on pin 5

float momentoprecedente = 0; // momento precedente impulso - previus pulse istant
int gialetto = 0; // semaforo rilevazione - switch to separe different pulses
float tempo = 0; // tempo intercorrente tra un due impulsi validi - time between two
// valid pulses
float impulsimetrosec = 0.85; // impulsi/secondo per una velocita un metro al secondo
// pulses/second for a one meter/second wind speed
int velocita = 0; // velocita' del vento in metri al secondo – wind speed
int impulso = 0;
//
//
void setup()
{
 Serial.begin (9600);
  pinMode (sensore, INPUT);
 momentoprecedente = millis ();
  Serial.println ("wind speed");
}
//
//
void loop()
{
 impulso = digitalRead (sensore);
 // Serial.println (impulso);
 if ((impulso == 1) && (gialetto == 0))
 {
 // Serial.println ("pulse");
 gialetto = 1; // semaforo doppie letture - switch to avoid multiple reading
 tempo = millis () - momentoprecedente; // tempo dal precedente impulso – previus pulse istant
 momentoprecedente = millis (); // memorizza momento evento - store current time
 velocita = ((1000 / tempo) / impulsimetrosec); //calcolo della velocita' in metri al secondo
//compute wind speed

  Serial.println(velocita);
  Serial.println (" metri/sec ");
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
}
