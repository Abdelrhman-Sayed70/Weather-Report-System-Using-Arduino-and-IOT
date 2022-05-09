//**include**//
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <SFE_BMP180.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

//*define*//
char auth[]="_8gDqN4pNeMi0PYWePv_cDpEinTNxGun";
char wifiname[] = "Gaber" ; //Galaxi co-working space
char wifipass[] = "123456789"; //88888888
#define ALTITUDE 90.0


//**Object**//
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(D3, DHT11) ;
SFE_BMP180 pressure;
WidgetLED led1(V4);

//  ldr  //
const int ldr_dig=D5;
const int ldr_anal=A0;

//  Rain  //
const int rain_dig=9;

//  DHT  // 
const int dht_pin=D3 ;

void setup()
{
  //  Blynk  // 
  Blynk.begin(auth, wifiname, wifipass);

  //  baud rate  //
  Serial.begin(115200) ;

  //  I2c module : used in lcd and bmP180 sensor  //
  Wire.begin(D2, D1);
  
  //  Connection  //
  WiFi.begin(wifiname,wifipass);  
  while (WiFi.status()!=WL_CONNECTED) {
  Serial.print(".");  
  delay(500) ; 
  }
  Serial.println("successful connected");
  Serial.println("----------------------") ;

  //  DHT  //
  pinMode(D3,OUTPUT); 

  //  Rain  //
  pinMode(rain_dig,INPUT) ;

  //  Buzzer  //
  pinMode(D8,OUTPUT) ;

  //  Led  //
  pinMode(10,OUTPUT) ;

  //  LCD Setup  // 
  lcd.init() ;
  lcd.backlight();
  lcd.clear();
  
  //  bmp180  //
  Serial.println("REBOOT");
  if (pressure.begin()) {
    Serial.println("BMP180 init success");
    Serial.println("----------------------") ; 
  }
  else
  {
    Serial.println("BMP180 init fail");
    Serial.println("----------------------") ;
    while(1); // Pause forever.
  }
 
}

void loop()
{
Blynk.run() ;
bool Israin=false;

//  Rain  //  
Serial.println("     RAIN      ") ;
Serial.print("Rain digital value : ");Serial.println(digitalRead(rain_dig)); 
if(digitalRead(rain_dig) == 0) 
{
    Serial.println("Rain:NO"); 
    Israin=false ;
    delay(100);  
}
else
{
    Serial.println("Rain:YES");
    //Blynk.notify("Rainning !");
    Israin=true ;
    delay(100);  
}
Serial.println("----------------------") ;  

//  DHT  //
delay(2000);
float humidity = dht.readHumidity() ; 
float temperature = dht.readTemperature();
Serial.println("     DHT      ") ;
Serial.print("Humidity: ");
Serial.print(humidity, 1);
Serial.print(" %");

Serial.print("\t\t");
 
Serial.print("Temp: ");
Serial.print(temperature, 1);
Serial.println(" C");
Serial.println("----------------------") ;

//  ldr  //
int value = analogRead(A0);
int ldrValue = map(value,0, 1024, 0, 100);
Serial.println("     LDR     ") ;
Serial.print("ldrValue : ");Serial.print(ldrValue);
Serial.print("  ") ;
 if (ldrValue < 10 )  {
    Serial.print("Light:Verybright");
  } else if (ldrValue < 40) {
    Serial.print("Light:Bright");
  } else if (ldrValue < 60) {
    Serial.print("Light:Light");
  } else if (ldrValue < 80 ) {
    Serial.print("Light: Dim");
  } else  {
    Serial.print("Light:Dark"); digitalWrite(D7,HIGH) ;
  }
Serial.println("----------------------") ;
delay(1000);


//  bmp180  //
  char status;
  double T,P,p0,a;
  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this project:
  
  //Serial.println();
  Serial.print("Altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.println(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");
  Serial.println("----------------") ;
  // You must first get a temperature measurement to perform a pressure reading.
  // Start a temperature measurement:
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature of bmp: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
      Serial.println("----------------") ;
      
      // Start a pressure measurement:
      
      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is variable P(pressure).
        // Note also that the function requires the previous temperature measurement (T).
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");
          Serial.println("------------------------");

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          
          p0 = pressure.sealevel(P,ALTITUDE); // we're at 90 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          /*Serial.print(p0,2);
          Serial.print(" mb, ");*/
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.println(" meters, ");
          Serial.println("-------------------") ;
        }
        else Serial.println("error retrieving pressure measurement");
      }
      else Serial.println("error starting pressure measurement");
    }
    else Serial.println("error retrieving temperature measurement");
  }
else Serial.println("error starting temperature measurement");
 
//  OUTPUT lcd and Notifications //


// Tempreture  //
lcd.setCursor(8,0) ;
lcd.print("T:");lcd.print(temperature, 1);lcd.print(" C ") ;

if (temperature>=30) {Blynk.notify("Temprature is HOT !");}
else if (temperature>=25) {Blynk.notify("Temprature is COOL !");}
else if (temperature>0){Blynk.notify("Temprature is COLD !");}

//  Rain //
if (digitalRead(rain_dig)==1) { lcd.setCursor(0,0); lcd.print("R:YES");digitalWrite(D8,HIGH) ;
led1.on();
}
else if (digitalRead(rain_dig)==0){lcd.setCursor(0,0); lcd.print("R:NO");digitalWrite(D8,LOW) ;
led1.off(); 
}


//  Humadity  //
lcd.setCursor(0,1) ;
lcd.print("H:");lcd.print(humidity, 1);lcd.print(" % ");

delay(3000);
lcd.clear() ;


//  LDR  //
lcd.setCursor(0,0) ;
 if (ldrValue < 10 )  {
    lcd.print("Light:Verybright");
    digitalWrite(10,LOW) ;
  } else if (ldrValue < 40) {
    lcd.print("Light:Bright");
    digitalWrite(10,LOW) ;
  } else if (ldrValue < 60) {
    lcd.print("Light:Light");
    digitalWrite(10,LOW) ;
  } else if (ldrValue < 80 ) {
    lcd.print("Light: Dim");
    digitalWrite(10,LOW) ;
  } else  {
    lcd.print("Light:Dark"); digitalWrite(10,HIGH) ;
  }

//  pressure  //
lcd.setCursor(0,1) ;
lcd.print("P:");lcd.print(P,2);lcd.print(" Hpa");

delay(3000) ;   
lcd.clear() ;


  Blynk.virtualWrite(V0, temperature);  //V0 is for Temperature
  Blynk.virtualWrite(V1,humidity );  //V1 is for Humidity
  Blynk.virtualWrite(V4,digitalRead(rain_dig) );  //V2 is for Rainfall
  ldrValue = map(value,0, 1024, 100, 0); 
  Blynk.virtualWrite(V2,ldrValue );
  ldrValue = map(value,0, 1024, 0, 100); 


}
