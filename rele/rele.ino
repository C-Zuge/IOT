int rele = 26;

void setup() {
  // put your setup code here, to run once:
  pinMode(rele,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(rele,HIGH);
  delay(2000);
  digitalWrite(rele,LOW);
  delay(2000);
}
