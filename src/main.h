#include <Arduino.h>
#include <Wire.h>
#include <CircularBuffer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SDA_PIN_SLAVE 18
#define SCL_PIN_SLAVE 19
#define SDA_PIN_MASTER 22
#define SCL_PIN_MASTER 23

const char* ssid     = "MORELLO";
const char* password = "997017725";
HTTPClient http;

typedef struct Message {
  int origin;
  uint8_t macOrigin[6];
  unsigned long timeMicros;
  float coreTemp;
  int type;
  unsigned long experiment;
  int position;
} Message;

Message message;

int iniId = 1;

CircularBuffer<Message, 100> queue;

// Endereço I2C do DS3231
#define DS3231_ADDRESS 0x68

// Função para converter BCD para decimal
byte bcdToDec(byte val) {
  return (val / 16 * 10) + (val % 16);
}

// Função para converter de decimal para BCD
byte decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}

// Função para configurar a data e hora
void setDateTime(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
  Wire1.beginTransmission(DS3231_ADDRESS);
  Wire1.write(0); // Configura o ponteiro de registro para 00h
  Wire1.write(decToBcd(second));     // Configura os segundos
  Wire1.write(decToBcd(minute));     // Configura os minutos
  Wire1.write(decToBcd(hour));       // Configura as horas
  Wire1.write(decToBcd(dayOfWeek));  // Configura o dia da semana
  Wire1.write(decToBcd(dayOfMonth)); // Configura o dia do mês
  Wire1.write(decToBcd(month));      // Configura o mês
  Wire1.write(decToBcd(year));       // Configura o ano
  Wire1.endTransmission();
}

// Função para ler a hora e a data do DS3231
void readDateTime(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
  Wire1.beginTransmission(DS3231_ADDRESS);
  Wire1.write(0); // Configura o ponteiro de registro para 00h
  Wire1.endTransmission();
  Wire1.requestFrom(DS3231_ADDRESS, 7); // Solicita 7 bytes a partir de 00h

  *second = bcdToDec(Wire1.read() & 0x7F);
  *minute = bcdToDec(Wire1.read());
  *hour = bcdToDec(Wire1.read() & 0x3F);
  *dayOfWeek = bcdToDec(Wire1.read());
  *dayOfMonth = bcdToDec(Wire1.read());
  *month = bcdToDec(Wire1.read());
  *year = bcdToDec(Wire1.read());
}