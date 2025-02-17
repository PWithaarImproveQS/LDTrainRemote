#include <Arduino.h>

#include <Lpf2Hub.h>      //legoino
#include <Lpf2HubConst.h> //legoino
#include <Bounce2.h>      //bounce2

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>


Lpf2Hub myHub;
byte port = (byte)PoweredUpHubPort::A;

//Pin declaration
#define BTN_MUSIC 1
#define BTN_LIGHT 2
#define BTN_WATER 3
#define BTN_STOP 4
#define BTN_SWITCH 44
#define PTI_SPEED A5
#define LED_MATRIX 5

Bounce pbMusic = Bounce();
Bounce pbLight = Bounce();
Bounce pbWater = Bounce();
Bounce pbStop = Bounce();
Bounce pbSwitch = Bounce();
int gLastStatePtiSpeed = 0;

static int gLightOn = 0;
static short gColor = NONE;
static int gSpeed =0;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, LED_MATRIX,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
    matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };
  

void handlePoti()
{
  int ptiSpeed = analogRead(PTI_SPEED);
  gLastStatePtiSpeed = ptiSpeed;
  //Serial.println(ptiSpeed);
  int speed = 0;
  if (ptiSpeed > 3500) speed = 64;     // Fast forward
  else if (ptiSpeed > 3000) speed = 32; // Normal forward
  else if (ptiSpeed > 2000) speed = 16; // Slow forward (might not work on low battery)
  else if (ptiSpeed > 1000) speed = 0;  // Stop
  else if (ptiSpeed > 500) speed = -32; // Slow backward
  else speed = -64;                     // Fast backward


  if(speed != gSpeed)
  { 
    Serial.print("Speed changed ");
    Serial.println(speed);
    if(gSpeed == 0 && speed > 0)
    {
        myHub.playSound((byte)DuploTrainBaseSound::STATION_DEPARTURE);
        delay(100);
    }
    gSpeed =speed;
    myHub.setBasicMotorSpeed(port, speed);
  }
}


Color getNextColor()
{
    if(gColor == 255)
    { 
        gColor = 0;
        return (Color)0;
    }
       
    gColor++;
    
    if(gColor == NUM_COLORS)
    {   
        gColor =255;
    }
    return (Color) gColor;
} 

void handleButtons()
{
  if (pbSwitch.update())

  {
    if (pbSwitch.fell())
    {
      Serial.println("Right");
    } else
    {
      Serial.println("Left");
    }
  }
  if(pbMusic.update())
  {
    if(pbMusic.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::HORN);
      delay(100);
    }
  }
  if(pbLight.update())
  {
    if(pbLight.fell())
    {
      myHub.setLedColor(getNextColor());
      delay(100);
    }
  }
  if(pbWater.update())
  {
    if(pbWater.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::WATER_REFILL);
      delay(100);  
    }
  }
  if(pbStop.update())
  {
    if(pbStop.fell())
    {
      myHub.setBasicMotorSpeed(port, 0);
      delay(100);
      myHub.playSound((byte)DuploTrainBaseSound::BRAKE);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(2500);  
  
  Serial.println("Init Start");
  //define Pin Modes

  pbMusic.attach(BTN_MUSIC, INPUT_PULLUP);
  pbLight.attach(BTN_LIGHT, INPUT_PULLUP);
  pbWater.attach(BTN_WATER, INPUT_PULLUP);
  pbStop.attach(BTN_STOP, INPUT_PULLUP);
  pbSwitch.attach(BTN_SWITCH, INPUT_PULLUP);

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(colors[0]);
  
  Serial.println("Init Done");
  myHub.init();
}

int x    = matrix.width();
int pass = 0;

void loop() {
    if (myHub.isConnecting()) {
        myHub.connectHub();
        if (myHub.isConnected()) {
            Serial.println("We are now connected to the HUB");
        } else {
            Serial.println("We have failed to connect to the HUB");
        }
    }
    if (myHub.isConnected()) {
        handleButtons();
        handlePoti();
    } 

    matrix.fillScreen(0);
    matrix.setCursor(x, 0);
    matrix.print(F("LEGO Duplo Train Controller"));
    if(--x < -216) {
      x = matrix.width();
      if(++pass >= 3) pass = 0;
      matrix.setTextColor(colors[pass]);
    }
    matrix.show();
    delay(50);
}