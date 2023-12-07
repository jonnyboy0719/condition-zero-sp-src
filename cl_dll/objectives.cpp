/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
// Health.cpp
//
// implementation of CHudHealth class
//

#include "stdio.h"
#include "stdlib.h"

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <list>
#include "Localize.h"

struct ActiveObjective_t
{
	bool _notransit;
	int _state;
	std::string _text;
};

std::list<ActiveObjective_t> m_objlist;

#define OBJ_SHOW_OBJ_UPDATE 20

DECLARE_MESSAGE(m_Objectives, OBJ)

bool CHudObjectives::Init()
{
	HOOK_MESSAGE(OBJ);
	m_iFlags = 0;
	m_fFade = 0;
	m_bDoFade = false;
	gHUD.AddHudElem(this);
	return true;
}

bool CHudObjectives::VidInit()
{
	m_objlist.clear();
	m_iFlags = 0;
	m_fFade = 0;
	m_bDoFade = false;
	return true;
}

static bool UpdateObjective(const char *szText, int iState, bool bNoTransit)
{
	if ( m_objlist.size() > 0 )
	{
		std::list< ActiveObjective_t >::iterator objectiveitem = m_objlist.begin();
		while ( objectiveitem != m_objlist.end() )
		{
			if ((strcmp(objectiveitem->_text.c_str(), szText) == 0))
			{
				objectiveitem->_notransit = bNoTransit;
				objectiveitem->_state = iState;
				return true;
			}
			// Increase
			++objectiveitem;
		}
	}
	return false;
}

bool CHudObjectives::MsgFunc_OBJ(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int state = READ_BYTE();	   // state
	bool notransit = (READ_BYTE() > 1) ? true : false; // no transit
	auto msg = READ_STRING(); // obj text

	if (UpdateObjective(msg, state, notransit))
		return true;

	ActiveObjective_t obj;
	obj._notransit = notransit;
	obj._state = state;
	obj._text = msg;
	m_objlist.push_back(obj);

	return true;
}

void CHudObjectives::Show()
{
	m_iFlags |= HUD_ACTIVE;
	m_fFade = 0;
	m_bDoFade = false;
}

void CHudObjectives::Hide()
{
	m_iFlags |= HUD_ACTIVE;
	m_fFade = OBJ_SHOW_OBJ_UPDATE;
	m_bDoFade = true;
}

static void CheckValidObjectives()
{
	if ( m_objlist.size() > 0 )
	{
		std::list< ActiveObjective_t >::iterator objectiveitem = m_objlist.begin();
		while ( objectiveitem != m_objlist.end() )
		{
			if ( objectiveitem->_state == 0 )
			{
				// Erase it
				m_objlist.erase( objectiveitem++ );
			}
			else
			{
				// Increase
				++objectiveitem;
			}
		}
	}
}

bool CHudObjectives::Draw(float flTime)
{
	int x, y;

	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) != 0 || 0 != gEngfuncs.IsSpectateOnly())
		return true;

	CheckValidObjectives();

	// Stop here if we have no obj text to draw
	if ( m_objlist.size() <= 0 )
	{
		m_iFlags = 0;
		m_fFade = 0;
		m_bDoFade = false;
		return true;
	}

	color24 rgb;
	rgb.r = 0;
	rgb.g = 128;
	rgb.b = 255;
	color24 rgb_org = rgb;
	ColorBlend( rgb, rgb_org );

	// Draw our mission text
	x = gHUD.GetSpriteRect(gHUD.m_Health.m_HUD_cross).right - gHUD.GetSpriteRect(gHUD.m_Health.m_HUD_cross).left;
	int fontx = x + 15;
	int fonth = gHUD.m_iFontHeight;
	y = fonth * 15;

	std::string szBuff( Localize::GetLocalizedTextv2( "#OBJECTIVES" ) );
	gHUD.DrawHudString( x, y, 320, szBuff.c_str(), rgb.r, rgb.g, rgb.b );

	y += fonth + 3;
	for each (auto item in m_objlist)
	{
		szBuff = Localize::GetLocalizedTextv2( item._text.c_str() );
		Localize::UTIL_STDReplaceString( szBuff, "%OBJ%", item._state == 2 ? "X" : "  " );

		switch ( item._state )
		{
			case 1:
			{
				rgb.r = rgb.g = rgb.b = 255;
			}
			break;
			case 2:
			{
				rgb.g = 255;
				rgb.b = 0;
			}
		}

		rgb_org = rgb;
		ColorBlend( rgb, rgb_org );
		gHUD.DrawHudString( fontx, y, 320, szBuff.c_str(), rgb.r, rgb.g, rgb.b );
		y += fonth + 10;
	}

	return true;
}

void CHudObjectives::ColorBlend( color24& rgb, color24 rgb_org )
{
	if (0 != m_fFade && m_bDoFade)
	{
		m_fFade -= (gHUD.m_flTimeDelta * 20);
		if (m_fFade <= 0)
		{
			m_fFade = 0;
			m_bDoFade = false;
			if ((m_iFlags & HUD_ACTIVE))
				m_iFlags = 0;
		}
		rgb.r = (m_fFade / OBJ_SHOW_OBJ_UPDATE) * rgb_org.r;
		rgb.g = (m_fFade / OBJ_SHOW_OBJ_UPDATE) * rgb_org.g;
		rgb.b = (m_fFade / OBJ_SHOW_OBJ_UPDATE) * rgb_org.b;
	}
}
