#include "SSD1306Wire.h"

#define BOMB_OUT 25  //Led 1
#define LED_COUNT 26 //Led 2
#define UP_BTN 13    //Pulsador Izquierdo
#define DOWN_BTN 32  //Pulsador del centro
#define ARM_BTN 33   //Pulsador Derecho

// Selecciona uno según tu display.
//SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);
//Dividir el problema por estados (armado y desarmado)

//la exploción que sea un mensaje *Boom!* en pantalla
enum class BombStates {SETINGS, WAITING_PASSWORD, CHECKING_PASSWORD,TOTAL_ANIQUILATION};

boolean IsCorrectPassword = false;
const uint32_t TimeLED_COUNT = 500; // 1Hz Tiempo Led contador
//Arreglo para almacenar la clave correcta y la clave del usuario:
String CorrectPassword[] = {"UP_BTN","UP_BTN","DOWN_BTN","DOWN_BTN","UP_BTN","DOWN_BTN","ARM_BTN"};
String UserPassword[] = { "", "", "", "","","","",};


void setup() {

  pinMode(LED_COUNT, OUTPUT); //Decimos que es un Led
  pinMode(BOMB_OUT, OUTPUT);
  pinMode(UP_BTN, INPUT_PULLUP); //Decimos que son pulsadores
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(ARM_BTN, INPUT_PULLUP);
  Serial.begin(115200); //En caso d eque necesitemos el puerto Serial

}

void desarmado() {
  //Permite configurar el tiempo (entre 10-60 seg)
  // Con UP_BTN y DOWN_BTN aumentar y disminuir 1seg

  //Mantiene el led Contador siempre encendido
  digitalWrite(LED_COUNT, HIGH);


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

  digitalWrite(LED_COUNT, led_countState); T
}

void passwordCheck(uint8_t *pCorrectPass, uint8_t *qUserPass){
  //Compara si CorrectPassword y UserPassword son iguales
  if (*pCorrectPass ==*qUserPass){
    IsCorrectPassword = true;
  }
  else{
    IsCorrectPassword = false;
  }
  //retorna IsCorrectPassword
}
void loop() {
  //uint8_t *pCorrectPass = &CorrectPassword;
  //uint8_t *qUserPass = &UserPassword;
  
  
  desarmado();
  //El Pulsador ARM arma la bomba (cambia de estado)
  
  armado();
}
