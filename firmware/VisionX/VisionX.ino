// VisionX | Uno R3 / ATmega328P. All intervals use unsigned elapsed time.
#include <Arduino.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <util/atomic.h>
const byte trigPin=6, echoPin=5, irPin=2, keyPin=3, soundPin=9;
const byte greenPin=10, yellowPin=11, redPin=12;
const bool passivePiezo=true, latchingSwitch=false;
const byte irActive=LOW, irSamples=3;
constexpr byte rangeCm[]={100,60,30}, bandCm[]={75,50,25,10};
const byte releaseMargin=2;
static_assert(rangeCm[0]<=100 && rangeCm[1]<=100 && rangeCm[2]<=100,"Maximum range is one meter");
enum Risk : byte {Clear, Low, Moderate, High, Danger, Critical, Unknown, Infrared};
byte mode=0, risk=Unknown, acoustic=Unknown, historyCount=0, historyIndex=0;
int rawCm=-1, filteredCm=-1, history[3];
bool enabled=true, approaching=false, irConfirmed=false;
volatile byte capture=0; // 1 awaits rise, 2 awaits fall, 3 has width
volatile unsigned long risingUs, widthUs;
unsigned long shotUs, lastShot, riskSince, cueSince;
byte cueCount=2;
unsigned int cuePitch=1600, cueLength=55, cueGap=90;

ISR(PCINT2_vect) {
  if ((PIND & _BV(PD5)) && capture==1) { risingUs=micros(); capture=2; }
  else if (!(PIND & _BV(PD5)) && capture==2) { widthUs=micros()-risingUs; capture=3; }
}

void cue(byte count,unsigned int pitch,unsigned int length,unsigned int gap) {
  cueCount=count; cuePitch=pitch; cueLength=length; cueGap=gap; cueSince=millis();
}
void classify() {
  byte limit=rangeCm[mode];
  if (rawCm<0) { acoustic=Unknown; return; }
  if (rawCm>limit || filteredCm>limit) { acoustic=rawCm<=bandCm[3]?Critical:Clear; return; }
  byte next=Low;
  for(byte i=0;i<4;i++) if(filteredCm<=bandCm[i]) next=i+2;
  if(rawCm<=bandCm[3]) next=Critical; // a single credible close echo bypasses median
  if(acoustic>=Moderate && acoustic<=Critical && next<acoustic &&
     filteredCm<=bandCm[acoustic-2]+releaseMargin) return;
  acoustic=next;
}
void acceptEcho(unsigned long width) {
  int previous=filteredCm;
  if(width<116 || width>23250) {
    rawCm=filteredCm=-1; historyCount=historyIndex=0;
  } else {
    rawCm=(width+29)/58; // nearest nominal centimetre
    history[historyIndex]=rawCm; historyIndex=(historyIndex+1)%3;
    if(historyCount<3) historyCount++;
    int a=history[0], b=history[1], c=history[2];
    filteredCm=historyCount<3 ? rawCm : max(min(a,b),min(max(a,b),c));
  }
  approaching=previous>=0 && filteredCm>=0 && previous-filteredCm>=3;
  classify();
}
void sampleUltrasonic(unsigned long now) {
  unsigned long width=0;
  bool finished=false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if(capture==3) { width=widthUs; finished=true; capture=0; }
    else if(capture && micros()-shotUs>=26000UL) { capture=0; finished=true; }
  }
  if(finished) acceptEcho(width);
  if(now-lastShot>=65 && !capture) {
    lastShot=now; shotUs=micros();
    capture=1;
    digitalWrite(trigPin,HIGH); delayMicroseconds(12); digitalWrite(trigPin,LOW);
  }
}
void sampleIR(unsigned long now) {
  static unsigned long last;
  static byte hits=0, misses=0;
  if(now-last<8) return;
  last=now;
  if(digitalRead(irPin)==irActive) {
    misses=0; if(hits<irSamples) hits++;
    if(hits==irSamples) irConfirmed=true;
  } else {
    hits=0; if(misses<irSamples) misses++;
    if(misses==irSamples) irConfirmed=false;
  }
}
void setMode(byte next) {
  mode=next; classify(); cue(mode+1,1700,65,100);
}
void readControl(unsigned long now) {
  static bool raw=false, stable=false, held=false;
  static unsigned long changed, pressed;
  bool down=digitalRead(keyPin)==LOW;
  if(down!=raw) { raw=down; changed=now; }
  if(raw!=stable && now-changed>=25) {
    stable=raw;
    if(latchingSwitch) setMode(stable?2:0);
    else if(stable) { pressed=now; held=false; }
    else if(!held && enabled) setMode((mode+1)%3);
  }
  if(!latchingSwitch && stable && !held && now-pressed>=1200) {
    held=true; enabled=!enabled;
    if(enabled) cue(2,2100,45,50);
    else cue(1,750,450,100);
  }
}
void driveSound(unsigned int hz) {
  static unsigned int last=0;
  if(hz==last) return;
  last=hz;
  if(passivePiezo) { if(hz) tone(soundPin,hz); else noTone(soundPin); }
  else digitalWrite(soundPin,hz?HIGH:LOW);
}
void outputs(unsigned long now) {
  byte next=irConfirmed?Infrared:acoustic;
  if(next!=risk) { risk=next; riskSince=now; }
  unsigned long t=now-riskSince, age=now-cueSince;
  unsigned int span=cueLength+cueGap;
  if(age>=cueCount*(unsigned long)span) cueCount=0;
  bool startup=age<360 && cueCount && cuePitch==1600;
  bool urgent=enabled && (risk==Critical || risk==Infrared || risk==Danger);
  bool g=false,y=false,r=false;
  unsigned int hz=0;
  if(enabled) {
    g=risk==Clear || risk==Low;
    y=risk==Moderate || (risk==Unknown && t%800<400);
    r=risk==High || risk==Infrared || (risk==Danger && t%400<200) ||
      (risk==Critical && t%160<80);
    if(risk>=Low && risk<=Danger) {
      const unsigned int periods[]={1000,500,250,100};
      if(t%periods[risk-Low]<45) hz=1850;
    } else if(risk==Critical) hz=2400;
    else if(risk==Infrared && t%120<90) hz=2600;
    else if(risk==Unknown && (t%2400<45 || (t%2400>=120 && t%2400<165))) hz=850;
  } else { y=now%2000<100; if(now%5000<90) hz=700; }
  if(urgent) cueCount=0;
  else if(cueCount) {
    hz=age%span<cueLength?cuePitch:0;
    if(startup) { g=age<90; y=age>=90 && age<180; r=age>=180; }
  }
  digitalWrite(greenPin,g); digitalWrite(yellowPin,y); digitalWrite(redPin,r);
  driveSound(hz);
}
// Text-only display uses little SRAM. The photo confirms I2C pin labels;
// select SH1106_128x64 here if that controller is listed for your module.
SSD1306AsciiWire display;
bool hasDisplay=false;
const DevType* const displayType=&Adafruit128x64;

void beginDisplay() {
  Wire.begin();
  Wire.setClock(100000UL);
  Wire.setWireTimeout(2500, true);
  for (byte address=0x3C; address<=0x3D; address++) {
    Wire.beginTransmission(address);
    byte result=Wire.endTransmission();
    Serial.print(F("OLED 0x")); Serial.print(address,HEX);
    Serial.print(F(" result ")); Serial.println(result);
    if (result!=0) continue;
    display.begin(displayType,address);
    display.setFont(System5x7);
    display.clear();
    display.println(F("VisionX"));
    hasDisplay=true;
    Serial.println(F("OLED text sent"));
    return;
  }
  Serial.println(F("No OLED response; audio monitoring active"));
}

void updateDisplay(unsigned long now) {
  static unsigned long last;
  if (!hasDisplay || now-last<250) return;
  last=now;
  static int shownCm = -2;
  static byte shownMode = 255, shownState = 254;
  if (filteredCm != shownCm) {
    display.set2X();
    display.setCursor(0,2);
    if (filteredCm < 0) display.print(F("--    "));
    else {
      display.print(filteredCm);
      display.print(F(" cm"));
      if (filteredCm < 100) display.print(' ');
      if (filteredCm < 10) display.print(' ');
    }
    shownCm = filteredCm;
  }
  if (mode != shownMode) {
    display.set1X();
    display.setCursor(0,4);
    display.print(F("Limit: "));
    display.print(rangeCm[mode]);
    display.print(F(" cm "));
    shownMode = mode;
  }
  const byte state = enabled ? risk : 255;
  if (state != shownState) {
    display.set2X();
    display.setCursor(0,6);
    switch (state) {
      case Clear:    display.print(F("OUTSIDE ")); break;
      case Low:      display.print(F("LOW     ")); break;
      case Moderate: display.print(F("MODERATE")); break;
      case High:     display.print(F("HIGH    ")); break;
      case Danger:   display.print(F("DANGER  ")); break;
      case Critical: display.print(F("CRITICAL")); break;
      case Infrared: display.print(F("IR ALERT")); break;
      case Unknown:  display.print(F("UNKNOWN ")); break;
      default:       display.print(F("PAUSED  ")); break;
    }
    shownState = state;
  }
  if (Wire.getWireTimeoutFlag()) hasDisplay=false;
}

void setup() {
  pinMode(trigPin,OUTPUT); pinMode(echoPin,INPUT);
  pinMode(irPin,INPUT_PULLUP); pinMode(keyPin,INPUT_PULLUP);
  pinMode(soundPin,OUTPUT); pinMode(greenPin,OUTPUT);
  pinMode(yellowPin,OUTPUT); pinMode(redPin,OUTPUT);
  PCMSK2 |= _BV(PCINT21); PCICR |= _BV(PCIE2);
  Serial.begin(115200);
  delay(200);  // Once at power-up, let the display stabilise.
  beginDisplay();
  lastShot=millis()-65UL; cueSince=millis();
}
void loop() {
  unsigned long now=millis();
  sampleUltrasonic(now); sampleIR(now); readControl(now); outputs(now);
  updateDisplay(now);
  static unsigned long lastLog;
  if(now-lastLog>=250) {
    lastLog=now;
    Serial.print(F("cm=")); Serial.print(rawCm); Serial.print(F(" med=")); Serial.print(filteredCm);
    Serial.print(F(" IR=")); Serial.print(irConfirmed); Serial.print(F(" band=")); Serial.print(risk);
    Serial.print(F(" range=")); Serial.print(rangeCm[mode]); Serial.print(F(" on=")); Serial.print(enabled);
    Serial.print(F(" nearer=")); Serial.println(approaching);
  }
}
// END OF VISIONX
