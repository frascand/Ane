*/
#define sensore 5 // l'anenmometro sulla porta 5 - anemometer on pin 5 #include <LiquidCrystal_I2C.h> // libreria di gestione del display lcd
// addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // tipo dipslay // LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
// ^ indirizzo alternativo, da attivare al posto della precedente istruzione se lcd non funziona // ^^ Alternative address, to activate in place of previous instruction if LCD doesn't work
//
float momentoprecedente = 0; // momento precedente impulso - previus pulse istant
= 0; // semaforo rilevazione - switch to separe different pulses
= 0; // tempo intercorrente tra un due impulsi validi - time between two
int gialetto
float tempo
// valid pulses
float impulsimetrosec
// pulses/second for a one meter/second wind speed
int velocita int impulso //
//
void setup()
{
= 0.85; // impulsi/secondo per una velocita un metro al secondo = 0; // velocita' del vento in metri al secondo – wind speed
= 0;
Serial.begin (9600);
lcd.begin(16, 2);
pinMode (sensore, INPUT); lcd.backlight();
lcd.print ("buongiorno"); // good day momentoprecedente = millis (); lcd.clear();
  lcd.print ("wind speed");
}
//
//
void loop() {
  impulso = digitalRead (sensore);
  //  Serial.println (impulso);
if ((impulso == 1) && (gialetto == 0)) {
// Serial.println ("pulse");
gialetto = 1; // semaforo doppie letture - switch to avoid multiple reading tempo = millis () - momentoprecedente; // tempo dal precedente impulso – previus pulse istant momentoprecedente = millis (); // memorizza momento evento - store current time velocita = ((1000 / tempo) / impulsimetrosec); //calcolo della velocita' in metri al secondo
//compute wind speed
    lcd.setCursor (0, 1);
    lcd.print (velocita);
    lcd.print (" metri/sec  ");
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
