#include <lte_mqtt.h>

#define RETRY(operation, result_type, result_var, success_value, retry_count, retry_delay, retry_msg)\
  result_type result_var;\
  for (int i=0; i<retry_count; i++) {\
    result_var = operation;\
    if (result_var == success_value) {\
      break;\
    }\
    printf(retry_msg);\
    delay(retry_delay);\
  }

unsigned int randomLocalPort() {
  return 2000 + rand() % 4000;
}

/**
  Register with a mobile network operator.

  @param lte The LTE_Shield
  @param mno Mobile network operator profile
  @param apn Access Point Name
  @param networkId Home network ID
  @return 0: success, -1: error
*/
int registerOperator(LTE_Shield *lte, mobile_network_operator_t mno, String apn, unsigned long networkId) {

  /*
  Serial.println(F("Setting MNO profile"));
  if (lte->setNetwork(mno)) {
  } else {
    Serial.println(F("Error setting MNO"));
    return -1;
  }
  */
  
  Serial.println(F("Setting APN"));
  if (lte->setAPN(apn) == LTE_SHIELD_SUCCESS) {
  } else {
    Serial.println(F("Error setting APN"));
    return -1;
  }
  
  Serial.println(F("Registering with operator"));
  if (lte->registerOperator(networkId) == LTE_SHIELD_SUCCESS) {
  } else {
    Serial.println(F("Error registering with operator"));
    return -1;
  }

  return 0;
}

/**
  Check current mobile network registration. Register with operator
  if not registered. Rety on failure.

  @param lte The LTE_Shield
  @param lteConfig The LTEConfig
  @param maxRetry Max number of retries
  @param retryDelay Delay between retries
  @return 0: success, -1: registration error (after retry), -2: error getting connection status
*/
int checkNetworkRegistration(LTE_Shield *lte, LTEConfig *lteConfig, int maxRetry, int retryDelay) {

  Serial.println(F("Getting current operator status"));
  String currentNetworkId = "";
  LTE_Shield_error_t operator_status = lte->getOperator(&currentNetworkId);
  if (operator_status == LTE_SHIELD_ERROR_SUCCESS) {
    Serial.println("Connected to: " + String(currentNetworkId));
  }
  else if (operator_status == LTE_SHIELD_ERROR_DEREGISTERED) {
    Serial.println("Not connected");
    RETRY(registerOperator(lte, lteConfig->mno, lteConfig->apn, lteConfig->networkId), 
          int, connect_status, 0, 
          maxRetry, retryDelay, 
          "Error connecting to operator. Will retry.")
    if (connect_status != 0) {
      return -1;
    }
  }
  else {
    return -2;
  }  
  return 0;
}
