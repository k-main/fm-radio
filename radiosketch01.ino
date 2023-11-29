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

int disp_mode = 0;

void set_displaymode(int& current_mode) {
  if (current_mode == 0) {
    current_mode = 1;
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("~");
  } else {
    current_mode = 0;
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
  Serial.begin(9600);
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
  menu_init();
  rotary_init();
  last_clk = digitalRead(CLK);
}

void loop() {
  current_clk = digitalRead(CLK);
  if (current_clk != last_clk && current_clk == 1){
    if (digitalRead(DT) != current_clk) {
      counter ++;
      c_direction = "CW";
    } else {
      counter --;
      c_direction = "CCW";
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
      set_displaymode(disp_mode);
    }
    last_btn_press = millis(); 
  }
  delay(1);
  // put your main code here, to run repeatedly:
  // lcd.setCursor(11, 0);
  // lcd.print(millis() / 1000);
  
}
