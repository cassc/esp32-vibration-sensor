#include "mqtt.h"

// C.F.,:
// https://github.com/marvinroger/async-mqtt-client/blob/develop/examples/FullyFeatured-ESP32/FullyFeatured-ESP32.ino
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;


void connectToMqtt()
{
  Serial.println(String("Connecting to MQTT broker ") + MQTT_HOST.toString() + " ...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent)
{
  Serial.println("MQTT connected!");
  char buf[128];
  sprintf(buf, "{\"ip\": \"%s\", \"mac\": \"%s\", \"tpe\": \"start\", \"version\": \"%d\"}", getIp().c_str(), mac.c_str(), VERSION);
  publisthMqtt(buf);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected())
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}
void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
}

void onMqttUnsubscribe(uint16_t packetId)
{
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{

}

void onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setupMqtt()
{
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToMqtt();
}

void publisthMqtt(const char *payload){
  String topic = String("vib/") + mac ;
  mqttClient.publish("vib", 0, false, payload);
}
