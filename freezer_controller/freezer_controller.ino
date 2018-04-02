/*
 * Program controlling freezer for thermal stuff
 * 
 * commands (case insensitive):
 * A automatic temprature output once a sec.
 * C+temp set temperature in celcius
 * K+temp set temperature in Kelvin
 * S get current set point
 * T current temp in Kelvins.
 * Z set current temp as setpoint.  
 * * 
 * Created 17 sept 2016 
 * Thomas S. Knutsen la3pna@gmail.com
 * 
 * Temperature reading based on example code from OneWire library by Paul J Stoffregen. 
 * Ethernet based on parts of the ethernet example by arduino.
 * This work is licenced by the  GNU GENERAL PUBLIC LICENSE.
 * 
 * Conversion to add WLAN 24 mar 16
 * 
 */

// Comment out the line below if you don't have ethernet connected.
// #define ethernet
#define wlan

 
#ifdef ethernet;
#include <SPI.h>        
#include <Ethernet.h>
#include <EthernetUdp.h> 
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
unsigned int localPort = 1990;      // local port to listen on
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
EthernetUDP Udp;
#endif

#ifdef wlan;
#endif

#include <OneWire.h>
OneWire  ds(19);  // on pin A5 (a 4.7K resistor is necessary)
//#define led 13
float temp, lasttemp, settemp;
unsigned long offtime = 0;
boolean runningcomp = false;
boolean automatic = false;
boolean serial = false;
byte addr[8];
int inData;
int cooler = 6;
int heater = 8;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(cooler, OUTPUT);
// pinMode(led, OUTPUT);
pinMode(heater, OUTPUT);
setuptemp();
// digitalWrite(led, LOW);
settemp = gettemp(); // set initial value to environemental temp to avoid it turning on. 
lasttemp = settemp;

#ifdef ethernet
Ethernet.begin(mac, ip);
  Udp.begin(localPort);
#endif

}


void loop() {
  // put your main code here, to run repeatedly:
#ifdef ethernet
int packetSize = Udp.parsePacket();
  if (packetSize) {
    //Udp.remotePort()
     Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
     inData=packetBuffer[0];
     packetBuffer[packetSize+1]=0x00;
     // Serial.write(inData);
  }
#endif
  
  if (Serial.available() > 0)   // see if incoming serial data:
  { 
    serial = HIGH;
    inData = Serial.read();  // read oldest byte in serial buffer:
  } 

 if (inData == 'C'||inData == 'c'){
  if(serial){
         settemp = (float)Serial.parseFloat()+ 273.15f;
         serial = false;
  }else{
    #ifdef ethernet
    settemp=(float)atof(&packetBuffer[1]) + 273.15f;
    
    #endif
    }
        inData = 0;
        }

 if (inData == 'A'||inData == 'a'){
        inData = 0;
        if(serial){
          serial = false;
        automatic = !automatic;
        }
 }
        
 if (inData == 'T'||inData == 't'){
        inData = 0;
        if(serial){
         serial = false;
        Serial.println(temp);
        }else{
          //need to return the temp here
   #ifdef ethernet
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.print(temp);
    Udp.endPacket();
    #endif
          }
 }

 if (inData == 'K'||inData == 'k'){
  if(serial){
         settemp = (float)Serial.parseFloat();
         serial = false;
      //   Serial.println(settemp);
  }else{
    #ifdef ethernet
    settemp = atof(&packetBuffer[1]);
  //  return settemp
     Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.print(settemp);
    Udp.endPacket();
    #endif
    }
        inData = 0;   
        }
 
  if (inData == 'S'||inData == 's'){
        inData = 0;  
        if(serial){
          serial = false;
         Serial.println(settemp);
      }
        else{
            #ifdef ethernet
       Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.print(settemp);
    Udp.endPacket();
    #endif
                   }
 }
        
    if (inData == 'Z'||inData == 'z'){
        inData = 0; 
        settemp = gettemp();     
        if(serial){
          serial = false;
        Serial.println(settemp);
        }else{
         #ifdef ethernet
                 Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
                 Udp.print(settemp);
                 Udp.endPacket();
                 #endif
                 }
  }

        
//fuzzy logic for bang-bang controller

  
 temp = gettemp();
 if(automatic){
Serial.println(temp);
 }

int tempint = (int)temp;
unsigned long timenow = millis();

  if(tempint < ((int)settemp)-1){
    // fire up the heater!
    digitalWrite(heater, HIGH);
    if(runningcomp == HIGH){
      offtime = timenow;
        digitalWrite(cooler, LOW);
        runningcomp = LOW;
        }
    }
    else if(tempint > ((int)settemp + 1)){
      // it needs to go colder, if the compressor would allow it without blowing a fuse...
      if((timenow - offtime >= 120000)&&( runningcomp == LOW)) {
    runningcomp = HIGH;
      digitalWrite(cooler, HIGH);
      }
      
       digitalWrite(heater, LOW);
      }
     else{
      
  digitalWrite(heater, LOW);
      if(runningcomp == HIGH){
      offtime = timenow;
        digitalWrite(cooler, LOW);
        runningcomp = LOW;
      } 
  }  
    
}
 
  
  
  
void setuptemp(){
   byte i;
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }
  }


float gettemp(){
byte present = 0;
  byte type_s;
  byte data[12];
   float kelvin;
   
   ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

 
  for ( int i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  kelvin = ((float)raw / 16.0)+ 273.15f;
return kelvin;
  }


