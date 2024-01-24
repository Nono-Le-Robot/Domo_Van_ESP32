#include "DHT.h"
#include <WiFi.h>
#include <WebServer.h>

#define DHTTYPE DHT22
const int DHTPIN = 21;

const int RED_LED = 22;
const int WHITE_LED = 23;

const int TRIG_PIN = 12;
const int ECHO_PIN = 14;

bool redState = false;
bool whiteState = false;

float temperature = 0;
float humidity = 0;

float distance = 0;

const char* ssid = "Vmax";
const char* password = "";

WebServer server(80);

DHT dht(DHTPIN, DHTTYPE);

void handleRoot(){
  String page = "<!DOCTYPE html>";
  page += "<html lang=\"en\">";
  page += "<head>";
  page += "<meta charset=\"UTF-8\">";
  page += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  page += "<title>TEST</title>";
  page += "</head>";
  page += "<body>";
  page += "<br/>";
  page += "<h1>RED LED :</h1>";
  page += "<br/>";
  page += "<br/>";
  page += "<a href='/red-on'>ON</a>";
  page += "<br/>";
  page += "<br/>";
  page += "<br/>";
  page += "<a href='/red-off'>OFF</a>";
  page += "<br/>";
  page += "<br/>";
  page += "<p>RED-LED = "; page += redState; page += "</p>";
  page += "<br/>";
  page += "<br/>";
  page += "<br/>";
  page += "<br/>";
  page += "<h1>WHITE LED :</h1>";
  page += "<br/>";
  page += "<br/>";
  page += "<a href='/white-on'>ON</a>";
  page += "<br/>";
  page += "<br/>";
  page += "<br/>";
  page += "<a href='/white-off'>OFF</a>";
  page += "<br/>";
  page += "<br/>";
  page += "<p>WHITE-LED = "; page += whiteState; page += "</p>";
  page += "<br/>";
  page += "<br/>";
  page += "<br/>";
  page += "<br/>";
  page += "<p>TEMPERATURE = "; page += temperature; page += " - C</p>";
  page += "<p>HUMIDITY = "; page += humidity; page += " %</p>";
  page += "<p>ULTRASON DISTANCE = "; page += distance; page += " cm</p>";
  page += "<br/>";
  page += "<br/>";
  page += "<a href='/get-all-data'>Update</a>";
  page += "<br/>";
  page += "<br/>";
  page += "</body>";
  page += "</html>";

  server.setContentLength(page.length());
  server.send(200, "text/html", page);
}

// RED LED
void handleRedOn(){
  redState = true;
  digitalWrite(RED_LED, redState);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleRedOff(){
  redState = false;
  digitalWrite(RED_LED, redState);
    server.sendHeader("Location", "/");
  server.send(303);
}

// WHITE LED
void handleWhiteOn(){
  whiteState = true;
  digitalWrite(WHITE_LED, whiteState);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleWhiteOff(){
  whiteState = false;
  digitalWrite(WHITE_LED, whiteState);
    server.sendHeader("Location", "/");
  server.send(303);
}

// GET DATA
void getTemperatureAndHumidity(){
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Echec reception"));
    server.send(404);
    return;
  }
  else{
    server.sendHeader("Location", "/");
    server.send(303);
  }
}

void getDistance(){
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration * 0.0343) / 2;
  server.sendHeader("Location", "/");
  server.send(303);
}

void getAllData(){
  getTemperatureAndHumidity();
  getDistance();
}

// ERROR

void handleNotFound(){
  server.send(404, "text/plain","404: Not found!");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  pinMode(RED_LED, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.print("\n\n");
  WiFi.persistent(false);
  WiFi.begin(ssid, password);
  Serial.print("Tentative de connexion...");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }

  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  
  server.on("/", handleRoot);

  server.on("/red-on", handleRedOn);
  server.on("/red-off", handleRedOff);

  server.on("/white-on", handleWhiteOn);
  server.on("/white-off", handleWhiteOff);

  server.on("/get-distance", getDistance);
  server.on("/get-temperature-humidity", getTemperatureAndHumidity);
  server.on("/get-all-data", getAllData);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Serveur web actif! ");
}

void loop() {
  server.handleClient();
}
