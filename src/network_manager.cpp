
#include <network_manager.h>
#include <logging.h>

#define LTEShieldSerial Serial1
#define LTEShieldSerialSpeed 9600

#define NET_MGR_RESPONSE_SIZE 256
#define NET_MGR_AT_TIMEOUT 4000
#define NET_MGR_REGISTARTION_TIMEOUT 3 * 60 * 1000
#define NET_MGR_WAIT_FOR_REGISTARTION_ITERATIONS 3 * 60
#define NET_MGR_WAIT_FOR_REGISTARTION_SLEEP 1000

const char LTE_SHIELD_RESPONSE_OK[] = "OK\r\n";

void fatal(const char* msg) {
  LOG(ERROR, msg);
  while(1);
}      

NetworkManager::NetworkManager(LTE_Shield *lte, LTEConfig *lteConfig, Metrics *metrics) : 
  lte_(lte), lteConfig_(lteConfig), metrics_(metrics) {}

NetMgrResult NetworkManager::On() {
  LOG(INFO, "Get modem ready");
  if (PowerOn() == NetMgrResult::OK) {
    InitReady();
    return NetMgrResult::OK;
  } else {
    return NetMgrResult::ERROR;
  }
}

NetMgrResult NetworkManager::PowerOn() {
  LOG(INFO, "Power modem on");
  if (lte_->begin(LTEShieldSerial, LTEShieldSerialSpeed)) {
    return NetMgrResult::OK;
  } else {
    LOG(ERROR, "Power on modem failed");
    return NetMgrResult::ERROR;
  }
}

void NetworkManager::InitReady() {
  // Configure LTE modem: MNO profile, disable power management
  
  LTE_Shield_error_t r;
  char response[NET_MGR_RESPONSE_SIZE] = {0};

  // Get MNO profile

  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+UMNOPROF?", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error UMNOPROF");
  }
  int profile = -1;
  if (sscanf(response, "\r\n+UMNOPROF: %d\r\n", &profile) == 1) {
    LOG(INFO, "MNO profile: %d", profile);
  }

  // Set COPS operator format to numeric

  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+COPS=3,2", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT); 
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error COPS=3,2");
  }

  /////////////////////////////
  // Force Cat NB-only mode
  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+URAT=8", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT); 
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error URAT=8");
  }
  /////////////////////////////

  // Disable eDRX

  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+CEDRXS=3,2", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT); // GPRS
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error CEDRXS");
  }
  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+CEDRXS=3,4", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT); // LTE Cat M1
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error CEDRXS");
  }
  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+CEDRXS=3,5", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT); // LTE Cat NB1
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error CEDRXS");
  }


  // Disable PSM

  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+CPSMS=0", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error CPSMS");
  }
}

NetMgrResult NetworkManager::Off() {
  return PowerOff();
}  

NetMgrResult NetworkManager::Connect() {
  
  LTE_Shield_error_t r;
  char response[NET_MGR_RESPONSE_SIZE] = {0};

  metrics_->networkRegisterCount++;

  // Configure PDP context
  
  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+CGDCONT=1,\"IP\",\"iot.1nce.net\"", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error setting PDP context");
    metrics_->networkRegisterErrorCount++;
    return NetMgrResult::ERROR;
  }

  // Set network registration response format

  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+CEREG=0", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error setting registration format");
    metrics_->networkRegisterErrorCount++;
    return NetMgrResult::ERROR;
  }

  // Register network

  LOG(INFO, "Register network");
  unsigned long reg_start = millis();
  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+COPS=1,2,\"26201\"", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_REGISTARTION_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(WARN, "Error register network: %d. Ignore error.", r);
  }
  unsigned long reg_time = millis() - reg_start;
  metrics_->lastRegistrationTime = reg_time;
  LOG(INFO, "Registration time: %u ms", reg_time);

  // Wait for network registration

  LOG(INFO, "Wait for network registration");
  boolean regOk = false;
  unsigned long reg_wait_start = millis();
  for (int i=0; i<NET_MGR_WAIT_FOR_REGISTARTION_ITERATIONS; i++) {
    int registration_status;
    if (RegistrationStatus(registration_status) == NetMgrResult::OK) {
      LOG(INFO, "Registration status: %d", registration_status);
      if (registration_status == 1 || registration_status == 5) {
        regOk = true;
        break;
      }
    }
    else {
      LOG(ERROR, "Error getting registration status");
    }  
    delay(NET_MGR_WAIT_FOR_REGISTARTION_SLEEP);
  }
  unsigned long reg_wait_time = millis() - reg_wait_start;
  metrics_->lastRegistrationWaitTime = reg_wait_time;
  LOG(INFO, "Registration wait time: %u ms", reg_wait_time);

  if (regOk) {
    LOG(INFO, "Registration successful");
    return NetMgrResult::OK;
  }
  else {
    LOG(ERROR, "Registration failed");
    metrics_->networkRegisterErrorCount++;
    return NetMgrResult::ERROR;
  }
}


NetMgrResult NetworkManager::Disconnect() {
  
  char response[NET_MGR_RESPONSE_SIZE] = {0};

  LOG(INFO, "Disconnect from network");

  lte_->sendCommandWithResponse("+COPS=2,2", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);

  return NetMgrResult::OK;

}


NetMgrResult NetworkManager::RegistrationStatus(int &registration_status) {
  char response[NET_MGR_RESPONSE_SIZE] = {0};
  LTE_Shield_error_t r = lte_->sendCommandWithResponse("+CEREG?", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    return NetMgrResult::ERROR;
  }
  int reg_mode;
  int reg_status;
  if (sscanf(response, "\r\n+CEREG: %d,%d", &reg_mode, &reg_status) == 2) {
    registration_status = reg_status;
    return NetMgrResult::OK;
  }
  else {
    return NetMgrResult::ERROR;
  }  
}

NetMgrResult NetworkManager::PowerOff() {
  if (lte_->at() == LTE_SHIELD_ERROR_SUCCESS) {
    return SoftPowerOff();
  } else {
    return HardPowerOff();
  }  
}  

NetMgrResult NetworkManager::SoftPowerOff() {
  LOG(INFO, "Power modem off (soft)");
  LTE_Shield_error_t r;
  char response[NET_MGR_RESPONSE_SIZE] = {0};
  r = lte_->sendCommandWithResponse("+CPWROFF", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "+CPWROFF failed: %d", r);
    return NetMgrResult::ERROR;
  }

  // Wait for AT interface to become unavailable
  for (int i=0; i<20; i++) {
    r = lte_->sendCommandWithResponse("", LTE_SHIELD_RESPONSE_OK, NULL, 2000);
    if (r == LTE_SHIELD_ERROR_NO_RESPONSE) {
      return NetMgrResult::OK;
    }
    delay(1000);
  }
  LOG(ERROR, "Power off modem failed");
  return NetMgrResult::ERROR;
}

NetMgrResult NetworkManager::HardPowerOff() {
  LOG(INFO, "Power modem off (hard)");
  lte_->powerOff();

  // Wait for AT interface to become unavailable
  LTE_Shield_error_t r;
  for (int i=0; i<20; i++) {
    r = lte_->sendCommandWithResponse("", LTE_SHIELD_RESPONSE_OK, NULL, 2000);
    if (r == LTE_SHIELD_ERROR_NO_RESPONSE) {
      return NetMgrResult::OK;
    }
    delay(1000);
  }
  LOG(ERROR, "Power off modem failed");
  return NetMgrResult::ERROR;
}


NetMgrResult NetworkManager::ModemReset() {
  LOG(INFO, "Reset the modem");
  PowerOff();
  LTE_Shield_error_t r = lte_->init(LTEShieldSerialSpeed, LTE_Shield::LTE_Shield_init_type_t::LTE_SHIELD_INIT_STANDARD);
  if (r == LTE_SHIELD_ERROR_SUCCESS) {
    return NetMgrResult::OK;
  } else {
    return NetMgrResult::ERROR;
  }
}

void NetworkManager::SetupSecurityProfile() {

  // settings for security profile 2
  LTE_Shield_error_t r;
  char response[NET_MGR_RESPONSE_SIZE] = {0};

  // certificate validation level 
  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+USECPRF=2,0,1", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error");
  }

  // trusted root certificate internal name
  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+USECPRF=2,3,\"rootCA.crt.der\"", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error");
  }

  // client certificate internal name
  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+USECPRF=2,5,\"client.crt.der\"", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error");
  }

  // client private key internal name
  memset(response, 0, NET_MGR_RESPONSE_SIZE);
  r = lte_->sendCommandWithResponse("+USECPRF=2,6,\"client.key.der\"", LTE_SHIELD_RESPONSE_OK, response, NET_MGR_AT_TIMEOUT);
  if (r != LTE_SHIELD_ERROR_SUCCESS) {
    LOG(ERROR, "Error");
  }

}