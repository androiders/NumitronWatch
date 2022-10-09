#include <Arduino.h>
//#include <ShiftRegister74HC595.h>
#include "tpic6595.h"
#include "Clock.h"

#define NUM0 0xEB
#define NUM1 0x60
#define NUM2 0xA7
#define NUM3 0xE6
#define NUM4 0x6C
#define NUM5 0xCE
#define NUM6 0xCF
#define NUM7 0xE0
#define NUM8 0xEF
#define NUM9 0xEC

#define NUM0_ 0xEE
#define NUM1_ 0x60
#define NUM2_ 0xA7
#define NUM3_ 0xE3
#define NUM4_ 0x69
#define NUM5_ 0xCB
#define NUM6_ 0xCF
#define NUM7_ 0xE0
#define NUM8_ 0xEF
#define NUM9_ 0xE9


#define SD1 4
#define G1 5

#define SD2 0
#define G2 1

#define SD3 2
#define G3 3

#define SD4 6
#define G4 7

#define SER_CK 16 //serial clock
#define SER_CLR 15 //serial clear, clear on low
#define R_CK 14 //register clock

#define SET_M_PIN 9
#define SET_H_PIN 10

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

uint8_t pwmMax = 0xff;

uint8_t pwm()
{
  static uint8_t dutyCycle = 1;
  static uint8_t val = 0;
  static uint8_t pwmOp = 1;
  static uint8_t valOp = 1;
  static uint8_t valTgt = 50;
  static uint8_t pwmTgt = 50;

  uint8_t ret = 0;
  if(val > dutyCycle)
    ret = 0;
  else
    ret = 1;

  val += 2;
  if(val == valTgt)
  {
    dutyCycle += 1;
    val = 0;
  }

  return ret;

}

ISR(TIMER1_OVF_vect)
{
    //digitalWrite(8,!digitalRead(8));
    setM = !digitalRead(SET_M_PIN);
    setH = !digitalRead(SET_H_PIN);

    if(setM && setH)
    {
      if(setCnt < 25)
      {
        setCnt++;
      }
      else
      {
        currentState = ClockState::SET;
        setCnt = 0;
      }
    }

    digitalWrite(8,pwm());

    TCNT1 = 0xFFF0;
}

ISR(TIMER2_COMP_vect)
{
  c.tickSeconds();
  digitalWrite(17,!digitalRead(17));
}

void setupTimer1()
{
  TCNT1 = 0xF000;
  TCCR1A = 0x00;//(0 << COM1A0) | (0 << COM1A1); //normal mode
  TCCR1B = (0 << CS12) | (0 << CS11) | (1 << CS10); //prescaler 1024
  TIMSK |= (1 << TOIE1); //overflow interrup enable
}

// void setupTimer2()
// {
//   // TCCR2 = (1 << WGM21) | (1 << COM20) ; //set CTC mode, toggle OC2 on compare match
//   TCCR2 = (1 << WGM21); //set CTC mode
//   TCCR2 |= (1 << CS22) | (1 << CS21); //set prescaler to 1/256 

//   OCR2 = 0xff; //just set this to something
//   TIMSK = (1 << OCIE2); //output compare interrup enable on timer2

// }

void setupAsynchTimer2()
{
  TCCR2 = (1 << WGM21); //set CTC mode
  TCCR2 |= (1 << CS22) | (1 << CS21) | (0 << CS20); //set prescaler to 1/256 

  OCR2 = (0xff >> 1); 
  TIMSK |= (1 << OCIE2); //output compare interrup enable on timer2
  ASSR = (1 << AS2); //set timer2 asynch mode
}

uint8_t lastMin = 0;
//uint8_t lastHour;
void setup() {
  // put your setup code here, to run once:
  pinMode(17, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(SET_H_PIN, INPUT_PULLUP);
  pinMode(SET_M_PIN, INPUT_PULLUP);

  setupTimer1();
  setupAsynchTimer2();
  sei();
  //u.clear();

  lastMin = c.getMinuts();
//  lastHour = c.getHours();

  //  Serial.begin(9600);
  //  Serial.flush();

  c.setCompensationSeconds(-2);
}


char hidx = 0;
char hidx_ = 0;
char midx_ = 0;
char midx = 0;
uint8_t nums[] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9};
uint8_t nums_[] = {NUM0_, NUM1_, NUM2_, NUM3_, NUM4_, NUM5_, NUM6_, NUM7_, NUM8_, NUM9_};

void clockToIdx(const Clock & c)
{
  uint8_t h = c.getHours();
  uint8_t m = c.getMinuts();

  uint8_t mtens = m / 10;
  uint8_t mones = m % 10;

  uint8_t htens = h / 10;
  uint8_t hones = h % 10;

  hidx = htens;
  hidx_ = hones;

  midx_ = mtens;
  midx = mones;

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

  // if(lastHour != c.getHours())
  // {
  //   lastHour = c.getHours();
  //   updated = true;
  // }
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

  //if(updated)
  else
  {
    setDisableCnt = 0;
    clockToIdx(c);
    uint8_t h1 = nums[hidx];
    uint8_t h2 = nums_[hidx_];
    uint8_t m1 = nums_[midx_];
    uint8_t m2 = nums[midx];
   
    u.writeBytes(h1,h2,m1,m2);
    updated = false;
  }


  // if(Serial.peek() != -1)
  // {
  //   String in = Serial.readString();
  //   in.trim();
  //   String cmd = in.substring(0,3);
  //   cmd.trim();
  //   if(in.startsWith("get"))
  //   {
  //     Serial.print((int)c.getHours());
  //     Serial.write(":");
  //     Serial.print((int)c.getMinuts());
  //     Serial.write(":");
  //     Serial.println((int)c.getSeconds());
  //   }
  //   else if(in.startsWith("set"))
  //   {
  //     uint8_t h = in.substring(4,6).toInt();
  //     uint8_t m = in.substring(7,9).toInt();
  //     uint8_t s = in.substring(10,12).toInt();

  //     c.set(h,m,s);

  //     Serial.write("Setting clock: ");
  //     Serial.print(h);
  //     Serial.write(":");
  //     Serial.print(m);
  //     Serial.write(":");
  //     Serial.println(s);
  //   }
  //   else
  //   {
  //     Serial.write("unknown command ");
  //     Serial.write(cmd.c_str());
  //     Serial.write("\n");
  //     //Serial.flush();
  //   }
  // }
  

  // for (byte value = 255; value >0; value-=1)
  // {
  //   analogWrite(17,value);
  //   delay(10);
  // }
  delay(500);

}