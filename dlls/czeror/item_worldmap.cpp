
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "triggers.h"
#include "player.h"
#include "UserMessages.h"

class CItemWorldMap : public CBaseTrigger
{
public:
	void Spawn() override;
	void EXPORT OnUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
};
LINK_ENTITY_TO_CLASS( item_worldmap, CItemWorldMap );

void CItemWorldMap::OnUse( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value )
{
	MESSAGE_BEGIN( MSG_ALL, gmsgWorldMap );
	MESSAGE_END();
}

void CItemWorldMap::Spawn()
{
	InitTrigger();
	SetUse( &CItemWorldMap::OnUse );
}
