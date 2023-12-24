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

LINK_ENTITY_TO_CLASS(weapon_ump45, CUMP45);


//=========================================================
//=========================================================
void CUMP45::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_ump45.mdl");
	m_iId = WEAPON_UMP45;

	m_iDefaultAmmo = UMP_DEFAULT_GIVE;

	FallInit(); // get ready to fall down.
}


void CUMP45::Precache()
{
	PRECACHE_MODEL("models/v_ump45.mdl");
	PRECACHE_MODEL("models/w_ump45.mdl");
	PRECACHE_MODEL("models/p_ump45.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shellTE_MODEL

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/ump45-1.wav");
	PRECACHE_SOUND("weapons/ump45_clipout.wav");
	PRECACHE_SOUND("weapons/ump45_clipin.wav");
	PRECACHE_SOUND("weapons/ump45_boltslap.wav");

	PRECACHE_SOUND("weapons/357_cock1.wav");

	m_usEvent = PRECACHE_EVENT(1, "events/ump45.sc");
}

bool CUMP45::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_UMP45);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "45acp";
	p->iMaxAmmo1 = _45ACP_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = UMP_MAX_CLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iFlags = 0;
	p->iId = m_iId = slot.id;
	p->iWeight = MP5_WEIGHT;

	return true;
}

bool CUMP45::Deploy()
{
	bool ret = DefaultDeploy("models/v_ump45.mdl", "models/p_ump45.mdl", UMP45_DEPLOY, "mp5");
	if ( !ret ) return false;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.03;
	return ret;
}


void CUMP45::PrimaryAttack()
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
	Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_4DEGREES, 8192, BULLET_PLAYER_45ACP, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);

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

	m_flNextPrimaryAttack = GetNextAttackDelay(0.09);

	if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.09;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
}


void CUMP45::SecondaryAttack()
{
}

void CUMP45::Reload()
{
	if (m_pPlayer->ammo_45acp <= 0)
		return;

	DefaultReload(UMP_MAX_CLIP, UMP45_RELOAD, 3.51);
}


void CUMP45::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	SendWeaponAnim( UMP45_IDLE );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15); // how long till we do this again.
}
