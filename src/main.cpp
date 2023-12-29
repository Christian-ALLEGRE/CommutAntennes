// CommutAntenneRpiPico.ino
#define VERSION "V1.2a"
//
// Programme : Commutateur d'Antennes + PA VHF 500W
// Conçu par : F4LAA 
// Version   : V2.0 (15/09/2023)
//
// Programme inspiré de : 
//   Contrôleur de commutateur Coaxial 8:1 Ameritron (ou similaire), Version 1.
//   Utilise un Raspberry Pi Pico. Développé sous Arduino IDE.
//   Électro-Bidouilleur, Juin 2023. https://www.youtube.com/Électro-Bidouilleur
//   Licence de droits: Attribution 4.0 International (CC BY 4.0)  Lisez https://creativecommons.org/licenses/by/4.0/deed.fr
//
// =========================================================================================================================
// V1.0 (14/09/2023)
//
// TFTArduino : Ecran Elegoo 320 x 240 couleur + TouchScreen
//   Programme Arduino gérant l'affichage LCD du commutateur (boutons Commut H/V en HF et VHF, PA Enable/Disable, PA On/Off)
//   Dérivé du programme exemple Elegoo, il se trouve sur ACSOFTDVLP 
//   dans E:\ACSoft\DVLP\arduino-0022\ACSoft\Afficheurs\TFT\TFTArduino
//   Utilise une version modifiée de la librairie EleGoo_GFX (.cpp, .h) 
//     fonctions containsV() et containsH() (au lieu de contains() dans la librairie d'origine)
//
// =========================================================================================================================
// V2.0 (14/09/2023)
//
// Portage Arduino vers Raspeberry Pico
//   CommutAntenneRpiPico.ino
//   ==> Ce programme est la version Rpi Pico
//
// RpiPico+TFT Elegoo et Librairie GFX Library for Arduino V1.3.7 par "Moon On Our Nation...""
// https://github.com/moononournation/Arduino_GFX
// Trouvé par : https://www.instructables.com/ArduinoGFX/
//
// Adaptation F4LAA : 
//    Arduino_RPiPicoPAR8.h, .c :
//     Ajout de la méthode int32_t Arduino_RPiPicoPAR8::digitalPinToBitMask(int32_t pin) 
//
//   Adaptation Arduino_GFX.h, .c :
//     Ajout classe Arduino_GFX_Button (repris de Elegoo_GFX_Button)
//
//   Adaptation Arduino_GFX_Library.h :
//     Mise en comentaire des databus/xxxx inutilisés 
//       Arduino_HWSPI, Arduino_RPiPicoPAR16, Arduino_RPiPicoSPI,  
//
// La define TARGET_RP2040 est active à la compilatiopn sur RpiPico.
// 
// =========================================================================================================================
//#include <EEPROM.h>

// Based on ELEGOO TouhScreen
#include <TouchScreenOnRpiPico.h> // by F4LAA

#define XP 0   // GPIO0 (Sur Arduino, c'était 8 = D0) can be a digital pin
#define YM 1   // GPIO1 (Sur Arduino, c'était 9 = D1) can be a digital pin
#define YP 26  // GPIO26 = ADC0 (Sur Arduino, c'était A3)  // must be an analog pin, use "An" notation!
#define XM 27  // GPIO27 = ADC1 (Sur Arduino, c'était A2)  // must be an analog pin, use "An" notation!

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define MINPRESSURE 75
#define MAXPRESSURE 300

// Touch For New ILI9341 TP / ADC RpiPico
// ATTENTION X, Y Ecran = Y, X TouchPad (à cause de la rotation de 270° non prise en compte par le TP)
//
// Pour le Y de l'écran (qui utilise le X du touchPad)
#define TS_MINX 786
#define TS_MAXX 3426

// Pour le X de l'écran (qui utilise le Y du touchPad)
#define TS_MINY 587 
#define TS_MAXY 3413

int xmin = 32000;
int xmax = 0;
int ymin = 32000;
int ymax = 0;

// TFT LCD connecté en // 8 bits (D0 ... D7 sur GPIO0 ... GPIO7)
// https://www.instructables.com/ArduinoGFX/
#include <Arduino_GFX_Library.h>

// Voici les connexions sur un Rasprerry Pi Pico. Les numéros indiqués correspondent aux appellations GPxx du brochage.
#define LCD_RD 8 // GPIO8 (Sur Arduino, c'était A1) // LCD Write goes to Analog 1
#define LCD_WR 9 // GPIO9 (Sur Arduino, c'était A0) // LCD Read goes to Analog 0
#define LCD_RESET 10 // GPIO10 (Sur Arduino, c'était A4) // Can alternately just connect to Arduino's reset pin
#define LCD_CS 26 // GPIO26_ADC0 (Sur Arduino, c'était A3, Must be Analog) // Chip Select goes to Analog 3
#define LCD_CD 27 // GPIO27_ADC1 (Sur Arduino, c'était A2 // Command/Data goes to Analog 2

//Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// Replaced by :
// Display D0-D7 connect to GPIO 0-7.
Arduino_DataBus *bus = new Arduino_RPiPicoPAR8(LCD_CD, LCD_CS, LCD_WR, LCD_RD); 
// More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class 

Arduino_GFX *tft = new Arduino_ILI9341(bus, LCD_RESET, 0 , false );
// More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******************* UI details */
#define BUTTON_X 48
#define BUTTON_Y 52
#define BUTTON_W 95 // 60
#define BUTTON_H 55 // 30
#define BUTTON_SPACING_X 15 // 20
#define BUTTON_SPACING_Y 10
#define BUTTON_TEXTSIZE 3

// We have a setHeader line for like, is FONA working
#define posHeader_X 0
#define posHeader_Y 10

// text box where numbers go
#define TEXT_X 1
#define TEXT_Y 218 // 10
#define TEXT_W 318 // 220
#define TEXT_H 20
#define TEXT_TSIZE 2
#define TEXT_TCOLOR ILI9341_YELLOW

#define RelaiAlimPA 11  // GPIO11
#define DTemp 14        // GPIO14
#define PttOut 18       // GPIO18
#define PttIn 19        // GPIO19
#define RelaiPA 20      // GPIO20
#define RelaiVHF 21     // GPIO21
#define RelaiHF 22      // GPIO22

String msg = "";

// #include <Arduino_GFX_Button.h>
Arduino_GFX_Button buttons[9];

/* create 15 buttons, in classic candybar phone style */
char buttonlabels[9][15] = {"HF", " Horiz.50 MHz", " Verti. CP6", "VHF", " Horiz.144 MHz", " Verti. X50", "PA", "On", "Off" };
uint16_t buttoncolors[9] = {ILI9341_CYAN, ILI9341_LIGHTGREY, ILI9341_GREEN,
                            ILI9341_PINK, ILI9341_LIGHTGREY, ILI9341_GREEN, 
                             ILI9341_LIGHTGREY, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY };
uint16_t buttonTextcolors[9] = {ILI9341_BLUE, ILI9341_BLACK, ILI9341_BLACK,
                                ILI9341_RED, ILI9341_BLACK, ILI9341_BLACK,
                                ILI9341_BLACK, ILI9341_WHITE, ILI9341_WHITE };
uint16_t buttonTextsizes[9] = { 5, 2, 2,
                                5, 2, 2,
                                5, 4, 4 };
                             
Arduino_GFX_Button mButtons[9];
char mButtonlabels[9][6] = {"Exit", "?", "?", "?", "?", "?", "?", "?", "?" };
uint16_t mButtoncolors[9] = {ILI9341_OLIVE, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY,
                            ILI9341_LIGHTGREY, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY, 
                             ILI9341_LIGHTGREY, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY };
uint16_t mButtonTextcolors[9] = {ILI9341_WHITE, ILI9341_WHITE, ILI9341_WHITE,
                                ILI9341_WHITE, ILI9341_WHITE, ILI9341_WHITE,
                                ILI9341_WHITE, ILI9341_WHITE, ILI9341_WHITE };

void setHeader(String msg) {
  tft->fillRect(posHeader_X, posHeader_Y, 240, 8, ILI9341_BLACK);
  tft->setCursor(posHeader_X, posHeader_Y);
  tft->setTextColor(ILI9341_WHITE);
  tft->setTextSize(1);
  tft->print(msg);
}

void setMessage(String s) {
  //Serial.println(s);
  tft->setCursor(TEXT_X + 3, TEXT_Y+3);
  tft->setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
  tft->fillRect(TEXT_X + 1, TEXT_Y + 1, TEXT_W - 2, TEXT_H - 2, ILI9341_BLACK); 
  tft->setTextSize(TEXT_TSIZE);
  tft->print(s);
}

/* */
enum eState {
  RUN, 
  CONFIG
  };
eState curState;
/* */

void clearButtons()
{
  tft->fillRect(0, BUTTON_Y - (BUTTON_H / 2), 320, BUTTON_Y - (BUTTON_H / 2) + 3 * (BUTTON_H + BUTTON_SPACING_Y), BLACK);
  // create 'text field'
  tft->drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE); 
}

void drawRunButtons()
{
  for (uint8_t row=0; row<3; row++) {
    for (uint8_t col=0; col<3; col++) {
      buttons[col + row*3].drawButton();
    }
  }
}

void drawConfigButtons()
{
  for (uint8_t row=0; row<3; row++) {
    for (uint8_t col=0; col<3; col++) {
      mButtons[col + row*3].drawButton();
    }
  }
}
void updateUI()
{
  clearButtons();

  switch (curState) {
    case RUN:
      drawRunButtons();
      break;
    case CONFIG:
      drawConfigButtons();
      break;
  }
}

void setup(void) {
  Serial.begin(9600);
  Serial.println(String("Commutateur d'Antennes et PA / Rpi Pico ") + VERSION);

  // Gestion du Commutateur d'antenne
  pinMode(RelaiHF, OUTPUT);                              
  digitalWrite(RelaiHF, LOW);                            // Les broches de relais au niveau bas pour commencer
  pinMode(RelaiVHF, OUTPUT);                              
  digitalWrite(RelaiVHF, LOW);                           // Les broches de relais au niveau bas pour commencer

  // Gestion du PA
  pinMode(RelaiAlimPA, OUTPUT);                              
  digitalWrite(RelaiAlimPA, LOW);                        // Les broches de relais au niveau bas pour commencer
  pinMode(RelaiPA, OUTPUT);                              
  digitalWrite(RelaiPA, LOW);                            // Les broches de relais au niveau bas pour commencer
  pinMode(DTemp, INPUT);                                 // Capteur Numérique de température du radiateur du PA

  // Gestion du TX (Port CI-V sur un fil ==> UART0)
  pinMode(PttIn, INPUT);                                 // La broche PttIn initalisée en entrée
  pinMode(PttOut, OUTPUT);                               // La broche PttOut initalisée en sortie

  // ADC en 12 bits (default is 10)
  analogReadResolution(12);

  //EEPROM.begin(256);                                      // Démarrer l'instance d'accès EEPROM (émulation dans la Flash)
  //while (!Serial && (millis() <= 1000));                  // Une pause d'au moins 1 seconde

  // Init Display
  if (!tft->begin())
  {
    //Serial.println("tft->begin() failed!");
  }
  tft->setRotation(1); // 0 = 0°, 1 = 90°, 2 = 180°, 3 = 270° 
  tft->fillScreen(BLACK);
  
  // create buttons
  for (uint8_t row=0; row<3; row++) {
    for (uint8_t col=0; col<3; col++) {
      buttons[col + row*3].initButton(tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                  BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[col+row*3], buttonTextcolors[col+row*3],
                  buttonlabels[col + row*3], buttonTextsizes[col+row*3]); 
    }
  }

  // create Menu buttons
  for (uint8_t row=0; row<3; row++) {
    for (uint8_t col=0; col<3; col++) {
      mButtons[col + row*3].initButton(tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                  BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, mButtoncolors[col+row*3], mButtonTextcolors[col+row*3],
                  mButtonlabels[col + row*3], BUTTON_TEXTSIZE); 
    }
  }
  
  clearButtons(); // Draw TextField
  drawRunButtons();
  setHeader(String("Commutateur Antennes & PA 500W 144MHz    F4LAA ") + VERSION);
  setMessage("Commutateur Ant+PA   F4LAA"); // 16 char max
  curState = RUN;
}

bool HorizHF = false;
bool HorizVHF = false;
bool sHorizVHF = false;
bool PAEnabled = false;
bool PAOn = false;

int cptPA = 0;
int cptPA2 = 0;

void setHFOn(bool on)
{
  if (on)
    digitalWrite(RelaiHF, HIGH);
  else
    digitalWrite(RelaiHF, LOW);
}

void setVHFOn(bool on)
{
  if (on)
    digitalWrite(RelaiVHF, HIGH);
  else
    digitalWrite(RelaiVHF, LOW);
}

void manageRun(TSPoint p)
{
  // now we can ask the buttons if their state has changed
  for (uint8_t b=0; b<9; b++) {    
    if (buttons[b].contains(p.x, p.y)) {      
      //Serial.print("Pressing: "); Serial.println(b);
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  for (uint8_t b=0; b<9; b++) {
    // if (buttons[b].justPressed()) 
    //   buttons[b].drawButton(true);  // draw invert!

    if (buttons[b].justReleased()) {
      // buttons[b].drawButton();  // draw normal
      switch (b)
      {
        case 0:
          curState = CONFIG;
          updateUI();
          msg = "Configuration";
          break;
        case 1:
          msg = "HF 100W Horiz Yagi 50MHz.";
          HorizHF = true;
          break;
        case 2:
          msg = "HF 100W Verticale CP6";
          HorizHF = false;
          break;
        case 4:
          msg = "VHF 50W Horiz Yagi 144MHz";
          HorizVHF = true;
          break;
        case 5: // Vertical 144/432
          msg = "VHF 50W Verticale X50";
          HorizVHF = false;
          sHorizVHF = HorizVHF;
          PAOn = false; // Pas de PA sur la X50
          break;
        case 6: // Enable / Disable PA
          PAEnabled = ! PAEnabled;
          if (!PAEnabled)
          {
            msg = "PA Disabled";
            PAOn = false;
            HorizVHF = sHorizVHF; // On remet la Polar VHF d'avant 
            buttons[b].setColor(ILI9341_LIGHTGREY, ILI9341_WHITE);  
            setAlimPAOn(false);  // On étaint l'alim du PA
          }
          else 
          {
            msg = "PA Enabled";
            buttons[b].setColor(ILI9341_GREEN, ILI9341_BLACK);   
            cptPA = 0;
            cptPA2 = 0;        
            setAlimPAOn(true);  // On allume l'alim du PA
          }
          break;

        case 7:
          if (!PAEnabled)
            msg = "Enable PA first";
          else {
            msg = "PA On 500W Horiz 144MHz";
            PAOn = true;
            sHorizVHF = HorizVHF;
            HorizVHF = true; // On force la Yagi 144
          }
          break;

        case 8:
          if (!PAEnabled)
            msg = "PA disabled: Already Off";
          else {
            msg = "PA Off";
            PAOn = false;
            HorizVHF = sHorizVHF; // On remet la Polar VHF d'avant
            if (HorizVHF)
              msg += " VHF 50W H";
            else
              msg += " VHF 50W V";
          }
          break;
      }

      // update the current text field
      setMessage(msg);

      if (curState == RUN) {
        int row, col;
        if (HorizHF)
        {
          setHFOn(true); // On allume le relai HF
          row = 0;
          col = 1;
          buttons[col + row*3].setColor(ILI9341_GREEN, ILI9341_BLACK);
          col = 2;
          buttons[col + row*3].setColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
        } else {
          setHFOn(false); // On éteind le relai HF
          row = 0;
          col = 1;
          buttons[col + row*3].setColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
          col = 2;
          buttons[col + row*3].setColor(ILI9341_GREEN, ILI9341_BLACK);
        }

        if (HorizVHF)
        {
          setVHFOn(true); // On allume le relai VHF
          row = 1;
          col = 1;
          buttons[col + row*3].setColor(ILI9341_GREEN, ILI9341_BLACK);
          col = 2;
          buttons[col + row*3].setColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
        } else {
          // Vertical VHF
          setVHFOn(false); // On éteind le relai VHF
          row = 1;
          col = 1;
          buttons[col + row*3].setColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
          col = 2;
          buttons[col + row*3].setColor(ILI9341_GREEN, ILI9341_BLACK);
        }

        if (PAOn)
        {
          // PA On
          setPAOn(true);
          row = 2;
          col = 1;
          buttons[col + row*3].setColor(ILI9341_GREEN, ILI9341_BLACK);
          col = 2;
          buttons[col + row*3].setColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
        } else {
          // PA Off
          setPAOn(false);
          row = 2;
          col = 1;
          if (PAEnabled)
            buttons[col + row*3].setColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
          else
            buttons[col + row*3].setColor(ILI9341_LIGHTGREY, ILI9341_WHITE);

          col = 2;
          if (PAEnabled)
            buttons[col + row*3].setColor(ILI9341_RED, ILI9341_BLACK);
          else
            buttons[col + row*3].setColor(ILI9341_LIGHTGREY, ILI9341_WHITE);
        }

      }
      delay(250); // UI debouncing
    }
  }
}

void manageConfig(TSPoint p)
{
  for (uint8_t b=0; b<9; b++) {    
    if (mButtons[b].contains(p.x, p.y)) {      
      //Serial.print("Pressing: "); Serial.println(b);
      mButtons[b].press(true);  // tell the button it is pressed
    } else {
      mButtons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  for (uint8_t b=0; b<9; b++) {
    // if (mButtons[b].justPressed()) 
    //   mButtons[b].drawButton(true);  // draw invert!

    if (mButtons[b].justReleased()) {
      // mButtons[b].drawButton();  // draw normal
      switch (b)
      {
        case 0:
          curState = RUN;
          updateUI();
          msg = "Running...";
          break;
      }

      // update the current text field
      setMessage(msg);

      if (curState == CONFIG) {
        // Todo...
      }
      delay(250); // UI debouncing
    }
  }
}

TSPoint getTouchPoint()
{
  TSPoint p = ts.getPoint();
  //Serial.print("X="); Serial.print(p.x); Serial.print(" Y="); Serial.print(p.y); Serial.print(" Z="); Serial.println(p.z); 

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  /* */
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->ADC_MAX to tft->width
    //
    // Si Vertical (Rot 2 = 180°)
    //p.x = map(p.x, TS_MINX, TS_MAXX, tft->width(), 0);
    //p.y = (tft->height()-map(p.y, TS_MINY, TS_MAXY, tft->height(), 0));

    /* Update min/max */
    if (p.x < xmin) xmin = p.x;
    if (p.x > xmax) xmax = p.x;
    if (p.y < ymin) ymin = p.y;
    if (p.y > ymax) ymax = p.y;
    /* */

    // Si Horiz (Rot 3 = 270°)
    int x = (tft->width()-map(p.y, TS_MINY, TS_MAXY, 0, tft->width()));
    int y = (tft->height()-map(p.x, TS_MINX, TS_MAXX, 0, tft->height()));
    // Rotation 270°
    //p.x = x;
    //p.y = y;
    // Rotation 90°
    p.x = tft->width() - x;
    p.y = tft->height() - y;
    //Serial.print("W="); Serial.print(tft->width()); Serial.print(" H="); Serial.print(tft->height()); 
    Serial.print(" xMin="); Serial.print(xmin); Serial.print(" xMax="); Serial.print(xmax); Serial.print(" ==> Y="); Serial.print(p.y);
    Serial.print(" yMin="); Serial.print(ymin); Serial.print(" yMax="); Serial.print(ymax); Serial.print(" ==> X="); Serial.print(p.x); 
    Serial.print(" z="); Serial.print(p.z);
    Serial.println();
  }
  return p;
}

// Simulation BarGraph Puissance
float fSin[10] = { 0.30886552,
0.587527526,
0.808736061,
0.950859461,
0.999999683,
0.951351376,
0.809671788,
0.588815562,
0.31037991,
0.001592653
};

void barGraph(int16_t val)
{
  tft->fillRect(TEXT_X + 1, TEXT_Y + 1, TEXT_W - 2, TEXT_H - 2, ILI9341_BLACK); 
  tft->drawRect(TEXT_X + 1, TEXT_Y + 1, TEXT_W - 2, TEXT_H - 2, ILI9341_WHITE); 
  if (val >= 316) val = 316; // Pour poas écraser le cadre blanc
  if (val >= 310)
    tft->fillRect(TEXT_X + 1, TEXT_Y + 1, val, 18, RED);
  else if (val > 250)
    tft->fillRect(TEXT_X + 1, TEXT_Y + 1, val, 18, ORANGE);
  else
    tft->fillRect(TEXT_X + 1, TEXT_Y + 1, val, 18, GREEN);
}

bool pttOut = false;
void setPttOut(bool on)
{
  if (on)
    digitalWrite(PttOut, HIGH);
  else
    digitalWrite(PttOut, LOW);
  pttOut = on;
}

void setAlimPAOn(bool on)
{
  if (on)
    digitalWrite(RelaiAlimPA, HIGH);
  else
    digitalWrite(RelaiAlimPA, LOW);
}

void setPAOn(bool on)
{
  if (on)
    digitalWrite(RelaiPA, HIGH);
  else
    digitalWrite(RelaiPA, LOW);
}

// Gestion du TX
bool pttActif = false;
// Mis à jour par le 2ème coeur
bool nouv_frequence_dispo = true;                               // Fanion d'avertissement qu'une nouvelle fréquence est disponible.
unsigned int frequence;                                         // Fréquence du transceiver, exprimée en MHz et arrondie
int old_frequence = -1;                                         // Fréquence préalable du transceiver

// 50ms de pause pour l'activation / l'arrêt des relais
#define WAITRELAY 50

void loop(void) {
  // Lecture du PTT et gestion des antennes "RX"
  if (pttActif != !digitalRead(PttIn))                            // Niveau bas actif sur la broche PTT_In?
    pttActif = !digitalRead(PttIn);                               // Oui, changer l'état de la variable PTT
  
  if (pttActif)        // Antenne ne permettant pas d'émettre (Tx)?
  { // PTT Enfoncé
    // On veut emmetre
    if (! pttOut)
    {
      // Il faut passer en émission
      if (PAOn)
      {
        // Séquenceur
        setPAOn(true);   // Commuter le relai transfer en émission
        delay(WAITRELAY);       // Pause séquenceur
        setPttOut(true); // Envoyer le PTT au TX (emission HF)
      }
      else
        setPttOut(true);
    }
  }
  else
  { // PTT Relaché
    // On veut arrêter d'emmetre
    if (pttOut)
    {
      // Il faut couper l'émission
      if (PAOn)
      {
        setPttOut(false); // On coupe la HF
        delay(WAITRELAY);        // Pause séquenceur
        setPAOn(false);   // Commuter le relai transfer en réception
      }
      else
        setPttOut(false); // On coupe la HF
    }
  }

  // Traitement des actions tactiles
  // Read TouchPad...
  TSPoint p = getTouchPoint();

  // go thru all the buttons, checking if they were pressed
  switch (curState) {
    case RUN:
      manageRun(p);

      if (PAOn)
      {
        // Faire bouger le BarGraph de Puissance
        cptPA2++;
        if (cptPA2 > 750)
        {
          cptPA2 = 0;        
          int val = (fSin[cptPA] * 320.0); 
          barGraph(val);
          cptPA++; 
          if (cptPA >= 10)
            cptPA = 0;
        }
      }
      break;

    case CONFIG:
      manageConfig(p);
      break;
  } // switch curState  

  // Gestion changement de fréquence sur le transceiver
  if (nouv_frequence_dispo)                                                 // Nouvelle fréquence disponible pour analyse?
  {
    // Affixcher la fréquence du TX

    // Traité
    nouv_frequence_dispo = false;                                       // Annuler le fanion de nouvelle fréquence disponible
  }
}

//************************************************************************************************************************
// 2ème coeur...
//************************************************************************************************************************

// Fonction d'initialisation du deuxième coeur du Pico, style Arduino, exécutée une seule fois à la mise sous tension
void setup1()
{
  Serial1.begin(9600);                                    // Port série UART0 / Serial1 connecté au transceiver (protocole CI-V)

  // Envoi d'une requête initiale de fréquence. Si pas de réponse obtenue, les antennes en EEPROM seront applicables.
  const byte tx_data[] = {0xFE, 0xFE, 0x94, 0xE0, 0x03, 0xFD}; // La syntaxe du message de requête de fréquence
  Serial1.write(tx_data, sizeof(tx_data));                     // Envoi du message
}

unsigned int caract;                                            // Caractère reçu sur le port CI-V
unsigned int rx_data[15];                                       // Chaîne de caractères reçus sur le port CI-V
unsigned int byte_pos = 0;                                      // Position du caractère lu sur la chaîne de caractères

// Cette boucle du deuxième coeur monitore la fréquence du transceiver et signale au premier coeur un changement de fréquence
void loop1()
{
  // Gestion des messages CI-V de fréquence courante du transceiver. Signale lorsque les MHz de la fréquence changent
  if (Serial1.available())                      // Octet(s) disponible en tampon du port CI-V?
  {
    rx_data[byte_pos] = Serial1.read();         // Oui, le(s) lire
    //    Serial.println(rx_data[byte_pos]);    // Afficher les octets reçus pour fins de déboggage
    if (rx_data[byte_pos] == 0xFD)              // Octet de fin de message reçu?
    {
      if (( rx_data[0] == 0xfe          // Message de fréquence automatiquement envoyé par le transceiver?...
            && rx_data[1] == 0xfe
            && rx_data[2] == 0x00
            && rx_data[3] == 0x94
            && rx_data[4] == 0x00)
          || ( rx_data[0] == 0xfe         // ...ou message de fréquence suite à une requête manuelle (mise sous tension)?
               && rx_data[1] == 0xfe
               && rx_data[2] == 0xE0
               && rx_data[3] == 0x94
               && rx_data[4] == 0x03))
      {
        frequence = ((rx_data[8] & 0x0F) + (((rx_data[8] >> 4) & 0x0F) * 10));    // Oui, extraire la fréquence (en MHz arrondis) du message
        if (frequence != old_frequence)                                           // Vérifier si c'est une nouvelle fréquence en MHz
        { 
          old_frequence = frequence;                                              // Oui, sauvegarder la nouvelle fréquence comme ancienne fréquence
          nouv_frequence_dispo = true;                                            // Aviser qu'une nouvelle fréquence est disponible
        }
      }
      byte_pos = 0;                                                               // Ré-initialiser le pointeur de position de la chaîne d'octets reçue
    }
    else byte_pos++;                                                              // Caractère de fin de message non reçu: Incrémenter le pointeur de position de la chaîne d'octets reçue
  }
}

