
#include <mqtt_manager.h>
#include <logging.h>

MqttManager::MqttManager(PubSubClient *pubSubClient, MQTTConfig *mqttConfig, Metrics *metrics) : 
  mqttConfig_(mqttConfig), pubSubClient_(pubSubClient), metrics_(metrics) {}

boolean MqttManager::connect() {

  LOG(INFO, "Connect to MQTT");

  metrics_->mqttConnectCount++;

  unsigned long mqttConnectStart = millis();
  boolean mqttConnected = pubSubClient_->connect(mqttConfig_->clientId(), mqttConfig_->username(), mqttConfig_->password(), 0, 0, 0, 0, mqttConfig_->cleanSession());
  unsigned long mqttConnectTime = millis() - mqttConnectStart;

  if (mqttConnected) {
    LOG(INFO, "MQTT connect time: %u ms", mqttConnectTime);
    metrics_->lastMqttConnectTime = mqttConnectTime;
  }
  else {
    LOG(ERROR, "Error connecting to MQTT (%d)", pubSubClient_->state());
    metrics_->mqttConnectErrorCount++;
    metrics_->lastMqttConnectTime = 0;
  }

  return mqttConnected;
}

boolean MqttManager::publish(char* topic, char* message) {
  metrics_->mqttPublishCount++;
  boolean success = pubSubClient_->publish(topic, message);
  if (!success) {
    metrics_->mqttPublishErrorCount++;
  }
  return success;
}

void MqttManager::disconnect() {
  LOG(INFO, "Disconnect from MQTT"); 
  pubSubClient_->disconnect();
} 

boolean MqttManager::connected() {
  return pubSubClient_->connected();
}

int MqttManager::state() {
  return pubSubClient_->state();
}
