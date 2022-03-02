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
  static uint8_t pushBtn; //Me permite saber que botón fue presionado

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
          pushBtn = DOWN_BTN;
          referenceTime = millis();
          bntsStates = BntsStates::WAITING_STABLE;
        }
        else if (digitalRead(UP_BTN) == LOW) {
          pushBtn = UP_BTN;
          referenceTime = millis();
          bntsStates = BntsStates::WAITING_STABLE;
        }
        else if (digitalRead(ARM_BTN) == LOW) {
          pushBtn = ARM_BTN;
          referenceTime = millis();
          bntsStates = BntsStates::WAITING_STABLE;
        }

        break;
      }
    case BntsStates::WAITING_STABLE: {

        if (pushBtn == DOWN_BTN) {
          if (digitalRead(DOWN_BTN) == HIGH) {
            bntsStates = BntsStates::WAITING_PRESS;
          }
          else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
            bntsStates = BntsStates::WAITING_RELEASE;
          }
        }
        else if (pushBtn == UP_BTN) {
          if (digitalRead(UP_BTN) == HIGH) {
            bntsStates = BntsStates::WAITING_PRESS;
          }
          else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
            bntsStates = BntsStates::WAITING_RELEASE;
          }
        }
        else if (pushBtn == ARM_BTN) {
          if (digitalRead(ARM_BTN) == HIGH) {
            bntsStates = BntsStates::WAITING_PRESS;
          }
          else if ( (millis() - referenceTime) >= STABLETIMEOUT) {
            bntsStates = BntsStates::WAITING_RELEASE;
          }
        }

        break;
      }
    case BntsStates::WAITING_RELEASE: {

        if (pushBtn == DOWN_BTN) {
          if (digitalRead(DOWN_BTN) == HIGH) {
            evBtns = true;
            evBtnsData = DOWN_BTN;
            Serial.println("DOWN_BTN");
            bntsStates = BntsStates::WAITING_PRESS;
          }
        }
        else if (pushBtn == UP_BTN) {
          if (digitalRead(UP_BTN) == HIGH) {
            evBtns = true;
            evBtnsData = UP_BTN;
            Serial.println("UP_BTN");
            bntsStates = BntsStates::WAITING_PRESS;
          }
        }
        else if (pushBtn == ARM_BTN) {
          if (digitalRead(ARM_BTN) == HIGH) {
            evBtns = true;
            evBtnsData = ARM_BTN;
            Serial.println("ARM_BTN");
            bntsStates = BntsStates::WAITING_PRESS;
          }
        }

        break;
      }
    default:
      Serial.println("ERROR.");
      break;

  }
}

void bombTask() {

  enum class BombStates {INIT, WAITING_CONFIG, COUNTING, BOOM};
  static BombStates bombStates = BombStates::INIT;
  static uint8_t counter = 20;

  switch (bombStates) {
    case BombStates::INIT: {
        pinMode(LED_COUNT, OUTPUT);
        pinMode(BOMB_OUT, OUTPUT);
        display.init(); //Para usar la pantalla
        display.setContrast(255);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);

        digitalWrite(BOMB_OUT, LOW);
        counter = 20; //Tiempo para la Cuenta regresiva (20 seg)
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
              counter--; //Disminue el timer 1 seg
            }
            display.clear();
            display.drawString(0, 5, String(counter));
            display.display();
          }
          else if (evBtnsData == UP_BTN) {
            if (counter < 60) {
              counter++; //Aumenta el timer 1 seg
            }
            display.clear();
            display.drawString(0, 5, String(counter));
            display.display();
          }
          else if (evBtnsData == ARM_BTN) {

            Serial.println("BombStates::COUNTING");
            Serial.println("La Bomba ha sido armado con Exito");
            display.clear();
            display.drawString(0, 5, String("Armado Exitoso"));
            display.display();
            bombStates = BombStates::COUNTING;
          }
        }
        //Mantiene el led Contador siempre encendido
        digitalWrite(LED_COUNT, HIGH);
        break;
      }
    case BombStates::COUNTING: {

        const uint32_t TimeLED_COUNT = 500; // 1Hz Tiempo Led contador
        static uint32_t previousTMinus = 0;
        static uint8_t led_countState = LOW;
        uint32_t currentTMinus = millis();

        boolean IsCorrectPassword = false;
        const uint8_t passLength = 7; //Tamaño maximo del arreglo
        //Arreglo para almacenar la clave correcta y la clave del usuario:
        static uint8_t CorrectPassword[passLength] = {UP_BTN, UP_BTN, DOWN_BTN, DOWN_BTN, UP_BTN, DOWN_BTN, ARM_BTN};
        static uint8_t UserPassword[passLength];
        static uint8_t passInput = 0; //3 de Botones presionados

        //Enciende y apaga el led de armado
        if (currentTMinus - previousTMinus >= TimeLED_COUNT) {
          previousTMinus = currentTMinus;
          if (led_countState == LOW) {
            led_countState = HIGH;
          } else {
            led_countState = LOW;
            counter--; //Convenientemente actualiza la cuenta atrás cada 1 seg
            display.clear();
            display.drawString(0, 5, String(counter));
            display.display();

          }
          digitalWrite(LED_COUNT, led_countState);
          //Activa la bomba cuando la cuenta atrás llegue a cero
          if (counter == 0) {
            bombStates = BombStates::BOOM;
          }
        }

        //Ingreso de la clave:
        if (evBtns == true) {
          evBtns = false; //Consumo el evento
          //Agrega el botón presionado al arreglo
          if (passInput < passLength) {
            if (evBtnsData == UP_BTN) {
              UserPassword[passInput] = UP_BTN;
              Serial.println("pass Input: Up");
            }
            else if (evBtnsData == DOWN_BTN) {
              UserPassword[passInput] = DOWN_BTN;
              Serial.println("pass Input: Down");
            }
            else if (evBtnsData == ARM_BTN) {
              UserPassword[passInput] = ARM_BTN;
              Serial.println("pass Input: Arm");
            }
            passInput++;
          }
        }
        else if (passInput == passLength) {
          Serial.println("UserPassword lleno.");
          PasswordCheck (UserPassword, CorrectPassword, passLength, &IsCorrectPassword);
          if (IsCorrectPassword == true) {

            Serial.println("Correct password");
            display.clear();
            display.drawString(0, 5, String("Disarm"));
            display.display();
            for (uint8_t k = 0; k < passLength; k++)
            {
              UserPassword[k] = 0;
            }
            delay(3500);
            bombStates = BombStates::INIT;
          }
          else {
            Serial.println("Contraseña incorrecta");
            passInput = 0;
            for (uint8_t j = 0; j < passLength; j++) {
              UserPassword[j] = 0;
            }
            delay(300);
          }
        }
      }
      break;
    case BombStates::BOOM: {

        digitalWrite(LED_COUNT, LOW); //Apagamos el Led contador
        digitalWrite(BOMB_OUT, HIGH); //Encendemos el Led de explosión
        Serial.println("BOOM! Fatal ANIQUILATION!!"); //Informamos por el Serial
        display.clear();
        display.drawString(9, 0, "BOOM!"); //Imprimimos en pantalla
        display.display();
        delay(3500); //Tiempo que "dura la explosión"
        bombStates = BombStates::INIT; //Volvemos al seudo-estado de inicio

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
          else if (dataIn == 'u') {
            evBtns = true;
            evBtnsData = UP_BTN;
            Serial.println("UP_BTN");
          }
          else if (dataIn == 'a') {
            evBtns = true;
            evBtnsData = ARM_BTN;
            Serial.println("ARM_BTN");
          }
        }

        break;
      }
    default:
      break;
  }
}

void PasswordCheck(uint8_t *UserPass, uint8_t *CorrectPass, uint8_t maxLength, bool *IsCorrect) {

  for (uint8_t i = 0; i < maxLength; i++) {
    //Verifica si UserPassword es igual a Password
    if (CorrectPass[i] == UserPass[i]) {
      *IsCorrect = true;
    }
    else {
      //Si en algún valor son diferentes:
      *IsCorrect = false; //cambia IsCorrIsCorrectPassword
      break;              //y Rompe el ciclo for
    }
  }
}
