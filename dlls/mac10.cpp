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
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_mac10, CMAC10);


//=========================================================
//=========================================================
void CMAC10::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_mac10.mdl");
	m_iId = WEAPON_MAC10;

	m_iDefaultAmmo = MAC10_DEFAULT_GIVE;

	FallInit(); // get ready to fall down.
}


void CMAC10::Precache()
{
	PRECACHE_MODEL("models/v_mac10.mdl");
	PRECACHE_MODEL("models/w_mac10.mdl");
	PRECACHE_MODEL("models/p_mac10.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shellTE_MODEL

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/mac10-1.wav");
	PRECACHE_SOUND("weapons/mac10_clipout.wav");
	PRECACHE_SOUND("weapons/mac10_clipin.wav");
	PRECACHE_SOUND("weapons/mac10_boltpull.wav");

	PRECACHE_SOUND("weapons/357_cock1.wav");

	m_usEvent = PRECACHE_EVENT(1, "events/mac10.sc");
}

bool CMAC10::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_MAC10);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "45acp";
	p->iMaxAmmo1 = _45ACP_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = MAC10_MAX_CLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iFlags = 0;
	p->iId = m_iId = slot.id;
	p->iWeight = MP5_WEIGHT;

	return true;
}

bool CMAC10::Deploy()
{
	bool ret = DefaultDeploy("models/v_mac10.mdl", "models/p_mac10.mdl", MAC10_DEPLOY, "mp5");
	if ( !ret ) return false;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.03;
	return ret;
}


void CMAC10::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;


	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_45ACP, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usEvent, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0);

	if (0 == m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);

	m_flNextPrimaryAttack = GetNextAttackDelay(0.1);

	if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
}


void CMAC10::SecondaryAttack()
{
}

void CMAC10::Reload()
{
	if (m_pPlayer->ammo_45acp <= 0)
		return;

	DefaultReload(MAC10_MAX_CLIP, MAC10_RELOAD, 3.26);
}


void CMAC10::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	SendWeaponAnim( UMP45_IDLE );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15); // how long till we do this again.
}
