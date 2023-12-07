
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

class CTriggerKillTarget : public CPointEntity
{
public:
	void Spawn() override;
	void EXPORT KillUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
};
LINK_ENTITY_TO_CLASS(trigger_killtarget, CTriggerKillTarget);

void CTriggerKillTarget::Spawn()
{
	pev->solid = SOLID_NOT;
	SetUse(&CTriggerKillTarget::KillUse);
}

void CTriggerKillTarget::KillUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	// Kill our target!
	edict_t *pentTarget = FIND_ENTITY_BY_TARGETNAME( NULL, STRING(pev->target) );
	ALERT(at_console, "TARGET: %s\n", STRING(pev->target));
	CBaseEntity* pTarget = CBaseEntity::Instance( pentTarget );
	if ( !pTarget ) return;
	auto pNPC = pTarget->MyMonsterPointer();
	if ( !pNPC ) return;
	//pNPC->pev->takedamage = DAMAGE_YES;
	//pNPC->pev->health = 1;
	//pNPC->TakeDamage( pev, pev, 10, DMG_BULLET );
	pNPC->pev->health = 0;
	pNPC->Killed( NULL, GIB_NEVER );
}
