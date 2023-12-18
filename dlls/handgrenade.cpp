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


#define HANDGRENADE_PRIMARY_VOLUME 450

LINK_ENTITY_TO_CLASS(weapon_hegrenade, CHandGrenade);


void CHandGrenade::Spawn()
{
	Precache();
	m_iId = WEAPON_HEGRENADE;
	SET_MODEL(ENT(pev), "models/w_hegrenade.mdl");

#ifndef CLIENT_DLL
	pev->dmg = gSkillData.plrDmgHandGrenade;
#endif

	m_iDefaultAmmo = HANDGRENADE_DEFAULT_GIVE;

	FallInit(); // get ready to fall down.
}


void CHandGrenade::Precache()
{
	PRECACHE_SOUND("weapons/hegrenade-1.wav");
	PRECACHE_SOUND("weapons/hegrenade-2.wav");
	PRECACHE_MODEL("models/w_hegrenade.mdl");
	PRECACHE_MODEL("models/v_hegrenade.mdl");
	PRECACHE_MODEL("models/p_hegreande.mdl");
}

bool CHandGrenade::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_HEGRENADE);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hand Grenade";
	p->iMaxAmmo1 = HANDGRENADE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iId = m_iId = slot.id;
	p->iWeight = HANDGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return true;
}


bool CHandGrenade::Deploy()
{
	m_flReleaseThrow = -1;
	return DefaultDeploy("models/v_hegrenade.mdl", "models/p_hegreande.mdl", HANDGRENADE_DRAW, "crowbar");
}

bool CHandGrenade::CanHolster()
{
	// can only holster hand grenades when not primed!
	return (m_flStartThrow == 0);
}

void CHandGrenade::Holster()
{
	//Stop any throw that was in process so players don't blow themselves or somebody else up when the weapon is deployed again.
	m_flStartThrow = 0;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	if (0 != m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		SendWeaponAnim(HANDGRENADE_HOLSTER);
	}
	else
	{
		// no more grenades!
		m_pPlayer->ClearWeaponBit(m_iId);
		SetThink(&CHandGrenade::DestroyItem);
		pev->nextthink = gpGlobals->time + 0.1;
	}

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}

void CHandGrenade::PrimaryAttack()
{
	if (0 == m_flStartThrow && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
	{
		m_flStartThrow = gpGlobals->time;
		m_flReleaseThrow = 0;

		SendWeaponAnim(HANDGRENADE_PINPULL);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
}


void CHandGrenade::WeaponIdle()
{
	if (m_flReleaseThrow == 0 && 0 != m_flStartThrow)
		m_flReleaseThrow = gpGlobals->time;

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (0 != m_flStartThrow)
	{
		Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if (angThrow.x < 0)
			angThrow.x = -10 + angThrow.x * ((90 - 10) / 90.0);
		else
			angThrow.x = -10 + angThrow.x * ((90 + 10) / 90.0);

		float flVel = (90 - angThrow.x) * 4;
		if (flVel > 500)
			flVel = 500;

		UTIL_MakeVectors(angThrow);

		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;

		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;

		// alway explode 3 seconds after the pin was pulled
		float time = m_flStartThrow - gpGlobals->time + 3.0;
		if (time < 0)
			time = 0;

		CGrenade::ShootTimed(m_pPlayer->pev, vecSrc, vecThrow, time, GrenadeType());

		if (flVel < 500)
		{
			SendWeaponAnim(HANDGRENADE_THROW1);
		}
		else if (flVel < 1000)
		{
			SendWeaponAnim(HANDGRENADE_THROW2);
		}
		else
		{
			SendWeaponAnim(HANDGRENADE_THROW3);
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		//m_flReleaseThrow = 0;
		m_flStartThrow = 0;
		m_flNextPrimaryAttack = GetNextAttackDelay(0.5);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

		if (0 == m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			// just threw last grenade
			// set attack times in the future, and weapon idle in the future so we can see the whole throw
			// animation, weapon idle will automatically retire the weapon for us.
			m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = GetNextAttackDelay(0.5); // ensure that the animation can finish playing
		}
		return;
	}
	else if (m_flReleaseThrow > 0)
	{
		// we've finished the throw, restart.
		m_flStartThrow = 0;

		if (0 != m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			SendWeaponAnim(HANDGRENADE_DRAW);
		}
		else
		{
			RetireWeapon();
			return;
		}

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
		m_flReleaseThrow = -1;
		return;
	}

	if (0 != m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		int iAnim;
		float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);
		if (flRand <= 0.75)
		{
			iAnim = HANDGRENADE_IDLE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15); // how long till we do this again.
		}
		else
		{
			iAnim = HANDGRENADE_FIDGET;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 75.0 / 30.0;
		}

		SendWeaponAnim(iAnim);
	}
}

//==========================================================================
// These are copies of the HE grenade, except it throws another type of grenade
//==========================================================================

//==========================================================================
// Flash Grenade
LINK_ENTITY_TO_CLASS(weapon_flashbang, CFlashGrenade);

void CFlashGrenade::Spawn()
{
	Precache();
	m_iId = WEAPON_FLASHGRENADE;
	SET_MODEL(ENT(pev), "models/w_flashbang.mdl");
	m_iDefaultAmmo = HANDGRENADE_DEFAULT_GIVE;
	FallInit();
}

void CFlashGrenade::Precache()
{
	PRECACHE_SOUND("weapons/flashbang-1.wav");
	PRECACHE_SOUND("weapons/flashbang-2.wav");
	PRECACHE_MODEL("models/w_flashbang.mdl");
	PRECACHE_MODEL("models/v_flashbang.mdl");
	PRECACHE_MODEL("models/p_flashbang.mdl");
}

bool CFlashGrenade::Deploy()
{
	m_flReleaseThrow = -1;
	return DefaultDeploy("models/v_flashbang.mdl", "models/p_flashbang.mdl", HANDGRENADE_DRAW, "crowbar");
}

bool CFlashGrenade::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_FLASHGRENADE);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Flash Grenade";
	p->iMaxAmmo1 = HANDGRENADE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iId = m_iId = slot.id;
	p->iWeight = HANDGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	return true;
}

//==========================================================================
// Smoke Grenade
LINK_ENTITY_TO_CLASS(weapon_smokegrenade, CSmokeGrenade);

void CSmokeGrenade::Spawn()
{
	Precache();
	m_iId = WEAPON_SMOKEGRENADE;
	SET_MODEL(ENT(pev), "models/w_smokegrenade.mdl");
	m_iDefaultAmmo = HANDGRENADE_DEFAULT_GIVE;
	FallInit();
}

void CSmokeGrenade::Precache()
{
	PRECACHE_SOUND("weapons/sg_explode.wav");
	PRECACHE_MODEL("models/w_smokegrenade.mdl");
	PRECACHE_MODEL("models/v_smokegrenade.mdl");
	PRECACHE_MODEL("models/p_smokegrenade.mdl");
}

bool CSmokeGrenade::Deploy()
{
	m_flReleaseThrow = -1;
	return DefaultDeploy("models/v_smokegrenade.mdl", "models/p_smokegrenade.mdl", HANDGRENADE_DRAW, "crowbar");
}

bool CSmokeGrenade::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_SMOKEGRENADE);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Smoke Grenade";
	p->iMaxAmmo1 = HANDGRENADE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iId = m_iId = slot.id;
	p->iWeight = HANDGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	return true;
}

//==========================================================================
// Nerv gas Grenade
LINK_ENTITY_TO_CLASS(weapon_nervgasgrenade, CNervGasGrenade);

void CNervGasGrenade::Spawn()
{
	Precache();
	m_iId = WEAPON_GASGRENADE;
	SET_MODEL(ENT(pev), "models/w_gasgrenade.mdl");
	m_iDefaultAmmo = HANDGRENADE_DEFAULT_GIVE;
	FallInit();
}

void CNervGasGrenade::Precache()
{
	PRECACHE_MODEL("models/w_gasgrenade.mdl");
	PRECACHE_MODEL("models/v_gasgrenade.mdl");
	PRECACHE_MODEL("models/p_gasgrenade.mdl");
}

bool CNervGasGrenade::Deploy()
{
	m_flReleaseThrow = -1;
	return DefaultDeploy("models/v_gasgrenade.mdl", "models/p_gasgrenade.mdl", HANDGRENADE_DRAW, "crowbar");
}

bool CNervGasGrenade::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_GASGRENADE);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "NervGas Grenade";
	p->iMaxAmmo1 = HANDGRENADE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iId = m_iId = slot.id;
	p->iWeight = HANDGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
	return true;
}
