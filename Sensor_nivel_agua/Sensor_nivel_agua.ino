int sensor_nivel = 34;
float value = 0.0;
float movMean[50];
float mean;
int contador = 0;
int level;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);


}

void loop() {
  // put your main code here, to run repeatedly:
  value = analogRead(sensor_nivel);
  movMean[contador]=value;
  contador++;
  if(contador==50){
    float soma = 0;
    for (int i=0; i<contador; i++){
      soma += movMean[i];
    }
    mean = soma/contador;
    Serial.print("A media eh: ");
    Serial.println(mean);
    if(mean<1000) level = 0;
    if(mean>=1000 || mean <1810) level=1;
    if(mean>=1810 || mean <1900) level=2;
    if(mean>=1900 || mean <1960) level=3;
    if(mean>=1960) level=4;
    contador=0; 
  }
  
  delay(500);
}
