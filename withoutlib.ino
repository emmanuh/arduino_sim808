#include <SoftwareSerial.h>
#include "dht.h"
 // Analog Pin sensor is connected to
 

#define DEBUG true
#define dht_apin A0

dht DHT;
SoftwareSerial sim808(10,11);

char phone_no[] = "+254712738104"; // replace with your phone no.
String data[5];
String state,timegps,latitude,longitude;

int hum;
int temp;


void setup() {
    sim808.begin(9600);
    Serial.begin(9600);
    delay(50);
    
    sim808.println("AT+CSMP=17,167,0,0");  // set this parameter if empty SMS received
    delay(500);
    sim808.println("AT+CMGF=1\r");
    delay(400);

    sim808.println("AT+CNMI=2,2,0,0,0"); //set how to handle incoming sms
    delay(400);
    
    sendData("AT+CGNSPWR=1",1000,DEBUG);
    delay(50);
    sendData("AT+CGNSSEQ=RMC",1000,DEBUG);
    delay(150);

    sim808.println("AT+CNMI=2,2,0,0,0"); //set how to handle incoming sms
    delay(400);

    sendData("AT+CMGL=\"ALL\"",1000,DEBUG);
    delay(1000);

    /*sendData("AT+CMGD=1,4",1000,DEBUG);
    delay(50);
    Serial.println("Deleted all the messages");
    */

    sendData("AT+CPMS=?",1000,DEBUG);
    delay(50);

     sendData("AT+CGNSINF",1000,DEBUG);

}

void loop() {
    reset();
    checkHudAndTemp();
 
    //readSMS( "AT+CMGL=\"ALL\"" , 1000 , true);
   delay(100);
    sendTabData("AT+CGNSINF",1000,DEBUG);
    
    if (state !=0) {
        Serial.println("State  :"+state);
        Serial.println("Time  :"+timegps);
        Serial.println("Latitude  :"+latitude);
        Serial.println("Longitude  :"+longitude);
        Serial.print("Temperature  :");
        Serial.print(temp);
        Serial.println(" C  ");
        Serial.print("Humidity  :");
        Serial.print(hum);
        Serial.println("%  ");

        if (temp>24){
            Serial.println("Sending SMS  ");
            sendSMS();
            delay(30000);
          }
    
    } else {
        Serial.println("GPS Initializing…");
    }
}

void checkHudAndTemp(){
   
    DHT.read11(dht_apin);
    hum = (float) DHT.humidity;
    temp = (float) DHT.temperature;
    
    delay(2000);//Wait 2 seconds before accessing sensor again.
    
}
void readSMS(String command , const int timeout , boolean debug){
    sim808.println(command);
    long int time = millis();
    int i = 0;
    
    while((time+timeout) > millis()){
        while(sim808.available()){
            char c = sim808.read();
            if (c != ',') {
                
                data[i] +=c;
                delay(100);
            } else {
                 i++;
            }
           /* if (i == 7) {
                delay(100);
                //goto exitL;
            } */
        }

        Serial.println("data[0]  :"+data[0]);
        Serial.println("data[1]  :"+data[1]);
        Serial.println("data[2]  :"+data[2]);
        Serial.println("data[3]  :"+data[3]);
        Serial.println("data[4]  :"+data[4]);
        Serial.println("data[5]  :"+data[5]);
    }
    exitL:

      if ( debug) { //Check whether its a valid GPS DATA  i.e Starting with : AT+CGNS

        Serial.println("data[0]  :"+data[0]);
        Serial.println("data[1]  :"+data[1]);
        Serial.println("data[2]  :"+data[2]);
        Serial.println("data[3]  :"+data[3]);
        Serial.println("data[4]  :"+data[4]);
        Serial.println("data[5]  :"+data[5]);
          
      } else {
        ///
      }
    
}
void sendSMS(){
    sim808.print("AT+CMGS=\"");
    sim808.print(phone_no);
    sim808.println("\"");
    
    delay(300);

    sim808.print("The temperatures in the truck is greater than 24 ; Temperature : ");
    sim808.print(temp );
    sim808.print("C  ");
    sim808.print(" humidity : ");
    sim808.print(hum );
    sim808.println("%  ");
    
    sim808.print("\nhttp://maps.google.com/maps?q=loc:");
    sim808.print(latitude);
    sim808.print(",");
    sim808.print (longitude);
    delay(200);
    sim808.println((char)26); // End AT command with a ^Z, ASCII code 26
    delay(200);
    sim808.println();
    delay(20000);  // DELAY FOR 1/3 A MUNUTE(60000)
    sim808.flush();

}

void sendTabData(String command , const int timeout , boolean debug){

    sim808.println(command);
    long int time = millis();
    int i = 0;
    int x = 0;
    
    while((time+timeout) > millis()){
        while(sim808.available()){
            char c = sim808.read();
            if (c != ',') {
                
                data[i] +=c;
                delay(100);
            } else {
                 i++;
            }
            if (i == 5) {
                delay(100);
                goto exitL;
            }
        }
    }
    
    exitL:

      if ( data[0].substring(0,7).equals("AT+CGNS")) { //Check whether its a valid GPS DATA  i.e Starting with : AT+CGNS

          state = data[1];
          timegps = data[2];
          latitude = data[3].substring(0,8);
          longitude =data[4].substring(0,8);
      } else {
        Serial.println("Invalid response "+data[0]);
      }
}
String sendData (String command , const int timeout ,boolean debug){
    String response = "";
    sim808.println(command);
    long int time = millis();
    int i = 0;
    
    while ( (time+timeout ) > millis()){
        while (sim808.available()){
            char c = sim808.read();
            response +=c;
        }
    }
    if (debug) {
       Serial.print(response);
    }
    return response;
}

void reset(){
          data[0] = "";
          state = data[1] = "";
          timegps = data[2] = "";
          latitude = data[3] = "";
          longitude =data[4] = "";  
          hum = temp=0;
}
