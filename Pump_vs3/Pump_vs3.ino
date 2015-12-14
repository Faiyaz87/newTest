
#include <Wire.h>
#include <Time.h>
#include <RTClib.h>
#include <EEPROM.h>


RTC_DS1307 RTC;
int alaramTimeAddress = 100;
int alaramDurationAddress = 110;
int motorPin = 2;
int loopCounter = 0;
int waterlevelInd_full = 8;
int sensorPin = A2;
int directWaterPin = A3;
int sensorValue =0;
int directWaterValue = 0;
int directWaterDurationCount = 0;
long alaramDuration = 3600;
long curr_TodayInSec = 0;
//long alaramTime = 32576;

long alaramTime = 21600;
long alermDurSum =0;
int tankLevelCount = 0;


void setup() {
  Serial.begin(9600);
  pinMode(waterlevelInd_full, OUTPUT);
  Wire.begin();
  RTC.begin();
  //RTC.adjust(DateTime(__DATE__, __TIME__));
 if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  } 
}

void loop() {
         
  

   MotorTimerMainLoop();
  
  

    
  delay(5000);
}


void SwitchOffMotor(){
  Serial.println("MOTOR OFF called . ");
 digitalWrite(motorPin, LOW);
}

void SwitchOnMotor(){
  Serial.println("MOTOR ON called . ");
 digitalWrite(motorPin, HIGH);
}


void MotorTimerMainLoop(){
    DateTime now = RTC.now();
    long curr_Hour = long(now.hour());
    long curr_Minute = long(now.minute());
    long curr_Second = long(now.second());
    Serial.println(curr_Hour);
    Serial.println(curr_Minute);
    Serial.println(curr_Second);
    curr_TodayInSec = curr_Hour*3600;
    //Serial.println(curr_TodayInSec);
    curr_TodayInSec += curr_Minute*60;
    curr_TodayInSec += curr_Second;
    //Serial.println(curr_TodayInSec);
    Serial.print("curr_TodayInSec >> ");
    Serial.print(curr_TodayInSec);
    //Serial.println();
    //alaramTime = 24526;//EEPROMReadlong(alaramTimeAddress);
    Serial.print("alaramTime >> ");
    Serial.println(alaramTime);

    //alaramDuration = 500;//EEPROMReadlong(alaramDurationAddress);
    //Serial.print("alaramDuration >> ");
    //Serial.println(alaramDuration);
    //int tankVal = digitalRead(tankPin); 
   // Serial.print("tankVal >> ");
   // Serial.println(tankVal);
    //int directWaterVal = digitalRead(directWaterPin); 
   // Serial.print("directWaterVal >> ");
    //Serial.println(directWaterVal);
    sensorValue = analogRead(sensorPin);
    Serial.println("tank WaterValue");
    Serial.println(sensorValue);
    if(sensorValue >= 250){
      digitalWrite(waterlevelInd_full, HIGH);
    }else{
      digitalWrite(waterlevelInd_full, LOW);
    }
  if(curr_TodayInSec >= alaramTime){
     Serial.println("curr_TodayInSec > alaramTime");
     alermDurSum = (alaramTime + alaramDuration);
     //Serial.println("alermDurSum");
     //Serial.println(alermDurSum);
    if(curr_TodayInSec <= alermDurSum){
      
       Serial.println("curr_TodayInSec <= alermDurSum");
       
       directWaterValue = analogRead(directWaterPin); 
       Serial.println("directWaterValue");
       Serial.println(directWaterValue);
      if(sensorValue >=250){
        tankLevelCount = tankLevelCount + 1;
        if(tankLevelCount > 8){
          SwitchOffMotor();
          tankLevelCount=0;
        }
      }else{
        tankLevelCount=0;
        if(directWaterValue < 250){
          directWaterDurationCount = directWaterDurationCount + 1;
          if(directWaterDurationCount > 6){
            SwitchOffMotor();
            directWaterDurationCount=0;
            delay(180000);
          }else{
            SwitchOnMotor();
          }

        }else{
          SwitchOnMotor();
        }
        
      }

    }else{
      SwitchOffMotor();
    }
  }else{
      SwitchOffMotor();
  }
}

void EEPROMWritelong(int address, long value){
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
}


long EEPROMReadlong(long address){
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}
