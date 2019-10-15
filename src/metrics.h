
#ifndef SPARKFUN_LTE_SHIELD_ARDUINO_LIBRARY_METRICE_H
#define SPARKFUN_LTE_SHIELD_ARDUINO_LIBRARY_METRICE_H

class Metrics {
 public:
  Metrics() {}
  ~Metrics() {}

  // LTE board power on count
  uint32_t ltePowerOnCount=0;
  
  // LTE board power on error count
  uint32_t ltePowerOnErrorCount=0;

  // Network register count
  uint32_t networkRegisterCount=0;

  // Network register error count
  uint32_t networkRegisterErrorCount=0;

  // Number of times the modem has been reset after a 
  // network registration error
  uint32_t networkRegisterModemResetCount = 0;

  // Duration of last network registration in ms
  uint32_t lastRegistrationTime = 0;

  // Duration of last waiting for network registration in ms
  uint32_t lastRegistrationWaitTime = 0;

  // Duration of last MQTT connect in ms
  uint32_t lastMqttConnectTime = 0;

  // MQTT connect count
  uint32_t mqttConnectCount=0;

  // MQTT connect error count
  uint32_t mqttConnectErrorCount=0;
  
  // MQTT publish count
  uint32_t mqttPublishCount=0;

  // MQTT publish error count
  uint32_t mqttPublishErrorCount=0;
};

#endif 