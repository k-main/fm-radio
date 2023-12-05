#include <Wire.h>
#include <LiquidCrystal.h>
#define CLK 2
#define DT 3
#define SW 4
#define cs A0



/*
PLL value formula -> N = 4(F(RF) + F(IF)) / F(REF)
where IF = 225*10^3 and REF = 32768
*/

const int RS = 5, EN = 6, D4 = 7, D5 = 8, D6 = 9, D7 = 10;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

int last_clk, current_clk, btn_state, counter = 0;
unsigned long last_btn_press = 0;
String c_direction;

typedef enum {
  CONTROL_STATION,
  CONTROL_VOLUME
} ControlState;

ControlState control = CONTROL_STATION;
int disp_mode = 0;

void set_controlstate(ControlState& current_state) {
  if (current_state == CONTROL_STATION) {
    current_state = CONTROL_VOLUME;
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("~");
  } else {
    current_state = CONTROL_STATION;
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(0, 0);
    lcd.print("~");
  }
}

void menu_init() {
  lcd.begin(16,2);
  lcd.print("~ Station: ");
  lcd.setCursor(11, 0);
  lcd.print("102.7");
  lcd.setCursor(2, 1);
  lcd.print("Vol: 100%");
}
void rotary_init() {
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(cs,OUTPUT);
}

void radio_init() {

  double frequency = 99.8; //starting Frequency;
  unsigned char frequencyH = 0;
  unsigned char frequencyL = 0;
  unsigned int frequencyB;

  frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
  frequencyH = frequencyB >> 8;
  frequencyL = frequencyB & 0XFF;


  Wire.begin();
  Wire.beginTransmission(0x60); // 0X60 is the module address
  Wire.write(frequencyH);
  Wire.write(frequencyL);
  Wire.write(0xB0);
  Wire.write(0x10);
  Wire.write((byte)0x00);
  Wire.endTransmission();
}

void setup() {
  // put your setup code here, to run once:
  // radio_init();
  Serial.begin(9600);
  menu_init();
  rotary_init();
  last_clk = digitalRead(CLK);
  radio_init();
}

void loop() {
  current_clk = digitalRead(CLK);
  if (current_clk != last_clk && current_clk == 1){
    if (digitalRead(DT) != current_clk) {
      counter ++;
      c_direction = "CW";
      // ADVANCE FORWARDS
      if (control == CONTROL_STATION){
        Serial.print("increasing station");
      } else {
        Serial.print("increasing vol");
      }
    } else {
      counter --;
      c_direction = "CCW";
      // ADVANCE BACKWARDS
      if (control == CONTROL_STATION){
          Serial.print("decreasing station");
        } else {
          Serial.print("decreasing vol");
        }
    }
    Serial.print("Direction: ");
    Serial.print(c_direction);
    Serial.print(" | Counter: ");
    Serial.println(counter);
  }
  last_clk = current_clk;

  // parse button press
  btn_state = digitalRead(SW);
  if (btn_state == LOW){
    if (millis() - last_btn_press > 50){
      Serial.println("Click.");
      set_controlstate(control);
    }
    last_btn_press = millis(); 
  }
  delay(1);
  // put your main code here, to run repeatedly:
  // lcd.setCursor(11, 0);
  // lcd.print(millis() / 1000);
  
}
