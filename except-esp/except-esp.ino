#include<LiquidCrystal.h>
int buzzerPin = 8;  // Buzzer pin
int R = 12; // Red pin of RGB led
int G = 9; // Green pin of RGB led
int B = 10; // Blue pin of RGB led
int humidity; //Current readed humidity
int fa28Pin = A0; // humidity sensor pin
#define VOLTAGE 1

LiquidCrystal lcd(7,6,5,4,3,2);  //initiliaze lcd screen

void loadColors();
void lightLed(int r, int g, int b);
void makeSound(int delayTime);
void stopSound(int delayTime);

void setup() 
{
  //set up pins
  pinMode(fa28Pin,INPUT);
  pinMode(buzzerPin,OUTPUT);
  pinMode(R,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(B,OUTPUT);
  lcd.begin(16,2); //start lcd screen
  lcd.print("Loading...");
  loadColors();
}
void loop() {
  humidity = analogRead(fa28Pin);
  humidity = map(humidity,0,1023,0,100); //map humidity to range of 0 100
  humidity = 100 - humidity; //map  it to per cent
  lcd.clear(); //clear lcd screen
  char buffer [16];  //create a buffer to write in a lcd screen
  sprintf (buffer, "     %c %d",'%',humidity); //set up humidity percent
  lcd.print("Soil Humidity");  //print header
  lcd.setCursor(0,1); //go to new line
  lcd.print(buffer); //print humidity per cent
  if(humidity>60){  //if percent above 60 it means plant not needs any water
      lightLed(0,255,255); //red color
      makeSound(100);
    }
  else if(humidity>=50 && humidity<=60){ // %50-%60 is good for plants
      lightLed(255,0,255); //green color
      makeSound(1000);      //make a calmer sound
    }
   else{
    lightLed(255,255,0); //blue color
    stopSound(200); //and not sound buzzer
   }
}

void stopSound(int delayTime){
  digitalWrite(buzzerPin,!VOLTAGE); 
  delay(delayTime);
}

void lightLed(int r, int g, int b){
  analogWrite(R,r);
  analogWrite(G,g);  
  analogWrite(B,b);
}

void makeSound(int delayTime){
  digitalWrite(buzzerPin,VOLTAGE);  
  delay(delayTime);
  digitalWrite(buzzerPin,!VOLTAGE);
  delay(delayTime);
}

void loadColors(){
  for(int r=0; r<=3; r++){
    for(int g=0; g<=3; g++){
      for(int b=0; b<=3; b++){
        lightLed(r*85,g*85,b*85);
        delay(30);
      }
    }
  }
  lightLed(0,0,0);
}
