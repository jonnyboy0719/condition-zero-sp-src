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

LINK_ENTITY_TO_CLASS(weapon_ak47, CAK47);


//=========================================================
//=========================================================
void CAK47::Spawn()
{
	pev->classname = MAKE_STRING("weapon_ak47");
	Precache();
	SET_MODEL(ENT(pev), "models/w_ak47.mdl");
	m_iId = WEAPON_AK47;

	m_iDefaultAmmo = AK47_MAX_CLIP;

	FallInit(); // get ready to fall down.
}


void CAK47::Precache()
{
	PRECACHE_MODEL("models/v_ak47.mdl");
	PRECACHE_MODEL("models/w_ak47.mdl");
	PRECACHE_MODEL("models/p_ak47.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shellTE_MODEL

	PRECACHE_MODEL("models/w_762nato.mdl");
	PRECACHE_MODEL("models/w_762nato_big.mdl");
	PRECACHE_MODEL("models/w_762natobox.mdl");
	PRECACHE_MODEL("models/w_762natobox_big.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/ak47-1.wav");
	PRECACHE_SOUND("weapons/ak47-1.wav");
	PRECACHE_SOUND("weapons/ak47-npc1.wav");
	PRECACHE_SOUND("weapons/ak47_boltpull.wav");
	PRECACHE_SOUND("weapons/ak47_clipin.wav");
	PRECACHE_SOUND("weapons/ak47_clipout.wav");

	PRECACHE_SOUND("weapons/357_cock1.wav");

	m_usM4A1 = PRECACHE_EVENT(1, "events/ak47.sc");
}

bool CAK47::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_AK47);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762nato";
	p->iMaxAmmo1 = _762nato_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = AK47_MAX_CLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iFlags = 0;
	p->iId = m_iId = slot.id;
	p->iWeight = MP5_WEIGHT;

	return true;
}

bool CAK47::Deploy()
{
	auto ret = DefaultDeploy("models/v_ak47.mdl", "models/p_ak47.mdl", AK47_DEPLOY, "mp5");
	if ( !ret ) return false;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.55;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.03;
	return true;
}


void CAK47::PrimaryAttack()
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

#ifdef CLIENT_DLL
	if (bIsMultiplayer())
#else
	if (g_pGameRules->IsMultiplayer())
#endif
	{
		// optimized multiplayer. Widened to make it easier to hit a moving player
		vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, 8192, BULLET_PLAYER_762NATO, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);
	}
	else
	{
		// single player spread
		vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_762NATO, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);
	}

	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usM4A1, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0);

	if (0 == m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);

	m_flNextPrimaryAttack = GetNextAttackDelay(0.1);

	if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
}


void CAK47::Reload()
{
	if (m_pPlayer->ammo_762nato <= 0)
		return;

	DefaultReload(AK47_MAX_CLIP, AK47_RELOAD, 2.45);
}


void CAK47::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	SendWeaponAnim(AK47_IDLE);

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15); // how long till we do this again.
}



class CAK47AmmoClip : public CBasePlayerAmmo
{
	void Spawn() override
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_762nato.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache() override
	{
		PRECACHE_MODEL("models/w_762nato.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo(CBaseEntity* pOther) override
	{
		bool bResult = (pOther->GiveAmmo(AK47_MAX_CLIP, "762nato", _762nato_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS(ammo_762nato, CAK47AmmoClip);

