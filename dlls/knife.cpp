/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"


#define CROWBAR_BODYHIT_VOLUME 128
#define CROWBAR_WALLHIT_VOLUME 512

LINK_ENTITY_TO_CLASS(weapon_knife, CKnife);

void CKnife::Spawn()
{
	Precache();
	m_iId = WEAPON_KNIFE;
	SET_MODEL(ENT(pev), "models/w_knife.mdl");
	m_iClip = -1;

	FallInit(); // get ready to fall down.
}


void CKnife::Precache()
{
	PRECACHE_MODEL("models/v_knife.mdl");
	PRECACHE_MODEL("models/w_knife.mdl");
	PRECACHE_MODEL("models/p_knife.mdl");
	PRECACHE_SOUND("weapons/knife_hit1.wav");
	PRECACHE_SOUND("weapons/knife_hit2.wav");
	PRECACHE_SOUND("weapons/knife_hit3.wav");
	PRECACHE_SOUND("weapons/knife_hit4.wav");
	PRECACHE_SOUND("weapons/knife_hitwall1.wav");
	PRECACHE_SOUND("weapons/knife_slash1.wav");
	PRECACHE_SOUND("weapons/knife_slash2.wav");
	PRECACHE_SOUND("weapons/knife_stab.wav");

	m_usMelee = PRECACHE_EVENT(1, "events/knife.sc");
}

bool CKnife::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_KNIFE);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iId = m_iId = slot.id;
	p->iWeight = CROWBAR_WEIGHT;
	return true;
}



bool CKnife::Deploy()
{
	return DefaultDeploy("models/v_knife.mdl", "models/p_knife.mdl", KNIFE_DRAW, "crowbar");
}

void CKnife::Holster()
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim(KNIFE_DRAW);
}


void FindHullIntersection(const Vector& vecSrc, TraceResult& tr, const Vector& mins, const Vector& maxs, edict_t* pEntity)
{
	int i, j, k;
	float distance;
	const Vector* minmaxs[2] = {&mins, &maxs};
	TraceResult tmpTrace;
	Vector vecHullEnd = tr.vecEndPos;
	Vector vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc) * 2);
	UTIL_TraceLine(vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace);
	if (tmpTrace.flFraction < 1.0)
	{
		tr = tmpTrace;
		return;
	}

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			for (k = 0; k < 2; k++)
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i]->x;
				vecEnd.y = vecHullEnd.y + minmaxs[j]->y;
				vecEnd.z = vecHullEnd.z + minmaxs[k]->z;

				UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace);
				if (tmpTrace.flFraction < 1.0)
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if (thisDistance < distance)
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}


void CKnife::PrimaryAttack()
{
	if (!Swing(true))
	{
		SetThink(&CKnife::SwingAgain);
		pev->nextthink = gpGlobals->time + 0.1;
	}
}


void CKnife::SecondaryAttack()
{
	// Don't do any primary swings
	SetThink( NULL );

	TraceResult tr;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

#ifndef CLIENT_DLL
	if (tr.flFraction >= 1.0)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);
		if (tr.flFraction < 1.0)
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);
			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict());
			vecEnd = tr.vecEndPos; // This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	if (tr.flFraction >= 1.0)
	{
		SendWeaponAnim(KNIFE_STAB_MISS);
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = GetNextAttackDelay(1.0);
		PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usMelee,
			0.0, g_vecZero, g_vecZero, 0, 0, 0,
			0.0, 0, 0.0);
	}
	else
	{
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = GetNextAttackDelay(1.5);
		SendWeaponAnim(KNIFE_STAB);
#ifndef CLIENT_DLL
		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);
		ClearMultiDamage();
		pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgKnifeStab, gpGlobals->v_forward, &tr, DMG_CLUB | DMG_NEVERGIB);
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		bool fHitWorld = true;

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/knife_stab.wav", 1, ATTN_NORM);
				m_pPlayer->m_iWeaponVolume = CROWBAR_BODYHIT_VOLUME;
				if (!pEntity->IsAlive())
					return;
				else
					flVol = 0.1;

				fHitWorld = false;
			}
		}

		if (fHitWorld)
		{
			float fvolbar = TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);

			if (g_pGameRules->IsMultiplayer())
			{
				// override the volume here, cause we don't play texture sounds in multiplayer,
				// and fvolbar is going to be 0 from the above call.

				fvolbar = 1;
			}

			// also play crowbar strike
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/knife_hitwall1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));

			// delay the decal a bit
			m_trHit = tr;
		}

		m_pPlayer->m_iWeaponVolume = flVol * CROWBAR_WALLHIT_VOLUME;
#endif
		SetThink( &CKnife::Smack );
		pev->nextthink = gpGlobals->time + 0.2;
	}
}


void CKnife::Smack()
{
	DecalGunshot(&m_trHit, BULLET_PLAYER_CROWBAR);
}


void CKnife::SwingAgain()
{
	Swing(false);
}


bool CKnife::Swing(bool fFirst)
{
	bool fDidHit = false;

	TraceResult tr;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

#ifndef CLIENT_DLL
	if (tr.flFraction >= 1.0)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);
		if (tr.flFraction < 1.0)
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);
			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict());
			vecEnd = tr.vecEndPos; // This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	if (fFirst)
	{
		PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usMelee,
			0.0, g_vecZero, g_vecZero, 0, 0, 0,
			0.0, 0, 0.0);
	}

	switch (((m_iSwing++) % 3) + 1)
	{
	case 0:
		SendWeaponAnim(KNIFE_SLASH1);
		break;
	case 1:
		SendWeaponAnim(KNIFE_SLASH2);
		break;
	case 2:
		SendWeaponAnim(KNIFE_MIDSLASH1);
		break;
	case 3:
		SendWeaponAnim(KNIFE_MIDSLASH2);
		break;
	}

	if (tr.flFraction >= 1.0)
	{
		if (fFirst)
		{
			// miss
			m_flNextSecondaryAttack = m_flNextPrimaryAttack = GetNextAttackDelay(0.5);

			// player "shoot" animation
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
		}
	}
	else
	{
		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

#ifndef CLIENT_DLL

		// hit
		fDidHit = true;
		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);

		ClearMultiDamage();

		if ((m_flNextPrimaryAttack + 1 < UTIL_WeaponTimeBase()) || g_pGameRules->IsMultiplayer())
		{
			// first swing does full damage
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgKnife, gpGlobals->v_forward, &tr, DMG_CLUB);
		}
		else
		{
			// subsequent swings do half
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgKnife / 2, gpGlobals->v_forward, &tr, DMG_CLUB);
		}
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

#endif

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = GetNextAttackDelay(0.25);

#ifndef CLIENT_DLL
		// play thwack, smack, or dong sound
		float flVol = 1.0;
		bool fHitWorld = true;

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				// play thwack or smack sound
				switch (RANDOM_LONG(0, 3))
				{
				case 0:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/knife_hit1.wav", 1, ATTN_NORM);
					break;
				case 1:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/knife_hit2.wav", 1, ATTN_NORM);
					break;
				case 2:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/knife_hit3.wav", 1, ATTN_NORM);
					break;
				case 3:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/knife_hit4.wav", 1, ATTN_NORM);
					break;
				}
				m_pPlayer->m_iWeaponVolume = CROWBAR_BODYHIT_VOLUME;
				if (!pEntity->IsAlive())
					return true;
				else
					flVol = 0.1;

				fHitWorld = false;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		if (fHitWorld)
		{
			float fvolbar = TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);

			if (g_pGameRules->IsMultiplayer())
			{
				// override the volume here, cause we don't play texture sounds in multiplayer,
				// and fvolbar is going to be 0 from the above call.

				fvolbar = 1;
			}

			// also play crowbar strike
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/knife_hitwall1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));

			// delay the decal a bit
			m_trHit = tr;
		}

		m_pPlayer->m_iWeaponVolume = flVol * CROWBAR_WALLHIT_VOLUME;
#endif
		SetThink(&CKnife::Smack);
		pev->nextthink = gpGlobals->time + 0.2;
	}
	return fDidHit;
}
