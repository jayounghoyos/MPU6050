#include <Wire.h>
#include <LiquidCrystal.h>
#include "Simple_MPU6050.h"

#define MPU6050_ADDRESS_AD0_LOW 0x68
Simple_MPU6050 mpu;
#define OFFSETS -5114, 484, 1030, 46, -14, 6

// Configurar el pin de la luz de fondo
const int backlightPin = 2;

LiquidCrystal lcd(12, 11, 5, 4, 3, 10); // Asume que has cambiado D7 al pin 10

void mostrar_valores(int16_t *gyro, int16_t *accel, int32_t *quat, uint32_t *timestamp) {
  Quaternion q;
  VectorFloat gravity;
  float ypr[3] = {0, 0, 0};
  float xyz[3] = {0, 0, 0};
  mpu.GetQuaternion(&q, quat);
  mpu.GetGravity(&gravity, &q);
  mpu.GetYawPitchRoll(ypr, &q, &gravity);
  mpu.ConvertToDegrees(ypr, xyz);
  
  // Actualiza la pantalla LCD con los nuevos valores
  lcd.clear(); // Limpia la pantalla antes de cada actualización
  lcd.setCursor(0, 0); // Primera fila
  lcd.print("Yaw: ");
  lcd.print(xyz[0], 2);
  lcd.setCursor(0, 1); // Segunda fila
  lcd.print("Pitch: ");
  lcd.print(xyz[1], 2);
}

void setup() {
  pinMode(backlightPin, OUTPUT); // Configura el pin de la luz de fondo como salida
  lcd.begin(16, 2); // Inicializa la pantalla LCD con el número de columnas y filas
  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("Inicio:"));
  mpu.SetAddress(MPU6050_ADDRESS_AD0_LOW).load_DMP_Image(OFFSETS);
  mpu.on_FIFO(mostrar_valores);
}

unsigned long lastToggleTime = 0;
const long toggleInterval = 150; // Intervalo para encender/apagar la luz de fondo en milisegundos

void loop() {
  mpu.dmp_read_fifo();
  
  // Encender/apagar la luz de fondo cada 1000 ms
  if (millis() - lastToggleTime >= toggleInterval) {
    digitalWrite(backlightPin, !digitalRead(backlightPin)); // Cambia el estado de la luz de fondo
    lastToggleTime = millis();
  }
}