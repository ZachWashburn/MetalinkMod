#include "Configuration.h"

__declspec(noinline) void SetConfigToGetter(Config* conf) {
	g_pConfig.SetConfig(conf);
}




