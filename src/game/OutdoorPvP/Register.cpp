#include "World.h"

void AddSC_outdoorpvp_ep();
void AddSC_outdoorpvp_si();
void AddSC_outdoorpvp_hy();

void RegisterZoneScripts()
{
#if SUPPORTED_CLIENT_BUILD > CLIENT_BUILD_1_11_2
    if (sWorld.GetWowPatch() >= WOW_PATCH_112 && sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_EP_ENABLE))
        AddSC_outdoorpvp_ep();
    if (sWorld.GetWowPatch() >= WOW_PATCH_112 && sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_SI_ENABLE))
        AddSC_outdoorpvp_si();
    if (sWorld.GetWowPatch() >= WOW_PATCH_102 && sWorld.getConfig(CONFIG_BOOL_OUTDOORPVP_HY_ENABLE))
        AddSC_outdoorpvp_hy();
#endif
}
