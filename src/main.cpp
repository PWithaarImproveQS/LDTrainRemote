#include <Lpf2Hub.h>      //legoino
#include <Lpf2HubConst.h> //legoino
#include <Bounce2.h>      //bounce2
#include <Arduino.h>

Lpf2Hub myHub;
byte port = (byte)PoweredUpHubPort::A;

//Pin declaration
#define BTN_MUSIC 1
#define BTN_LIGHT 2
#define BTN_WATER 3
#define BTN_STOP 4
#define BTN_SWITCH 44
#define PTI_SPEED A5

Bounce pbMusic = Bounce();
Bounce pbLight = Bounce();
Bounce pbWater = Bounce();
Bounce pbStop = Bounce();
Bounce pbSwitch = Bounce();
int gLastStatePtiSpeed = 0;

static int gLightOn = 0;
static short gColor = NONE;
static int gSpeed =0;

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
  
  Serial.println("Init Done");
  myHub.init();
}

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
    delay(50);
}