#include <Wire.h>
#define slave  0x11//direccion del pic esclavo = direccion programada / 2
 
void setup(){
  Wire.begin(); //inicializar i2c
  Serial.begin(9600); //la salida de datos se
  //realizara mediante la terminal serial
 
  delay(1000);
}
 
void loop(){
  int k;
  Serial.println("Lectura del esclavo");
  Wire.beginTransmission(slave); //Establecer comunicacion con el slave
  Wire.endTransmission();
 
  delay(5);
 
  Wire.requestFrom( slave, 1);//pide 1 byte al slave
  delay(10);
 
  while( Wire.available() ){ //lee byte por byte los datos recibidos desde el slave
     //este ciclo while solamente se realiza una vez,
     //puesto que solamente se solicitó un Byte al slave
     k = Wire.read();
     Serial.println(k);
  }
  delay(200);
}
