#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoOTA.h>
#include <Lpf2Hub.h>      //legoino
#include <Lpf2HubConst.h> //legoino
#include <Bounce2.h>      //bounce2
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "esp_log.h"

#define WIFI_SSID "KruItbOsT"
#define WIFI_PASS "zukweg-Zidpa5"

WiFiMulti WiFiMulti;
WiFiServer telnetServer(23);
WiFiClient telnetClient;

Lpf2Hub myHub;
byte port = (byte)PoweredUpHubPort::A;

// Pin declaration
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
Bounce pbBlack = Bounce();

Bounce pbDirectionSwitch = Bounce();
Bounce pbSpeedModeSwitch = Bounce();

bool TrainStopped = true;
bool DirectionSwitchState = true;
int gLastStatePtiSpeed = 0;

static int gLightOn = 0;
static short gColor = NONE;
static int gSpeed = 0;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, LED_MATRIX,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
    matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

bool useTelnet = true;

int customLogFunction(const char *fmt, va_list args) {
  char buffer[256];
  int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
  if (useTelnet && telnetClient && telnetClient.connected()) {
    telnetClient.write(buffer, len);
  } else {
    Serial.write(buffer, len);
  }
  return len;
}

void handlePoti()
{
  int ptiSpeed = analogRead(PTI_SPEED);
  gLastStatePtiSpeed = ptiSpeed;
  ESP_LOGI("ptiSpeed", "ptiSpeed: %d", ptiSpeed);

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
      ESP_LOGI("Direction", "Direction switch is HIGH, setting positive speed");
    } else {
      // Negative speed
      speed = -speed;
      ESP_LOGI("Direction", "Direction switch is LOW, setting negative speed");
    }

  }

  if (speed != gSpeed)
  { 
    ESP_LOGI("Speed", "Speed changed to: %d", speed);
    if (speed == 0)
    {
      TrainStopped = false;
      matrix.fillScreen(matrix.Color(0, 0, 0)); // Red

    }

    if (gSpeed == 0 && speed > 0)
    {
      
        myHub.playSound((byte)DuploTrainBaseSound::STATION_DEPARTURE);
        delay(100);
    }
    gSpeed = speed;

    if (!TrainStopped)
    {
      myHub.setBasicMotorSpeed(port, speed);
    }
    
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
        gColor = 255;
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
      ESP_LOGI("Button", "Direction Change");
      matrix.fillScreen(matrix.Color(0, 255, 0)); // Green
    }
  }
  if (pbSpeedModeSwitch.update())
  {
    if (pbSpeedModeSwitch.fell())
    {
      ESP_LOGI("Button", "Speed Modus Changed High");
    } else
    {
      ESP_LOGI("Button", "Speed Modus Changed Low");
    }
  }
  if(pbMusic.update())
  {
    if(pbMusic.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::HORN);
      delay(100);
      matrix.fillScreen(matrix.Color(255, 255, 0)); // Yellow
    }
  }
  if(pbLight.update())
  {
    if(pbLight.fell())
    {
      myHub.setLedColor(getNextColor());
      delay(100);
      matrix.fillScreen(matrix.Color(255, 255, 255)); // White
    }
  }
  if(pbWater.update())
  {
    if(pbWater.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::WATER_REFILL);
      delay(100);
      matrix.fillScreen(matrix.Color(0, 0, 255)); // Blue
    }
  }
  if(pbStop.update())
  {
    if(pbStop.fell())
    {
      myHub.setBasicMotorSpeed(port, 0);      
      TrainStopped = true;
      myHub.playSound((byte)DuploTrainBaseSound::BRAKE);
      matrix.fillScreen(matrix.Color(255, 0, 0)); // Red
      delay(100);      
    }
  }
  if(pbBlack.update())
  {
    if(pbBlack.fell())
    {
      matrix.fillScreen(matrix.Color(255, 165, 0)); // Orange
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(colors[0]);
  matrix.fillScreen(0);
  matrix.setPixelColor(0, 0, 0, 255);
  matrix.show();

  Serial.println("Init Start");
  WiFi.setHostname("DuploTrainController");

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

  Serial.print("\n\nWaiting for WiFi... ");

  // WIFI Connection, Reboot after 30 attempts
  uint8_t not_connected_counter = 0;
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
    not_connected_counter++;
    if (not_connected_counter > 30)
    {
      Serial.println("Resetting due to Wifi not connecting...");
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Hostname: ");
  Serial.println(WiFi.getHostname());

  // OTA Configuration and Enable OTA
  Serial.println("\nEnabling OTA Feature");
  ArduinoOTA.setPassword("duplotrain");
  ArduinoOTA.begin();

  // Define Pin Modes
  pbMusic.attach(BTN_MUSIC, INPUT_PULLUP);
  pbLight.attach(BTN_LIGHT, INPUT_PULLUP);
  pbWater.attach(BTN_WATER, INPUT_PULLUP);
  pbStop.attach(BTN_STOP, INPUT_PULLUP);
  pbDirectionSwitch.attach(BTN_DIRECTION, INPUT_PULLUP);
  pbSpeedModeSwitch.attach(BTN_SWITCH, INPUT_PULLUP);

  // Start Telnet server
  telnetServer.begin();
  telnetServer.setNoDelay(true);

  Serial.println("Init Done");
  myHub.init();

  // Set custom log function
  esp_log_set_vprintf(customLogFunction);

  // Set log level to INFO
  esp_log_level_set("*", ESP_LOG_INFO);
}

int x    = matrix.width();
int pass = 0;

void loop() {
  ArduinoOTA.handle();

  // Handle Telnet client
  if (telnetServer.hasClient()) {
    if (!telnetClient || !telnetClient.connected()) {
      if (telnetClient) telnetClient.stop();
      telnetClient = telnetServer.available();
      telnetClient.flush();
      telnetClient.println("Connected to DuploTrainController");
    }
  }

  if (myHub.isConnecting()) {
    myHub.connectHub();
    if (myHub.isConnected()) {
      ESP_LOGI("Hub", "We are now connected to the HUB");
      matrix.fillCircle(4, 4, 3, matrix.Color(0, 255, 0));
    } else {
      ESP_LOGI("Hub", "We have failed to connect to the HUB");
      matrix.fillCircle(4, 4, 3, matrix.Color(255, 0, 0));
    }
  }
  if (myHub.isConnected()) {
    handleButtons();
    handlePoti();
  }

  // matrix.fillScreen(0);
  // matrix.setCursor(x, 0);
  // matrix.print(F("LEGO Duplo Train Controller"));
  // if (--x < -216) {
  //   x = matrix.width();
  //   if (++pass >= 3) pass = 0;
  //   matrix.setTextColor(colors[pass]);
  // }
  matrix.show();
  delay(50);
}