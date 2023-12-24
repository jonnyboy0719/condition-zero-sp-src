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

LINK_ENTITY_TO_CLASS(weapon_glock, CGlock);

enum GlockState_e
{
	STATE_SINGLE = 0,
	STATE_BURST,
};

void CGlock::Spawn()
{
	Precache();
	m_iId = WEAPON_GLOCK;
	SET_MODEL(ENT(pev), "models/w_glock.mdl");

	m_iDefaultAmmo = GLOCK_DEFAULT_GIVE;
	m_iWeaponState = GlockState_e::STATE_SINGLE;

	FallInit(); // get ready to fall down.
}


void CGlock::Precache()
{
	PRECACHE_MODEL("models/v_glock.mdl");
	PRECACHE_MODEL("models/w_glock.mdl");
	PRECACHE_MODEL("models/p_glock.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shell

	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/9mmclip2.wav");
	PRECACHE_SOUND("items/mode_switch.wav");

	PRECACHE_SOUND("weapons/pl_gun1.wav"); //silenced handgun
	PRECACHE_SOUND("weapons/pl_gun2.wav"); //silenced handgun
	PRECACHE_SOUND("weapons/slideback1.wav");
	PRECACHE_SOUND("weapons/clipout1.wav");
	PRECACHE_SOUND("weapons/clipin1.wav");
	PRECACHE_SOUND("weapons/sliderelease1.wav");
	PRECACHE_SOUND("weapons/glock18-1.wav"); // Double
	PRECACHE_SOUND("weapons/glock18-2.wav"); // Single

	m_usFire = PRECACHE_EVENT(1, "events/glock.sc");
}

bool CGlock::GetItemInfo(ItemInfo* p)
{
	WeaponSlots slot = GetWeaponSlotInfo(WEAPON_GLOCK);
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
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

bool CGlock::Deploy()
{
	// pev->body = 1;
	bool ret = DefaultDeploy("models/v_glock.mdl", "models/p_glock.mdl", GLOCK_DRAW, "onehanded");
	if ( !ret ) return false;
	m_iBullets = 0;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.55;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.11;
	return true;
}

void CGlock::ItemPostFrame()
{
	// For burst fire
	if ( m_iBullets > 0 && m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() )
	{
		GlockFire();
		return;
	}
	CBasePlayerWeapon::ItemPostFrame();
}

void CGlock::PrimaryAttack()
{
	if ( m_iBullets > 0 ) return;
	switch (m_iWeaponState)
	{
		case GlockState_e::STATE_SINGLE: m_iBullets = 1; break;
		case GlockState_e::STATE_BURST: m_iBullets = 3; break;
	}
	if ( m_iClip == 1 )
		m_iBullets = 1;
	GlockFire();
}

void CGlock::SecondaryAttack()
{
	if ( m_iBullets > 0 ) return;
	switch (m_iWeaponState)
	{
		case GlockState_e::STATE_SINGLE:
		{
			m_iWeaponState = GlockState_e::STATE_BURST;
			ClientPrint( m_pPlayer->pev, HUD_PRINTCENTER, "#Switch_To_BurstFire" );
		}
		break;
		case GlockState_e::STATE_BURST:
		{
			m_iWeaponState = GlockState_e::STATE_SINGLE;
			ClientPrint( m_pPlayer->pev, HUD_PRINTCENTER, "#Switch_To_SemiAuto" );
		}
		break;
	}
	EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/mode_switch.wav", 1, ATTN_NORM);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(0.5);
}

void CGlock::GlockFire()
{
	if (m_iClip <= 0)
	{
		//if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(0.2);
		}
		m_iBullets = 0;
		return;
	}

	float flSpread;
	float flCycleTime;
	switch (m_iWeaponState)
	{
		case GlockState_e::STATE_SINGLE:
		{
			flSpread = 0.01;
			flCycleTime = 0.3;
		}
		break;
		case GlockState_e::STATE_BURST:
		{
			flSpread = 0.1;
			flCycleTime = 0.1;
		}
		break;
	}

	m_iClip -= 1;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	int flags;

#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming;

	vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, Vector(flSpread, flSpread, flSpread), 8192, BULLET_PLAYER_9MM, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFire, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, m_iBullets, m_iWeaponState, (m_iClip == 0) ? 1 : 0, 0);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	if (0 == m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);

	m_iBullets--;

	if ( m_iBullets <= 0 )
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.03;
}



void CGlock::Reload()
{
	if (m_pPlayer->ammo_9mm <= 0)
		return;

	bool iResult = DefaultReload(17, GLOCK_RELOAD, 1.5);
	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
	}
}



void CGlock::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// only idle if the slid isn't back
	if (m_iClip != 0)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 16.0;
		SendWeaponAnim(GLOCK_IDLE1);
	}
}








class CGlockAmmo : public CBasePlayerAmmo
{
	void Spawn() override
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_9mmclip_big.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache() override
	{
		PRECACHE_MODEL("models/w_9mmclip_big.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo(CBaseEntity* pOther) override
	{
		if (pOther->GiveAmmo(AMMO_GLOCKCLIP_GIVE, "9mm", _9MM_MAX_CARRY) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(ammo_9mmclip, CGlockAmmo);
LINK_ENTITY_TO_CLASS(ammo_9mm, CGlockAmmo);
