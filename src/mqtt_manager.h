
#ifndef SPARKFUN_LTE_SHIELD_ARDUINO_LIBRARY_MQTT_MANAGER_H
#define SPARKFUN_LTE_SHIELD_ARDUINO_LIBRARY_MQTT_MANAGER_H

#include <PubSubClient.h>
#include <metrics.h>
#include <lte_mqtt.h>

class MqttManager {
  public:
  MqttManager(PubSubClient *pubSubClient, MQTTConfig *mqttConfig, Metrics *metrics);
  boolean connect();
  boolean publish(char* topic, char* message);
  void disconnect();
  boolean connected();
  int state();
  
  private:
  MQTTConfig *mqttConfig_;
  PubSubClient *pubSubClient_;
  Metrics *metrics_;
};

#endif 