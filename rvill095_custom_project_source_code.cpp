#include <dht.h>

#define dhtPin A3
dht DHT;

enum LightStates{INIT1, L1, TRIP, REST};
enum Joy{INIT2, Check};
enum Ultra{INIT3, CheckEcho};
enum TAH{INIT4, CheckTAH};
enum Wind{INIT5, ChangeWind};
enum Tilt{INIT6, CheckTilt};

const int b_size = 7;
const int b[b_size] = {7, 8, 9, 10, 11, 12, 13};
//const int b[b_size] = {7, 8, 9, 10};
int ledOff = 0;

signed int showPos = 0;
const int show_size = 7;
const unsigned int Show1[show_size] = {1, 2, 4, 8, 16, 32, 64};
//const unsigned int Show1[show_size] = {1, 2, 4, 8};

const int buzzer = 3;
const int joyButton = 2;
const int trig = 5;
const int echo = 4;
const int tilt = 6;

const int xAxis = A0; // Recall that A0 is really a number under the hood.
const int yAxis = A1;
const int range = 12;
const int center = (range / 2) - 1;
unsigned int readingX;
unsigned int readingY;

unsigned int directionLR = 0;
unsigned int trip = 0;
unsigned int rest = 0;
unsigned int count = 0;
//unsigned int speedCount = 5;
//unsigned int r = 0;

unsigned long duration;
unsigned int distance;
unsigned int obstacled;
unsigned int averageTemp = 22; //celsius
unsigned int humidityMax = 70;
unsigned int humidityHigh;
unsigned int temperatureHigh;
unsigned int temperatureLow;

const long n = 2145678965L;
const long a = 763214L;
const long t = 88844L;
const long r = 7766L; //i
static long seed = 12345678L;
unsigned long windNum = 0;
unsigned int previousChange = 0;
unsigned int windBlowing = 0;

unsigned int tilted = 0;

// Reset the Output Buffer. 
void resetBuffer() {
    for(int i = 0; i < b_size; i++) {
        // Note this is an arduino function call to the pins
        digitalWrite(b[i], LOW);
    }
}
// Writes to the buffer. Note this function ORs the current value with the new value
// Note that size is an optional argument with default size the same as the buffer
void writeBuffer(int size = b_size)
{
    for (int i = (size - 1); i >= 0; i--) {
        if ((Show1[showPos] >> i) & 0x01) {
            //Serial.print("Here");
            digitalWrite(b[i], HIGH);
        }
        else{
          digitalWrite(b[i], LOW);
        }
    }
    if(directionLR){
      showPos--;
    }
    else{
      showPos++;
    }
    adjShowPos();

}

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
    
} task;

int delay_gcd;
const unsigned short tasksNum = 6;
task tasks[tasksNum];

int Light_Tick(int state){
    //Read thing
    switch(state){ // State transitions
      case INIT1:
        state = L1;
        break;
      case L1:
        if(trip == 1){
          state = TRIP;
          count = 0;
        }
        else if(rest == 1){
          state = REST;
          count = 0;
        }
        else{
          state = L1;
        }
        break;
      case TRIP:
        if(count > 4){
          count = 0;
          state = L1;
          trip = 0;
        }
        else{
          state = TRIP;
        }
        break;
      case REST:
        if(digitalReadHelper(joyButton) == HIGH){
          state = L1;
          count = 0;
        }
        else{
          state = REST;
        }
        break;
      default:
        break;
    }
    switch(state){ // State Action
      case INIT1:
        break;
      case L1:
        rest = 0;
        writeBuffer();
        break;
      case TRIP:
        count++;
        break;
      case REST:
        break;
      default:
        break;
    }

    return state;
}

int Joy_Tick(int state){
  switch(state){
    case INIT2:
      state = Check;
      break;
    case Check:
      state = Check;
      break;
  }
  switch(state){
    case INIT2:
      break;
    case Check:
        if((readingX < (center - 2))){//LEFT
          if(directionLR == 0){
            directionLR = 1; 
          }
          else if(directionLR == 1){
            trip = 1;
          }
        }
        else if((readingX > (center + 2))){//RIGHT
          if(directionLR == 1){
            directionLR = 0; 
          }
          else if(directionLR == 0){
            trip = 1;
          }
        }
        else if((digitalReadHelper(joyButton) == HIGH) && (count > 4)){
          rest = 1;
        }
        else if((readingY > (center + 2))){//UP
          tasks[0].period -= 100;
          Serial.print("up");
        }
        else if((readingY < (center - 2))){//DOWN
          tasks[0].period += 100;
          Serial.print("down");
        }
        count++;
      break;
  }
  return state;   
}

int Ultra_Tick(int state){
  switch(state){
    case INIT3:
      state = CheckEcho;
      break;
    case CheckEcho:
      state = CheckEcho;
      break;
  }
  switch(state){
    case INIT3:
      break;
    case CheckEcho:
      if(obstacled == 1){
        if(distance > 8){
          tasks[0].period -= 200;
          obstacled = 0;
        }
      }
      else if(obstacled == 0){
        if(distance <= 8){
          //Serial.print("Ting in da way");
          //Serial.print("\n\n");
          tasks[0].period += 200;
          obstacled = 1;
        }
      }
      break;
  }
  return state;
}

int TAH_Tick(int state){
  switch(state){
    case INIT4:
      state = CheckTAH;
      break;
    case CheckTAH:
      state = CheckTAH;
      break;
  }
  switch(state){
    case INIT4:
      break;
    case CheckTAH:
      if(temperatureHigh || temperatureLow){
        if((DHT.temperature > averageTemp - 5) && (DHT.temperature < averageTemp + 5)){
            tasks[0].period -= 100;
            temperatureHigh = 0;
            temperatureLow = 0;
        }
      }
      else if(!temperatureHigh && !temperatureLow){
        if((DHT.temperature <= averageTemp - 5) && (DHT.temperature >= averageTemp + 5)){
          tasks[0].period += 100;
          if(DHT.temperature <= averageTemp - 5){
            temperatureLow = 1;
          }
          if(DHT.temperature >= averageTemp + 5){
            temperatureHigh = 1;
          }
        }
      }

      if(humidityHigh){
        if(DHT.humidity < humidityMax){
          tasks[0].period -= 100;
          humidityHigh = 0;
        }
      }
      else if(humidityHigh == 0){
        if(DHT.humidity >= humidityMax){
          tasks[0].period += 100;
          humidityHigh = 1;
        }
      }
      break;
  }
  return state;
}

int Wind_Tick(int state){
  switch(state){
    case INIT5:
      state = ChangeWind;
      break;
    case ChangeWind:
      state = ChangeWind;
      break;
  }
  switch(state){
    case INIT5:
      break;
    case ChangeWind:
      Serial.print("here");
      Serial.print("\n");
      windNum = 0;
      long randNum = abs(randoGen());
      windNum = randNum % 10;
      Serial.print(randNum);
      Serial.print("\n");
      Serial.print(windNum);

      if(windNum == 0){
        noTone(buzzer);
        if(previousChange != 0){
          tasks[0].period -= previousChange;
        }
        windBlowing = 0;
        previousChange = 0;
      }
      else if(windNum <= 3){
        tone(buzzer, 171);
        if(previousChange != 100){
          tasks[0].period -= previousChange;
          tasks[0].period += 100;
        }
        windBlowing = 1;
        previousChange = 100;
      }
      else if(windNum <= 7){
        tone(buzzer, 201);
        if(previousChange != 200){
          tasks[0].period -= previousChange;
          tasks[0].period += 200;
        }
        windBlowing = 1;
        previousChange = 200;
      }
      else{
        tone(buzzer, 231);
        if(previousChange != 300){
          tasks[0].period -= previousChange;
          tasks[0].period += 300;
        }
        windBlowing = 1;
        previousChange = 300;
      }
      break;
  }
  return state;
}

int Tilt_Tick(int state){
  switch(state){
    case INIT5:
      state = CheckTilt;
      break;
    case CheckTilt:
      state = CheckTilt;
      break;
  }
  switch(state){
    case INIT5:
      break;
    case CheckTilt:
      if(digitalReadHelper(tilt) == LOW){
        Serial.print("\n\n");
        Serial.print("LOW");
        Serial.print("\n\n");
      }
      if(digitalReadHelper(tilt) == HIGH){
        Serial.print("\n\n");
        Serial.print("HIGH");
        Serial.print("\n\n");
      }
    
      if(tilted == 1){
        if(digitalReadHelper(tilt) == LOW){
          tasks[0].period -= 200;
          tilted = 0;
        }
      }
      else if(tilted == 0){
        if(digitalReadHelper(tilt) == HIGH){
          tasks[0].period += 200;
          tilted = 1;
        }
      }
      break;
  }
  return state;
}

void setup() {
    // OUTPUT is a defined macro in Arduino!
    for(int i = 0; i < b_size; i++)
        {
            pinMode(b[i], OUTPUT);
        }
    pinMode(joyButton, INPUT_PULLUP);
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    pinMode(xAxis, INPUT);
    pinMode(yAxis, INPUT);
    pinMode(tilt, INPUT);

    unsigned char i = 0;
    tasks[i].state = INIT1;
    tasks[i].period = 500;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Light_Tick;
    i++;
    tasks[i].state = INIT2;
    tasks[i].period = 500;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Joy_Tick;
    i++;
    tasks[i].state = INIT3;
    tasks[i].period = 100;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Ultra_Tick;
    i++;
    tasks[i].state = INIT4;
    tasks[i].period = 100;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &TAH_Tick;
    i++;
    tasks[i].state = INIT5;
    tasks[i].period = 10000;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Wind_Tick;
    i++;
    tasks[i].state = INIT6;
    tasks[i].period = 500;
    tasks[i].elapsedTime = 0;
    tasks[i].TickFct = &Tilt_Tick;

    delay_gcd = 100;
    Serial.begin(9600);
}

void loop() { 
  unsigned int b;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2; //The runner only slows down if distance <= 8 (cm)

  b = DHT.read11(dhtPin);
  /*Serial.print(b);
  Serial.print("\n\n");
  Serial.print("Humidity: ");
  Serial.print(DHT.humidity);
  Serial.print("\n\nTemperature: ");
  Serial.print(DHT.temperature);
  Serial.print("\n\n");*/
  
  readingX = analogRead(xAxis);
  readingX = map(readingX, 0, 1023, 0, range);
  readingY = analogRead(yAxis);   
  readingY = map(readingY, 0, 1023, 0, range);
    for (unsigned char i = 0; i < tasksNum; ++i) {
      if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
          tasks[i].state = tasks[i].TickFct(tasks[i].state);
          tasks[i].elapsedTime = millis(); // Last time this task was ran
      }
   }
  delay(delay_gcd);
}

void adjShowPos(){
  if(showPos > 6){
    showPos = 0;
  }
  else if(showPos < 0){
    showPos = 6;
  }
}

int digitalReadHelper(const int button){
  int buttonState = digitalRead(button);
  /*if(button == buttonX && buttonState == HIGH){
     return HIGH;
  }*/
  if(button == joyButton && buttonState == LOW){
     return HIGH;
  }
  if(button == tilt && buttonState == LOW){
     return HIGH;
  }
  
  return LOW;
}

long randoGen(){
  long hi = seed / t;
  long lo = seed - t * hi;
  long l = a * lo - r * hi;
  if (l > 0)
     seed = l;
  else
     seed = l + n;
  return seed;
}