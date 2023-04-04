#include <Keypad.h>
#include <LiquidCrystal.h>
#include <TimeLib.h>
#include <RTClib.h> // include RTC library for communicating with RTC module
#include <NewPing.h> //Useful for controlling the ultrasonic sensor

RTC_DS3231 rtc; // create an instance of RTC


// Define the keypad pins and keys
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {10, 9, 8, 7};
byte colPins[COLS] = {6, 5, 4, 3};

// Define the LCD pins
LiquidCrystal lcd(23, 25, 27, 29, 31, 33);

// Define variables
unsigned long time_now = 0;
int echoPin = 11;
int trigPin = 12;
char password[] = "1234";
String systemState = "OFF";
int systemArmed = 0;
int pressed = 0;
int entryExit_trig = 0;
int period = 1000;
int buzzerPin = 2;
int bedroomPin = 39;
int kitchenPin = 41;

NewPing sensor(trigPin,echoPin, 400); //ultrasonic sensor is initialized


// Define the keypad and password variables
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char enteredPassword[5] = "";

void setup() {
  Wire.begin(); // start I2C communication
  rtc.begin(); // start RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");  
    while (1);
  }
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // Set up the LCD display
  lcd.begin(16, 2);
  Serial.begin(9600);

  // Set up inputs
  pinMode(kitchenPin, INPUT);
  pinMode(bedroomPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  
}

void loop() {
  DateTime now = rtc.now(); // get the current time from the RTC module
  lcd.setCursor(0, 0); // set the cursor to the first column of the first row
  lcd.print("Alarm System:");
  lcd.print(systemState); // ON or OFF
  String hour = String(now.hour()); String min = String(now.minute()); String day = String(now.day()); String month = String(now.month()); String year = String(now.year());
  int lenHOUR = hour.length(); int lenMIN = min.length(); int lenDAY = day.length(); int lenMONTH = month.length();
  if(lenHOUR==1) hour = "0"+hour;
  if(lenMIN==1) min = "0"+min;
  if(lenDAY==1) day = "0"+day;
  if(lenMONTH==1) month = "0"+month;
  lcd.setCursor(0, 1); // set the cursor to the first column of the second row
  lcd.print(hour); // print the current hour in decimal format
  lcd.print(':');
  lcd.print(min); // print the current minute in decimal format

  lcd.print(" "); // space
  
  lcd.print(day); // print the current month in decimal format
  lcd.print('/');
  lcd.print(month); // print the current month in decimal format
  lcd.print('/');
  lcd.print(year); // print the current year in decimal format

  keypad_lcd(); //Created using millis()

  lcd.clear();
  lcd.setCursor(0,0);
  
  entryExit_trig = sensor.ping_cm(); // takes distance from ultrasonic sensor
  int entryKitchen = digitalRead(kitchenPin); // 1 if there is a movement in the kitchen
  int entryBedroom = digitalRead(bedroomPin); // 1 if there is a movement in the bathroom

  while(systemArmed == 1 && entryBedroom == 1){
    lcd.print("Motion Detected");
    lcd.setCursor(0, 1);
    lcd.print("   BEDROOM   ");
    delay(3000);
    lcd.clear();
    Serial.println("BEDROOM"); // Send "BEDROOM" to Python 
    Serial.flush();
    serialFlush();
    while(true){
      lcd.print("Alarm has been");
      lcd.setCursor(0, 1);
      lcd.print("triggered!");
      lcd.setCursor(0,0);
      siren();
      int faceRec = face_check();
      
      if(faceRec == 1){
        lcd.clear();
        lcd.print("Alarm has been");
        lcd.setCursor(0,1);
        lcd.print("turned off!");
        delay(3000);
        systemArmed = 0;
        entryKitchen = 0;
        systemState = "OFF";
        digitalWrite(buzzerPin, LOW);
        break;
      }
    }
  }
  while(systemArmed == 1 && entryKitchen == 1){ 
    lcd.print("Motion Detected");
    lcd.setCursor(0, 1);
    lcd.print("   KITCHEN   ");
    delay(3000);
    lcd.clear();
    Serial.println("KITCHEN"); // Send "KITCHEN" to Python 
    Serial.flush();
    serialFlush();
    while(true){
      lcd.print("Alarm has been");
      lcd.setCursor(0, 1);
      lcd.print("triggered!");
      lcd.setCursor(0,0);
      siren();
      int faceRec = face_check();
      
      if(faceRec == 1){
        lcd.clear();
        lcd.print("Alarm has been");
        lcd.setCursor(0,1);
        lcd.print("turned off!");
        delay(3000);
        systemArmed = 0;
        entryKitchen = 0;
        systemState = "OFF";
        digitalWrite(buzzerPin, LOW);
        break;
      }
    }
  }

  while(systemArmed == 1 && entryExit_trig < 6){
    lcd.print("Motion Detected");
    lcd.setCursor(0, 1);
    lcd.print("  ENTRY EXIT  ");
    delay(3000);
    Serial.println("ENTRY_EXIT"); // Send "ENTRY_EXIT" to Python 
    Serial.flush();
    serialFlush();
    lcd.clear();
    for(int i = 10; i>=0; i--){ //Initiate countdown
      lcd.print(i);
      lcd.print(" seconds");
      lcd.setCursor(0,1);
      lcd.print("remaining.");
      delay(1000);
      int faceRec = face_check();
      lcd.clear();
      if(faceRec == 1){
        lcd.clear();
        lcd.print("Alarm has been");
        lcd.setCursor(0,1);
        lcd.print("turned off!");
        delay(3000);
        systemArmed = 0;
        entryKitchen = 0;
        systemState = "OFF";
        digitalWrite(buzzerPin, LOW);
        break;
      }            
    }
    while(systemState == "ON"){
      lcd.print("Alarm has been");
      lcd.setCursor(0, 1);
      lcd.print("triggered!");
      lcd.setCursor(0,0);
      siren();
      int faceRec = face_check();
      
      if(faceRec == 1){
        lcd.clear();
        lcd.print("Alarm has been");
        lcd.setCursor(0,1);
        lcd.print("turned off!");
        delay(3000);
        systemArmed = 0;
        entryKitchen = 0;
        systemState = "OFF";
        digitalWrite(buzzerPin, LOW);
      }
    }
  }  
}

void siren(){
  for (int i = 100; i <= 5000; i += 100) { // increase frequency from 100Hz to 5kHz
    digitalWrite(buzzerPin, HIGH); // turn on the speaker
    delayMicroseconds(500000 / i); // set the duration of the high state based on the frequency
    digitalWrite(buzzerPin, LOW); // turn off the speaker
    delayMicroseconds(500000 / i); 
}
}
void keypad_lcd() {
  time_now = millis();
  while(millis() < time_now + period)	{

    // Check if a key was pressed
    char key = keypad.getKey();

    if (key != NO_KEY) {

      // If the key is a number, add it to the entered password
      if (isdigit(key)) {
        if (strlen(enteredPassword) < 4) {
          enteredPassword[strlen(enteredPassword)] = key;
        }
      }
      // If the key is the #, check if the entered password is correct
      else if (key == '#') {
        if (strcmp(enteredPassword, password) == 0) {
          lcd.clear();
          lcd.print("Correct");
          lcd.setCursor(0, 1);
          lcd.print("Password!");
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);

          if (systemState == "OFF"){ systemState = "ON"; systemArmed = 1;} // OFF => ON
          else if(systemState == "ON"){ systemState = "OFF"; systemArmed = 0;} // ON => OFF
        }
        else {
          lcd.clear();
          lcd.print("Incorrect");
          lcd.setCursor(0, 1);
          lcd.print("Password");
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
        }
        // Reset the entered password
        memset(enteredPassword, 0, sizeof(enteredPassword));
        lcd.setCursor(0, 1);
      }
    }
    lcd.setCursor(0, 0);
  }
}

int face_check(){
  char input = '0';
  time_now = millis();
  while(millis() < time_now + period){
    
    if(Serial.available()) input = Serial.read();
    
    if(input == '1'){
      serialFlush(); //If you don't do that, serial reads 1 from its buffer
      // and disarms the alarm by itself 
      return 1;
    }

    if(input == '0') {
      serialFlush();
      return 0;
      }    
  }
}

void serialFlush(){ //Method to clear the Serial's buffer
  while(Serial.available() > 0) {
    char t = Serial.read(); // It reads all entries in the buffer, which results in deleting them.
  }
}