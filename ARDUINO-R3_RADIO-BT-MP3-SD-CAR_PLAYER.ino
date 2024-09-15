//-----------------------------SOURCES------------------------------------//
    /* https://forum.arduino.cc/t/doesnt-stay-in-sub-menu/850619 */      // This is where I got the code for buttons and the basic idea for the menu.
    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||//      // - Marking code sourced from a website
//-------------------------------BUGS------------------------------------// Although the code contained a bug, it has been fixed in my implementation.
/*
BUG #1: One call to the updateMenu() function is not enough to refresh the display at startup, so it must be used twice.
*/
//-----------------------------LIBRARIES----------------------------------//
#include <TEA5767.h>                                                     // I2C library for the radio module // Modified for custom use
#include <Wire.h>                                                        // Wire library for I2C communication (Uno, Nano = A4 (SDA), A5 (SCL))
#include <LiquidCrystal_I2C.h>                                           // I2C library for the LCD display
#include "SoftwareSerial.h"                                              // DFPlayer serial communication library
#include "DFRobotDFPlayerMini.h"                                         // DFPlayer library
//-------------------------------PINS------------------------------------//
#define buttonSelect 2                                                   // PIN 2 - Button
#define buttonUp 3                                                       // PIN 3 - Button
#define buttonDown 4                                                     // PIN 4 - Button
#define radio_pin 5                                                      // PIN 5 - Radio
#define buttonright 6                                                    // PIN 6 - Button
#define buttonleft 7                                                     // PIN 7 - Button
const int chipSelect = 10;                                               // PIN 10 - Button
SoftwareSerial mySoftwareSerial(12, 13);                                 // PIN RX, TX - MP3 communication
//------------------------------VARIABLES---------------------------------//
DFRobotDFPlayerMini myDFPlayer;                                          // Assigning function name DFPlayer
TEA5767 radio = TEA5767();                                               // Assigning function name TEA5767
LiquidCrystal_I2C lcd(0x27, 16, 2);                                      // Assigning LCD address, pins
int menu, menur, menup, file = 1;                                        // Variables for the menu, file for DFPlayer
int counter, count, lolan = 0;                                           // Counters, bug workaround
bool muted = true;                                                       // Mute status variable
bool mp_player, state_radio, bt_player = false;                          // MP3 player detection
bool playing = false;                                                    // MP3 playback detection
void(*resetArduino) (void) = 0;                                          // Arduino reset function
const float radio_float[25] = { 107.90, 105.90, 105.00, 103.70, 102.50, 101.60, 101.10, 99.70, 99.30, 98.70, 98.10, 96.60, 96.20, 95.70, 95.30, 94.60, 93.70, 92.60, 91.90, 91.20, 90.70, 90.30, 89.50, 88.20, 87.80 };

char* radio_stations[25] =
{
  "Dalnice",
  "RockZone",
  "Vltava",
  "RockRadio",
  "Frekvence",
  "HeyRadio",
  "RadioZ",
  "Bonton",
  "RFI",
  "Classic",
  "Kiss",
  "Impuls",
  "Spin",
  "Signal",
  "Beat",
  "Zurnal",
  "Hitradio",
  "Rozhlas",
  "Radio 1",
  "Dvojka",
  "ColorMusic",
  "Expres",
  "Country",
  "Evropa 2",
  "Blanik"
};
//-------------------------------LCD_CHAR-------------------------------//
byte upchar[] = 
{                                                                       // Special characters for menu styling [coding style]
  B00100,  B01110,  B11111,  B11111,  B11011,  B10001,  B00000,  B00000
};
byte downchar[] = 
{
  B00000,  B00000,  B10001,  B11011,  B11111,  B11111,  B01110,  B00100
};
byte leftuc[] = 
{
  B11100,  B10000,  B10000,  B10000,  B10000,  B10000,  B00000,  B00000
};

byte leftdc[] = 
{
  B00000,  B10000,  B10000,  B10000,  B10000,  B10000,  B10000,  B11110
};

byte rightuc[] = 
{
  B01111,  B00001,  B00001,  B00001,  B00001,  B00001,  B00001,  B00000
};

byte rightdc[] = 
{
  B00001,  B00001,  B00001,  B00001,  B00001,  B00001,  B00001,  B01111
};
//-------------------------------SETUP----------------------------------//
void setup()
{
  pinMode(A1, OUTPUT);                                                  // Pin initialization
  pinMode(A2, OUTPUT);                                                  // Pin initialization
  digitalWrite(A1, LOW);                                                // Set logic 0 (off)
  digitalWrite(A2, LOW);                                                // Set logic 0 (off)
  pinMode(buttonUp, INPUT_PULLUP);                                      // Pin initialization
  pinMode(buttonDown, INPUT_PULLUP);                                    // Pin initialization
  pinMode(buttonSelect, INPUT_PULLUP);                                  // Pin initialization
  pinMode(buttonleft, INPUT_PULLUP);                                    // Pin initialization
  pinMode(buttonright, INPUT_PULLUP);                                   // Pin initialization
  pinMode(radio_pin, OUTPUT);                                           // Pin initialization
  mp_player_state();                                                    // DFPlayer initialization
  radio_state(false);                                                   // Turn off radio
  lcdstart();                                                           // LCD initialization
  updateMenu();                                                         // Update function to first menu
  updateMenu();                                                         // Bug #1 fix
}
//--------------------------------LOOP----------------------------------//
void loop()
{
  LCD_updater();
}
//----------------------------Startup-Functions-------------------------//

void lcdstart()                                                         // Startup function for display
{
  lcd.init();
  lcd.clear();
  lcd.backlight();
}
void radiostart()
{
  radio.init(6);                                                        // Minimum required signal (0-5 = unusable, 6-9 = weak, 10-12 = good, 13-14 = very good, 15 = excellent)
  muted = true;
}

//--------------------------------Functions------------------------------//
void updateMenu()                                                       // Basic menu divided into 4 functions
{

  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:                                                          // Radio
      lcd.clear();
      menu_style(1, 1, 1, 1, 0, 1);
      lcd.setCursor(5, 0);
      lcd.print(F(">Radio"));
      lcd.setCursor(6, 1);
      lcd.print(F(" MP3"));
      break;
    case 2:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(5, 0);
      lcd.print(F(" Radio"));
      lcd.setCursor(6, 1);
      lcd.print(F(">MP3"));                                   // MP3 Player
      break;
    case 3:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      if (bt_player == false)
      {
        lcd.setCursor(3, 0);
        lcd.print(F(">Start BT"));
      }

      if (bt_player == true)
      {
        lcd.setCursor(4, 0);
        lcd.print(F(">Stop BT"));                                   // If Bluetooth module is on, menu displays option to turn it off
      }
      lcd.setCursor(5, 1);
      lcd.print(F(" Reset"));
      break;
    case 4:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 0);
      if (bt_player == false)                                       // If Bluetooth module is off, menu displays option to turn it on
      {
        lcd.setCursor(3, 0);
        lcd.print(F(" Start BT"));
      }

      if (bt_player == true)
      {
        lcd.setCursor(4, 0);
        lcd.print(F(" Stop BT"));
      }

      lcd.setCursor(5, 1);
      lcd.print(F(">Reset"));                                      // Reset Arduino
      break;
    case 5:
      menu = 4;                                                     // Return back to case 4
      break;
  }
}
void executeAction()                                                // Function to execute commands selected by the user from the menu
{

  switch (menu) 
  {
    case 1:
      digitalWrite(A1, LOW);
      digitalWrite(A2, LOW);
      Serial.end();
      mySoftwareSerial.end();
      actionradio();
      break;
    case 2:                                                         
      digitalWrite(A1, HIGH);                                       // Turn on MP3 player
      digitalWrite(A2, LOW);
      delay(500);
      mySoftwareSerial.begin(9600);                                 // MP3 player's own serial communication
      Serial.begin(250000);
      delay(1000);
      actionsongs();
      break;
    case 3:
      if (bt_player == false)
      {
        digitalWrite(A1, LOW);
        radio_state(false);
        digitalWrite(A2, HIGH);
        bt_player = true;
      }
      else
      {
        digitalWrite(A2, LOW);
        bt_player = false;
      }
      break;
    case 4:
      resetArduino();
      break;
  }
}
//-------------------------------Radio-Menu-----------------------------//
void actionradio() 
{
  lcd.clear();
  bt_player = false;
  if (state_radio == false)
    radio_state(true);

  counter = 1;
  menur = 0;
  lolan = 2;
  menuRadio();
}

void actionsongs() 
{
  lcd.clear();
  bt_player = false;
  if (state_radio == true)
    radio_state(false);

  counter = 2;
  menup = 0;
  lolan = 3;
  updatePlayerMenu();
}

void menuRadio() 
{
  switch (menur) 
  {
    case 0:
      menur = 1;
      break;
    case 1:
      lcd.clear();
      menu_style(1, 1, 1, 1, 0, 1);
      lcd.setCursor(5, 0);
      lcd.print(F(">Next"));
      lcd.setCursor(5, 1);
      lcd.print(F(" Find"));
      break;
    case 2:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(5, 0);
      lcd.print(F(" Next"));
      lcd.setCursor(5, 1);
      lcd.print(F(">Find"));
      break;
    case 3:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(2, 0);
      lcd.print(F(">"));
      lcd.print(radio_stations[count]);
      lcd.setCursor(2, 1);
      lcd.print(F(" Frequency"));
      break;
    case 4:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(2, 0);
      lcd.print(F(" "));
      lcd.print(radio_stations[count]);
      lcd.setCursor(2, 1);
      lcd.print(F(">Frequency"));
      break;
    case 5:
      lcd.clear();

      if (muted == true)
      {
        menu_style(1, 1, 1, 1, 1, 1);
        lcd.setCursor(4, 0);
        lcd.print(F(">Muted"));
        lcd.setCursor(5, 1);
        lcd.print(F(" Back"));
      }
      else
      {
        menu_style(1, 1, 1, 1, 1, 1);
        lcd.setCursor(3, 0);
        lcd.print(F(">Unmuted"));
        lcd.setCursor(5, 1);
        lcd.print(F(" Back"));
      }
      break;
    case 6:
      lcd.clear();
      if (muted == true)
      {
        menu_style(1, 1, 1, 1, 1, 1);
        lcd.setCursor(4, 0);
        lcd.print(F(" Muted"));
        lcd.setCursor(5, 1);
        lcd.print(F(">Back"));
      }
      else
      {
        menu_style(1, 1, 1, 1, 1, 0);
        lcd.setCursor(3, 0);
        lcd.print(F(" Unmuted"));
        lcd.setCursor(5, 1);
        lcd.print(F(">Back"));
      }
      break;
    case 7:
      menur = 6;
      break;
  }
}
//-------------------------------Radio-Functions------------------------//
void executeRadio() 
{
  switch (menur) {
    case 1:
      actionnextr();
      break;
    case 2:
      actionfind();
      break;
    case 3:
      actionset();
      break;
    case 4:
      actionget();
      break;
    case 5:
      actionmute();
      break;
    case 6:
      actionback();
      break;
  }
}

void actionnextr()                                      // Radio function for next station (requires found stations) with menu
{
  lcd.clear();
  menu_style(1, 1, 1, 1, 0, 0);
  radio.nextStation();
  lcd.setCursor(5, 0);
  lcd.print(F(">Next"));
  lcd.setCursor(4, 1);
  lcd.print(radio.getFrequency());
  delay(1500);
}

void actionfind()                                       // Radio function for finding stations with menu
{                                       
  int i = 0;
  if (muted == false)                                   // Save current speaker state
    i = 0;
  else if (muted == true)
    i = 1;
  lcd.clear();
  menu_style(1, 1, 1, 1, 0, 0);
  lcd.setCursor(3, 0);
  lcd.print(F(">Finding.."));
  radio.setMuted(false);
  muted = false;
  radio.findStations(6);
  if (i == 1)                                           // Apply previous speaker state
  {
    radio.setMuted(true);
    muted = true;
  }
  else if (i == 0)
  {
    radio.setMuted(false);
    muted = false;
  }

  lcd.clear();
  menu_style(1, 1, 1, 1, 0, 0);
  lcd.setCursor(5, 0);
  lcd.print(F(">Found "));
  lcd.setCursor(3, 1);
  lcd.print(F("Stations "));
  lcd.print(radio.getStations());
  delay(1000);
}

void actionset()
{
  radio.setFrequency(radio_float[count]);                               // Manual station setting
}

void actionget()                                                        // Display current station frequency
{
  lcd.clear();
  menu_style(1, 1, 1, 1, 0, 0);
  lcd.setCursor(2, 0);
  lcd.print(F(">Frequency"));
  lcd.setCursor(5, 1);
  lcd.print(radio.getFrequency());                                      // Display FM frequency of playing station on the screen
  delay(1500);
}

void actionmute()                                                       // Function to mute radio output
{                                                                       // Identify previous state
  lcd.clear();
  int i = 0;
  if (muted == true && i == 0)
  {
    radio.setMuted(false);
    muted = false;
  }
  else {
    radio.setMuted(true);
    muted = true;
  }

}

void actionback()                                                           // Return to main menu (module selection)
{
  lcd.clear();
  counter = 0;
  lolan = 1;
}
//-------------------------------MP3-MENU-------------------------------//
void updatePlayerMenu()                                                     // MP3 player menu display
{                                                                           // Exact commands are defined in the function below
  switch (menup) {
    case 0:
      menup = 1;
      break;
    case 1:
      lcd.clear();
      menu_style(1, 1, 1, 1, 0, 1);
      lcd.setCursor(4, 0);
      lcd.print(F(">Play "));
      lcd.print(file);
      lcd.setCursor(5, 1);
      lcd.print(F(" Next"));
      break;
    case 2:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(4, 0);
      lcd.print(F(" Play "));
      lcd.print(file);
      lcd.setCursor(5, 1);
      lcd.print(F(">Next"));
      break;
    case 3:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(3, 0);
      lcd.print(F(">Previous"));
      if (playing == true)
      {
        lcd.setCursor(5, 1);
        lcd.print(F(" Pause"));

      }

      else
      {
        lcd.setCursor(3, 1);
        lcd.print(F(" Continue"));
      }

      break;
    case 4:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(3, 0);
      lcd.print(F(" Previous"));
      if (playing == true)
      {
        lcd.setCursor(5, 1);
        lcd.print(F(">Pause"));

      }

      else
      {
        lcd.setCursor(3, 1);
        lcd.print(F(">Continue"));
      }

      break;
    case 5:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      if (playing == true)
      {
        lcd.setCursor(5, 0);
        lcd.print(F(" Pause"));
      }

      if (playing == false)
      {
        lcd.setCursor(3, 0);
        lcd.print(F(" Continue"));
      }
      lcd.setCursor(4, 1);
      lcd.print(F(">Eq None"));
      break;
    case 6:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(4, 0);
      lcd.print(F(">Eq Pop"));
      lcd.setCursor(4, 1);
      lcd.print(F(" Eq Rock"));
      break;
    case 7:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(4, 0);
      lcd.print(F(" Eq Pop"));
      lcd.setCursor(4, 1);
      lcd.print(F(">Eq Rock"));
      break;
    case 8:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(4, 0);
      lcd.print(F(">Eq Jazz"));
      lcd.setCursor(3, 1);
      lcd.print(F(" Eq Class"));
      break;
    case 9:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(4, 0);
      lcd.print(F(" Eq Jazz"));
      lcd.setCursor(3, 1);
      lcd.print(F(">Eq Class"));
      break;
    case 10:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 1);
      lcd.setCursor(4, 0);
      lcd.print(F(">Eq Bass"));
      lcd.setCursor(5, 1);
      lcd.print(F(" Back"));
      break;
    case 11:
      lcd.clear();
      menu_style(1, 1, 1, 1, 1, 0);
      lcd.setCursor(4, 0);
      lcd.print(F(" Eq Bass"));
      lcd.setCursor(5, 1);
      lcd.print(F(">Back"));
      break;
    case 12:
      menup = 11;
      break;
  }
}
void executePlayerAction()
{
  switch (menup) 
  {
    case 1:                                                                     // Play selected song (automatically 1 at startup)
      myDFPlayer.volume(30);
      myDFPlayer.play(file);
      playing = true;
      break;
    case 2:
      myDFPlayer.volume(30);                                                    // Play the next song in the file list
      myDFPlayer.next();
      playing = true;
      break;
    case 3:
      myDFPlayer.volume(30);                                                    // Play the previous song
      myDFPlayer.previous();
      playing = true;
      break;
    case 4:
      if (playing == true)
      {
        myDFPlayer.pause();                                                     // Pause the music
        playing = false;
        updatePlayerMenu();
      }

      else
      {
        myDFPlayer.volume(30);
        myDFPlayer.start();                                                     // Resume the music where it left off
        playing = true;
        updatePlayerMenu();
      }
      break;
    case 5:
      myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);                                          // Initial equalizer setting
      break;
    case 6:
      myDFPlayer.EQ(DFPLAYER_EQ_POP);                                             // Pop music equalizer
      break;
    case 7:
      myDFPlayer.EQ(DFPLAYER_EQ_ROCK);                                            // Rock and roll equalizer
      break;
    case 8:
      myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);                                            // Jazz equalizer
      break;
    case 9:
      myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);                                         // Classic music equalizer
      break;
    case 10:
      myDFPlayer.EQ(DFPLAYER_EQ_BASS);                                            // Bass equalizer
      break;
    case 11:
      actionback();
      break;
  }
}
//----------------------------Status-Helper-Functions--------------------//
void radio_state(bool state)                                                      // Function for radio feedback and switching
{                                                                                 // Automatically mutes the sound
  if (state == true)                                                             
  {
    digitalWrite(radio_pin, HIGH);
    delay(100);
    radiostart();
    radio.setMuted(muted);
    state_radio = true;
  }
  else if (state == false)
  {
    digitalWrite(radio_pin, LOW);
    state_radio = false;
  }

}

void mp_player_state()                                                           // Function for MP3 player status feedback
{
  if (!myDFPlayer.begin(mySoftwareSerial))
  {
    mp_player = false;
  }
  else
  {

    mp_player = true;
  }
}
void menu_style(int lu, int ld, int ru, int rd, int up, int down)               // Function for special characters
{                                                                               // Each logic 1 represents a special character in the menu
                                                                                // Refreshed with each movement in the menu
  if (lu == 1)
  {
    lcd.createChar(2, leftuc);
    lcd.setCursor(1, 0);
    lcd.write(2);
  }

  if (ld == 1)
  {
    lcd.createChar(3, leftdc);
    lcd.setCursor(1, 1);
    lcd.write(3);
  }

  if (ru == 1)
  {
    lcd.createChar(4, rightuc);
    lcd.setCursor(14, 0);
    lcd.write(4);
  }

  if (rd == 1)
  {
    lcd.createChar(5, rightdc);
    lcd.setCursor(14, 1);
    lcd.write(5);
  }

  if (up == 1)
  {
    lcd.createChar(0, upchar);
    lcd.setCursor(13, 0);
    lcd.write(0);
  }

  if (down == 1)
  {
    lcd.createChar(1, downchar);
    lcd.setCursor(13, 1);
    lcd.write(1);
  }
}
void LCD_updater()
{
  if (count < 0)                                                        // Function for array overflow
  {
    count = 24;
    menuRadio();
  }
  else if (count > 24)                                                  // Function for array overflow
  {
    count = 0;
    menuRadio();
  }

  if (file < 1)                                                        // Function for variable overflow
  {
    file = 99;
    updatePlayerMenu();
  }
  else if (file > 99)
  {
    file = 1;
    updatePlayerMenu();
  }
  //-------------------------------MENU-STATUS-DISPLAY----------------------------------//
  if (counter == 0 && lolan == 1 && counter != 1 && counter != 2)     //
  {
    updateMenu();
    lolan = 0;
  }

  else if (counter == 1 && lolan == 2 && counter != 0 && counter != 2)
  {
    menuRadio();
    lolan = 0;
  }

  else if (counter == 2 && lolan == 3 && counter != 0 && counter != 1)
  {
    updatePlayerMenu();
    lolan = 0;
  }
  //-------------------------------BUTTONS------------------------------------//
  //|||||||||||||||||||||||||||||||||||||||||||||||||||||||// sourced code from the website  
  if (counter == 0 && counter != 1)
  {
    if (!digitalRead(buttonDown)) {
      menu++;
      updateMenu();
      while (!digitalRead(buttonDown));
    }
    if (!digitalRead(buttonUp)) {
      menu--;
      updateMenu();;
      while (!digitalRead(buttonUp));
    }
    if (!digitalRead(buttonSelect)) {
      executeAction();
      updateMenu();
      while (!digitalRead(buttonSelect));
    }
  }
  else if (counter == 1 && counter != 0)
  {

    if (!digitalRead(buttonDown)) {
      menur++;
      menuRadio();
      while (!digitalRead(buttonDown));
    }
    if (!digitalRead(buttonUp)) {
      menur--;
      menuRadio();
      while (!digitalRead(buttonUp));
    }

    if (!digitalRead(buttonleft)) {
      count--;
      menuRadio();
      while (!digitalRead(buttonleft));
    }
    if (!digitalRead(buttonright)) {

      count++;
      menuRadio();
      while (!digitalRead(buttonright));
    }

    if (!digitalRead(buttonSelect)) {
      executeRadio();
      menuRadio();
      while (!digitalRead(buttonSelect));
    }
  }

  else if (counter == 2 && counter != 0 && counter != 1)
  {

    if (!digitalRead(buttonDown)) {
      menup++;
      updatePlayerMenu();
      while (!digitalRead(buttonDown));
    }
    if (!digitalRead(buttonUp)) {
      menup--;
      updatePlayerMenu();
      while (!digitalRead(buttonUp));
    }
    if (!digitalRead(buttonSelect)) {
      executePlayerAction();
      updatePlayerMenu();
      while (!digitalRead(buttonSelect));
    }



    if (!digitalRead(buttonleft)) {
      file--;
      updatePlayerMenu();
      while (!digitalRead(buttonleft));
    }
    if (!digitalRead(buttonright)) {

      file++;
      updatePlayerMenu();
      while (!digitalRead(buttonright));
    }
  }
  //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||//
}
