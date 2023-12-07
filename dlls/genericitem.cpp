//=========================================================
// Generic Item
//=========================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "schedule.h"
#include "soundent.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================

class CGenericItem : public CBaseMonster
{
public:
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];

	void Spawn() override;
	void Precache() override;
	void SetYawSpeed() override;
	int Classify() override;
	void StartMonster() override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override;
	int ISoundMask() override;
	bool TakeHealth(float flHealth, int bitsDamageType) override { return false; }
	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override { return false; }
    void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	bool KeyValue(KeyValueData* pkvd) override;
	string_t m_iPose;
	Vector m_oldOrigin;
	bool m_removeonuse;
};
LINK_ENTITY_TO_CLASS(item_generic, CGenericItem);

TYPEDESCRIPTION CGenericItem::m_SaveData[] =
{
	DEFINE_FIELD(CGenericItem, m_iPose, FIELD_STRING),
	DEFINE_FIELD(CGenericItem, m_oldOrigin, FIELD_VECTOR),
	DEFINE_FIELD(CGenericItem, m_removeonuse, FIELD_BOOLEAN),
};

IMPLEMENT_SAVERESTORE(CGenericItem, CBaseMonster);

int CGenericItem::Classify()
{
	return CLASS_NONE;
}


void CGenericItem::SetYawSpeed()
{
	pev->yaw_speed = 0;
}


void CGenericItem::HandleAnimEvent(MonsterEvent_t* pEvent)
{
	switch (pEvent->event)
	{
	case 0:
	default:
		CBaseMonster::HandleAnimEvent(pEvent);
		break;
	}
}


int CGenericItem::ISoundMask()
{
	return bits_SOUND_NONE;
}

void CGenericItem::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	if ( m_removeonuse )
	{
		UTIL_Remove( this );
		return;
	}
}

bool CGenericItem::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "sequencename"))
	{
		m_iPose = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "removeonuse"))
	{
		m_removeonuse = atoi(pkvd->szValue) >= 1 ? true : false;
		return true;
	}

	return CBaseMonster::KeyValue(pkvd);
}


void CGenericItem::StartMonster()
{
	CBaseMonster::StartMonster();
	UTIL_SetOrigin(pev, m_oldOrigin);
	//pev->solid = SOLID_BBOX;
	if (!FStringNull(m_iPose))
	{
		// When the monster starts, apply the pose
		pev->sequence = LookupSequence(STRING(m_iPose));
		if (pev->sequence == -1)
			ALERT(at_console, "item_generic with bad pose. Tried to find [%s], but found nothing.\n", STRING(m_iPose));
	}
}


void CGenericItem::Spawn()
{
	Precache();

	SET_MODEL(ENT(pev), STRING(pev->model));

	UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);
	UTIL_SetOrigin(pev, pev->origin);
	m_oldOrigin = pev->origin;

	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_FLY;
	pev->health = -1;
	m_bloodColor = DONT_BLEED;
	m_flFieldOfView = 0;
	m_MonsterState = MONSTERSTATE_NONE;

	MonsterInit();
}


void CGenericItem::Precache()
{
	PRECACHE_MODEL((char*)STRING(pev->model));
}
