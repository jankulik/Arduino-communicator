#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include "RF24.h"

//Configuration of a particular radio (0 or 1)
bool radioNumber = 1;

//Pins where NRF is connecte (CE, CSN)
RF24 radio(14,15);

byte addresses[][6] = {"1Node","2Node"};
bool role = 0;

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

//Pins where keypad is connected
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

String message_str;
int char_num = 0;

bool menu = true;
bool sending = false;
bool receiving = false;
bool message = false;
bool fail = false;

int page = 1;
int page_rec = 1;

char text[1] = "";
String received;

void setup()
{                
  Serial.begin(11520);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);

  if(radioNumber)
  {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }
  
  else
  {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }

  radio.startListening();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
}


void loop() 
{
  char customKey = customKeypad.getKey();
  
  if(menu)
  {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("A - new message");
    display.println("B - send");
    if(message) display.println("* - receive (!)");
    else display.println("* - receive");
    display.println("D - menu");

    if(customKey && customKey == 'A')
    {
      display.clearDisplay();
      display.setCursor(0, 0);
      menu = false;
      sending = true;
    }

    if(customKey && customKey == '*' && message)
    {
      display.clearDisplay();
      display.setCursor(0, 0);
      menu = false;
      receiving = true;
    }
  }
  
  if(sending && customKey && char_num < 252)
  {
    if(customKey == '#' || customKey == '0' || customKey == '1' || customKey == '2' || customKey == '3' || customKey == '4' || customKey == '5' || customKey == '6' || customKey == '7' || customKey == '8' || customKey == '9')
    {
      if((page == 1 && char_num > 83) || (page == 2 && char_num > 167))
      {
        display.clearDisplay();
        display.setCursor(0, 0);
        page++;
      }

      if(customKey == '0') display.print(" ");
      else if(customKey == '#') display.print("/");
      else display.print(customKey);
      
      message_str += customKey;
      char_num ++;
    }

    else if(customKey == 'D')
    {
      menu = true;
      sending = false;
    }

    else if(customKey == 'B')
    {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Sending...");

      String paczka;
      char text[1];
      
      for(int i = 0; i <= char_num; i++)
      {
        paczka = message_str[i];
        paczka.toCharArray(text, 2);
        paczka = "";
          
        radio.stopListening();
        if(!radio.write(&text, sizeof(text))) fail = true;
        radio.startListening();

        if(fail)
        {
          break;
          display.clearDisplay();
          display.setCursor(0, 0);
          display.print("Sending failed");
          display.display();
          delay(3000);
          menu = true;
          sending = false;
        }
      }
      
      if(!fail)
      {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Message sent");
        display.display();
        delay(3000);
        menu = true;
        sending = false; 
      }
      else fail = false;

      char_num = 0;
      message_str = "";
    }
  }

  if(receiving)
  {
    int i, j;
    if(page_rec == 1) { i = 0; j = 83; }
    else if(page_rec == 2) { i = 84; j = 167; }
    else if(page_rec == 3) { i = 168; j = 251; }

    display.clearDisplay();
    display.setCursor(0, 0);
    
    while(i <= j)
    {
      display.print(received[i]);
      i++;
    }

    if(customKey && customKey == '*' && page_rec > 1)
    {
      page_rec--;
    }

    else if(customKey && customKey == '#' && page_rec < 3)
    {
      page_rec++;
    }

    else if(customKey && customKey == 'D')
      {
        display.clearDisplay();
        display.setCursor(0, 0);
        received = "";
        menu = true;
        receiving = false;
        message = false;
      }
  }

  if(radio.available())
  {
    while(radio.available()) 
    {
      radio.read(&text, sizeof(text));
      received += text[0];
    }
    message = true;
  }

  display.display();
}

