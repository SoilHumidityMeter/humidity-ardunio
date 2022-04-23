#include<LiquidCrystal.h>
#include <SoftwareSerial.h> 

int buzzerPin = 8;  // Buzzer pin
int R = 11; // Red pin of RGB led
int G = 12; // Green pin of RGB led
int B = 13; // Blue pin of RGB led
int rX = 9;
int tX = 10;
int nemDegeri; //Current readed humidity
int nemSensorPin = A0; // humidity sensor pin
#define VOLTAGE 1
String wifiName = "SOILHM";  
String wifiPassword = "7e8r9GH6D2FF32T";
String deviceId = "7e8r9GH6D2FF32T";
String apiIp = "13.38.214.178";
String ipGetterApiIp = "3.220.57.224";
String deviceIp;
LiquidCrystal lcd(7,6,5,4,3,2);  //initiliaze lcd screen

void loadColors();
void lightLed(int r, int g, int b);
void makeSound(int delayTime);
void stopSound(int delayTime);
String getIp();

SoftwareSerial esp(rX, tX); 

void setup() 
{
  Serial.begin(9600);  
  Serial.println("Started");
  esp.begin(115200);   
  esp.println("AT");   
  Serial.println("AT Yolland?");
  while(!esp.find("OK")){
    esp.println("AT");
    Serial.println("ESP8266 Bulunamad?.");
  }
  Serial.println("OK Komutu Al?nd?");
  esp.println("AT+CWMODE=1");
  while(!esp.find("OK")){    
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yap?l?yor....");
  }
  Serial.println("Client olarak ayarland?");
  Serial.println("Aga Baglaniliyor...");
  esp.println("AT+CWJAP=\""+wifiName+"\",\""+wifiPassword+"\"");    
  while(!esp.find("OK"));                                     //A?a ba?lanana kadar bekliyoruz.
  Serial.println("Aga Baglandi.");
  delay(1000);
  //set up pins
  pinMode(nemSensorPin,INPUT);
  pinMode(buzzerPin,OUTPUT);
  pinMode(R,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(B,OUTPUT);
  lcd.begin(16,2); //start lcd screen
  lcd.print("Loading...");
  loadColors();
  deviceIp = getIp();
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\""+apiIp+"\",80");

  if(esp.find("Error")){                                      
    Serial.println("AT+CIPSTART Error");
  }

  String veri = "POST https://shmb.eu-west-3.elasticbeanstalk.com/measurements?deviceId="+deviceId;
  veri+="&amp;humidity="+String(nemDegeri);
  veri += "&amp;ip="+deviceIp;

  veri += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");    
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find("&gt;")){                                          //ESP8266 haz?r oldu?unda i�indeki komutlar �al???yor.
    esp.print(veri);                                          //Veriyi g�nderiyoruz.
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  Serial.println("Baglant? Kapatildi.");
  esp.println("AT+CIPCLOSE");               
  nemDegeri = analogRead(nemSensorPin);
  nemDegeri = map(nemDegeri,0,1023,0,100); //map humidity to range of 0 100
  nemDegeri = 100 - nemDegeri; //map  it to per cent
  lcd.clear(); //clear lcd screen
  char buffer [16];  //create a buffer to write in a lcd screen
  sprintf (buffer, "     %c %d",'%',nemDegeri); //set up humidity percent
  lcd.print("Toprak Nem Yuzde");  //print header
  lcd.setCursor(0,1); //go to new line
  lcd.print(buffer); //print humidity per cent
  if(nemDegeri>60){  //if percent above 60 it means plant not needs any water
      lightLed(0,255,255); //red color
      makeSound(100);
    }
  else if(nemDegeri>=50 && nemDegeri<=60){ // %50-%60 is good for plants
      lightLed(255,0,255); //green color
      makeSound(1000);      //make a calmer sound
    }
   else{
    lightLed(255,255,0); //blue color
    stopSound(200); //and not sound buzzer
   }
}

String getIp(){
  esp.println("AT+CIPSTART=\"TCP\",\""+ipGetterApiIp+"\",80");

  if(esp.find("Error")){                                      
    Serial.println("AT+CIPSTART Error");
  }

  String veri = "GET http://api.ipify.org/";
  veri += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");    
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find("&gt;")){                                          
    esp.print(veri);                                        
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
    Serial.println(esp.read());
    String ip = esp.readString();
    Serial.println("Baglant? Kapatildi.");
    esp.println("AT+CIPCLOSE");
    return ip;
  }
  Serial.println("Baglant? Kapatildi.");
  esp.println("AT+CIPCLOSE");   
  return "";
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

