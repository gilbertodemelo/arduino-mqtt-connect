/* Connects sensor to a MQTT Broker and publishes messages */
#include <DHT.h>
#include <ArduinoMqttClient.h>
#include <WiFi101.h>
#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient client;
MqttClient mqttClient(client);

#define DHTPIN 2  // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11

/* Initialize DHT sensor */
DHT dht(DHTPIN, DHTTYPE);

/* MQTT */
const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic1[] = "humidity";

void setup() {
  Serial.begin(9600);

  Serial.println(F("DHT11 Test."));
  dht.begin();

  // connect to WiFi
     if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected to the network.");
   }

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection faile! Error code = ");
    Serial.println(mqttClient.connectError());

    while(1);
  }

  Serial.println("You're connect to the MQTT broker");
  Serial.println();


} // setup

void loop() {
  
  // poll() permite manter a conexão com o broker de MQTT 
  mqttClient.poll();

  // Leitura de temperatura ou umidade feita em aproximadamente
  // 250 milisegundos (0,001 segundos). Porém, algumas leituras
  // podem estar atrasadas em até 2 segundos (DHT11 é um modelo
  // mais antigo de sensor)
  float h = dht.readHumidity();
  // Temperatura é lida em Celsius (padrão)
  float t = dht.readTemperature();

  // Checar se alguma leitura falhou e tentar novamente
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("Sending message to topic: ");
  Serial.println(topic1);
  Serial.println(h);

  // Manda mensagem
  mqttClient.beginMessage(topic1);
  mqttClient.print(h);
  mqttClient.endMessage();

  Serial.println();

  // Serial.print(F("Humidity: "));
  // Serial.println(h);
  // Serial.print(F("Temperature: "));
  // Serial.print(t);
  // Serial.println("°C");

  // Esperar 2 segundos entre leituras;
  delay(2000);

} // loop
