



#include <LiquidCrystal.h>
#include <SoftwareSerial.h>



int buzzerPin = 8; // Buzzer pin



int R = 11; // Red pin of RGB led
int G = 12; // Green pin of RGB led
int B = 13; // Blue pin of RGB led



int nemDegeri; // Current readed humidity



int nemSensorPin = A0; // humidity sensor pin
#define RX 10
#define TX 9



String wifiName = "SOILHM";
String wifiPassword = "7e8r9GH6D2FF32T";
String deviceId = "7e8r9GH6D2FF32T";
String ipApi = "api.ipify.org";



#define VOLTAGE 1
#define DEBUG true



LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // initiliaze lcd screen
SoftwareSerial esp8266(RX, TX);



String response;
int countTrueCommand;
int countTimeCommand;
boolean found = false;



void loadColors();
void lightLed(int r, int g, int b);
void makeSound(int delayTime);
void stopSound(int delayTime);



void setup()



{
  // set up pins
  pinMode(nemSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  lcd.begin(16, 2); // start lcd screen
  lcd.print("Loading...");
  loadColors();
  Serial.begin(9600);
  esp8266.begin(115200);
  
  esp8266.println("AT");
  while (!esp8266.find("OK"))
  { // Mod?l haz?r olana kadar bekliyoruz.
    esp8266.println("AT");
		Serial.println("ESP8266 not found");
  }
Serial.println("Found module");
  esp8266.println("AT+CWMODE=1"); // ESP8266 mod?l?n? client olarak ayarl?yoruz.
  while (!esp8266.find("OK"))
  { // Ayar yap?lana kadar bekliyoruz.
    esp8266.println("AT+CWMODE=1");
Serial.println("setting up");
  }



  Serial.println("connecting");
  esp8266.println("AT+CWJAP=\"" + wifiName + "\",\"" + wifiPassword + "\""); // A??m?za ba?lan?yoruz.
  while (!esp8266.find("OK"))
    ;
  Serial.println("connected");
  delay(1000); // SSID va PASS




  // sendData("AT+CIPSTART=\"TCP\",\"52.20.78.240\",80", 5000, DEBUG);
  // String say = "GET http://api.ipify.org/ HTTP/1.0";
  // String sayCmd = "AT+CIPSEND=";
  // sayCmd += String(say.length());
  // sendData(sayCmd, 5000, DEBUG);
  // String result = sendData(say, 5000, DEBUG);
  // lcd.clear();
  // lcd.print(result);
  // sendData("AT+CIPCLOSE=5", 1000, DEBUG);



  //delay(2000);
}
void loop()
{
  nemDegeri = analogRead(nemSensorPin);
  nemDegeri = map(nemDegeri, 0, 1023, 0, 100); // map humidity to range of 0 100
  nemDegeri = 100 - nemDegeri;                 // map it to per cent
  lcd.clear();                                 // clear lcd screen
  char buffer[16];                             // create a buffer to write in a lcd screen
  sprintf(buffer, " %c %d", '%', nemDegeri);   // set up humidity percent
  lcd.print("Toprak Nem Yuzde");               // print header
  lcd.setCursor(0, 1);                         // go to new line
  lcd.print(buffer);                           // print humidity per cent
  if (nemDegeri > 60)
  {                        // if percent above 60 it means plant not needs any water
    lightLed(0, 255, 255); // red color
    makeSound(100);
  }



  else if (nemDegeri >= 50 && nemDegeri <= 60)
  {                        // %50-%60 is good for plants
    lightLed(255, 0, 255); // green color
    makeSound(1000);       // make a calmer sound
  }



  else
  {
    lightLed(255, 255, 0); // blue color
    stopSound(200);        // and not sound buzzer
  }
}



void stopSound(int delayTime)
{
  digitalWrite(buzzerPin, !VOLTAGE);
  delay(delayTime);
}



void lightLed(int r, int g, int b)
{
  analogWrite(R, r);
  analogWrite(G, g);
  analogWrite(B, b);
}



void makeSound(int delayTime)
{



  digitalWrite(buzzerPin, VOLTAGE);
  delay(delayTime);
  digitalWrite(buzzerPin, !VOLTAGE);
  delay(delayTime);
}



void loadColors()
{



  for (int r = 0; r <= 3; r++)
  {
    for (int g = 0; g <= 3; g++)
    {
      for (int b = 0; b <= 3; b++)
      {
        lightLed(r * 85, g * 85, b * 85);
        delay(30);
      }
    }
  }



  lightLed(0, 0, 0);
}



void ESP_init()
{
  esp8266.println("AT");
  while (!esp8266.find("OK"))
  { // Mod?l haz?r olana kadar bekliyoruz.
    esp8266.println("AT");
    lcd.clear();
    lcd.print("ESP8266 not found");
  }
  lcd.clear();
  lcd.print("Found module");
  esp8266.println("AT+CWMODE=1"); // ESP8266 mod?l?n? client olarak ayarl?yoruz.
  while (!esp8266.find("OK"))
  { // Ayar yap?lana kadar bekliyoruz.
    esp8266.println("AT+CWMODE=1");
    lcd.clear();
    lcd.print("setting up");
  }



  lcd.clear();
  lcd.print("connecting...");
  esp8266.println("AT+CWJAP=\"" + wifiName + "\",\"" + wifiPassword + "\""); // A??m?za ba?lan?yoruz.
  while (!esp8266.find("OK"))
    ;
  lcd.clear();
  lcd.print("connected");
  delay(1000); // SSID va PASS
}



String sendData(String cmd, int timeout, boolean debug)
{
  String response = "";
  cmd += "\r\n";
  esp8266.print(cmd);
  Serial.println();
  Serial.println(cmd);
  long int time = millis();
  while ((time + timeout) > millis())
  {
    response = esp8266.readString();
  }
  if (debug)
  {
    Serial.print(response + "\r\n");
  }
  return response;
}


