
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

class CTriggerFreezePlayer : public CPointEntity
{
public:
	void Spawn() override;
	void EXPORT FreezeUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
};
LINK_ENTITY_TO_CLASS(trigger_freezeplayer, CTriggerFreezePlayer);

void CTriggerFreezePlayer::Spawn()
{
	pev->solid = SOLID_NOT;
	SetUse(&CTriggerFreezePlayer::FreezeUse);
}

void CTriggerFreezePlayer::FreezeUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	// Grab the local player
	CBasePlayer* pBasePlayer = (CBasePlayer *)UTIL_GetLocalPlayer();
	if (!pBasePlayer) return;
	bool bFreezePlayer = (pBasePlayer->pev->flags & FL_FROZEN) != 0 ? false : true;
	pBasePlayer->EnableControl(!bFreezePlayer);
	if (bFreezePlayer)
		ALERT(at_console, "[trigger_freezeplayer :: DEBUG] Local player is now frozen!\n");
	else
		ALERT(at_console, "[trigger_freezeplayer :: DEBUG] Local player is free to move around!\n");
}
