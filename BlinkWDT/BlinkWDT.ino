

#include <avr/sleep.h> //Needed for sleep_mode



const byte statLED = 0;
const byte button = 1;
//Variables


volatile int watchdog_counter;

//This runs each time the watch dog wakes us up from sleep
ISR(WDT_vect) {
  watchdog_counter++;
}

void setup()
{
 
  pinMode(statLED, OUTPUT);
  pinMode(button,INPUT_PULLUP);

  //pinMode(waterSensor, INPUT_PULLUP);
  


  digitalWrite(statLED, LOW);

  watchdog_counter = 0;

  //Power down various bits of hardware to lower power usage  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Power down everything, wake up from WDT
  sleep_enable();
  ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA
  
  setup_watchdog(0); //Wake up after 1 sec
}

void loop() 
{
  sleep_mode(); //Go to sleep!

  if(watchdog_counter > 1)
  {
    watchdog_counter = 0;

    if(digitalRead(button) == 0){
       for (int fadeValue = 0; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
      analogWrite(statLED, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(statLED, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

    }

    
    
    
  }
  else{
    digitalWrite(statLED,LOW);
  }
}


// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
// 6=1sec, 7=2sec, 8=4sec, 9=8sec
// From: http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void setup_watchdog(int timerPrescaler) {

  if (timerPrescaler > 9 ) timerPrescaler = 9; //Correct incoming amount if need be

  byte bb = timerPrescaler & 7; 
  if (timerPrescaler > 7) bb |= (1<<5); //Set the special 5th bit if necessary

  //This order of commands is important and cannot be combined
  MCUSR &= ~(1<<WDRF); //Clear the watch dog reset
  WDTCR |= (1<<WDCE) | (1<<WDE); //Set WD_change enable, set WD enable
  WDTCR = bb; //Set new watchdog timeout value
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int
}
