#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>
#include <SoftwareSerial.h> //for GSM
//user interface
#include <Keypad.h>
#include <LiquidCrystal.h> 
 LiquidCrystal lcd(22,24,26,28,30,32);  //heeeeeeeeeeeeeeeeeeeeeeere

//GSM configuration
SoftwareSerial SIM900(10,11);  //heeeeeeeeeeeeeeeeeeeeeeere

int resPin = 12;  // for MAX heeeeeeeeeeeeeeeeeeeeeeeeeere
int mfioPin = 13;
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 
bioData body;  
int Scan = 0; // for Disconnection warning
int warning_HR = 0; // to avoid repeatings
int warning_SPO2 = 0; // to avoid repeatings
int warning_count =0; // for extendng the duration of buzzer and valve actions

//for Keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {35, 33, 31, 29}; //connect to the row pinouts of the keypad //watch out heeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeere
byte colPins[COLS] = {27, 25, 23}; //connect to the column pinouts of the keypad
int count=0;
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char key[12];   // This can hold up to 4 digits
int i=0;
long MobileNum, Min_SPO2 , Min_HR, Max_HR;
unsigned long num = 0;

int Valve = 7;   //heeeeeeeeeeeeeeeeeeeeeeeeeeeeere
int Buzzer = 2;

const byte Button = 3; //heeeeeeeeeeeeeeeeeeeeeeeere

int GSM_Auto = 4;  //heeeeeeeeeeeeeeeeeeeeeeeere
int GSM_Status = 8;

void setup(){

  pinMode(Valve,OUTPUT);
  pinMode(Buzzer,OUTPUT);
  digitalWrite(Buzzer,LOW);
  digitalWrite(Valve,HIGH);
  Serial.begin(115200);
  lcd.begin(16, 2);
  GSM_Setup();
  lcd.setCursor(0,0);
  lcd.print("Team 17");
  lcd.setCursor(0,1);
  lcd.print("Dr.Umar alqasemi");
  delay(2000);
  lcd.clear();  
  lcd.setCursor(0,0);
  lcd.print("Setting up...");
  pinMode(GSM_Status, INPUT);
  int result = digitalRead(GSM_Status); 
  if (result == 0)
  {
  digitalWrite(GSM_Auto, HIGH);
  delay(1000);
  digitalWrite(GSM_Auto, LOW);
  delay(5000);
  }
  UI();
  attachInterrupt(digitalPinToInterrupt(Button), OFF, CHANGE); //interrupt
    lcd.setCursor(0,0);
    lcd.print("Calebrating...");
    Serial.println("Calebrating...");
  MAX_Setup(); // be last (has while inside)
    
if (body.heartRate == 0)
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please, place");
    lcd.setCursor(0,1);
    lcd.print("your finger");
    Serial.println("Please, place your finger");
}
if (body.heartRate != 0 && body.oxygen == 0){  
lcd.clear();    
lcd.setCursor(0,0);
lcd.print("Calculating...");
Serial.println("Calculating...");                            
}

      while(body.heartRate == 0 || body.oxygen == 0 || body.heartRate > 220 || body.oxygen > 100 )
    {
        body = bioHub.readBpm();
        delay(250);
        Scan++;
           if ( Scan > 1000)
            {
                    Serial.println("Set your finger tightly");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Set your finger");
                    lcd.setCursor(0,1);
                    lcd.print("tightly");
                 while(body.heartRate == 0 || body.oxygen == 0)
                 {
                  body = bioHub.readBpm();
                  delay(250);
                           if (body.heartRate != 0 && body.oxygen == 0){  
                              lcd.clear();    
                              lcd.setCursor(0,0);
                              lcd.print("Calculating...");
                              Serial.println("Calculating...");} 
                 }
            }
         if (body.heartRate != 0 && body.oxygen == 0){  
            lcd.clear();    
            lcd.setCursor(0,0);
            lcd.print("Calculating...");  }                          
    }
    Scan = 0;// clear Disconnection warning flag

        lcd.clear(); 
        lcd.setCursor(0,0);
        lcd.print("Calculating...");
        Serial.println("Calculating...");
}

void OFF() {      // interrupt service routine 
  digitalWrite(Buzzer,LOW);
}

void MAX_Setup (){
    
    Wire.begin();
  int result = bioHub.begin();
  if (result == 0) //Zero errors!
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!!!");
 
  Serial.println("Configuring Sensor...."); 
  int error = bioHub.configBpm(MODE_TWO); // Configuring just the BPM settings. 
  if(error == 0){ // Zero errors
    Serial.println("Sensor configured.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }
 
  Serial.println("Loading up the buffer with data....");
  delay(4000); 

}

void GSM_Setup(){
  
  SIM900.begin(19200);
 // delay(20000);    // heeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeere
}

void UI(){

    lcd.setCursor(0,0);
    Serial.print("Enter Mobile No.  ");
    lcd.print("Enter Mobile No.  "); 
    lcd.setCursor(0,1);
    GetNumber();
    MobileNum = num;
    Serial.println (MobileNum);
    lcd.clear();
    
    lcd.setCursor(0,0); 
    lcd.print("Enter Min SPO2:   ");
    Serial.print("Enter Min SPO2:   "); 
    lcd.setCursor(0,1);
    GetNumber();
    Min_SPO2= num;
    Serial.println (Min_SPO2);  
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Enter Max HR:"); 
    Serial.print("Enter Max HR:"); 
    lcd.setCursor(0,1);
    GetNumber();
    Max_HR = num;
    Serial.println (Max_HR);
    lcd.clear(); 
    
    lcd.setCursor(0,0);
    lcd.print("Enter Min HR:"); 
    Serial.print("Enter Min HR:"); 
    lcd.setCursor(0,1);
     GetNumber();
    Min_HR = num;
    Serial.println (Min_HR);
    lcd.clear();  
   
    }

    
 int GetNumber()
{
    num = 0;
    char key = kpd.getKey();
    while(key != '#')
    {
        switch (key)
        {
            case NO_KEY:
            break;

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            lcd.print(key);
            num = num * 10 + (key - '0');
            break;

            case '*':
            num = 0;
            lcd.setCursor(0,1);
            lcd.print("                ");
            lcd.setCursor(0,1);
            break;
        }
        key = kpd.getKey();
    }
    //Serial.print(num); 
}




void loop(){


 
    body = bioHub.readBpm();
    delay(250);
   
    while(body.heartRate == 0 || body.oxygen == 0 || body.heartRate > 220 || body.oxygen > 100 )
    {
        body = bioHub.readBpm();
        delay(250);
        Scan++;
           if ( Scan > 105)
            {
               Serial.println("Please, place your finger");
                   lcd.clear();
                   lcd.setCursor(0,0);
                   lcd.print("Please, place");
                   lcd.setCursor(0,1);
                   lcd.print("your finger");
               while(body.heartRate == 0 || body.oxygen == 0 || body.heartRate > 220 || body.oxygen > 100 )
               {
                Scan++;
                body = bioHub.readBpm();
                delay(250);
                
                        if ( Scan > 320)
                   {
                        Serial.println("Sending disconnection message");
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Sending disc.");
                        lcd.setCursor(0,1);
                        lcd.print("message");
                        digitalWrite(Buzzer,HIGH);
                        sendSMS();
                                 while(body.heartRate == 0 || body.oxygen == 0 || body.heartRate > 220 || body.oxygen > 100 )
                              {
                                body = bioHub.readBpm();
                                delay(250);
                                    if ( body.extStatus == -1)
                                  {  
                                    lcd.clear(); 
                                    lcd.setCursor(0,0);
                                    lcd.print("Calculating...");
                                  }
                              }
                        digitalWrite(Buzzer,LOW);
                   }
                   if ( body.extStatus == -1)
               {  
                lcd.clear(); 
                lcd.setCursor(0,0);
                lcd.print("Calculating...");
               }
                                 }
                 if ( body.extStatus == -1)
           {  
             lcd.clear(); 
             lcd.setCursor(0,0);
             lcd.print("Calculating...");
           }
                                }
              if ( body.extStatus == -1)
          {  
            lcd.clear(); 
            lcd.setCursor(0,0);
            lcd.print("Calculating...");
          }
    }
    Scan = 0; // clear Disconnection warning flag

    lcd.clear(); 
    Serial.print("Heartrate: ");
    Serial.println(body.heartRate);
    lcd.setCursor(0,0);
    lcd.print((String)"Heartrate: "+body.heartRate);
    Serial.print("Oxygen: ");
    Serial.println(body.oxygen);
    lcd.setCursor(0,1);
    lcd.print((String)"Oxygen: "+body.oxygen+"%");
    delay(2500); 
    
    
    if ((body.heartRate < Min_HR ||  body.heartRate > Max_HR)&& body.oxygen > Min_SPO2 && warning_HR == 0) 
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Calling");
      Serial.println("Calling");
      digitalWrite(Buzzer,HIGH);
      call_and_message();
      warning_HR++;
      MAX_Setup();
      delay(5000);
      warning_count = 0;
    }

    if (body.oxygen < Min_SPO2 && warning_SPO2 == 0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Calling");
      Serial.println("Calling");
      digitalWrite(Buzzer,HIGH);
      digitalWrite(Valve,LOW);
      call_and_message();
      warning_SPO2++; 
      MAX_Setup();
      delay(5000);
      warning_count = 0;
    }

    if (body.heartRate > Min_HR && body.heartRate < Max_HR && body.oxygen > Min_SPO2)
    {
      warning_count++;
           if ( warning_count > 240)
           {
               digitalWrite(Buzzer,LOW); // if statment later to avoid turn off while emergency
               digitalWrite(Valve,HIGH); // if statment later to avoid turn off while emergency
               warning_count = 0;
               warning_HR = 0;
               warning_SPO2 = 0;
               lcd.clear();  
               lcd.setCursor(0,0);
               lcd.print("Excellent,");
               lcd.setCursor(0,1);
               lcd.print("getting better");
               delay(2000);
               Serial.println("Better");
           }
    }else{
      warning_count = 0;
    }
}




void call_and_message() {
  SIM900.println((String)"ATD + +966" + MobileNum + ";");           
  delay(100);
  SIM900.println();
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);
  SIM900.println((String)"AT + CMGS = \"+966" + MobileNum + "\"");
  delay(100);
    if (body.heartRate < Min_HR && body.oxygen > Min_SPO2)
  {
    SIM900.println("Heart rate is low!!");
  } 
    if(body.heartRate > Max_HR && body.oxygen > Min_SPO2)
  {
    SIM900.println("Heart rate is high!!");
  }
    if(body.oxygen < Min_SPO2)
  {
    SIM900.println("Oxygen saturation (SPO2) is low!!");
  }
  SIM900.println("Caution");
  SIM900.println((String)"Heartrate:" + body.heartRate +"  BPM");
  SIM900.println((String)"Oxygen Saturation (SPO2):" + body.oxygen +"%");
  delay(100);
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  delay(20000);
  SIM900.println("ATH"); // hang up
}

void sendSMS() {
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);
  SIM900.println((String)"AT + CMGS = \"+966" + MobileNum + "\"");
  delay(100);
  SIM900.println("Sensor got disconnected"); 
  delay(100);
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  delay(5000); 
}
