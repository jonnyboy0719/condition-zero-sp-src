/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
//
//  hud_msg.cpp
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "r_efx.h"
#include "ScriptedSequence.h"
#include "particleman.h"
#include "rain.h"

extern IParticleMan* g_pParticleMan;
extern rain_properties Rain;

#ifdef HL_WEAPONS
extern BEAM* pBeam;
extern BEAM* pBeam2;
extern TEMPENTITY* pFlare; // Vit_amiN
#endif


/// USER-DEFINED SERVER MESSAGE HANDLERS

bool CHud::MsgFunc_ResetHUD(const char* pszName, int iSize, void* pbuf)
{
	ASSERT(iSize == 0);

	// clear all hud data
	HUDLIST* pList = m_pHudList;

	while (pList)
	{
		if (pList->p)
			pList->p->Reset();
		pList = pList->pNext;
	}

	//Reset weapon bits.
	m_iWeaponBits = 0ULL;

	// reset sensitivity
	m_flMouseSensitivity = 0;

	// reset concussion effect
	m_iConcussionEffect = 0;

	return true;
}

void CAM_ToFirstPerson();

void CHud::MsgFunc_ViewMode(const char* pszName, int iSize, void* pbuf)
{
	CAM_ToFirstPerson();
}

void CHud::MsgFunc_InitHUD(const char* pszName, int iSize, void* pbuf)
{
	// prepare all hud data
	HUDLIST* pList = m_pHudList;

	while (pList)
	{
		if (pList->p)
			pList->p->InitHUDData();
		pList = pList->pNext;
	}


	//TODO: needs to be called on every map change, not just when starting a new game
	if (g_pParticleMan)
		g_pParticleMan->ResetParticles();

#ifdef HL_WEAPONS
	//Probably not a good place to put this.
	pBeam = pBeam2 = NULL;
	pFlare = NULL; // Vit_amiN: clear egon's beam flare
#endif
}


bool CHud::MsgFunc_GameMode(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	//Note: this user message could be updated to include multiple gamemodes, so make sure this checks for game mode 1
	//See CHalfLifeTeamplay::UpdateGameMode
	//TODO: define game mode constants
	m_Teamplay = READ_BYTE() == 1;

	return true;
}


bool CHud::MsgFunc_Damage(const char* pszName, int iSize, void* pbuf)
{
	int armor, blood;
	Vector from;
	int i;
	float count;

	BEGIN_READ(pbuf, iSize);
	armor = READ_BYTE();
	blood = READ_BYTE();

	for (i = 0; i < 3; i++)
		from[i] = READ_COORD();

	count = (blood * 0.5) + (armor * 0.5);

	if (count < 10)
		count = 10;

	// TODO: kick viewangles,  show damage visually

	return true;
}

bool CHud::MsgFunc_Concuss(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_iConcussionEffect = READ_BYTE();
	if (0 != m_iConcussionEffect)
	{
		int r, g, b;
		UnpackRGB(r, g, b, RGB_YELLOWISH);
		this->m_StatusIcons.EnableIcon("dmg_concuss", r, g, b);
	}
	else
		this->m_StatusIcons.DisableIcon("dmg_concuss");
	return true;
}

bool CHud::MsgFunc_RainData( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
		Rain.dripsPerSecond =	READ_SHORT();
		Rain.distFromPlayer =	READ_COORD();
		Rain.windX =			READ_COORD();
		Rain.windY =			READ_COORD();
		Rain.randX =			READ_COORD();
		Rain.randY =			READ_COORD();
		Rain.weatherMode =		READ_SHORT();
		Rain.globalHeight =		READ_COORD();
	return true;
}

bool CHud::MsgFunc_Weapons(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	const std::uint64_t lowerBits = READ_LONG();
	const std::uint64_t upperBits = READ_LONG();

	m_iWeaponBits = (lowerBits & 0XFFFFFFFF) | ((upperBits & 0XFFFFFFFF) << 32ULL);

	return true;
}

bool CHud::MsgFunc_ScritSeq(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	char text_file[64];
	char text_message[64];

	sprintf(text_file, "%s", READ_STRING());
	sprintf(text_message, "%s", READ_STRING());

	ScriptedSequence::SetCommandLine( text_file, text_message );
	return true;
}

bool CHud::MsgFunc_DynCrosshair(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	switch ( READ_BYTE() )
	{
		case 1: UpdateDynamicCrosshair( 5, 5, 0.2 ); break;
		case 2: OnCrosshairHit(); break;
	}

	return true;
}
