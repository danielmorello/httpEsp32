#include "main.h"

void printDataOnSerial()
{
  Serial.print("Origin: ");
  Serial.println(message.origin);
  Serial.print("MAC: ");
  for (int i = 0; i < 6; i++)
  {
    Serial.printf("%02X:", message.macOrigin[i]);
  }
  Serial.println();
  Serial.print("Micros: ");
  Serial.println(message.timeMicros);
  Serial.print("Temp: ");
  Serial.println(message.coreTemp);
  Serial.print("Type: ");
  Serial.println(message.type);
  Serial.print("Experiment: ");
  Serial.println(message.experiment);
  Serial.print("Position: ");
  Serial.println(message.position);
  Serial.println("=========================");
}

void requestEvent() {
  
}

void receiveEvent(int howMany) {
  printDataOnSerial();
  if(howMany == sizeof(Message)) {
    Wire.readBytes((char *)&message, sizeof(message));
    JsonDocument doc;
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
            message.macOrigin[0], message.macOrigin[1], message.macOrigin[2],
            message.macOrigin[3], message.macOrigin[4], message.macOrigin[5]);
    doc["origin"] = message.origin;
    doc["macOrigin"] = macStr;
    doc["timeMicros"] = message.timeMicros;
    doc["coreTemp"] = message.coreTemp;
    doc["type"] = message.type;
    doc["experiment"] = message.experiment;
    doc["position"] = message.position;
    String jsonStr;
    serializeJson(doc, jsonStr);
    int httpResponseCode = http.POST(jsonStr);
    if(httpResponseCode > 0){
      String response = http.getString(); 
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Erro no código: ");
      Serial.println(httpResponseCode);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
  if( WiFi.status()== WL_CONNECTED){
    http.begin("http://192.168.68.110:3333/message");
    http.addHeader("Content-Type", "application/json");
  } else {
    Serial.println("Erro na conexão WiFi");
  }

  Wire.begin(8, SDA_PIN_SLAVE, SCL_PIN_SLAVE, 100000);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  Wire1.begin(22, 23); // Inicia o Wire1 library nos pinos 23 e 22
  // Configura a data e hora atual
  // Segundos, Minutos, Horas, Dia da semana, Dia do mês, Mês, Ano
  // Exemplo: 00 segundos, 34 minutos, 12 horas (formato 24h), 2 (Terça-feira), 10 (dia), 4 (Abril), 23 (2023)
  setDateTime(0, 31, 17, 3, 17, 4, 23);
}

void loop() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDateTime(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  // Imprime a data e horapr
  Serial.print("Date: ");
  Serial.print(dayOfMonth);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" Day of week: ");
  Serial.print(dayOfWeek);
  Serial.print(" Time: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

  delay(1000); // Aguarde um segundo
}
