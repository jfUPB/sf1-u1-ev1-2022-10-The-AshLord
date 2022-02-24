#include "SSD1306Wire.h"

#define BOMB_OUT 25//Led 1
#define LED_COUNT 26//Led 2
#define UP_BTN 13
#define DOWN_BTN 32
#define ARM_BTN 33

// Selecciona uno según tu display.
//SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

//Dividir el problema por estados (armado y desarmado)
//la exploción que sea un mensaje *Boom!* en pantalla
enum class BombStates {SETINGS, WAITING_PASSWORD, CHECKING_PASSWORD, TOTAL_ANIQUILATION};

boolean IsCorrectPassword = false;
boolean evBtns = false; //Nos avisa cuando el evento Btns está activo...
uint8_t evBtnsData = 0; //nos dece que botón se precinó
uint32_t countdownTime = 20000; //Tiempo d ela cuenta regresiva, por defoult es 20 seg

const uint32_t TimeLED_COUNT = 500; // 1Hz Tiempo Led contador
//Arreglo para almacenar la clave correcta y la clave del usuario:
String CorrectPassword[] = {"UP_BTN", "UP_BTN", "DOWN_BTN", "DOWN_BTN", "UP_BTN", "DOWN_BTN", "ARM_BTN"};
String UserPassword[] = { "", "", "", "", "", "", "",};


void setup() {

  pinMode(LED_COUNT, OUTPUT); //Decimos que son Leds
  pinMode(BOMB_OUT, OUTPUT);
  pinMode(UP_BTN, INPUT_PULLUP); //Decimos que son pulsadores
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(ARM_BTN, INPUT_PULLUP);
  Serial.begin(115200); //En caso de que necesitemos el puerto Serial

}

void desarmado() {
  //Inicia en estado de configuración:
  static BombStates bombStates =  BombStates::SETINGS;
  //Permite configurar el tiempo (entre 10-60 seg)

  if (evBtns == true) {

    evBtns = false; //Consumo el evento
    switch (evBtnsData) {
      case DOWN_BTN:
        //Aumenta 1 seg la cuenta regrestiva
        countdownTime = countdownTime + 1000;
        break;
      case UP_BTN:
        // disminuye 1 seg la cuenta regresiva
        countdownTime = countdownTime - 1000;
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
  static uint8_t led_countState = LOW;
  uint32_t currentMillis1 = millis();
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
    switch (evBtnsData) {
      case DOWN_BTN:

        break;
      case UP_BTN:

        break;
      case ARM_BTN:

        break;

    }
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

void bntsTask() {
  //Se determina si un botón se precionó, detectando caundo este es liberado(?)
  if (digitalRead(DOWN_BTN) == LOW) {
    evBtns = true;
    evBtnsData = DOWN_BTN;
    Serial.println("Boton Precionado:DOWN_BTN");
  }
  else if(digitalRead(UP_BTN) == LOW) {
      evBtns = true;
      evBtnsData = UP_BTN;
      Serial.println("Boton Precionado:UP_BTN");
    }
  else if(digitalRead(ARM_BTN) == LOW) {
      evBtns = true;
      evBtnsData = ARM_BTN;
      Serial.println("Boton Precionado:ARM_BTN");
    }
  else {
    evBtns == false;
  }
}

void loop() {
  //uint8_t *pCorrectPass = &CorrectPassword;
  //uint8_t *qUserPass = &UserPassword;

  bntsTask(); //me avisa que botón a sido pulsado

  desarmado();
  //El Pulsador ARM arma la bomba (cambia de estado)

  armado();
}
