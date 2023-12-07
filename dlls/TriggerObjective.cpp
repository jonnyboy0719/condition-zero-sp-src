
#include "extdll.h"
#include "util.h"
#include "cbase.h"

class CTriggerObjective : public CPointEntity
{
public:
	bool KeyValue(KeyValueData* pkvd) override;
	void Spawn() override;
	void DoObjectiveSend();
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	void Think() override;
    void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	static TYPEDESCRIPTION m_SaveData[];
	string_t m_obj;
	int m_notransit;
	int m_state;
};
LINK_ENTITY_TO_CLASS(trigger_objective, CTriggerObjective);

TYPEDESCRIPTION CTriggerObjective::m_SaveData[] =
{
	DEFINE_FIELD(CTriggerObjective, m_obj, FIELD_STRING),
	DEFINE_FIELD(CTriggerObjective, m_notransit, FIELD_INTEGER),
	DEFINE_FIELD(CTriggerObjective, m_state, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CTriggerObjective, CPointEntity);

bool CTriggerObjective::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "objective"))
	{
		m_obj = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "notransition"))
	{
		m_notransit = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "state"))
	{
		int y = atoi(pkvd->szValue);
		if (y < 0) y = 0;
		else if (y > 2) y = 2;
		m_state = y;
		return true;
	}
	return CPointEntity::KeyValue(pkvd);
}

void CTriggerObjective::Think()
{
	SetThink(NULL);
	DoObjectiveSend();
}

void CTriggerObjective::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	int y = m_state;
	y++;
	if (y < 0) y = 0;
	else if (y > 2) y = 2;
	m_state = y;
	DoObjectiveSend();
}

void CTriggerObjective::Spawn()
{
	CPointEntity::Spawn();
	pev->nextthink = gpGlobals->time + 1.0;
}

extern int gmsgObjective;

void CTriggerObjective::DoObjectiveSend()
{
	char text[64];
	sprintf(text, "%s", STRING(m_obj));

	MESSAGE_BEGIN(MSG_ALL, gmsgObjective);
	WRITE_BYTE(m_state);
	WRITE_BYTE(m_notransit);
	WRITE_STRING(text);
	MESSAGE_END();
}
