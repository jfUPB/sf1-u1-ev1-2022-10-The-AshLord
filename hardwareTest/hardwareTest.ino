#include "SSD1306Wire.h"

#define LED1 25
#define LED2 26
#define LED3 27
#define LED4 14
#define BUTTON1 13
#define BUTTON2 32
#define BUTTON3 33

//0.66: 64*48 otros: 128*32

// Selecciona uno según tu display.
//SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48); 


void setup() {
  Serial.begin(115200);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  display.init();
  display.setContrast(255);
  display.clear();
}

void loop() {
  static uint8_t counter = 0;
  static uint32_t oldTime = 0;
  uint32_t newTime;
  static int x=10;
  static int y=20;

  if (Serial.available() > 0) {
    Serial.print("Hello from ESP32!\n");
    int dataRx = Serial.read();
    if (dataRx == '1') digitalWrite(LED1, HIGH);
    else if (dataRx == '2') digitalWrite(LED2, HIGH);
    else if (dataRx == '3') digitalWrite(LED3, HIGH);
    else if (dataRx == '4') digitalWrite(LED4, HIGH);
    else if (dataRx == '5') {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
      digitalWrite(LED4, LOW);
    }
    else if(dataRx == 'r'){
      Serial.print(digitalRead(BUTTON1));
      Serial.print(',');
      Serial.print(digitalRead(BUTTON2));
      Serial.print(',');
      Serial.print(digitalRead(BUTTON3));
      Serial.print('\n');
    }
    else if(dataRx == 'd'){
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.clear();
        //0.66: 10,20 otros 0,5
        display.drawString(x, y, String(counter));  
        display.display();
        counter = (counter + 1) % 20;
    }
    else if(dataRx == 'c'){
       String xy = Serial.readStringUntil('*');
       int indexOfComma = xy.indexOf(',');
       x = xy.substring(0,indexOfComma).toInt();
       y = xy.substring(indexOfComma+1).toInt();
    }
  }
}
