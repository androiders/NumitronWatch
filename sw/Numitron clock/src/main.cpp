#include <Arduino.h>
#include "tpic6595.h"
#include "Clock.h"

#define NUM0 0x7D // -
#define NUM1 0x60 // -
#define NUM2 0x5B // -
#define NUM3 0x73 // -
#define NUM4 0x66 // -
#define NUM5 0x37 // -
#define NUM6 0x3F // -
#define NUM7 0x70 // -
#define NUM8 0x7D // -
#define NUM9 0x77 //

#define SD1 18
#define G1 19

#define SD2 2
#define G2 3

#define SD3 4
#define G3 5

#define SD4 6
#define G4 7

#define SER_CK 16 //serial clock
#define SER_CLR 15 //serial clear, clear on low
#define R_CK 14 //register clock

#define SET_M_PIN 8
#define SET_H_PIN 17

#define AUX_LED 9
#define SEK_LED 10

Tpic6595 u(SER_CK, SER_CLR, R_CK, G1, SD1, G2, SD2, G3, SD3, G4, SD4);
Clock c;

bool setM = false;
bool setH = false;

enum class ClockState
{
  NORMAL,
  SET
};

ClockState currentState = ClockState::NORMAL;
uint8_t setCnt = 0;

ISR(TIMER1_COMPA_vect)
{
  digitalWrite(AUX_LED,!digitalRead(AUX_LED));
}

#define cntVal 1;

ISR(TIMER1_OVF_vect)
{
  static uint8_t cnt = cntVal;
  static uint8_t duty = 0;
  OCR1B = duty;
  duty+= cnt;

  if(duty == 80)
    cnt = -cntVal;
  if(duty == 1)
    cnt = cntVal;

  }

ISR(TIMER2_COMP_vect)
{
  c.tickSeconds();
}

void setupTimer1PWM()
{

  TCCR1A = (1 << WGM10) | (1 << COM1B1);
  TCCR1B = (1 << WGM12) | (1 << CS11)  | (1 << CS10); 
  TCNT1 = 0x00;
  TIMSK |=  (1 << TOIE1); 
  OCR1B = 0x00;
}

void setupAsynchTimer2()
{
  TCCR2 = (1 << WGM21); //set CTC mode
  TCCR2 |= (1 << CS22) | (1 << CS21) | (0 << CS20); //set prescaler to 1/256 

  OCR2 = (0xff >> 1); 
  TIMSK |= (1 << OCIE2); //output compare interrup enable on timer2
  ASSR = (1 << AS2); //set timer2 asynch mode
}

uint8_t lastMin = 0;
void setup() {
  pinMode(SET_H_PIN, INPUT_PULLUP);
  pinMode(SET_M_PIN, INPUT_PULLUP);

  pinMode(SEK_LED, OUTPUT);
  pinMode(AUX_LED, OUTPUT);

  cli();
  setupTimer1PWM();
  setupAsynchTimer2();
  sei();

  lastMin = c.getMinuts();

  Serial.begin(9600);
  Serial.flush();

  c.setCompensationSeconds(-2);
}


uint8_t hidxt = 0;
uint8_t hidxo = 0;
uint8_t midxt = 0;
uint8_t midxo = 0;
uint8_t nums[] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9};

void clockToIdx(const Clock & c)
{
  uint8_t h = c.getHours();
  uint8_t m = c.getMinuts();

  uint8_t mtens = m / 10;
  uint8_t mones = m % 10;

  uint8_t htens = h / 10;
  uint8_t hones = h % 10;

  hidxt = htens;
  hidxo = hones;

  midxt = mtens;
  midxo = mones;
}

bool updated = true;
uint8_t  setDisableCnt = 0;

void setClock()
{
  updated = true;
  if(setM && !setH)
    c.tickMinutes();
  else if(setH && !setM)
    c.tickHours();
  else
    updated = false;
}

void loop() {

  if(currentState == ClockState::SET)
  {
    setClock();
  }
  else if(currentState == ClockState::NORMAL)
  {
    if(lastMin != c.getMinuts())
    {
      lastMin = c.getMinuts();
      updated = true;
    }
  }

  if(!updated)
  {
    if(currentState == ClockState::SET)
    {
        u.toggle();
        setDisableCnt++;
        if(setDisableCnt == 10)
        {
          currentState = ClockState::NORMAL;
          setDisableCnt = 0;
          c.setSeconds(0);
        }
    }
  }
  else
  {
    setDisableCnt = 0;
    clockToIdx(c);
    uint8_t h1 = nums[hidxt];
    uint8_t h2 = nums[hidxo];
    uint8_t m1 = nums[midxt];
    uint8_t m2 = nums[midxo];
   
    u.writeBytes(h1,h2,m1,m2);
    updated = false;
  }

  if(Serial.peek() != -1)
  {
    String in = Serial.readString();
    in.trim();
    String cmd = in.substring(0,3);
    cmd.trim();
    if(in.startsWith("get"))
    {
      Serial.print((int)c.getHours());
      Serial.write(":");
      Serial.print((int)c.getMinuts());
      Serial.write(":");
      Serial.println((int)c.getSeconds());
    }
    else if(in.startsWith("set"))
    {
      uint8_t h = in.substring(4,6).toInt();
      uint8_t m = in.substring(7,9).toInt();
      uint8_t s = in.substring(10,12).toInt();

      c.set(h,m,s);

      Serial.write("Setting clock: ");
      Serial.print(h);
      Serial.write(":");
      Serial.print(m);
      Serial.write(":");
      Serial.println(s);
    }
    else
    {
      Serial.write("unknown command ");
      Serial.write(cmd.c_str());
      Serial.write("\n");
      //Serial.flush();
    }
  }

  delay(500);
}