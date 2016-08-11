struct WiserConfig;
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct WiserConfig * WiserConfigHandle;
  // currently creates a wiser config from the string of a file name
  WiserConfigHandle create_wiser_config(char const *);
  void    free_WiserConfig(WiserConfigHandle);
  int GetLinkCost(WiserConfigHandle, const char * ip1, const char * ip2);

#ifdef __cplusplus
}
#endif


