//gcc -o encoder encoder.c -lpthread -lwiringPi -lwiringPiDev
#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>

#define   PIN_A   8
#define   PIN_B   9
#define BUTTON_PIN 7

#define   DEBOUNCE   5

static volatile int counter ;
// -------------------------------------------------------------------------
// the event counter
volatile int eventCounter = 0;

void myInterrupt(void) {
  eventCounter++;
}

// -------------------------------------------------------------------------
void encoder (void)
{
  static unsigned int debounce = 0 ;

// If PIN_A is high then ignore it
//   This may be due to bouncing, or a bug/feature of the edge detect

  if (digitalRead (PIN_A) == HIGH) return ;

// Ignore multiple interrupts inside our debounce time

  if (millis () < debounce) return ;
  if (digitalRead (PIN_B) == LOW)   // Anti-clockwise
    --counter ;
  else
    ++counter ;
  debounce = millis () + DEBOUNCE ;
}

int main ()
{
  int last ;
  last = counter = 0 ;
  wiringPiSetup () ;
    pinMode(PIN_A, INPUT);
    pinMode(PIN_B, INPUT);
    pullUpDnControl(PIN_A, PUD_UP);
    pullUpDnControl(PIN_B, PUD_UP);
// -------------------------------------------------------------------------
  pinMode(BUTTON_PIN, INPUT);
  pullUpDnControl (BUTTON_PIN, PUD_UP) ;
  wiringPiISR (BUTTON_PIN, INT_EDGE_FALLING, myInterrupt) ;
 // -------------------------------------------------------------------------

  wiringPiISR (PIN_A, INT_EDGE_FALLING, encoder) ;

  printf ("\nRunning... \n") ;

  for (;;)
  {
// -------------------------------------------------------------------------
    if (eventCounter != 0) system("curl http://192.168.0.3:9000/status.html?p0=pause");
    eventCounter = 0;
// -------------------------------------------------------------------------
    if (counter != last)
    {
    if (counter > last) system("curl  'http://192.168.0.3:9000/status.html?p0=mixer&p1=volume&p2=%2B5'");
    if (counter < last) system("curl  'http://192.168.0.3:9000/status.html?p0=mixer&p1=volume&p2=-5'");
    last = counter ;
    }
// -------------------------------------------------------------------------
    delay( 100 ); // wait 0.1 second
// -------------------------------------------------------------------------
    
  }

  return 0 ;
}