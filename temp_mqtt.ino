#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Configuración del sensor DHT11
#define DHTPIN 8          // Pin donde está conectado el sensor
#define DHTTYPE DHT11     // Tipo de sensor DHT11
DHT dht(DHTPIN, DHTTYPE);
#define Pecho 6
#define Ptrig 7

// Configuración de la red
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Dirección MAC del Ethernet Shield
IPAddress ip(192,168,1,88);
IPAddress server(192, 168, 1, 85); // IP del servidor MQTT

// Crear un cliente Ethernet y un cliente MQTT
EthernetClient ethClient;
PubSubClient client(ethClient);

int duracion, distancia;
// Credenciales de MQTT
//const char* mqttUser = "username";
//const char* mqttPassword = "password";

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Inicializar Ethernet
  Ethernet.begin(mac, ip);
  
  // Conectar al servidor MQTT
  client.setServer(server, 1883);

  // Esperar a que se asigne una IP por DHCP
  delay(1000);
  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

  // Conectar al servidor MQTT con credenciales
  if (client.connect("arduinoClient")) {
    Serial.println("Connected to MQTT server");
  } else {
    Serial.println("Failed to connect to MQTT server");
  }

  pinMode(Pecho, INPUT);
  pinMode(Ptrig, OUTPUT);
  digitalWrite(Ptrig, LOW);
}

void loop() {
  if (!client.connected()) {
    // Reintentar la conexión si se pierde
    reconnect();
  }
  client.loop();

  // Leer los datos del sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Comprobar si la lectura es válida
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  digitalWrite(Ptrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Ptrig, LOW);
  duracion = pulseIn(Pecho, HIGH);
  distancia = duracion/59;

  // Publicar los datos en el servidor MQTT
  // Cambia los tópicos aquí si es necesario
  char tempStr[8];
  char humStr[8];
  char disStr[8];
  dtostrf(t, 6, 2, tempStr);
  dtostrf(h, 6, 2, humStr);
  dtostrf(distancia, 6, 2, disStr);

  // Crear el objeto JSON
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["temperatura"] = t;
  jsonDoc["humedad"] = h;
  jsonDoc["distancia"] = distancia;
  jsonDoc["status"] = "OK";

  // Convertir el objeto JSON a string
  char buffer[256];
  serializeJson(jsonDoc, buffer);

  client.publish("testTopic", buffer);

  // Publicar temperatura en el tópico "sensor/temperature"
  //client.publish("testTopic", tempStr);

  // Publicar humedad en el tópico "sensor/humidity"
  //client.publish("testTopic", humStr);

  // Esperar 10 segundos antes de la próxima lectura
  delay(10000);
}

void reconnect() {
  // Bucle hasta que el cliente MQTT se reconecte
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Esperar 5 segundos antes de reintentar
      delay(5000);
    }
  }
}