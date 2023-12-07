
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "triggers.h"

class CChangeKeyValueEntity : public CPointEntity
{
public:
	bool KeyValue(KeyValueData* pkvd) override;
	void Spawn() override;
	void EXPORT ManagerUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	int m_cTargets = 0;
	int m_iTargetName[MAX_MULTI_TARGETS];
	int m_iTargetValue[MAX_MULTI_TARGETS];
};
LINK_ENTITY_TO_CLASS(logic_changekeyvalue, CChangeKeyValueEntity);


bool CChangeKeyValueEntity::KeyValue(KeyValueData* pkvd)
{
	if (m_cTargets < MAX_MULTI_TARGETS)
	{
		char tmp[128];

		UTIL_StripToken(pkvd->szKeyName, tmp);
		m_iTargetName[m_cTargets] = ALLOC_STRING(tmp);
		m_iTargetValue[m_cTargets] = ALLOC_STRING(pkvd->szValue);
		m_cTargets++;
		return true;
	}
	return false;
}


void CChangeKeyValueEntity::Spawn()
{
	pev->solid = SOLID_NOT;
	SetUse(&CChangeKeyValueEntity::ManagerUse);
}


void CChangeKeyValueEntity::ManagerUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	// Don't call this again
	SetUse(NULL);

	// Call the entity, if it exists
	edict_t* pFind = FIND_ENTITY_BY_TARGETNAME(NULL, STRING(pev->target));
	if (!FNullEnt(pFind))
	{
		CBaseEntity* pEnt = CBaseEntity::Instance(pFind);
		// Go trough the target values
		for (int i = 0; i < m_cTargets; i++)
		{
			KeyValueData kvd;
			kvd.szKeyName = STRING(m_iTargetName[i]);
			kvd.szValue = STRING(m_iTargetValue[i]);
			pEnt->KeyValue(&kvd);
		}
	}

	// Delete this entity after use.
	UTIL_Remove(this);
}
