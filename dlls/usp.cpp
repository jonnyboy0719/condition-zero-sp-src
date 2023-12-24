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

LINK_ENTITY_TO_CLASS(weapon_usp, CUSP);

void CUSP::Spawn()
{
	Precache();
	m_iId = WEAPON_USP;
	SET_MODEL(ENT(pev), "models/w_usp.mdl");

	m_iDefaultAmmo = GLOCK_DEFAULT_GIVE;

	FallInit(); // get ready to fall down.
}


void CUSP::Precache()
{
	PRECACHE_MODEL("models/v_usp.mdl");
	PRECACHE_MODEL("models/w_usp.mdl");
	PRECACHE_MODEL("models/p_usp.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shell

	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/9mmclip2.wav");

	PRECACHE_SOUND("weapons/usp_clipin.wav");
	PRECACHE_SOUND("weapons/usp_clipout.wav");
	PRECACHE_SOUND("weapons/usp_silencer_off.wav");
	PRECACHE_SOUND("weapons/usp_silencer_on.wav");
	PRECACHE_SOUND("weapons/usp_slideback.wav");
	PRECACHE_SOUND("weapons/usp_sliderelease.wav");
	PRECACHE_SOUND("weapons/usp_unsil-1.wav");

	m_usFire = PRECACHE_EVENT(1, "events/usp.sc");
}

bool CUSP::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_USP);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "45acp";
	p->iMaxAmmo1 = _45ACP_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = GLOCK_MAX_CLIP;
	p->iSlot = slot.slot;
	p->iPosition = slot.position;
	p->iFlags = 0;
	p->iId = m_iId = slot.id;
	p->iWeight = GLOCK_WEIGHT;

	return true;
}

bool CUSP::Deploy()
{
	// pev->body = 1;
	return DefaultDeploy("models/v_usp.mdl", "models/p_usp.mdl", USP_DRAW, "onehanded");
}

void CUSP::PrimaryAttack()
{
	GlockFire(0.01, 0.3, true);
}

void CUSP::GlockFire(float flSpread, float flCycleTime, bool fUseAutoAim)
{
	if (m_iClip <= 0)
	{
		//if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(0.2);
		}

		return;
	}

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	int flags;

#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	// silenced
	if (pev->body == 1)
	{
		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
	}
	else
	{
		// non-silenced
		m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	}

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming;

	if (fUseAutoAim)
	{
		vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);
	}
	else
	{
		vecAiming = gpGlobals->v_forward;
	}

	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, Vector(flSpread, flSpread, flSpread), 8192, BULLET_PLAYER_45ACP, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFire, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, (m_iClip == 0) ? 1 : 0, 0);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	if (0 == m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
}


void CUSP::Reload()
{
	if (m_pPlayer->ammo_45acp <= 0)
		return;

	bool iResult;

	if (m_iClip == 0)
		iResult = DefaultReload(17, USP_RELOAD, 1.5);
	else
		iResult = DefaultReload(17, USP_RELOAD_NOT_EMPTY, 1.5);

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
	}
}



void CUSP::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// only idle if the slid isn't back
	if (m_iClip != 0)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
		SendWeaponAnim(USP_IDLE1);
	}
}








class CUSPAmmo : public CBasePlayerAmmo
{
	void Spawn() override
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_45acp.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache() override
	{
		PRECACHE_MODEL("models/w_45acp.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo(CBaseEntity* pOther) override
	{
		if (pOther->GiveAmmo(AMMO_GLOCKCLIP_GIVE, "45acp", _9MM_MAX_CARRY) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(ammo_45acp, CUSPAmmo);
