#include <msp430.h>

#define IsTime(X) ((CurrentTime.sec == X.sec) && (CurrentTime.ms == X.ms))

#define GREEN_LED BIT0
#define YELLOW_RGB_LED BIT1 + BIT3 //P2
#define RED_LED BIT6
#define RED_RGB_LED BIT1
#define BUZZER BIT0

#define SWITCH BIT3
#define ClockPeriod 32 //Clock tick internal in 16384's of a second

volatile int buzzerPeriod = 2;
volatile int buzzerOneDutyCycle = 1;
volatile int toneDuration = 250;

struct Time {
    int sec;
    int ms;
};

struct Time CurrentTime = {0,0};
struct Time switchSchedule = {0,-1};
struct Time buzzerOneSchedule = {0, 1};
struct Time toneSwitch = {0,250};
struct Time buzzerOneDutyCycleSchedule = {0,-1};
struct Time buzzerTwoSchedule = {0, -1};

struct Time Schedule (int duration)
{
    struct Time newtime;
    newtime.sec = CurrentTime.sec;
    newtime.ms = CurrentTime.ms+duration; //add in the duration in ms
    while ((newtime.ms-=1024) >= 0) //subtract whole seconds until negative
        if (newtime.sec++ == 60) newtime.sec = 0; //adding to seconds each time
                                    //roll seconds over at a minute
    newtime.ms+=1024; //add back the extra 1024 that made us negative
    return newtime; //and return that time
}

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    P1DIR |= GREEN_LED + RED_LED; // Make the LED bit(s) output
    P1OUT = 0x00;
    P2DIR |= BUZZER;
    P2OUT = 0x00;
    P1OUT |= SWITCH; // Select pull up resistor on switch
    P1REN |= SWITCH; // and enable
    P1IES |= SWITCH; // Switch high to low edge
    P1IFG &= ~SWITCH; // Switch IFG cleared
    P1IE |= SWITCH; // Switch interrupt enabled
    TA1CCTL0 = CCIE; // CCR0 interrupt enabled
    TA1CCR0 = ClockPeriod; // Have time tick every ~ms
    TA1CTL = TASSEL_1 + MC_1; // ACLK, upmode
    
    __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ interrupt
}

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void)
{
    if (CurrentTime.ms++ == 1024){ // Update soft clock. Has second passed?
        CurrentTime.ms = 0; // If so, reset milliseconds and count a sec
        if (CurrentTime.sec++ == 60) CurrentTime.sec = 0; // If now a minute, reset sec to 0
    }
                                  // Check for scheduled events
    if (IsTime(switchSchedule)){ // Is it time for the switch debounce?
      if (!(P1IN & SWITCH)){ // If so, is it a valid press (still pressed)
        ;
      }
      switchSchedule.ms = -1; // Turn off the debounce event
    }
    if(IsTime(toneSwitch)){
      buzzerPeriod ^= 0x03;
      toneSwitch = Schedule(toneDuration);
    }
    if (IsTime(buzzerOneSchedule)){ // Is it time for the LED event?
        P2OUT |= BUZZER;                                // If so, toggle the LED
        buzzerOneSchedule = Schedule(buzzerPeriod);
        buzzerOneDutyCycleSchedule = Schedule(buzzerOneDutyCycle);
    }
    if(IsTime(buzzerOneDutyCycleSchedule)){
      P2OUT &= ~BUZZER;
      buzzerOneDutyCycleSchedule.ms = -1;
    }
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port1 (void)
{
    if(P1IES & SWITCH){
      switchSchedule = Schedule(20); // Schedule an end debounce in ~10ms
    }
    else{
      buzzerOneSchedule.ms = -1;
    }
    P1IES ^= SWITCH;
    P1IFG &= ~SWITCH; // Clear switch interrupt flag
}