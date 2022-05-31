#include <LiquidCrystal.h>
#include "WiFiEsp.h"
#include "secrets.h"
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif
#define VOLTAGE 1

int buzzerPin = 8; // Buzzer pin
int R = 12;        // Red pin of RGB led
int G = 9;         // Green pin of RGB led
int B = 10;        // Blue pin of RGB led
int humidity;      // Current readed humidity
int fa28Pin = A0;  // humidity sensor pin

int status = WL_IDLE_STATUS; // the Wifi radio's status

// Initialize the Ethernet client object
WiFiEspClient client;
LiquidCrystal lcd(13, 11, 5, 4, 3, 2); // initiliaze lcd screen

void loadColors();
void lightLed(int r, int g, int b);
void makeSound(int delayTime);
void stopSound(int delayTime);

void setup()
{

  Serial.begin(115200);
  Serial1.begin(115200);
  WiFi.init(&Serial1);

  pinMode(fa28Pin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  lcd.begin(16, 2); // start lcd screen
  lcd.print("Loading...");

  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true)
      ;
  }
  Serial.println(WiFi.SSID());
  // attempt to connect to WiFi network
  while (status != WL_CONNECTED)
  {
    delay(500);
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);
  }

  delay(5000);

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  loadColors();
}
void loop()
{
  humidity = analogRead(fa28Pin);
  humidity = map(humidity, 0, 1023, 0, 100);    // map humidity to range of 0 100
  humidity = 100 - humidity;                    // map  it to per cent
  lcd.clear();                                  // clear lcd screen
  char buffer[16];                              // create a buffer to write in a lcd screen
  sprintf(buffer, "     %c %d", '%', humidity); // set up humidity percent
  lcd.print("Soil Humidity");                   // print header
  lcd.setCursor(0, 1);                          // go to new line
  lcd.print(buffer);                            // print humidity per cent
  if (humidity > 60)
  { // if percent above 60 it means plant not needs any water
    lightLed(0, 255, 255); // red color
    makeSound(100);
  }
  else if (humidity >= 50 && humidity <= 60)
  { // %50-%60 is good for plants
    lightLed(255, 0, 255); // green color
    makeSound(1000);       // make a calmer sound
  }
  else
  {
    lightLed(255, 255, 0); // blue color
    stopSound(200);        // and not sound buzzer
  }

  // if you get a connection, report back via serial
  if (client.connect(SERVER, 80))
  {
    char buffer[102];                              // create a buffer to write in a lcd screen
    sprintf(buffer, "GET /measurements/%s?humidity=%f&ip=%s HTTP/1.1", DEVICE_ID, humidity, IP);
    Serial.println("Connected to server");
    client.println(buffer);
    client.println("Host: 13.38.214.178");
    client.println("Accept: */*");
    client.println();
  }
  delay(2000);
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
