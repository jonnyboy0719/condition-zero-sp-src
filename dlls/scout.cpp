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

LINK_ENTITY_TO_CLASS(weapon_scout, CScout);


//=========================================================
//=========================================================
void CScout::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_scout.mdl");
	m_iId = WEAPON_SCOUT;

	m_iDefaultAmmo = SCOUT_MAX_CLIP;

	FallInit(); // get ready to fall down.
}


void CScout::Precache()
{
	PRECACHE_MODEL("models/v_scout.mdl");
	PRECACHE_MODEL("models/w_scout.mdl");
	PRECACHE_MODEL("models/p_scout.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shellTE_MODEL

	PRECACHE_MODEL("models/w_762nato.mdl");
	PRECACHE_MODEL("models/w_762nato_big.mdl");
	PRECACHE_MODEL("models/w_762natobox.mdl");
	PRECACHE_MODEL("models/w_762natobox_big.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/zoom.wav");
	PRECACHE_SOUND("weapons/scout_fire-1.wav");
	PRECACHE_SOUND("weapons/scout_bolt.wav");
	PRECACHE_SOUND("weapons/scout_clipin.wav");
	PRECACHE_SOUND("weapons/scout_clipout.wav");

	PRECACHE_SOUND("weapons/357_cock1.wav");

	m_usScout = PRECACHE_EVENT(1, "events/scout.sc");
}

bool CScout::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_SCOUT);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762nato";
	p->iMaxAmmo1 = _762nato_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SCOUT_MAX_CLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iFlags = 0;
	p->iId = m_iId = slot.id;
	p->iWeight = AWP_WEIGHT;

	return true;
}

void CScout::UpdateZoomState()
{
	switch ( m_ScopeZoom )
	{
		case ZOOM_NONE: m_pPlayer->m_iFOV = 0; break;
		case ZOOM_4X: m_pPlayer->m_iFOV = 40; break;
		case ZOOM_8X: m_pPlayer->m_iFOV = 20; break;
	}
}

bool CScout::Deploy()
{
	auto ret = DefaultDeploy("models/v_scout.mdl", "models/p_scout.mdl", SCOUT_DEPLOY, "mp5");
	if ( !ret ) return false;
	m_ScopeZoom = ZOOM_NONE;
	UpdateZoomState();
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.01;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.01;
	return true;
}

void CScout::Holster()
{
	m_ScopeZoom = ZOOM_NONE;
	UpdateZoomState();
}


void CScout::PrimaryAttack()
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
	Vector vecDir;

	Vector vCone;
	switch (m_ScopeZoom)
	{
		default:
		case ZOOM_NONE: vCone = VECTOR_CONE_8DEGREES; break;
		case ZOOM_4X: vCone = VECTOR_CONE_1DEGREES; break;
		case ZOOM_8X: vCone = Vector(0, 0, 0); break;
	}
	vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, vCone, 8192, BULLET_PLAYER_762NATO, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usScout, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0);

	if (0 == m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);

	m_flNextPrimaryAttack = GetNextAttackDelay(1.31);

	if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.31;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
}

void CScout::SecondaryAttack()
{
	m_flNextSecondaryAttack = GetNextAttackDelay(0.85);
	switch (m_ScopeZoom)
	{
		case ZOOM_NONE: m_ScopeZoom = ZOOM_4X; break;
		case ZOOM_4X: m_ScopeZoom = ZOOM_8X; break;
		case ZOOM_8X: m_ScopeZoom = ZOOM_NONE; break;
	}
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0, 0xF));
	UpdateZoomState();
}


void CScout::Reload()
{
	if (m_pPlayer->ammo_762nato <= 0)
		return;

	m_ScopeZoom = ZOOM_NONE;
	UpdateZoomState();

	DefaultReload(SCOUT_MAX_CLIP, SCOUT_RELOAD, 2.03);
}


void CScout::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	SendWeaponAnim(SCOUT_IDLE);

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15); // how long till we do this again.
}
