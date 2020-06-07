#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define State "TS_SaGs_State"
#define Out_Choice "TP_SaGs_Out_Choice"
#define ADC_signal "TP_SaGs_ADC"
#define In_signal_1 "TP_SaGs_In_1"
#define In_signal_2 "TP_SaGs_In_2"
#define In_signal_3 "TP_SaGs_In_3"

const char* ssid = "NET_2G84DBAD";
const char* pswd = "E284DBAD";
const char* Mqtt_Server = "broker.mqtt-dashboard.com";
const int Mqtt_Port = 1883;
String Status = "Waiting..."; 
String Choice = "0";
int analogIn = 4;
float temperatura = 0;

void init_Wifi();
void Reconnect_MQTT();
void Callback(char* topic, byte* message, unsigned int length);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  init_Wifi();
  client.setServer(Mqtt_Server, 1883);
  client.setCallback(Callback);
}

void loop() {
  if (!client.connected()) {
    Reconnect_MQTT();
  }

  temperatura = (analogRead(analogIn) / 2048.0) * 3300 * 0.1;
  client.publish(ADC_signal,String(temperatura).c_str(),true);
    
  client.subscribe(State);
  client.subscribe(Out_Choice);
  if(Status == "ON"){
    Serial.println("Status on");
    if(Choice == "1"){
      float *x;
      Serial.println("\nChoice 1");
      x = Gen_Heaviside();
    }
    else if(Choice == "2"){
      float *x;
      Serial.println("\nChoice 2");
      x = Gen_Positive_Straight();
    }
    else if(Choice == "3"){
      float *x;
      Serial.println("\nChoice 3");
      x = Gen_Negative_Straight();
    }
    else Serial.println(Choice);
  }else{Serial.println(Status);}
  client.loop();
}

void init_Wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pswd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void Reconnect_MQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // Attempt to connect
    if (client.connect("Zurg")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
      client.subscribe(State);
      delay(100);
      client.subscribe(Out_Choice);
    } else {
      Serial.print("Failed, Current State is: ");
      Serial.println(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

void Callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  for (int i = 0; i < length; i++) {
    char aux = (char)payload[i];
    msg += aux;
  }
  if(String(topic) == State) Status = msg;
  else if (String(topic) == Out_Choice) Choice = msg;
  else Serial.print("No Topic correspondence ");
  Serial.println("");
}

float *Gen_Positive_Straight(){
  float Values[255];
  for(int x = 0; x < 255; x += 1){
    Values[x]=x*0.0197;
    dacWrite(25,Values[x]);
    client.publish(In_signal_2,String(Values[x]).c_str(),true);
    Serial.print(Values[x]);
    Serial.print(" ");
  }
  return Values;
}

float *Gen_Negative_Straight(){
  float Values[255];
  for(int x = 0; x < 255; x += 1){
    Values[x]=x*-0.0197;
    dacWrite(25,Values[x]);
    client.publish(In_signal_3,String(Values[x]).c_str(),true);
    Serial.print(Values[x]);
    Serial.print(" ");
  }
  return Values;
}

float *Gen_Heaviside(){
  float Values[255];
  for(int i=0; i<255; i++){
    if (i<128) {
      Values[i]=5.0;
    }else{
      Values[i]=-5.0;
    }
    dacWrite(25,Values[i]);
    client.publish(In_signal_1,String(Values[i]).c_str(),true);
    Serial.print(Values[i]);
    Serial.print(" ");
  }
  return Values;
}
