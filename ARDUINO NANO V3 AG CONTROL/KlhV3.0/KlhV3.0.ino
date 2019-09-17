#include <EtherCard.h>
#include<stdlib.h>
#include <EEPROM.h>
#include <Wire.h>
#define DEBUG 
//#define DEBUG_GSM
//#define HTTP-GET 
#define slave  0x11//direccion del pic esclavo = direccion programada /
#define SERIALch  "1988"
#define SERIALA1  "s=1988&v1=0&v2=0&r1=0&r2=0&r3=0&r4=0&rst=0&ip=192.168.001.001&readserver=1"
#define SERIALA2  "s=1988&v1=----&v2=----&r1=0&r2=0&r3=0&r4=0&rst=0&ip=192.168.001.001"
#define GSM_ON     digitalWrite(6, LOW)
#define GSM_OFF    digitalWrite(6, HIGH)
////////////////////////////////////////////////////
char              serial_device[]  = SERIALch;      //1981 serial para tarjeta para el retiro.19 10 2016
////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// Default network config                                             //
byte              myip[] = {192,168,20,241};                      //
byte              mymac[] = {0x74,0x69,0x69,0x2D,0x30,0x30 };   //
byte              gwip[] = {192,168,20,1};                      //
byte              static_dns[] = {8,8,8,8};                      //
byte              netmask[] = {255,255,255,0};                   //
////////////////////////////////////////////////////////////////////////
char              serial_temp[] = SERIALch;
int               sensorPin =  A1;            //entrada principal de la tarjeta 7-17 vdc
int               sensorPin2 = A0;            //sensor modulo de baterias 2 en A0??
int               load_control =  A2;         //salida control de carga
int               sensor_a3 =  A3;
int               sensor_corriente_a6 =  A6;
int               sensor_corriente_a7 =  A7;
int               trigerA =     2;             //OUTPUT activador para lectura sensor de voltaje
int               trigerB =     3;             //OUTPUT activador para lectura sensor de voltaje
int               releB = 4;
int               releC = 5;
int               releD = 0;                    //por compatibilidad lo agregamos pero no se usa
int               buzzer = 6;
int               releE = 7;
int               rst_eth = 9;
int               pwm = 10;
int               getreply = 0;
int               repeatstore = 0;
int               flagstore = 0;
int               getrequest = 0;
int               timeout_dns = 0;
byte              x =10;
byte              porcentaje=50;
float             Sensibilidad=0.1;           //sensibilidad en Voltios/Amperio para sensor de 20A
const char        website[] PROGMEM = "www.redesagingenieria.com";
#define REQUEST_RATE 1800000 //1800000 milliseconds  --30 minutos
#define REQUEST_RATE_REPEAT 15000
int               read_sensor_delay = 1;
byte              Ethernet::buffer[225];//copy and pasted in agc.php    225
char              myArray[0];
static uint32_t   timer;
static uint32_t   timer2;
static uint32_t   timer3;
int               segs = 0;
int               contsegs = 0;
int               pass = 0;
unsigned long     time;
BufferFiller bfill;
int releb_value = 1; int relec_value = 1; int reled_value = 0;  int relee_value = 1;
char              ipadd[16];
char              gwadd[16];
char              nmadd[16];
char              part1[4];
char              part2[4];
char              part3[3];
float             sensor1Value;
float             sensor2Value;
char              charVal[10];
char              charVal2[10];               //temporarily holds data from vals
int               dstart = 0;
int               dlen = 0;
const char htmlHeaderphone[] PROGMEM =
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "Pragma: no-cache\r\n"
  "\r\n"
  ;
// ---------------GSM-------------------------------
int step1=1;
int step2=1;
String hh;
String sms;
int flag=0;
unsigned long previousMillis = 0;  
unsigned long previousMillisx = 0;  
const long interval0 = 200;
const long interval1 = 35000;
const long interval2 = 55000;
const long interval3 = 65000;
      int  interval4 = 0; //200 ms x 9000 ->30 minutos.
      int  interval5 = 0;
// ------------------------------------------------- 
static void my_result_cb (byte status, word off, word len)
{ 
  #ifdef DEBUG
  Serial.print(F("<<< reply "));
  Serial.print(millis() - timer);
  Serial.println(" ms");
  #endif
  char* data = (char *) Ethernet::buffer + off;
  #ifdef DEBUG
  Serial.println(data);
  Serial.println("--");
  Serial.println(" R?:");
  Serial.println(data[157]);
  #endif
  myArray[0] = data[157]; //equivale a caracter      echo "R11116789RRRRRRRRRRR
  pass = 0;
  if (strncmp("R", myArray, 1) == 0)
    {
    pass = 0;
    getreply = 1;
    repeatstore = 0;
    flagstore = 0;
    #ifdef DEBUG
    Serial.println(F("get reply = 1:)"));
    Serial.println(F("r1:"));
    Serial.println(data[158]);
    Serial.println(F("r2:"));
    Serial.println(data[159]);
    Serial.println(F("r3:"));
    Serial.println(data[160]);
    Serial.println(F("r4:"));
    Serial.println(data[161]);
    Serial.println(F("dev cod:"));
    Serial.println(data[165]);
    Serial.println(data[166]);
    Serial.println(data[167]);
    Serial.println(data[168]);
    #endif
    serial_temp[0] = data[165];
    serial_temp[1] = data[166];
    serial_temp[2] = data[167];
    serial_temp[3] = data[168];
    if (strncmp(serial_device, serial_temp, 4) == 0)
    { 
      #ifdef DEBUG 
      Serial.println(F("getserialdev:)"));
      #endif
      myArray[0] = data[158];
      if (strncmp("1", myArray, 1) == 0)
      {
        if (digitalRead(releB) == 0)
        {
          digitalWrite(releB, HIGH);
          EEPROM.write(1, 1);
        }
        #ifdef DEBUG
        Serial.println(F("r1 en 1"));
        #endif
      }
      if (strncmp("0", myArray, 1) == 0)
      {
        if (digitalRead(releB) == 1)
        {
          digitalWrite(releB, LOW);
          EEPROM.write(1, 0);
        }
        #ifdef DEBUG
        Serial.println(F("r1 en 0"));
        #endif
      }
      myArray[0] = data[159];
      if (strncmp("1", myArray, 1) == 0)
      {
        if (digitalRead(releC) == 0)
        {
          digitalWrite(releC, HIGH);
          EEPROM.write(2, 1);
        }

      }
      if (strncmp("0", myArray, 1) == 0)
      {
        if (digitalRead(releC) == 1)
        {
          digitalWrite(releC, LOW);
          EEPROM.write(2, 0);
        }
      }
      myArray[0] = data[160];
      if (strncmp("1", myArray, 1) == 0)
      {
        if (digitalRead(releD) == 0)
        {
          digitalWrite(releD, HIGH);
          EEPROM.write(3, 1);
        }
      }
      if (strncmp("0", myArray, 1) == 0)
      {
        if (digitalRead(releD) == 1)
        {
          digitalWrite(releD, LOW);
          EEPROM.write(3, 0);
        }

      }
      myArray[0] = data[161];
      if (strncmp("1", myArray, 1) == 0)
      {
        if (digitalRead(releE) == 0)
        {
          digitalWrite(releE, HIGH);
          EEPROM.write(4, 1);
        }
        #ifdef DEBUG
        Serial.println(F("r4 en 1"));
        #endif
      }
      if (strncmp("0", myArray, 1) == 0)
      {
        if (!digitalRead(releE) == 1)
        {
          digitalWrite(releE, LOW);
          EEPROM.write(4, 0);
        }
        #ifdef DEBUG
        Serial.println(F("r4 en 0"));
        #endif
      }
    }
  }
  else       {
    getreply = 0;
    repeatstore = 1;
    #ifdef DEBUG
    Serial.println(F("get reply=:-("));
    #endif
  }
  return;
}

float sensor1() { //esta es la entrada PRINCIPAL de voltaje en la tarjeta
  digitalWrite(trigerA, HIGH);
  digitalWrite(trigerB, HIGH);
  delay(read_sensor_delay);
  
  float sumlectura=0;  
  int muestras=10;
  int promlectura=0;
  for(int i=0;i<muestras;i++)
        {
          sumlectura+=analogRead(sensorPin);
          delay(5);          
        }
  promlectura=sumlectura/muestras;      
  sensor1Value= (promlectura*0.055)+6.5;    
  
  #ifdef DEBUG
  Serial.println(F("++++++++volt1 entrada PRINCIPAL.."));
  Serial.println(promlectura);
  Serial.println(sensor1Value);
  Serial.println(F("++++++++"));
  #endif
  //digitalWrite(trigerA, LOW);
  //digitalWrite(trigerB, LOW);
  
  //delay(read_sensor_delay);
  dtostrf(sensor1Value, 4, 1, charVal);
  return sensor1Value;

}
// ---------------------entrada secundaria de voltaje-------------------------
void sensor2() {
  delay(read_sensor_delay);  // 17 03 16 --por q la segunda lectura molesta?
  digitalWrite(trigerB, HIGH);
  digitalWrite(trigerB, HIGH);
  delay(1);
  float prom=0;
  int muestras=5;
  for(int k=1;k<=muestras;k++)
      {
      prom+=analogRead(sensorPin2);
      delay(5);
      }
  sensor2Value=((prom/muestras)*0.055)+6.5;    
  if (sensor2Value < 0)
    sensor2Value = 0;
  #ifdef DEBUG  
  Serial.println(F("*****+++++volt 2 Entrada Secundaria"));
  Serial.println(prom);
  Serial.println(sensor2Value);
  Serial.println(F("*****+++++++++++++++++++++++++++++"));
  #endif
  //digitalWrite(trigerA, LOW);
  //digitalWrite(trigerB, LOW);
 dtostrf(sensor2Value, 4, 1, charVal2);

}

static word homePagephone() {
  sensor1();
  sensor2();
  releb_value  = digitalRead(releB);
  relec_value  = digitalRead(releC);
  reled_value  = relec_value;
  relee_value  = digitalRead(releE);
  int lectiny=analogRead(A7); 
  int lect=analogRead(A6);
  bfill = ether.tcpOffset();
  bfill.emit_p( PSTR (
                  "$F"
                  "$D:$D:$D:$D:$S:$S:$Dx$D\r\n"
                ) , htmlHeaderphone, releb_value, relec_value, reled_value, relee_value, charVal, charVal2,lectiny,lect) ;
  return bfill.position();
}
// ----------------------------------------------
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void setup () {
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  time = millis();
  timer2 = 0;
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(115200);
  //Arduino (Atmega) pins default to inputs, so they don't need to be explicitly declared as inputs with pinMode(). --
  pinMode(load_control , OUTPUT);
  pinMode(trigerA, OUTPUT); pinMode(trigerB, OUTPUT); pinMode(releB, OUTPUT); pinMode(releC, OUTPUT); pinMode(buzzer, OUTPUT);  pinMode(releE, OUTPUT); pinMode(pwm, OUTPUT); //input rst_eth=9
  digitalWrite(releB, EEPROM.read(1));
  digitalWrite(releC, EEPROM.read(2));
  digitalWrite(releD, EEPROM.read(3));
  digitalWrite(releE, EEPROM.read(4));
  timer = - REQUEST_RATE; // start timing out right away
  #ifdef DEBUG
  Serial.println(F("eprom restart."));
  Serial.println(EEPROM.read(7));
  #endif
  if (EEPROM.read(7) == 1)
      {
      EEPROM.write(8, 55);
      EEPROM.write(9, 249);
      EEPROM.write(10, 1);
  
      EEPROM.write(7, 0);
      restart();
      }
  if (EEPROM.read(7) == 3)  //    kevin leandro
      {
      myip[2] = EEPROM.read(8);
      gwip[2] = EEPROM.read(8);
      myip[3] = EEPROM.read(9);
      gwip[3] = EEPROM.read(10);
      #ifdef DEBUG
      Serial.println(F( "eepromm 8:"));
      Serial.println( EEPROM.read(8));
      Serial.println(F( "eepromm 9:"));
      Serial.println(  EEPROM.read(9));
      Serial.println(F ("eepromm 10:"));
      Serial.println( EEPROM.read(10));
      #endif
      }
  GSM_OFF; 
  delay(5000);
  ethconfig();
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
  GSM_ON;
  #ifdef DEBUG
  GSM_OFF;
  #endif
}
// ----------------------------------------------
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void restart() 
    {
        while (1)
        {
          //definir nuevo codigo
        }
    }

void ethconfig()
    {
      sprintf (ipadd, "%u.%u.%u.%u", myip[0], myip[1], myip[2], myip[3]);  
      #ifdef DEBUG
      Serial.println( "Ip:");
      Serial.println(ipadd);
      #endif
      sprintf (nmadd, "%u.%u.%u.%u", netmask[0], netmask[1], netmask[2], netmask[3]);  //char ipadd[16];
      #ifdef DEBUG
      Serial.println( "netMask:");
      Serial.println(nmadd);
      #endif
      sprintf (gwadd, "%u.%u.%u.%u", gwip[0], gwip[1], gwip[2], gwip[3]);  //char ipadd[16];
      #ifdef DEBUG
      Serial.println( "Gw:");
      Serial.println(gwadd);
      #endif
      ether.hisport = 80 ;
      Serial.println(F( "Starting Network interface..."));
      if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
          {
          #ifdef DEBUG
          Serial.println(F( "Failed Ethernet  start"));
          #endif
          }
      if (ether.begin(sizeof Ethernet::buffer, mymac) != 0) {
        ether.staticSetup(myip, gwip, static_dns, netmask);
        #ifdef DEBUG
        Serial.println(F( "test dns"));
        #endif
      }
//      delay(1);//prueba de desbloqueo  enero  1000
      dnscheckup();
      #ifdef DEBUG
      Serial.println("DNS Checked.");
      #endif
    }
void dnscheckup()
    { 
      #ifdef DEBUG
      Serial.println(F("..in.F()..dnscheckup().test "));
      #endif
      if (ether.dnsLookup(website))
        while (ether.clientWaitingGw())
            {
            ether.packetLoop(ether.packetReceive());
            #ifdef DEBUG
            Serial.println("dns ok");
            #endif
            }
      else  
        {
        timeout_dns = 1;
        #ifdef DEBUG 
        Serial.println(F("timeout dns.."));
        #endif
        }
      #ifdef DEBUG  
      Serial.println(F(".out...dnscheckup()"));
      #endif
    }

//void enviari2c()
//      {
//      
//    //  x++;
//      
//      
//      
//      }
float get_corriente_iny(int n_muestras)
    {
    float voltajeSensor=0;
    float corriente=0;
    float proma7=0;
    for(int i=0;i<n_muestras;i++)
        {
          voltajeSensor = (analogRead(A7)) * (5.0 / 1023.0);////lectura del sensor
          corriente=corriente+(voltajeSensor-2.55)/Sensibilidad; //Ecuación  para obtener la corriente--lectura de 100 mv de resolucion 02 01 2017  0v->lectura 522->2.55
          proma7+=analogRead(A7);
        }
    corriente=corriente/n_muestras;
    proma7=proma7/n_muestras;
    #ifdef DEBUG
    Serial.print(F(" c inyec: "));
    Serial.println(proma7);
    #endif
    return(corriente);
    }      
float get_corriente_load(int n_muestras)
    {
    float voltajeSensor=0;
    float corriente=0;
    for(int i=0;i<n_muestras;i++)
        {
          voltajeSensor = (analogRead(A6)) * (5.0 / 1023.0);////lectura del sensor
          corriente=corriente+(voltajeSensor-2.55)/Sensibilidad; //Ecuación  para obtener la corriente
          //delay(1);
        }
    corriente=corriente/n_muestras;
    #ifdef DEBUG
    Serial.print(F("ura c carga: "));
    Serial.println(analogRead(A6));
    #endif
    return(corriente);
    }
void ACchargingcontrol()
    {
      
    if (sensor2Value<8)
        porcentaje=100-30;//100-x 30%
    if ((sensor2Value>=8)&&(sensor2Value<11.5)) 
        porcentaje=100-70;//100-x 70%
    if ((sensor2Value>=11.5) && (sensor2Value<=13)) 
        porcentaje=100-99;//100-x 99%
    if (sensor2Value>13)
        porcentaje=100-20;//100-x   60%
          
    }
void controlcargaonoff()
      {
      #ifdef DEBUG
      Serial.print(F("sensor2: ")); 
      Serial.print(sensor2Value); 
      #endif
      if(sensor2Value < 12.0)
          {  
          digitalWrite(releE, HIGH);
          #ifdef DEBUG
          Serial.print(F("Carga off: "));
          #endif
          }
      else
          {
          if(sensor2Value >=13.5)
              {//si corriente de carga = corriente inyectada ..si corriente de carga < corriente inyectada ..si corriente de carga > corriente inyectada ..
              //si la corriente de carga consumida = medida de corriente inyectada  es por q la reistencia de la bateria es alta--> entonces desconectar inyeccion de corriente?¡¡?
              //o limitar la inyeccion de corriente a la carga consumida para no sobrecargar la bateria.  
              //digitalWrite(releE, HIGH);
              }
          if((sensor2Value < 13.5) && (sensor2Value >= 12.0))
              {  
              digitalWrite(releE, LOW);
              #ifdef DEBUG
              Serial.print(F("corriente :-) "));
              #endif
              }
          }         
      }
      
//*************************************************************************************************************************************************************************       
void loop () {
  //-------------------------GSM------------------------------------------------------
  #ifdef DEBUG_GSM
  unsigned long currentMillis = millis();
    if ((currentMillis - previousMillisx >= interval0))
        {
        previousMillisx=currentMillis;  
        if (Serial.available()) 
              {
               hh = Serial.readStringUntil('\n'); 
               if (hh.indexOf("+CMT") >= 0) 
                    {
                    flag=1;
                    Serial.println("AT+CMGF=1");    
                    delay(1000);
                    Serial.print("AT+CMGS=\""); 
                    Serial.print(3147654655); 
                    Serial.write(0x22);   //hex de comillas
                    Serial.write(0x0D);  // hex equivalente de Carraige Return    
                    Serial.write(0x0A);  // hex equivalente de newline
                    delay(1000);
                    float I=get_corriente_iny(100);//obtenemos la corriente inyectada a la bateria promedio de x muestras 
                    float I2=get_corriente_load(100);
                    int b=digitalRead(releB);int c=digitalRead(releC);int d=digitalRead(releD);int e=digitalRead(releE); 
                    String msr=String(b)+String(c)+String(d)+String(e);
                    sms="V1:"+String(sensor2Value)+"V2:"+String(sensor1Value)+"R:"+msr+"-AmpAC:"+String(I)+"ALoad:"+String(I2);
                    Serial.print(sms);
                    delay(1500);
                    Serial.println (char(26));
                    delay(1500);                
                    //the ASCII code de ctrl+z is 26 y envia el mensaje  
                    }   
              }
         interval4+=1;
         if(interval4>=9000)
            {            
            GSM_OFF;
            interval5+=1;  
            if(interval5>=5)
                {
                GSM_ON;
                interval5=0;
                interval4=0;  
                }                
            }     
        }    
    if ((currentMillis - previousMillis >= interval1)&&(step1==1)) 
          {
          if(flag==1)
              {
              
               
              }  
          if(flag!=1)
              {
              Serial.println("AT");
              }
          step1=0;
          }
    if ((currentMillis - previousMillis >= interval2)&&(step2==1))
          {
          //Serial.println("ATD3147654655"); 
          step2=0; 
          }
    if ((currentMillis - previousMillis >= interval3))
          {
          flag=0;  
          //Serial.println("ATH");
          step1=1;
          step2=1;  
          previousMillis = currentMillis;
          }
  #endif        
  //---------------------END-GSM-------------------------------------------------------------
  time = millis();
  if (time >= 43200000)
  {
    //generar nuevo codigo de reset cada 12 horas
  }
  if (time < 43200000)
  {
    //codigo de no reset
  }
 //////////////////////////////////////////////////////////////////////
  #ifdef DEBUG
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
          Serial.print("Recibido:");
          Serial.print( number );
          Serial.println("<>");
          delay(2000);
          
          
          }
      }
  #endif  
  /////////////////////////////////////////////////////////////////////
  if ((millis() > timer3) )
      {
        timer3 += 1000;
        segs += 1;
        contsegs += 1;
        sensor1();   
        sensor2();
        ACchargingcontrol();
        #ifdef DEBUG
        Serial.print(F("<> voltaje1:"));
        Serial.print(sensor2Value);
        Serial.print(F("<> i2c:"));
        Serial.println((x));//100-porcentaje
        delay(2000);
        #endif
        Wire.beginTransmission(slave); // transmit to device #8
        Wire.write(x);              // sends one byte
        Wire.endTransmission();    // stop transmitting
        #ifdef DEBUG
        Serial.println(F("end tranmision")); 
        #endif
        byte k;
        #ifdef DEBUG
        Serial.println(F("read i2c:"));
        #endif
        Wire.beginTransmission(slave); //Establecer comunicacion con el slave
        Wire.endTransmission();
        Wire.requestFrom( slave, 1);//pide 1 byte al slave
        delay(1); 
        while( Wire.available() )
            { //lee byte por byte los datos recibidos desde el slave
            //este ciclo while solamente se realiza una vez,
            //puesto que solamente se solicitó un Byte al slave
            //18092016 user time de espera para no dejar q se bloquee 
            k = Wire.read();
            #ifdef DEBUG
            Serial.println(k); 
            #endif
            }
          
        controlcargaonoff();
        float I=get_corriente_iny(100);//obtenemos la corriente inyectada a la bateria promedio de x muestras 
        #ifdef DEBUG
        Serial.print(F("amp inyec: "));
        Serial.println(I,3);
        #endif
        float I2=get_corriente_load(100); 
        #ifdef DEBUG
        Serial.print(F("amp de carga: "));
        Serial.println(I2,3);
        #endif    
        if (segs == (REQUEST_RATE_REPEAT / 1000))
            segs = 0;
        #ifdef DEBUG    
        Serial.print(F("-cnts-:"));
        Serial.print(contsegs);
        #endif
        if (contsegs > ((REQUEST_RATE_REPEAT / 1000) + 10))
            {
              flagstore = 0;
              #ifdef DEBUG
              Serial.println(F("-cntsreini-:"));
              Serial.print(contsegs);
              #endif
              if (segs == 0)
                pass = 1;
              else pass = 0;
            }
      }

  if (((millis() > timer + timer2 + 10000)  && (getrequest == 1)) or (pass == 1)) //si ya se  un get pero no respuesta , cada x-timer2- segundos intenta reconectar.
  {
    pass = 0;
    contsegs = 0;
    if (flagstore == 1)repeatstore = 1;
    timer2 += REQUEST_RATE_REPEAT;
    #ifdef DEBUG
    Serial.println();
    Serial.print(F("getreply:"));
    Serial.println();
    Serial.print(getreply);
    Serial.println();
    Serial.print(F("repeatstore:"));
    Serial.println();
    Serial.print(repeatstore);
    Serial.println();
    #endif
    delay(1);//enero  1000
    if (getreply == 0)
    {
      ethconfig();
      getreply = 1;
    }
    if ((getreply == 1)  && (repeatstore == 0))
    { 
      
      char cbuff[] = SERIALA1; //1212(nocv 2015),1213(ene 2016),1982(oct 2016)
      cbuff[2] = serial_device[0];
      cbuff[3] = serial_device[1];
      cbuff[4] = serial_device[2];
      cbuff[5] = serial_device[3];
      #ifdef DEBUG
      Serial.print(F("send GET sin almacenar"));
      #endif
      #ifdef HTTP-GET
      ether.browseUrl(PSTR("/agcontrol/agc.php?"), cbuff, website, my_result_cb);
      #endif
      getreply = 0;
    }
    flagstore = 0;
  }

  if ((millis() > timer + REQUEST_RATE) or ((repeatstore == 1) && (millis() > timer + timer2 + 1000 - REQUEST_RATE_REPEAT )) ) {
    contsegs = 0;
    flagstore = 1;
    repeatstore = 0;
    timer = millis();
    getrequest = 1;
    timer2 = 0;
    ethconfig();
    releb_value  = digitalRead(releB);
    relec_value  = digitalRead(releC);
    reled_value  = 0;
    relee_value  = digitalRead(releE);
    sensor1();
    sensor2();
    char tbuff[16];
    memset( &tbuff, 0, sizeof(tbuff));
    char cbuff[] = SERIALA2; //1212(nocv 2015),1213(ene 2016),1981(oct 2016)
    cbuff[2] = serial_device[0];
    cbuff[3] = serial_device[1];
    cbuff[4] = serial_device[2];
    cbuff[5] = serial_device[3];
    //-----------------v1-------------------------
    dtostrf(sensor1Value, 2, 2, tbuff);
    dstart = 10;
    //start of replace
    dlen = 3;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));

    //-----------------v2-------------------------
    dtostrf(sensor2Value, 2, 2, tbuff);
    dstart = 18;
    //start of replace
    dlen = 3;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    //-----------------r1-------------------------
    dtostrf(releb_value, 2, 2, tbuff);
    dstart = 26;
    //start of replace
    dlen = 0;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    //-----------------r2-------------------------
    dtostrf(relec_value, 2, 2, tbuff);
    dstart = 31;
    //start of replace
    dlen = 0;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    //-----------------r3-------------------------
    dtostrf(reled_value, 2, 2, tbuff);
    dstart = 36;
    //start of replace
    dlen = 0;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    //-----------------r4-------------------------
    dtostrf(relee_value, 2, 2, tbuff);
    dstart = 41;
    //start of replace
    dlen = 0;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = tbuff[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    //-----------------rst-------------------------
    /** dtostrf(relee_value,2,2,tbuff);
     dstart = 47;
     //start of replace
     dlen = 0;
     for (int i=dstart; i <= dstart+dlen; i++)
         {
         cbuff[i] = tbuff[i-dstart];
         }
     memset( &tbuff, 0, sizeof(tbuff)); **/
    //-----------------ip-------------------------

    dstart = 52;
    //start of replace.
    dlen = 15;
    for (int i = dstart; i <= dstart + dlen; i++)
    {
      cbuff[i] = ipadd[i - dstart];
    }
    memset( &tbuff, 0, sizeof(tbuff));
    #ifdef DEBUG
    Serial.println();
    
    Serial.print(F("Buffer"));
    Serial.println();
    Serial.print(cbuff);
    Serial.println();    
    #endif
    #ifdef HTTP-GET
      #ifdef DEBUG
      Serial.print(F("Enviando! -->HTTP-GET ALMACENAR"));
      #endif
    ether.browseUrl(PSTR("/agcontrol/agc.php?"), cbuff, website, my_result_cb);
    #endif
   }
   word len = ether.packetReceive();// go receive new packets
   word pos = ether.packetLoop(len);// respond to incoming
   if (pos) { // check if valid tcp data is received
    char* data = (char *) Ethernet::buffer + pos;
    #ifdef DEBUG
    Serial.println(F("----------------"));
    Serial.println("data :");
    Serial.println(data);
    Serial.println(F("----------------"));
    #endif
    if (strncmp("GET /50", data, 7) == 0) {
       ether.httpServerReply(homePagephone()); // send web page data
       }
    if (strncmp("GET /RST", data, 8) == 0) {  //definir si es para version rele de estado solido o rele de bobina
      digitalWrite(releB, HIGH);
      digitalWrite(releC, HIGH);
      digitalWrite(releD, HIGH);
      digitalWrite(releE, HIGH);
      delay(500);
      digitalWrite(releB, LOW);
      digitalWrite(releC, LOW);
      digitalWrite(releD, LOW);
      digitalWrite(releE, LOW);
      EEPROM.write(6, 1);
      ether.httpServerReply(homePagephone()); // send web page data
    }
    if (strncmp("GET /RSTNTW", data, 11) == 0)
    {
      byte myip[] = { 192, 168, 55, 249 };
      EEPROM.write(7, 1);
      ethconfig();
    }
    //correcion:/IP=IPBYTE3.IPBYTE4.GATEWAYBYTE4
    if (strncmp("GET /IP=", data, 8) == 0) // 055.250.254    /IP=SUBNETBYTE3.IPBYTE4.GATEWAYBYTE4  {--myip[2]--gwip[2]} {--myip[3]} {---gwip[3]}    192.168.55.249    example   55.249.001
    {
      char myipok[] = "192.168.254";
      #ifdef DEBUG
      Serial.println("begin:");
      Serial.println(data[8]);
      #endif
      dstart = 8;
      dlen = 11;
      int j = -1;
      for (int i = dstart; i < dstart + dlen; i++)
      {
        j += 1;
        myipok[j] = data[i];
      }
      #ifdef DEBUG
      Serial.println("myipok:");
      Serial.println(myipok);
      #endif
      int myipok2[2];
      char *p = myipok;
      char *str;
      int i = -1;
      int number;
      while ((str = strtok_r(p, ".", &p)) != NULL) {
        i += 1;
        #ifdef DEBUG
        Serial.println(F("ip recibida"));
        Serial.println(str);
        #endif
        number = atoi( str );
        myipok2[i] = number;
        #ifdef DEBUG
        Serial.println(F("number:"));
        Serial.println(number);
        Serial.println(F("ip en array"));
        Serial.println(myipok2[i]);
        #endif
      }
      EEPROM.write(8, myipok2[0]);
      EEPROM.write(9, myipok2[1]);
      EEPROM.write(10, myipok2[2]);
      EEPROM.write(7, 3);
      ether.httpServerReply(homePagephone()); // send web page data
      #ifdef DEBUG
      Serial.println(F("restart"));
      #endif
      restart();
    }

  }

}
//********************************************************************************************************************************************************************************



