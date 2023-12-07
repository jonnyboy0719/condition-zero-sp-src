/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
//=========================================================
// Generic Monster - purely for scripted sequence work.
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

class CGenericMonster : public CBaseMonster
{
public:
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];

	void Spawn() override;
	void Precache() override;
	void SetYawSpeed() override;
	int Classify() override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override;
	int ISoundMask() override;
	void StartMonster() override;
	Vector m_oldOrigin;
};
LINK_ENTITY_TO_CLASS(monster_generic, CGenericMonster);

TYPEDESCRIPTION CGenericMonster::m_SaveData[] =
{
	DEFINE_FIELD(CGenericMonster, m_oldOrigin, FIELD_VECTOR),
};

IMPLEMENT_SAVERESTORE(CGenericMonster, CBaseMonster);


int CGenericMonster::Classify()
{
	return CLASS_NONE;
}


void CGenericMonster::SetYawSpeed()
{
	pev->yaw_speed = 0;
}


void CGenericMonster::HandleAnimEvent(MonsterEvent_t* pEvent)
{
	switch (pEvent->event)
	{
	case 0:
	default:
		CBaseMonster::HandleAnimEvent(pEvent);
		break;
	}
}


int CGenericMonster::ISoundMask()
{
	return bits_SOUND_NONE;
}


void CGenericMonster::Spawn()
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


void CGenericMonster::Precache()
{
	PRECACHE_MODEL((char*)STRING(pev->model));
}


void CGenericMonster::StartMonster()
{
	CBaseMonster::StartMonster();
	UTIL_SetOrigin(pev, m_oldOrigin);
	m_iDeathAngle = pev->angles.y;
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
}
