//LCD Declarations
#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 11, 5, 4, 6, 2);
const int OTHER = 0;
const int METAL = 1;
const int COMPOST = 2;
const int PAPER = 3;
int wasteType;

//Laser declarations
const int laserPin = 7;
const int photoPin = A1;
int laserVal = 0;
int avgLaser = 0;

//Ohmmeter Declarations
int analogPin= A4;
float Vin= 5, Vout= 0,R1= 4700, R2= 0, sensorValue = 0, temp = 0;

//Capacitive sensor declarations
const int capPin = 3;
const int capPowerPin = 10;
int capSense;

//Calibration declarations
float VoutControl;
int avgLaserControl;
boolean calibrated = false;

void setup()
{
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(laserPin, OUTPUT);
//  pinMode(motionPinA, INPUT);
//  pinMode(motionPinB, INPUT);
  pinMode(capPin, INPUT);
  pinMode(capPowerPin, OUTPUT);
}

void loop()
{
  digitalWrite(capPowerPin, HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  

  if(!calibrated)
  {
    lcd.print("Calibrating");
    calibrate();
    calibrated = true;
    delay(10000);
  }
  else
  {
    lcd.print("Welcome to Urbin");
  wasteType = 0;
  
  //capacitve sensor first patch
  capSense = LOW;
  while(capSense == LOW)
  {
    capSense = digitalRead(capPin);
    Serial.println(capSense);
  }

  delay(2000); //time for conveyor belt to reach ohmmeter
   
  //ohmmeter test
  wasteType = ohmmeter();
  Vout = 0;
  temp = 0;
  Serial.println(wasteType);
   
  //laser test
  //have an if statement here so that the laser test only runs if the ohmmeter test was false
  if(wasteType==0)
  {
    //capacitive sensor second patch
    capSense = LOW;
    while(capSense == LOW)
    {
      capSense = digitalRead(capPin);
      Serial.println(capSense);  
    }
    
    delay(1500);
    avgLaser = laserTest();
    if (avgLaser >= 230 && avgLaser <= 650)
    { 
      wasteType = PAPER;
    }
    else
    {
      wasteType = OTHER;
    }
    Serial.println(wasteType);
  }
  
  //print stuff on LCD
  lcdDisplay(lcd, wasteType); 

  delay(16500);
  }
}

void lcdDisplay(LiquidCrystal lcd, int w)
{
  lcd.clear();
  lcd.setCursor(0,0);
  
if(w == METAL)
{
  lcd.print("Metal");
}
else if(w == COMPOST)
{
  lcd.print("Compost");
}
else if(w == PAPER)
{
  lcd.print("Paper");
}
else
{
  lcd.print("Other");
}
}

int ohmmeter(){
int count = 0;
while (count < 5000){
sensorValue= analogRead(analogPin);
temp = (Vin * sensorValue) / 1023;    // Convert Vout to volts
if (temp > Vout){
  Vout = temp;
}
count++;
}
count = 0;
Serial.print("Vout: ");
Serial.println(Vout);

if (Vout >= 2)
{
  return METAL;
}
else if (Vout >= VoutControl+0.01)
{
  return COMPOST;
}
else
{
  return OTHER;
}
}

int laserTest()
{
  int count = 0;
  unsigned long total = 0;
  digitalWrite(laserPin, HIGH);
  while(count<=10000)
  {
    count++;
    laserVal = analogRead(photoPin);
    total = total+laserVal;
   // Serial.print("Total: ");
    /*Serial.println(total);
    Serial.print("LaserVal: ");
    Serial.println(laserVal);
    Serial.print("Count: ");
    Serial.println(count);*/
  }
  digitalWrite(laserPin, LOW);
  Serial.print("AVG: ");
  Serial.println(total/count);
  return total/count;
}


void calibrate()
{
  capSense = LOW;
    while(capSense == LOW)
    {
      capSense = digitalRead(capPin);
      Serial.println(capSense);  
    }

    delay(2000);
  int count = 0;
  VoutControl = 0;
  while (count < 5000){
  sensorValue= analogRead(analogPin);
  temp = (Vin * sensorValue) / 1023;    // Convert Vout to volts
  //Serial.print("Temp: ");
  //Serial.println(temp);
  if (temp > VoutControl){
    VoutControl = temp; 
  }
  count++;
  }
  count = 0;
  Serial.print("VoutControl: ");
  Serial.println(VoutControl);
  
  //avgLaserControl = laserTest();
}

