#include <SparkFun_ADS1015_Arduino_Library.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//keypad includes
#include <Wire.h>
#include "SparkFun_Qwiic_Keypad_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_keypad
KEYPAD keypad1; //Create instance of this object

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED1 A0
#define LED2 A1
#define LED3 A2
#define LED4 A3
ADS1015 pinkySensor;
ADS1015 indexSensor;
uint16_t hand[4] = {0, 0, 0, 0};

byte Length = 0;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  //LAD VÆRE MED AT PILLE VED DET UNDER TAK

  if (keypad1.begin() == false)   // Note, using begin() like this will use default I2C address, 0x4B.
    // You can pass begin() a different address like so: keypad1.begin(Wire, 0x4A).
  {
    Serial.println("Keypad does not appear to be connected. Please check wiring. Freezing...");
    while (1);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.setRotation(2);


  if (pinkySensor.begin(Wire, 100000, ADS1015_ADDRESS_SDA) == false) {
    Serial.println("Pinky not found. Check wiring.");
    while (1);
  }
  if (indexSensor.begin(Wire, 100000, ADS1015_ADDRESS_GND) == false) {
    Serial.println("Index not found. Check wiring.");
    while (1);
  }
  
}

void loop() {
  keypad1.updateFIFO();  // necessary for keypad to pull button from stack to readable register
  char button = keypad1.getButton();
  char button2 = keypad1.getButton();
  display.setCursor(0, 10);
  display.clearDisplay();
  display.print("* for LED");
  display.setCursor(0, 20);
  display.print("# for AUTO");
  display.display();
  delay(200);

  while (button2 == '*'){
    loop1();
  }
  while (button2 =='#'){
    loop2();
  }
}

void loop1() {
  keypad1.updateFIFO();  // necessary for keypad to pull button from stack to readable register
  char button = keypad1.getButton();
  char button2 = keypad1.getButton();

    display.setCursor(0, 10);
    display.clearDisplay();
  if (button != -1)
  {
    if ((button >= '0') && (button <= '9'))
    {
      Length = Length * 10;
      Length = Length + button - '0';
    }
    if (button == '#')
    {
      Serial.print(Length);
      Length = 0;
    }
  }

  if (Length == 2 or Length == 3 or Length == 6 or Length == 7 or Length == 10 or Length == 11 or Length == 14 or Length == 15) {
    digitalWrite(LED3, HIGH);
  }

  if (Length == 4 or Length == 5 or Length == 6 or Length == 7 or Length == 12 or Length == 13 or Length == 14 or Length == 15) {
    digitalWrite(LED1, HIGH);
  }

  if (Length % 2 != 0) {
    digitalWrite(LED4, HIGH);
  }

  if (Length >= 8) {
    digitalWrite(LED2, HIGH);
  }
  if (Length == 10 or Length == 12 or Length == 14) {
    digitalWrite(LED4, LOW);
  }

  if (button == '#') {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
  }
  display.print(Length);
  display.setCursor(0, 20);
  display.print("* to Return");
  display.display();
  delay(200);

  while (button == '*'){
    loop();
  }

}

void loop2() {
      keypad1.updateFIFO();  // necessary for keypad to pull button from stack to readable register
  char button = keypad1.getButton();
  char button2 = keypad1.getButton();

    display.setCursor(0, 10);
    display.clearDisplay();
    uint16_t data;
    int sum = 0;
    for (int finger = 0; finger < 2; finger++) {
      hand[finger] = pinkySensor.getAnalogData(finger);
      hand[finger + 2] = indexSensor.getAnalogData(finger);
    }
    for (int finger = 0; finger < 4; finger++)
    {
      if (hand[finger] >= 900) {
        sum = sum + pow(2, finger);
      }

    }
   display.print(sum);
   display.setCursor(0, 20);
   display.print("* to Return");
   display.display();
   delay(200);

    while (button == '*'){
    loop();
  }
  
}
