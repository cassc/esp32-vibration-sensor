#include "mqtt.h"

// Min delay between two MQTT messages
#define MIN_MQ_DELAY 20
// Lower value increase sensitivity
#define DEBOUNCE_TIME_MS 1
#define GPIO_VIBR 35
#define GPIO_VIBR2 34

// Comment to disable MQTT message
#define ENABLE_MQTT

#define GPIO_LED 27

boolean btnState = false;

String mac;
String onMsg;
String offMsg;

unsigned long prevMQSentTs = 0;


void maybeReportBackByGPIO(uint8_t pin)
{
  auto now = millis();
  boolean state = digitalRead(pin);
  auto msgStr = state ? onMsg : offMsg;
  auto msg = msgStr.c_str();

  // send message if state changed
  if (state != btnState)
  {
    digitalWrite(GPIO_LED, state);

    prevMQSentTs = now;
    btnState = state;
#ifdef ENABLE_MQTT
    publisthMqtt(msg);
#endif
    sendUDP(msg);
    return;
  }

  // send message if still in vibration and MIN_MQ_DELAY time has passed
  if ((now - prevMQSentTs > MIN_MQ_DELAY) && state)
  {
    prevMQSentTs = now;

#ifdef ENABLE_MQTT
    publisthMqtt(msg);
#endif

    sendUDP(msg);
  }
}


void setup()
{
  delay(1000);
  Serial.begin(115200);

  setUpNetwork();

#ifdef ENABLE_MQTT
  setupMqtt();
#endif

  pinMode(GPIO_VIBR, INPUT_PULLDOWN);
  pinMode(GPIO_VIBR2, INPUT_PULLDOWN);

  pinMode(GPIO_LED, OUTPUT);

  onMsg += mac + ":1";
  offMsg += mac + ":0";
}

void loop()
{
  portalLoop();

  maybeReportBackByGPIO(GPIO_VIBR);
  maybeReportBackByGPIO(GPIO_VIBR2);
}
