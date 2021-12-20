const byte   interruptPin = 14; // Or other pins that support an interrupt
unsigned int Debounce_Timer, Current_Event_Time, Last_Event_Time, Event_Counter, RPS;
float        WindSpeed;
const float r = 2.733018;

void setup() {
  Serial.begin(115200);
        // Set the Font size
  pinMode(interruptPin, INPUT_PULLUP);
  noInterrupts();                                // Disable interrupts during set-up
  attachInterrupt(digitalPinToInterrupt(interruptPin), WSpeed_ISR, RISING); //Respond to a LOW on the interrupt pin and goto WSpeed_ISR
  timer0_isr_init();                             // Initialise Timer-0
  timer0_attachInterrupt(Timer_ISR);             // Goto the Timer_ISR function when an interrupt occurs
  timer0_write(ESP.getCycleCount() + 80000000L); // Pre-load Timer-0 with a time value of 1-second
  interrupts();                                  // Enable interrupts
}

void loop() {
 
Serial.print(WindSpeed); 
Serial.println(" Km/h");
delay(10000);
}

//#########################################################################################
// Interrupt service routine
ICACHE_RAM_ATTR void WSpeed_ISR (void) {
  if (!(millis() - Debounce_Timer) < 5) {
    Debounce_Timer = millis();                                        // Set debouncer to prevent false triggering
    Event_Counter++;
  }
}

void Timer_ISR (void) {                                                       // Timer reached zero, now re-load it to repeat
  timer0_write(ESP.getCycleCount() + 80000000L); // Reset the timer, do this first for timing accuracy
  WindSpeed = (Event_Counter*r);
  //Serial.println(Event_Counter);
  Event_Counter =0;
}
