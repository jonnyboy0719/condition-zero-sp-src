
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

class CTriggerHUD : public CPointEntity
{
public:
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
	void Spawn() override;
	bool KeyValue(KeyValueData* pkvd) override;
	void EXPORT FreezeUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	bool m_ShowHUD;
};
LINK_ENTITY_TO_CLASS(trigger_hud, CTriggerHUD);

TYPEDESCRIPTION CTriggerHUD::m_SaveData[] =
	{
	DEFINE_FIELD(CTriggerHUD, m_ShowHUD, FIELD_BOOLEAN),
};

IMPLEMENT_SAVERESTORE(CTriggerHUD, CPointEntity);

bool CTriggerHUD::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "display"))
	{
		m_ShowHUD = ( atoi(pkvd->szValue) > 0 ) ? true : false;
		return true;
	}
	return CPointEntity::KeyValue(pkvd);
}
void CTriggerHUD::Spawn()
{
	pev->solid = SOLID_NOT;
	SetUse(&CTriggerHUD::FreezeUse);
}

void CTriggerHUD::FreezeUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	// Grab the local player
	CBasePlayer* pBasePlayer = (CBasePlayer*)UTIL_GetLocalPlayer();
	if ( !pBasePlayer ) return;
	pBasePlayer->SetHasSuit( m_ShowHUD );
}
