// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

const int buzzer = 6; //buzzer to arduino pin 9

#include <Wire.h>
#define slave  0x11//direccion del pic esclavo = direccion programada /
void setup() {
  pinMode(13, OUTPUT);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 6 as an output
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600); 
  Serial.println("Hola mundo");  
  tone(buzzer, 6000); // Send 1KHz sound signal...
  delay(500);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(500);
  tone(buzzer, 5000); // Send 1KHz sound signal...
  delay(100);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(100);
  tone(buzzer, 5000); // Send 1KHz sound signal...
  delay(100);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
}

byte x = 99;

void loop() {
  
if ( Serial.available () > 0 ) 
    {
    static char input[16];
    static uint8_t i;
    char c = Serial.read ();
  
    if ( c != '\r' && i < 15 ) // assuming "Carriage Return" is chosen in the Serial monitor as the line ending character
      input[i++] = c;
      
    else
      {
      input[i] = '\0';
      i = 0;
        
      int number = atoi( input );
      x=100-number;
      Serial.println( number );
      }
    }
   
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000); 
  Serial.println("Empieza transmision-Enviando:");Serial.print(x);
  Wire.beginTransmission(slave); // transmit to device #8
  Wire.write(x);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  Serial.println("termina tranmision"); 
//  x++;
  byte k;
  Serial.println("Lectura del esclavo:");
  Wire.beginTransmission(slave); //Establecer comunicacion con el slave
  Wire.endTransmission();
  delay(5); 
  Wire.requestFrom( slave, 1);//pide 1 byte al slave
  delay(10); 
  while( Wire.available() ){ //lee byte por byte los datos recibidos desde el slave
     //este ciclo while solamente se realiza una vez,
     //puesto que solamente se solicitó un Byte al slave
     //18092016 user time de espera para no dejar q se bloquee 
     k = Wire.read();
     Serial.println(k);
  }
  delay(200);
  
  

}
