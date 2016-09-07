struct WiserConfig;
struct GeneralConfiguration;
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct WiserConfig * WiserConfigHandle;
  // currently creates a wiser config from the string of a file name
  WiserConfigHandle create_wiser_config(char const *);
  void    free_WiserConfig(WiserConfigHandle);
  int GetLinkCost(WiserConfigHandle, const char * ip1, const char * ip2);

  /* Generalconfiguration class methods below here */
  // currently creates a general config from the string of a file name
  typedef struct GeneralConfiguration* GeneralConfigurationHandle;
  GeneralConfigurationHandle CreateGeneralConfig(char const *);
  void FreeGeneralConfig(GeneralConfigurationHandle);
  

#ifdef __cplusplus
}
#endif


