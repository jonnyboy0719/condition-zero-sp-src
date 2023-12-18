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

LINK_ENTITY_TO_CLASS(weapon_m60, CM60);


//=========================================================
//=========================================================
void CM60::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_m60.mdl");
	m_iId = WEAPON_M60;

	m_iDefaultAmmo = M60_MAX_CLIP;

	FallInit(); // get ready to fall down.
}


void CM60::Precache()
{
	PRECACHE_MODEL("models/v_m60.mdl");
	PRECACHE_MODEL("models/w_m60.mdl");
	PRECACHE_MODEL("models/p_m60.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shellTE_MODEL

	PRECACHE_MODEL("models/w_762nato.mdl");
	PRECACHE_MODEL("models/w_762nato_big.mdl");
	PRECACHE_MODEL("models/w_762natobox.mdl");
	PRECACHE_MODEL("models/w_762natobox_big.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/m60-1.wav");
	PRECACHE_SOUND("weapons/m60-1.wav");
	PRECACHE_SOUND("weapons/m60_boxin.wav");
	PRECACHE_SOUND("weapons/m60_boxout.wav");
	PRECACHE_SOUND("weapons/m60_chain.wav");
	PRECACHE_SOUND("weapons/m60_coverdown.wav");
	PRECACHE_SOUND("weapons/m60_coverup.wav");
	PRECACHE_SOUND("weapons/m60_slideback1.wav");

	PRECACHE_SOUND("weapons/357_cock1.wav");

	m_usM60 = PRECACHE_EVENT(1, "events/m60.sc");
}

bool CM60::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_M60);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762nato";
	p->iMaxAmmo1 = _762nato_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = M60_MAX_CLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iId = m_iId = slot.id;
	p->iFlags = 0;
	p->iWeight = M60_WEIGHT;

	return true;
}

bool CM60::Deploy()
{
	auto ret = DefaultDeploy("models/v_m60.mdl", "models/p_m60.mdl", M60_DRAW, "mp5");
	if ( !ret ) return false;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.55;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.03;
	return true;
}


void CM60::PrimaryAttack()
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

	// single player spread
	vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, 8192, BULLET_PLAYER_762NATO, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usM60, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0);

	if (0 == m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);

	m_flNextPrimaryAttack = GetNextAttackDelay(0.075);

	if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.075;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
}


void CM60::Reload()
{
	if (m_pPlayer->ammo_762nato <= 0)
		return;

	DefaultReload(M60_MAX_CLIP, M60_RELOAD, 5.8);
}


void CM60::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_8DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	SendWeaponAnim(M60_IDLE);

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15); // how long till we do this again.
}



class CM60AmmoBox : public CBasePlayerAmmo
{
	void Spawn() override
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_762natobox.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache() override
	{
		PRECACHE_MODEL("models/w_762natobox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo(CBaseEntity* pOther) override
	{
		bool bResult = (pOther->GiveAmmo(M60_MAX_CLIP, "762nato", _762nato_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS(ammo_762natobox, CM60AmmoBox);

