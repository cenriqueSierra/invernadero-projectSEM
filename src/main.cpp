#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>              //for ESP8266 use bug free i2c driver https://github.com/enjoyneering/ESP8266-I2C-Driver
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <TimeLib.h>  // Librería para el manejo del tiempo

#define DHTPIN 15     // Pin al que está conectado el sensor DHT11
#define DHTTYPE DHT11 // Tipo de sensor DHT
#define COLUMS 16
#define ROWS   2
#define PAGE   ((COLUMS) * (ROWS))

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
//boolean estado = false;
/*Funcion para mapear el valor de entrada del sensor de lluvia, u otro sensor*/


DHT dht(DHTPIN, DHTTYPE);
// Define el pin al que está conectado el relé
const char* ssid = "Claro_RAMIRES";
const char* password = "0914709258";

const int sensorPin = 34;  // Pin analógico al que está conectado el sensor
const int pinBombaA = 13; // De riego. Cambia el número de pin según tu conexión
const int pinVenti= 12; 
const int pinBombaB = 14; //De llenado
const int pinMotor= 27;
int trigPin = 22;    // Disparador (Trigger)
int echoPin = 21;    // Eco (Echo)
long duration, cm, inches;

void setup() {
  // Inicializa el pin del relé como salida
  Serial.begin(115200);

   // Conéctate a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  
  Serial.println("Conectado a WiFi");

  // Inicializa la librería de tiempo
  configTime(0, -18000, "pool.ntp.org");  // Configura el servidor NTP

   while (lcd.begin(COLUMS, ROWS) != 1) //colums - 20, rows - 4
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);   
  }

  lcd.print(F("PCF8574 is OK..."));    //(F()) saves string to flash & keeps dynamic memory free
  delay(2000);

  lcd.clear();

  lcd.setCursor(0,0);	//columna - fila
  lcd.print("Proyecto");
  lcd.setCursor(2,1);
  lcd.print("Embebidos");
  delay(2000);

  dht.begin();
  

  pinMode(pinBombaA, OUTPUT);
  pinMode(pinBombaB, OUTPUT);
  pinMode(pinVenti, OUTPUT);
  pinMode(pinMotor, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Apaga el relé al inicio
  digitalWrite(pinBombaA, LOW);
  digitalWrite(pinBombaB, LOW);
  digitalWrite(pinVenti, LOW);
  digitalWrite(pinMotor, LOW);
}

void loop() {
  // Enciende el relé durante 2 segundosdelay(2000); // Espera 2 segundos entre mediciones

  // Obtiene y muestra la hora actual
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  int hour = timeinfo.tm_hour;
  int minutes = timeinfo.tm_min;
  String horaMinuto = String(hour)+":"+String(minutes);
  Serial.println("Horita "+String(hour));
  
  //getLocalTime(&timeinfo)) {
    //Serial.print("Hora actual: ");
    //Serial.println(&timeinfo, "%H:%M");
    //Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
    //lcd.setCursor(6,1);
  //}
Serial.println();
  delay(1000);  // Espera 10 segundos antes de mostrar la siguiente hora
  //4095 Seco -> 100% antes de pasar a la variable percentageHGround 
  //1600 Mojado ->0% antes de pasar a la variable percentageHGround
  int sensorValue = analogRead(sensorPin);  // Lee el valor analógico del sensor YL-69
  int percentageHGround = 100 - map(sensorValue, 1600, 4095, 0, 100);  // Mapea el valor a un porcentaje (0-100%)
  lcd.clear();
  lcd.setCursor(0,0);

  float humidity = dht.readHumidity();     // Lee la humedad relativa
  float temperature = dht.readTemperature(); // Lee la temperatura en grados Celsius

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
 pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
/**
 - Dio o noche
 -Dia: 
 22:54:31
  Dia: 06:00 -17:30
  Noche: 17:31 - 5:59
  tm fechaHora = getLocalTime(&timeinfo);


*/

//CONDICIONES PARA VENTILADOR
//PARA EL DIA
int condition1Mornig = hour >=6 && minutes >= 0;
int condition2Morning = hour <=17 && minutes <= 30;

if  (condition1Mornig | condition2Morning){ //Al  menos una de las 2 condiciones debe cumplirse para ejecutarse.
  if (temperature<23.00 && temperature>=20.00){
    digitalWrite(pinVenti, LOW);
    lcd.setCursor(0,0);
    lcd.print("TEMP BAJA");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("FAN OFF");
    delay(1000);
    //delay(2000);
  }
  else if(temperature>=23.00){
  // Apaga el relé durante 2 segundos
    digitalWrite(pinVenti, HIGH);
    lcd.setCursor(0,0);
    lcd.print("TEMP ALTA");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("FAN ON");
    delay(1000);
    //delay(2000);
  }
}

//PARA LA NOCHE
//Noche: 17:31 - 5:59
int condition1Night = hour >=17 && minutes > 30;
int condition2Night = hour <6 && minutes <= 59;

if(condition1Night | condition2Night){
  if (temperature<22.00 && temperature>=18.00){
    digitalWrite(pinVenti, LOW);
    lcd.setCursor(0,0);
    lcd.print("TEMP BAJA");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("FAN OFF");
    delay(1000);
    //delay(2000);
  }
  else if(temperature>=22.00){
  // Apaga el relé durante 2 segundos
    digitalWrite(pinVenti, HIGH);
    lcd.setCursor(0,0);
    lcd.print("TEMP BAJA");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("FAN ON");
    delay(1000);
    //delay(2000);
  }
}
//PARA LA HUMEDAD RELATIVA
if (humidity<=60.00){
  digitalWrite(pinVenti, HIGH);
  lcd.setCursor(0,0);
  lcd.print("HUM.R BAJA");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("FAN ON");
  delay(1000);

}
else if(humidity>60.00){
  digitalWrite(pinVenti, LOW);
  lcd.setCursor(0,0);
  lcd.print("HUM.R ALTA");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("FAN OFF");
  delay(1000);
}

//CONDICIONES PARA BOMBA DE RIEGO
if (percentageHGround>=75){
  digitalWrite(pinBombaA, LOW);
  lcd.setCursor(0,0);
  lcd.print("HUM.S ALTA");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("PUMP A OFF");
  delay(1000);

  //delay(2000);
}else if(percentageHGround<=50){
  digitalWrite(pinBombaA, HIGH);
  lcd.setCursor(0,0);
  lcd.print("HUM.S BAJA");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("PUMP A ON");
  delay(1000);

}

  /*digitalWrite(pinBombaA, HIGH);
  delay(2000);

  // Apaga el relé durante 2 segundos
  digitalWrite(pinBombaA, LOW);
  delay(2000);

   // Enciende el relé durante 2 segundos
  digitalWrite(pinBombaB, HIGH);
  delay(2000);

  // Apaga el relé durante 2 segundos
  digitalWrite(pinBombaB, LOW);
  delay(2000);

   // Enciende el relé durante 2 segundos
  digitalWrite(pinVenti, HIGH);
  delay(2000);

  // Apaga el relé durante 2 segundos
  digitalWrite(pinVenti, LOW);
  delay(2000);

     // Enciende el relé durante 2 segundos
  digitalWrite(pinMotor, HIGH);
  delay(2000);

  // Apaga el relé durante 2 segundos
  digitalWrite(pinMotor, LOW);
  delay(2000);*/

  Serial.print("Valor del sensor: ");
  Serial.println(sensorValue);
  Serial.print("Humedad del suelo (%): ");
  Serial.println(percentageHGround);

  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" °C");


  lcd.clear();
  lcd.print("H:");
  lcd.print(humidity);
  lcd.print(" ");
  //lcd.print(" %\t");
  lcd.print("T:");
  lcd.print(temperature);
  //lcd.println("°C");
  lcd.setCursor(0,1);
  lcd.print("Hs: ");
  lcd.print(percentageHGround);


  cm = (duration/2) / 29.1;     // Divide entre 29.1 o multiplica por 0.0343
  inches = (duration/2) / 74;   // Divide entre 74 o multiplica por 0.0135
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  
  float altura = 50;
  float resta = altura -cm; //Me da cantidad ocupada de agua 40 
  
  /**
  *Condicion par activar bomba de agua 
  */
  float porcentajeNivelAgua = cm/altura *100; ///es menor o igual a 20% se activa
  //Debes enviar una notificación 
  if(porcentajeNivelAgua<=20 && percentageHGround <= 50){
    digitalWrite(pinBombaA,HIGH);
    digitalWrite(pinBombaB,LOW);

  }



  delay(3000);

  lcd.clear();
  lcd.print(&timeinfo);
  delay(2000);

}