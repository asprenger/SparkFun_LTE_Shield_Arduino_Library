
#ifndef SPARKFUN_LTE_SHIELD_ARDUINO_LIBRARY_MQTT_MANAGER_H
#define SPARKFUN_LTE_SHIELD_ARDUINO_LIBRARY_MQTT_MANAGER_H

#include <PubSubClient.h>
#include <metrics.h>

class MQTTConfig {
public:
  MQTTConfig(const char* brokerHost, uint16_t brokerPort, const char* username, const char* password, bool cleanSession) :
    brokerHost_(brokerHost), brokerPort_(brokerPort), username_(username), password_(password), cleanSession_(cleanSession) {}

  ~MQTTConfig() {
    delete []clientId_;
    delete []dataTopic_;
    delete []healthTopic_;
    delete []commandTopic_;
  }

  void initDynamicProperties(const char* appName, const char* imei) {
    clientId_ = new char[strlen(imei) + 1];
    strcpy(clientId_, imei);
    dataTopic_ = new char[strlen(appName) + strlen(imei) + 7];
    sprintf(dataTopic_, "%s/%s/data", appName, imei);
    healthTopic_ = new char[strlen(appName) + strlen(imei) + 9];
    sprintf(healthTopic_, "%s/%s/health", appName, imei);
    commandTopic_ = new char[strlen(appName) + strlen(imei) + 6];
    sprintf(commandTopic_, "%s/%s/cmd", appName, imei);
  }

  const char* brokerHost() { return brokerHost_; }
  uint16_t brokerPort() { return brokerPort_; }
  const char* username() { return username_; }
  const char* password() { return password_; }
  const char* clientId() { return clientId_; }
  const char* dataTopic() { return dataTopic_; }
  const char* healthTopic() { return healthTopic_; }
  const char* commandTopic() { return commandTopic_; }
  bool cleanSession() { return cleanSession_; }
  
private:
  const char* brokerHost_;
  uint16_t brokerPort_;
  const char* username_;
  const char* password_;
  char* clientId_;
  char* dataTopic_;
  char* healthTopic_;
  char* commandTopic_;
  bool cleanSession_;
};


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