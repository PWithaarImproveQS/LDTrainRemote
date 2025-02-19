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
#define BTN_BLACK 8
#define BTN_DIRECTION 43
#define BTN_SWITCH 44
#define PTI_SPEED A5
#define LED_MATRIX 5

Bounce pbMusic = Bounce();
Bounce pbLight = Bounce();
Bounce pbWater = Bounce();
Bounce pbStop = Bounce();
Bounce pbDirectionSwitch = Bounce();
Bounce pbSpeedModeSwitch = Bounce();

bool DirectionSwitchState = true;
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


  int speed = 0;
  // Serial.print("PTI Speed: ");
  // Serial.println(ptiSpeed);
  if (pbSpeedModeSwitch.read() == HIGH) {
    // Speed mode switch is HIGH, use the first half for reverse and second half for forward
    if ((ptiSpeed > 2500) && (ptiSpeed < 3000))
    {
      speed = 0;
    } else if (ptiSpeed <= 2970) {
      // Map ptiSpeed (0 to 2048) to speed (0 to -64) in steps of 8
      speed = map(ptiSpeed, 2770, 1920, 0, -64);
    } else {
      // Map ptiSpeed (2048 to 4096) to speed (0 to 64) in steps of 8
      speed = map(ptiSpeed, 2770, 4000, 0, 64);
    }

  } else {
    // Speed mode switch is LOW, use the direction switch logic
    // Map ptiSpeed (0 to 4096) to speed (0 to 64) in steps of 8
    speed = map(ptiSpeed, 1920, 4000, 0, 64);
    if (ptiSpeed < 2200)
    {
      speed = 0;
    }
    
    // Check the state of pbDirectionSwitch to determine if speed should be positive or negative
    if (DirectionSwitchState) {
      // Positive speed
      Serial.println("Direction switch is HIGH, setting positive speed");
    } else {
      // Negative speed
      speed = -speed;
      Serial.println("Direction switch is LOW, setting negative speed");
    }

  }

  if (speed != gSpeed)
  { 
    // Serial.print("Speed changed to: ");
    // Serial.print(ptiSpeed);
    // Serial.print(" -> ");
    // Serial.println(speed);
    if (gSpeed == 0 && speed > 0)
    {
        myHub.playSound((byte)DuploTrainBaseSound::STATION_DEPARTURE);
        delay(100);
    }
    gSpeed = speed;
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
  if (pbDirectionSwitch.update())
  {
    if (pbDirectionSwitch.fell())
    {
      DirectionSwitchState = !DirectionSwitchState;
      Serial.println("Direction Change");

    }
  }
  if (pbSpeedModeSwitch.update())
  {
    if (pbSpeedModeSwitch.fell())
    {
      Serial.println("Speed Modus Changed High");
    } else
    {
      Serial.println("Speed Modus Changed Low");
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
  pbDirectionSwitch.attach(BTN_DIRECTION, INPUT_PULLUP);
  pbSpeedModeSwitch.attach(BTN_SWITCH, INPUT_PULLUP);

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(100);
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