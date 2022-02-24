#include "SSD1306Wire.h"

#define BOMB_OUT 25//Led 1
#define LED_COUNT 26//Led 2
#define UP_BTN 13
#define DOWN_BTN 32
#define ARM_BTN 33

SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);

void bntsTask();
void bombTask();
void serialTask();

void setup() {
  serialTask();
  bntsTask(); //me avisa que botón a sido pulsado
  bombTask();
}


boolean evBtns = false; //Nos avisa cuando el evento Btns está activo...
uint8_t evBtnsData = 0; //nos dece que botón se presionó

void loop() {
  serialTask();
  bntsTask(); //me avisa que botón a sido pulsado
  bombTask();
}

void bntsTask() {
  enum class BntsStates {INIT, WAITING_PRESS, WAITING_STABLE, WAITING_RELEASE};
  static BntsStates bntsStates = BntsStates::INIT;
  static uint32_t referenceTime;
  const uint32_t STABLETIMEOUT = 100;

  switch (bntsStates) {
    case BntsStates::INIT: {
        pinMode(UP_BTN, INPUT_PULLUP); //Decimos que son pulsadores
        pinMode(DOWN_BTN, INPUT_PULLUP);
        pinMode(ARM_BTN, INPUT_PULLUP);
        bntsStates = BntsStates::WAITING_PRESS;
        break;
      }
    case BntsStates::WAITING_PRESS: {
        if (digitalRead(DOWN_BTN) == LOW) {
          referenceTime = millis();
          bntsStates = BntsStates::WAITING_STABLE;
        }
        break;
      }
    case BntsStates::WAITING_STABLE: {
        if (digitalRead(DOWN_BTN) == HIGH) {
          bntsStates = BntsStates::WAITING_PRESS;
        }
        else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
          bntsStates = BntsStates::WAITING_RELEASE;
        }
        break;
      }
    case BntsStates::WAITING_RELEASE: {
        if (digitalRead(DOWN_BTN) == HIGH) {
          evBtns = true;
          evBtnsData = DOWN_BTN;
          Serial.println("DOWN_BTN");
          bntsStates = BntsStates::WAITING_PRESS;
        }
        break;
      }
    default:
      break;

  }

  /*


               evBtns = true;
          evBtnsData = DOWN_BTN;
          Serial.println("Boton Precionado:DOWN_BTN");
    //Se determina si un botón se precionó, detectando caundo este es liberado(?)
    if (digitalRead(DOWN_BTN) == LOW) {
      evBtns = true;
      evBtnsData = DOWN_BTN;
      Serial.println("Boton Precionado:DOWN_BTN");
    }
    else if (digitalRead(UP_BTN) == LOW) {
      evBtns = true;
      evBtnsData = UP_BTN;
      Serial.println("Boton Precionado:UP_BTN");
    }
    else if (digitalRead(ARM_BTN) == LOW) {
      evBtns = true;
      evBtnsData = ARM_BTN;
      Serial.println("Boton Precionado:ARM_BTN");
    }
  */
}

void bombTask() {

  enum class BombStates {INIT, WAITING_CONFIG, COUNTING, BOOM};
  static BombStates bombStates = BombStates::INIT;
  static uint8_t counter;

  switch (bombStates) {
    case BombStates::INIT: {
        pinMode(LED_COUNT, OUTPUT);
        pinMode(BOMB_OUT, OUTPUT);
        display.init(); //Para usar la pantalla
        display.setContrast(255);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        counter = 20;

        display.clear();
        display.drawString(0, 5, String(counter));
        display.display();
        bombStates = BombStates::WAITING_CONFIG;
        break;
      }
    case BombStates::WAITING_CONFIG: {

        if (evBtns == true) {
          evBtns = false;

          if (evBtnsData == DOWN_BTN) {
            if (counter > 10) {
              counter--;
            }
            display.clear();
            display.drawString(0, 5, String(counter));
            display.display();
          }

        }
        break;
      }
    case BombStates::COUNTING: {
        break;
      }
    case BombStates::BOOM: {
        break;
      }
    default:
      break;

  }

}

void serialTask() {
  enum class SerialStates {INIT, READING_COMMANDS};
  static SerialStates serialStates = SerialStates::INIT;

  switch (serialStates) {
    case SerialStates::INIT: {
        Serial.begin(115200);
        serialStates = SerialStates::READING_COMMANDS;
        break;
      }
    case SerialStates::READING_COMMANDS: {

        if (Serial.available() > 0) {
          int dataIn = Serial.read();
          if (dataIn == 'd') {
            evBtns = true;
            evBtnsData = DOWN_BTN;
            Serial.println("DOWN_BTN");
          }
        }

        break;
      }
    default:
      break;

  }
}


/*
  //Dividir el problema por estados (armado y desarmado)
  enum class BombStates {SETINGS, WAITING_PASSWORD, CHECKING_PASSWORD, TOTAL_ANIQUILATION};

  boolean IsCorrectPassword = false;

  uint32_t countdownTime = 20000; //Tiempo d ela cuenta regresiva, por defoult es 20 seg
  const uint32_t TimeLED_COUNT = 500; // 1Hz Tiempo Led contador

  //Arreglo para almacenar la clave correcta y la clave del usuario:
  String CorrectPassword[] = {"UP_BTN", "UP_BTN", "DOWN_BTN", "DOWN_BTN", "UP_BTN", "DOWN_BTN", "ARM_BTN"};
  String UserPassword[] = { "", "", "", "", "", "", "",};



  Serial.begin(115200); //Para usar el puerto Serial


  void desarmado() {

  //Inicia en estado de configuración:
  static BombStates bombStates =  BombStates::SETINGS;

  //Permite configurar el tiempo (entre 10-60 seg)
  if (evBtns == true) {
    evBtns = false; //Consumo el evento
    //Segun el botón que presinó
    switch (evBtnsData) {
      case DOWN_BTN:
        //Aumenta 1 seg la cuenta regrestiva
        if (countdownTime < 60000) {
          countdownTime = countdownTime + 1000;
          //Imprime countdownTime en pantalla
        }
        break;
      case UP_BTN:
        // disminuye 1 seg la cuenta regresiva
        if (countdownTime > 20000) {
          countdownTime = countdownTime - 1000;
          //Imprime countdownTime en pantalla
        }
        break;
      case ARM_BTN:
        //arma la bomba
        bombStates = BombStates::WAITING_PASSWORD;
        Serial.println("BombStates::WAITING_PASSWORD");
        break;
    }

    //Mantiene el led Contador siempre encendido
    digitalWrite(LED_COUNT, HIGH);

  }
  }

  void armado() {

  static uint32_t previousMillis1 = 0;
  static uint32_t previousTMinus = 0;
  static uint8_t led_countState = LOW;
  uint32_t currentMillis1 = millis();
  uint32_t currentTMinus = millis();
  static BombStates bombStates =  BombStates::SETINGS;
  //Muestra cuenta regresiva en la pantalla

  //Enciende y apaga el led de armado
  if (currentMillis1 - previousMillis1 >= TimeLED_COUNT) {
    previousMillis1 = currentMillis1;
    if (led_countState == LOW) {
      led_countState = HIGH;
    } else {
      led_countState = LOW;
    }
  }
  digitalWrite(LED_COUNT, led_countState);

  if (evBtns == true) {
    evBtns = false; //Consumo el evento
    //aquí deberia registrar los botenes almacenados en el arrego
    //for

  }

  if (IsCorrectPassword = true) {
    //se pausa la cuenta atrás
    //vuelve al estado configuracion
    bombStates = BombStates::SETINGS;
  }
  else {
    //se da/continua la cuenta atrás
  }

  }

  void passwordCheck(uint8_t *pCorrectPass, uint8_t *qUserPass) {
  //Compara si CorrectPassword y UserPassword son iguales
  if (*pCorrectPass == *qUserPass) {
    IsCorrectPassword = true;
  }
  else {
    IsCorrectPassword = false;
  }
  //retorna IsCorrectPassword
  }
  void explotionTask() {
  //Solo se debería activar cuando la cuenta regresiva llegue a cero
  static BombStates bombStates =  BombStates::WAITING_PASSWORD;
  if ( countdownTime < 0) {

    digitalWrite(LED_COUNT, HIGH);
    Serial.println("Boom!! Fatal Aniquilation");
    delay(1000); //Porque si explotó no puede hacer nada

    //vuelve al estado configuracion
    bombStates = BombStates::SETINGS;


  }
  }



*/
