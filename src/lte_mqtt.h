#ifndef LTE_MQTT_H
#define LTE_MQTT_H

#include <SparkFun_LTE_Shield_Arduino_Library.h>
#include <climits>

typedef struct {
  // Mobile network operator
  mobile_network_operator_t mno;
  // Access Point Name 
  const char* apn;
  // Home network identity (HNI): MCC + MNC
  const unsigned int networkId;
} LTEConfig;

typedef struct {
  // Client ID
  char *clientId;
  // Server hostname  
  const char *hostname;
  // Server port 
  const unsigned int port;
  // Topic for outgoing data messages
  char *dataTopic;
  // Topic for incoming command messages
  char *commandTopic;
  // Username
  const char *username;
  // Password
  const char *password;
  // Clean session flag
  const boolean cleanSession;
} MQTTConfig;

int registerOperator(LTE_Shield *lte, mobile_network_operator_t mno, String apn, 
                     unsigned long networkId);

int checkNetworkRegistration(LTE_Shield *lte, LTEConfig *lteConfig, 
                             int maxRetry=INT_MAX, int retryDelay=2000);

#endif
