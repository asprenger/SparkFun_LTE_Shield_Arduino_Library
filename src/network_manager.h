
#include <SparkFun_LTE_Shield_Arduino_Library.h>
#include <lte_mqtt.h>
#include <metrics.h>

enum class NetMgrResult {
  OK,
  ERROR
};

class NetworkManager {
  public:
  NetworkManager(LTE_Shield *lte, LTEConfig *lteConfig, Metrics *metrics);
  NetMgrResult On();
  NetMgrResult Off();
  NetMgrResult Connect();
  NetMgrResult Disconnect();
  NetMgrResult RegistrationStatus(int &registration_status);
  NetMgrResult ModemReset();
  
  private:
  void InitReady();
  NetMgrResult PowerOn();
  NetMgrResult PowerOff();
  NetMgrResult SoftPowerOff();
  NetMgrResult HardPowerOff();
  void SetupSecurityProfile();
  LTE_Shield *lte_;
  LTEConfig *lteConfig_;
  Metrics *metrics_;
};

