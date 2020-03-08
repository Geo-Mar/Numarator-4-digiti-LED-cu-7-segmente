// base info from GeoMar - http://hobby-geomar.blogspot.com/
// ver.0 -  common cathode or common anode
// 01.03.2020 - Mereni TR.
// --------------------------------------------------------------------------
#include <SimpleTimer.h>  // http://www.arduino.cc/playground/Code/SimpleTimer.html


byte digits[21] = {  //Bit pattern for 7 segment display
  // -ABCDEFG            Segments labelled as per datasheet for Kingbright DA56-11EW dual common anode LED display module.
  B10000001,  // 0     1 represents segment OFF, 0 is ON
  B11001111,  // 1     the MSB is not used.
  B10010010,  // 2
  B10000110,  // 3
  B11001100,  // 4
  B10100100,  // 5
  B10100000,  // 6
  B10001111,  // 7
  B10000000,  // 8
  B10000100,  // 9
  B10011100,  // Degree symbol
  B10110001,  // Letter C
  B11111110,  // - Symbol
  B10011111,  //  Letter E
  B11100011,   // u
  B11111111,   // Blank (OFF)
  B11110000,   // t
  B11101010,   // n
  B11000010,   // d
  B11100010,   // a
  B11111110,   // -
};

int anodes[4] = {12, 11, 10, 9}; //Common anodes for 7 segment displays connected to these digital outputs via NPN transistors
//Pin 11 is Left digit, 6 is Right digit.
int cathodes[8] = {8, 7, 6, 5, 4, 3, 2, A5}; //Cathodes for each segment all tied together and connected to these digital outputs.
//Segment to pin assignment
//A=2,B=3,C=4,D=5,E=12,F=7,G=8,DP=13 segments
int data[4];  //4 byte array stores value to be displayed.

unsigned long starttime;
unsigned long endtime;

boolean bitState;
byte p = 2; // brigthness

int maxim = 3000; // maxim brigghtness
int DISPLAY_BRIGHTNESS = 2000;
//Display brightness
//Each digit is on for a certain amount of microseconds
//Then it is off until we have reached a total of 20ms for the function call
//Let's assume each digit is on for 1000us
//Each digit is on for 1ms, there are 4 digits, so the display is off for 16ms.
//That's a ratio of 1ms to 16ms or 6.25% on time (PWM).
//Let's define a variable called brightness that varies from:
//5000 blindingly bright (15.7mA current draw per digit)
//2000 shockingly bright (11.4mA current draw per digit)
//1000 pretty bright (5.9mA)
//500 normal (3mA)
//200 dim but readable (1.4mA)
//50 dim but readable (0.56mA)
//5 dim but readable (0.31mA)
//1 dim but readable in dark (0.28mA)

#define SW0 A2   // pin for MENU/change
#define SW1 A3   // pin for increase value (+)

byte meniu = 0;   // 0 = usual state (clack, thetmometer & higrometer)
// 1 - read clock and data
// 2 - adjust hour
// 3 - adjust minutes
// 4 - adjus year
// 5 - adjust month
// 6 - adjust day of month
// 7 - store data

int numar = 0;
int luminamax = 3000;  // for control the brightness (maxim is 3000 for not see the flickering effect)
int lumina = 0;

#define senzorlumina A4 //  pin for photoresitor: VCC (+5VDC) ---|= 10k =|------ Analog pin (A4) ------|= LDR =|------| (GND)

byte tip = 1; // 0 = common cathode
// 1 = common anode

//--------------------------------------------------------------------------------------------------------

void setup()
{
  for (int i = 0; i < 4; i++)  //Set anode pins mode to output and put them LOW.
  {
    pinMode(anodes[i], OUTPUT);
    if (tip == 1) digitalWrite(anodes[i], LOW);
    else  digitalWrite(anodes[i], HIGH);
  }

  for (int i = 0; i < 8; i++)  //Set cathode pins mode to output and put them HIGH.
  {
    pinMode(cathodes[i], OUTPUT);
    if (tip == 1) digitalWrite(cathodes[i], HIGH);
    else digitalWrite(cathodes[i], LOW);
  }

  pinMode(SW0, INPUT);  // for this use a slide switch
  pinMode(SW1, INPUT);  // N.O. push button switch
  digitalWrite(SW0, HIGH); // pull-ups on
  digitalWrite(SW1, HIGH);

  lumina = 3 * analogRead(senzorlumina); // for control the brigntness

  numar = 3333;
}
// --------------------------------------------------------------------------------------------

void loop()
{numarare();
  if (meniu == 0)  // normal state
  {
    numar = 4444;
    lumina = 3 * analogRead(senzorlumina); // for control the brigntness
    scrollIn(data);

    for (byte i = 0; i <= 10; i++) // show clock
    {
      lumina = 3 * analogRead(senzorlumina); // for control the brigntness
      starttime = millis();
      endtime = starttime;                    //Store the internal timer counter value to make this loop run for a set period.
      while ((endtime - starttime) <= 50) // do this loop for 5000mS
      {
        outputDisplay2(data, lumina);
        endtime = millis();                  //Read internal timer counter to see how long this loop has been running.
        if (!(digitalRead(SW0)))
        {
          meniu = 1; //go to menu for change hour
          i = 10;  // force exit in next menu
        }
      }
      starttime = millis();
      endtime = starttime;
      while ((endtime - starttime) <= 50) // do this loop for 5000mS
      {
        outputDisplay3(data, lumina);
        endtime = millis();                  //Read internal timer counter to see how long this loop has been running.
        if (!(digitalRead(SW0)))
        {
          meniu = 1; //go to menu for change hour
          i = 10;  // force exit in next menu
        }
      }
    }
    scrollOut(data);
  }

}  // end main loop

// ----------------------------------------------------------------------------------

void outputDigit(int seg)    //Outputs segment data for an individual digit.
{
  for (int s = 0; s < 7; s++)  //Read a bit at a time from the selected digit in the digits array and output it to the correct
  { //pin
    if (tip == 1) bitState = bitRead(digits[seg], s);  //Read the current bit.
    else bitState = 1 - bitRead(digits[seg], 6 - s); //Read the current bit.
    digitalWrite(cathodes[s], bitState);         //and output it.
  }
}

void numarare()
{
  numar = numar + 1111;
}
/*
void Exterior()
{ data[0] = 0;
  data[1] = 14;
  data[2] = 16;
  data[3] = 15;
}
*/
void scrollIn(int sDig[4])    //Scrolls data on to the display from blank.
{
  int scrollBuffer[8];        //Stores the entire set of data that will be shifted along the display.
  scrollBuffer[0] = 15;       //15 represents a blank digit.
  scrollBuffer[1] = 15;
  scrollBuffer[2] = 15;
  scrollBuffer[3] = 15;
  scrollBuffer[4] = sDig[0];
  scrollBuffer[5] = sDig[1];
  scrollBuffer[6] = sDig[2];
  scrollBuffer[7] = sDig[3];

  for (int st = 0; st < 4; st++) //Loop 4 times and read from next entry in the array each time.
  {
    int sData[4] = {scrollBuffer[(0 + st)], scrollBuffer[(1 + st)], scrollBuffer[(2 + st)], scrollBuffer[(3 + st)]};
    for (int sc = 0; sc < (DISPLAY_BRIGHTNESS * 10) / (DISPLAY_BRIGHTNESS + lumina); sc++) //Refresh the display 10 times after each shift to give the illusion of scrolling.
    {
      outputDisplay2(sData, lumina);
    }
  }
}

void scrollOut(int sDig[4])   //Scrolls data off the display to blank
{
  int scrollBuffer[8];        //Stores the entire set of data that will be shifted along the display.
  scrollBuffer[0] = sDig[0];
  scrollBuffer[1] = sDig[1];
  scrollBuffer[2] = sDig[2];
  scrollBuffer[3] = sDig[3];
  scrollBuffer[4] = 15;        //15 represents a blank digit.
  scrollBuffer[5] = 15;
  scrollBuffer[6] = 15;
  scrollBuffer[7] = 15;

  for (int st = 0; st < 4; st++) //Loop 4 times and read from next entry in the array each time.
  {
    int sData[4] = {scrollBuffer[(0 + st)], scrollBuffer[(1 + st)], scrollBuffer[(2 + st)], scrollBuffer[(3 + st)]};
    for (int sc = 0; sc < (DISPLAY_BRIGHTNESS * 10) / (DISPLAY_BRIGHTNESS + lumina); sc++) //Refresh the display 10 times after each shift to give the illusion of scrolling.
    {
      outputDisplay2(sData, lumina);
    }
  }
}

void outputDisplay2(int dig[4], float dilei)        //Scan the display once with the 4 digit int array passed in.
{
  for (int d = 0; d < 4; d++)
  {
    outputDigit(dig[d]);                //Set up the segment pins with the correct data.
    if (tip == 1) digitalWrite(anodes[d], HIGH);      //Turn on the digit.
    else digitalWrite(anodes[d], LOW);      //Turn on the digit.
    //  delay(p);   //Hold it on for 2ms to improve brightness.
    if (tip == 1) digitalWrite(cathodes[7], 1);
    else digitalWrite(cathodes[7], 0);
    delayMicroseconds(DISPLAY_BRIGHTNESS);

    if (tip == 1) digitalWrite(anodes[d], LOW);       //And turn it off again before looping untl all digits have been displayed.
    else digitalWrite(anodes[d], HIGH);       //And turn it off again before looping untl all digits have been displayed.
    delayMicroseconds(dilei);
  }
}

void outputDisplay3(int dig[4], float dilei)        //Scan the display once with the 4 digit int array passed in.
{
  for (int d = 0; d < 4; d++)
  {
    outputDigit(dig[d]);                //Set up the segment pins with the correct data.
    if (d == 1)
    {
      if (tip == 1) digitalWrite(cathodes[7], 0);
      else digitalWrite(cathodes[7], 1);
    }
    else
    {
      if (tip == 1) digitalWrite(cathodes[7], 1);
      else digitalWrite(cathodes[7], 0);
    }
    if (tip == 1) digitalWrite(anodes[d], HIGH);      //Turn on the digit.
    else digitalWrite(anodes[d], LOW);      //Turn on the digit.
    //  delay(p);                           //Hold it on for 2ms to improve brightness.
    delayMicroseconds(DISPLAY_BRIGHTNESS);

    if (tip == 1)  digitalWrite(anodes[d], LOW);       //And turn it off again before looping untl all digits have been displayed.
    else digitalWrite(anodes[d], HIGH);       //And turn it off again before looping untl all digits have been displayed.
    delayMicroseconds(dilei);
  }
}
