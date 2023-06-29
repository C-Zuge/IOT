//Import libs
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Define pinout for each element
#define sens_temp 34
#define filtro 32
#define oxigenador 33
#define aquecedor 25

//Define MQTT Relative Path
#define Temp1 "iot/cesariana/projeto-maki/2022/temperatura_1"
#define Temp2 "iot/cesariana/projeto-maki/2022/temperatura_2"
#define cmd_filtro "iot/cesariana/projeto-maki/2022/filtro"
#define cmd_oxigenador "iot/cesariana/projeto-maki/2022/oxigenador"
#define cmd_aquecedor "iot/cesariana/projeto-maki/2022/aquecedor"

// Start up variables
const char* ssid = "JG164";
const char* pwd = "facafaca";
const char* broker = "broker.hivemq.com";
const int port = 1883;
int attempt = 0;
int ndispositivos = 0;
float temperatura = 0.0;
String filtroStatus = "waiting...";
String oxigenadorStatus = "waiting...";
String aquecedorStatus = "waiting...";

//Instantiating Objects

OneWire oneWire(sens_temp);
DallasTemperature sensors_temp(&oneWire);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  
  pinMode(filtro, OUTPUT);
  pinMode(oxigenador,OUTPUT);
  pinMode(aquecedor,OUTPUT);
  
  sensors_temp.begin();
  ndispositivos = sensors_temp.getDeviceCount();
  
  init_Wifi();

  client.setServer(broker, port);
  client.setCallback(Callback);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    init_Wifi();
  }
  if (!client.connected()) {
    Reconnect_MQTT();
  }
  
  getTemperatura(ndispositivos,sensors_temp);
  client.subscribe(cmd_filtro);
  client.subscribe(cmd_oxigenador);
  client.subscribe(cmd_aquecedor);

  if(filtroStatus=="0") Command_OFF(filtro,"filtro");
  if(filtroStatus=="1") Command_ON(filtro,"filtro");
  if(oxigenadorStatus=="0") Command_OFF(oxigenador,"oxigenador");
  if(oxigenadorStatus=="1") Command_ON(oxigenador,"oxigenador");
  if(aquecedorStatus=="0") Command_OFF(aquecedor,"aquecedor");
  if(aquecedorStatus=="1") Command_ON(aquecedor,"aquecedor");
  
  client.loop();
}

//Deve-se melhorar função para armazenar n valores de n sensores em um vetor onde a posição do vetor é o numero do sensor
void getTemperatura(int ndispositivos, DallasTemperature sensors){
  Serial.print("Numero de sensores: ");
  Serial.println(ndispositivos);
  
  sensors.requestTemperatures();

  Serial.println("Temperatura: ");
  for (int i=0; i<ndispositivos; i++){
      temperatura = sensors.getTempCByIndex(i);
      Serial.print("Sensor ");
      Serial.print(i+1);
      Serial.print(": ");
      Serial.println(temperatura);
  }
}

void Command_ON(int equip,String equip_name){
  digitalWrite(equip, HIGH);
  Serial.print("Equip ");
  Serial.print(equip_name);
  Serial.println("Ligado!");
}

void Command_OFF(int equip,String equip_name){
  digitalWrite(equip, LOW);
  Serial.print("Equip ");
  Serial.print(equip_name);
  Serial.println("Desligado!");
}

void Callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");
  for (int i = 0; i < length; i++) {
    char aux = (char)payload[i];
    msg += aux;
  }
  if(String(topic) == cmd_filtro){
    if (msg=="0" || msg=="1"){
      filtroStatus = msg;
    }}
  else if (String(topic) == cmd_oxigenador){
    if (msg=="0" || msg=="1"){
      oxigenadorStatus = msg;
    }}
  else if(String(topic) == cmd_aquecedor){
    if (msg=="0" || msg=="1"){
      aquecedorStatus = msg;
    }}
  else Serial.println("No Topic correspondence");
}

void init_Wifi(){
  Serial.print("Connecting to WiFi ");
  Serial.print(ssid);
  Serial.println("...");
  WiFi.begin(ssid,pwd);
  attempt = 0;
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if(attempt >= 60){
        Serial.println("Unable to connect... exiting!");
        break;
      }
      attempt++;
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.print("WiFi Successfully Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void Reconnect_MQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // Attempt to connect
    if (client.connect("ZurgAninhaMakiProject2022")) {
      Serial.println("Connected!");
      // Once connected, publish an announcement...
      // ... and resubscribe
      client.subscribe(cmd_filtro);
      delay(100);
      client.subscribe(cmd_oxigenador);
      delay(100);
      client.subscribe(cmd_aquecedor);
    } else {
      Serial.printf("Failed! Current State is: ",client.state());
      Serial.println(" trying again in 5 seconds");
      // Wait 2 seconds before retrying
      delay(5000);
    }
  }
}
