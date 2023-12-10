//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#pragma once

#include "VGUI_BitmapTGA.h"

vgui::BitmapTGA* vgui_LoadTGA(char const *pFilename);
vgui::BitmapTGA* vgui_LoadTGANoInvertAlpha(char const *pFilename);


class BitmapTGAWorldMap : public vgui::BitmapTGA
{
public:
	BitmapTGAWorldMap( vgui::InputStream* is, bool invertAlpha ) : vgui::BitmapTGA( is, invertAlpha ) {}
	void SetMapSzie( int wide, int tall )
	{
		setSize( wide, tall );
	}
};
BitmapTGAWorldMap* vgui_LoadWorldMapTGA(char const* pFilename);
