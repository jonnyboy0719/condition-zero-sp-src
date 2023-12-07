
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "shake.h"

class CTriggerEndMission : public CPointEntity
{
public:
	void Spawn( void );
	bool KeyValue( KeyValueData* pkvd ) override;
    void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void ChangeLevel();
	static TYPEDESCRIPTION m_SaveData[];
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	string_t m_music = 0;
	int m_r = 0;
	int m_g = 0;
	int m_b = 0;
	int m_a = 255;
	float m_duration = 2.0;
	float m_holdtime = 0.0;
};
LINK_ENTITY_TO_CLASS(trigger_endmission, CTriggerEndMission);

TYPEDESCRIPTION CTriggerEndMission::m_SaveData[] =
	{
	DEFINE_FIELD(CTriggerEndMission, m_music, FIELD_STRING),
	DEFINE_FIELD(CTriggerEndMission, m_duration, FIELD_FLOAT),
	DEFINE_FIELD(CTriggerEndMission, m_holdtime, FIELD_FLOAT),
	DEFINE_FIELD(CTriggerEndMission, m_r, FIELD_INTEGER),
	DEFINE_FIELD(CTriggerEndMission, m_g, FIELD_INTEGER),
	DEFINE_FIELD(CTriggerEndMission, m_b, FIELD_INTEGER),
	DEFINE_FIELD(CTriggerEndMission, m_a, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CTriggerEndMission, CPointEntity);

bool CTriggerEndMission::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "nextmap"))
	{
		pev->message = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "music"))
	{
		m_music = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "fade_red"))
	{
		m_r = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "fade_green"))
	{
		m_g = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "fade_blue"))
	{
		m_b = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "fade_alpha"))
	{
		m_a = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "duration"))
	{
		m_duration = atof(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		m_holdtime = atof(pkvd->szValue);
		return true;
	}
	return CPointEntity::KeyValue(pkvd);
}

void CTriggerEndMission::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	SetUse(NULL);
	if (FStringNull(pev->message))
	{
		UTIL_Remove( this );
		return;
	}

	CBaseEntity* pClient = UTIL_GetLocalPlayer();
	if (pClient)
	{
		if (!FStringNull(m_music))
		{
			char string[64];
			sprintf(string, "mp3 play %s\n", STRING(m_music));
			CLIENT_COMMAND(pClient->edict(), string);
		}
		UTIL_ScreenFade( pClient, Vector( m_r, m_g, m_b ), m_duration, m_holdtime, m_a, FFADE_OUT );
	}

	pev->nextthink = gpGlobals->time + m_duration + m_holdtime + 1.0;
	SetThink(&CTriggerEndMission::ChangeLevel);
}

void CTriggerEndMission::Spawn()
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
}

void CTriggerEndMission::ChangeLevel()
{
	CBaseEntity* pClient = UTIL_GetLocalPlayer();
	if (pClient)
	{
		char string[64];
		sprintf(string, "map %s\n", STRING(pev->message));
		CLIENT_COMMAND(pClient->edict(), string);
	}
	UTIL_Remove( this );
}
