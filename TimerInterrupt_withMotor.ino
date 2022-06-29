/*
  Arduino Timer Interrupt Flash Demo
  timer-int-flash.ino
  Flash LED using Timer1

  DroneBot Workshop 2022
  https://dronebotworkshop.com/interrupts
*/
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 16);
// Define the LED pin
#define ledPin 13
#define en1Pin 5
#define dir1Pin 7
#define dir2Pin 8

volatile bool ledStatus = false;
bool prevStatus = false;
volatile int counts = 0;
unsigned long prevTime = 0;
unsigned long time;
float rpm = 0.0;
volatile bool dir;
// Define timer compare match register value
int timer1_compare_match;


ISR(TIMER1_COMPA_vect)
// Interrupt Service Routine for compare mode
{
  // Preload timer with compare match value
  TCNT1 = timer1_compare_match;
  ledStatus = !ledStatus;
  
}

void isr_1() {
  counts++;
  //dir = digitalRead(encB);
}

void setup()
{
  Serial.begin(115200);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  // Set LED as output
  pinMode(ledPin, OUTPUT);
  pinMode(en1Pin, OUTPUT);
  pinMode(dir1Pin, OUTPUT);
  pinMode(dir2Pin, OUTPUT);

  // Disable all interrupts
  noInterrupts();

  // Initialize Timer1
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_compare_match to the correct compare match register value
  // 256 prescaler & 31246 compare match = 2Hz
  timer1_compare_match = 15624;

  // Preload timer with compare match value
  TCNT1 = timer1_compare_match;

  // Set prescaler to 256
  TCCR1B |= (1 << CS12);

  // Enable timer interrupt for compare mode
  TIMSK1 |= (1 << OCIE1A);

  // Enable all interrupts
  interrupts();
  digitalWrite(dir1Pin, HIGH);
  digitalWrite(dir2Pin, LOW);
  analogWrite(en1Pin, 255);
}

void loop()
{
  if (ledStatus != prevStatus) {
    Serial.print(ledStatus);
    Serial.write(9);
    prevStatus = ledStatus;
    stuff();
  }
  digitalWrite(ledPin, ledStatus);
}

void stuff() {
  detachInterrupt(0);
  time = millis() - prevTime;
  rpm = ((float)(counts/3)/time)*60000./84.;
  /*
  Serial.write(9);
  Serial.print(dir);
  Serial.write(9);
  Serial.println(rpm, 6);
  */
  lcd.clear();
  lcd.setCursor(7, 0);
  if (dir == 0) lcd.print("CCW");
  if (dir == 1) lcd.print("CW");
  lcd.setCursor(7,1);
  lcd.print(rpm, 0);
  counts = 0;
  prevTime = millis();
  attachInterrupt(0,isr_1, RISING);
}
