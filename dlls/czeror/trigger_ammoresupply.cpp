
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "triggers.h"
#include "weapons.h"
#include "player.h"
#include "UserMessages.h"

class CTriggerAmmoResupply : public CBaseTrigger
{
public:
	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;
	static TYPEDESCRIPTION m_SaveData[];
	void Spawn() override;
	void Precache() override;
	bool KeyValue(KeyValueData* pkvd) override;
	void EXPORT TouchResupply(CBaseEntity* pOther);
	void EXPORT OnUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	int m_iDelay;
	bool m_bDisabled = false;
	float m_flLastDelay;
	int m_iResupply;
};
LINK_ENTITY_TO_CLASS(trigger_ammoresupply, CTriggerAmmoResupply);

TYPEDESCRIPTION CTriggerAmmoResupply::m_SaveData[] =
	{
	DEFINE_FIELD(CTriggerAmmoResupply, m_flLastDelay, FIELD_FLOAT),
	DEFINE_FIELD(CTriggerAmmoResupply, m_iDelay, FIELD_INTEGER),
	DEFINE_FIELD(CTriggerAmmoResupply, m_bDisabled, FIELD_BOOLEAN),
	DEFINE_FIELD(CTriggerAmmoResupply, m_iResupply, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CTriggerAmmoResupply, CBaseTrigger);

bool CTriggerAmmoResupply::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "delay"))
	{
		m_iDelay = atoi(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "disabled"))
	{
		m_bDisabled = (atoi(pkvd->szValue) > 0) ? true : false;
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "resupply"))
	{
		m_iResupply = atoi(pkvd->szValue);
		return true;
	}
	return CBaseTrigger::KeyValue(pkvd);
}

void CTriggerAmmoResupply::TouchResupply( CBaseEntity* pOther )
{
	// Disabled? Then stop here.
	if ( m_bDisabled ) return;

	// Make sure its a player
	if ( !FClassnameIs( pOther->pev, "player" ) )
		return;

	// Make sure they are alive
	if ( pOther->pev->deadflag != DEAD_NO )
		return;

	// Don't be able to use this resupply right away if we already used it
	if ( m_flLastDelay > gpGlobals->time ) return;

	const char *szSoundFile = "items/9mmclip1.wav";
	bool bResult = false;
	CBasePlayer* pPlayer = (CBasePlayer*)pOther;
	gEvilImpulse101 = true;
	switch ( m_iResupply )
	{
		// For all active weapons
		case 0:
		{
			for ( int i = 0; i < MAX_ITEM_TYPES; i++ )
			{
				for (auto pCheck = pPlayer->m_rgpPlayerItems[i]; pCheck; pCheck = pCheck->m_pNext)
				{
					bool bCheck = (pOther->GiveAmmo(pCheck->iMaxAmmo1(), (char*)pCheck->pszAmmo1(), pCheck->iMaxAmmo1()) != -1);
					if ( bCheck )
						bResult = true;
				}
			}
		}
		break;

		// Armor
		case 1:
		{
			if ( pOther->pev->armorvalue < MAX_NORMAL_BATTERY )
			{
				MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pOther->pev );
					WRITE_STRING( "item_armor" );
				MESSAGE_END();
				pOther->pev->armorvalue = MAX_NORMAL_BATTERY;
				szSoundFile = "items/kevlar.wav";
				bResult = true;
			}
		}
		break;

		// Health
		case 2:
		{
			if ( pOther->TakeHealth( 100, DMG_GENERIC ) )
			{
				MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pOther->pev );
					WRITE_STRING( "item_healthkit" );
				MESSAGE_END();
				szSoundFile = "items/healthpickup.wav";
				bResult = true;
			}
		}
		break;

		// For the Glock 18, Elites, MP5 and TMP.
		case 3:
		{
			bResult = (pOther->GiveAmmo(_9MM_MAX_CARRY, "9mm", _9MM_MAX_CARRY) != -1);
		}
		break;

		// For the P228.
		case 4:
		{
			// TODO: Add 357 Sig Ammo
		}
		break;

		// For the USP, UMP45 and MAC-10.
		case 5:
		{
			bResult = (pOther->GiveAmmo(_9MM_MAX_CARRY, "45acp", _9MM_MAX_CARRY) != -1);
		}
		break;

		// For the M3 and XM1014.
		case 6:
		{
			bResult = (pOther->GiveAmmo(BUCKSHOT_MAX_CARRY, "buckshot", BUCKSHOT_MAX_CARRY) != -1);
		}
		break;

		// For the P90 and Five-Seven.
		case 7:
		{
			// TODO: 57MM Ammo
		}
		break;

		// For the AWP.
		case 8:
		{
			// TODO: Add 338 Magnum Ammo
		}
		break;

		// For the AK47, Scout, M60, and G3SG1.
		case 9:
		{
			bResult = (pOther->GiveAmmo(_AK47_MAX_CARRY, "762nato", _AK47_MAX_CARRY) != -1);
		}
		break;

		// For the M249, AUG, Famas, Galil, SG550, SG552 and M4A1.
		case 10:
		{
			bResult = (pOther->GiveAmmo(_M4A1_MAX_CARRY, "m4a1", _M4A1_MAX_CARRY) != -1);
		}
		break;

		// For the Desert Eagle.
		case 11:
		{
			bResult = (pOther->GiveAmmo(_357_MAX_CARRY, "357", _357_MAX_CARRY) != -1);
		}
		break;

		// For HE grenades
		case 12:
		{
			bResult = pPlayer->GiveNamedItem("weapon_hegrenade") ? true : false;
		}
		break;

		// For nerv gas grenades
		case 13:
		{
			bResult = pPlayer->GiveNamedItem("weapon_nervgasgrenade") ? true : false;
		}
		break;

		// For smoke grenades
		case 14:
		{
			bResult = pPlayer->GiveNamedItem("weapon_smokegrenade") ? true : false;
		}
		break;

		// For flash grenades
		case 15:
		{
			bResult = pPlayer->GiveNamedItem("weapon_flashbang") ? true : false;
		}
		break;

		// For LAW rocket launcher
		case 16:
		{
			bResult = pPlayer->GiveNamedItem("weapon_law") ? true : false;
		}
		break;
	}
	gEvilImpulse101 = false;

	if ( bResult )
	{
		m_flLastDelay = gpGlobals->time + m_iDelay;
		EMIT_SOUND( pOther->edict(), CHAN_ITEM, szSoundFile, 1, ATTN_NORM );
	}
	else
		m_flLastDelay = gpGlobals->time + 1.0;
}

void CTriggerAmmoResupply::OnUse( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value )
{
	m_bDisabled = !m_bDisabled;
}

void CTriggerAmmoResupply::Spawn()
{
	Precache();
	InitTrigger();
	m_flLastDelay = gpGlobals->time + 1.0f;
	SetTouch(&CTriggerAmmoResupply::TouchResupply);
	SetUse(&CTriggerAmmoResupply::OnUse);
}

void CTriggerAmmoResupply::Precache()
{
	PRECACHE_SOUND( "items/9mmclip1.wav" );
	PRECACHE_SOUND( "items/kevlar.wav" );
	PRECACHE_SOUND( "items/healthpickup.wav" );
}

