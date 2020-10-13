//forked from https://web.archive.org/web/20200618173024/https://github.com/TylerGlaiel/voicemask 
//code by Tyler Glaiel
//for this mask: https://twitter.com/TylerGlaiel/status/1265035386109128704

//modifications for bsides pdx speaker gift:
// - port to smaller and more robust arduino pro micro
// - pull out pin assignments for easy modification
// - move pin assignments around to match
// - add pushbutton for on/standby switch
// - add power control of microphone for standby mode

#define LEDPIN 21
#define MICGND 15
#define MICVCC 14
#define MICPIN A0
#define SWGND 6
#define SWITCH 9
// BRIGHTNESS is passed to the matrix.brightness parameter. B is set in the individual colors. 
#define BRIGHTNESS 255
#define B 250


#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <gamma.h>

#define lengthof(A) ((sizeof((A))/sizeof((A)[0])))

const PROGMEM uint8_t mouth_0[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {6,6,6,6,6,6,6,6},
    {6,6,6,6,6,6,6,6},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
};

const PROGMEM uint8_t mouth_4[8][8] = {
    {0,0,6,6,6,6,0,0},
    {0,6,0,0,0,0,6,0},
    {6,0,0,0,0,0,0,6},
    {6,0,0,0,0,0,0,6},
    {6,0,0,0,0,0,0,6},
    {6,0,0,0,0,0,0,6},
    {0,6,0,0,0,0,6,0},
    {0,0,6,6,6,6,0,0}
};

const PROGMEM uint8_t mouth_3[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,0,6,6,6,6,0,0},
    {0,6,0,0,0,0,6,0},
    {6,0,0,0,0,0,0,6},
    {6,0,0,0,0,0,0,6},
    {0,6,0,0,0,0,6,0},
    {0,0,6,6,6,6,0,0},
    {0,0,0,0,0,0,0,0}
};

const PROGMEM uint8_t mouth_2[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,6,6,6,6,6,6,0},
    {6,0,0,0,0,0,0,6},
    {6,0,0,0,0,0,0,6},
    {0,6,6,6,6,6,6,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
};

const PROGMEM uint8_t mouth_1[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,6,6,0,0,0},
    {6,6,6,0,0,6,6,6},
    {6,6,6,0,0,6,6,6},
    {0,0,0,6,6,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
};

const PROGMEM uint8_t mouth_smile[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {6,0,0,0,0,0,0,6},
    {6,6,0,0,0,0,6,6},
    {0,6,6,6,6,6,6,0},
    {0,0,6,6,6,6,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
};

uint16_t palette[10] = {};
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, LEDPIN,
  NEO_MATRIX_BOTTOM     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS    + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

void drawImage(short image_addr){
    for(int x = 0; x<8; x++){
        for(int y = 0; y<8; y++){
            uint8_t index = pgm_read_byte(image_addr+x+y*8);
            matrix.drawPixel(x, y, palette[index]); 
        }
    }

    matrix.show();
}

int pop_detection = 0;
bool smiling = false;
unsigned long smiletimer = 0;
unsigned long last_face = 0;

int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

bool banner = true;
int x    = matrix.width();

void setup() {
  //LEDPIN connects to 8x8 matrix
    pinMode(LEDPIN, OUTPUT);
  //MICGND is a digital out set to low for mic GND
    pinMode(MICGND, OUTPUT);
    digitalWrite(MICGND, LOW);
  //MICVCC is a digital out, set to high to power on mic
    pinMode(MICVCC, OUTPUT);
    digitalWrite(MICVCC, HIGH);
  //MICPIN is analog input of mic
    pinMode(MICPIN, INPUT);
  //SWGND is the far end of the pushbutton that pulls button low when pressed.
    pinMode(SWGND, OUTPUT);
    digitalWrite(SWGND, LOW);
  //SWITCH is pushbutton input with internal pullup so defaults to high
    pinMode(SWITCH, INPUT_PULLUP);

    
    matrix.begin();

    palette[0] = matrix.Color(0,0,0);   // black
    palette[1] = matrix.Color(B,0,0);   // red
    palette[2] = matrix.Color(B,0,B/2); // pink
    palette[3] = matrix.Color(0,0,B);   // blue
    palette[4] = matrix.Color(0,B,B);   // aqua
    palette[5] = matrix.Color(B,0,B);   // purple
    palette[6] = matrix.Color(0,B,0);   // green
    palette[7] = matrix.Color(B,B,0);   // yellow
    palette[8] = matrix.Color(B,B/2,0); // orange
    palette[9] = matrix.Color(B,B,B);   // white

//    matrix.setBrightness(BRIGHTNESS);


    Serial.begin(9600);
}

float vol = 0;
const uint16_t samples = 128;

void loop() {
  // Low power mode through push button https://www.arduino.cc/en/tutorial/switch
  reading = digitalRead(SWITCH);    //check status of switch
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH)
      state = LOW;
    else
      state = HIGH;

    time = millis();    
  }

  digitalWrite(MICVCC, state);     // button toggles mic on & off
  previous = reading;
  if(state == LOW) {
    matrix.clear();                // if low power mode, clear display
    matrix.show();
    banner = true;                 // reset welcome banner
  }
  else {                           // resume normal operation:
    if(banner == true) {           // print welcome banner upon reset
      matrix.fillScreen(0);
      matrix.setCursor(x, 0);
      matrix.setTextColor(palette[2]);
      matrix.print(F("B"));
      matrix.setTextColor(palette[5]);
      matrix.print(F("S"));
      matrix.setTextColor(palette[3]);
      matrix.print(F("i"));
      matrix.setTextColor(palette[4]);
      matrix.print(F("d"));
      matrix.setTextColor(palette[6]); 
      matrix.print(F("e"));
      matrix.setTextColor(palette[7]);
      matrix.print(F("s"));
      matrix.setTextColor(palette[8]); 
      matrix.print(F("P"));
      matrix.setTextColor(palette[1]);
      matrix.print(F("D"));
      matrix.setTextColor(palette[9]); 
      matrix.print(F("X"));
      if(--x < -60) {
        x = matrix.width();
      }
      if(x == -60) {
        banner = false;
      }
        matrix.show();
        delay(100);
      }
    else {
      float nvol = 0;
      int previous_peak = -1;
      
      for (int i = 0; i<samples; i++){
          auto analog = analogRead(MICPIN);
          auto micline = abs(analog - 512);
  
          nvol = max(micline, nvol);
      }
  
      vol = (nvol + 1.0*vol)/2.0;
  
      if(nvol > 200){
          pop_detection += 1;
          if(pop_detection > 2) {
              smiling = false;
              last_face = millis();
          }
      } else {
          if(pop_detection > 0 && pop_detection <= 2) {
              if(millis() > last_face + 500){
                  smiling = true;
                  smiletimer = millis() + 2000;
              }
          }
          pop_detection = 0;
      }
  
      if(millis() > smiletimer) smiling = false;
  
      if(smiling){
          drawImage(mouth_smile);
      } else if(vol < 200){
          drawImage(mouth_0);
      } else if(vol < 250){
          drawImage(mouth_1);
      } else if(vol < 350){
          drawImage(mouth_2);
      } else if(vol < 450){
          drawImage(mouth_3);
      } else {
          drawImage(mouth_4);
      }
    }
  }
} 
