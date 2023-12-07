
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"

class CDeadNPC : public CBaseMonster
{
public:
	void MonsterInitDead() override;
	void Spawn() override;
	int Classify() override { return CLASS_PLAYER_ALLY; }
};

LINK_ENTITY_TO_CLASS(monster_npc_dead, CDeadNPC);


void CDeadNPC::MonsterInitDead()
{
	CBaseMonster::MonsterInitDead();
	// When the monster starts, apply the pose
	pev->sequence = LookupSequence(STRING(m_iDeathAnim));
	if (pev->sequence == -1)
		ALERT(at_console, "monster_npc_dead with bad pose. Tried to find [%s], but found nothing.\n", STRING(m_iDeathAnim));
	else
		ALERT(at_console, "found pose [%s] for monster_npc_dead.\n", STRING(m_iDeathAnim));
}


void CDeadNPC::Spawn()
{
	PRECACHE_MODEL((char*)STRING(pev->model));
	SET_MODEL(ENT(pev), STRING(pev->model));

	pev->effects = 0;
	pev->yaw_speed = 8;
	pev->sequence = 0;
	m_bloodColor = BLOOD_COLOR_RED;

	// Corpses have less health
	pev->health = 8;

	MonsterInitDead();
}