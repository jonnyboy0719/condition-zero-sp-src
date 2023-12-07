//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

// Triangle rendering, if any

#include "hud.h"
#include "cl_util.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "Exports.h"

#include "fog.h"
#include "rain.h"

#include "particleman.h"
#include "tri.h"

#include "eiface.h"
#include "enginecallback.h"

extern IParticleMan* g_pParticleMan;

//-----------------------------------------------------

void SetPoint( float x, float y, float z, float (*matrix)[4])
{
	Vector point, result;
	point[0] = x;
	point[1] = y;
	point[2] = z;

	VectorTransform(point, matrix, result);

	gEngfuncs.pTriAPI->Vertex3f(result[0], result[1], result[2]);
}

/*
=================================
DrawRain

draw raindrips and snowflakes
=================================
*/
extern cl_drip FirstChainDrip;
extern rain_properties Rain;

void DrawRain( void )
{
	if (FirstChainDrip.p_Next == NULL)
		return; // no drips to draw

	if (gHUD.ProcessWeather->value == 0.0)
		return; // no drips to draw

	V_HSPRITE hsprTexture;
	const model_s *pTexture;
	float visibleHeight = Rain.globalHeight - SNOWFADEDIST;

	// Load our sprite texture
	switch ( Rain.weatherMode )
	{
		case WEATHER_RAIN:
		default:
			hsprTexture = LoadSprite( "sprites/effects/rain.spr" );
		break;

		case WEATHER_SNOW:
			hsprTexture = LoadSprite( "sprites/effects/snowflake.spr" );
		break;

		case WEATHER_DUST:
			hsprTexture = LoadSprite( "sprites/effects/fx_dust.spr" );
		break;
	}

	// usual triapi stuff
	pTexture = gEngfuncs.GetSpritePointer( hsprTexture );
	gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)pTexture, 0 );
	gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );

	// go through drips list
	cl_drip* Drip = FirstChainDrip.p_Next;
	cl_entity_t *player = gEngfuncs.GetLocalPlayer();

	switch ( Rain.weatherMode )
	{
		case WEATHER_RAIN:
		default:
		{
			while (Drip != NULL)
			{
				cl_drip* nextdDrip = Drip->p_Next;
					
				Vector2D toPlayer; 
				toPlayer.x = player->origin[0] - Drip->origin[0];
				toPlayer.y = player->origin[1] - Drip->origin[1];
				toPlayer = toPlayer.Normalize();
	
				toPlayer.x *= DRIP_SPRITE_HALFWIDTH;
				toPlayer.y *= DRIP_SPRITE_HALFWIDTH;

				float shiftX = (Drip->xDelta / DRIPSPEED) * DRIP_SPRITE_HALFHEIGHT;
				float shiftY = (Drip->yDelta / DRIPSPEED) * DRIP_SPRITE_HALFHEIGHT;

			// --- draw triangle --------------------------
				gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, Drip->alpha );
				gEngfuncs.pTriAPI->Begin( TRI_TRIANGLES );

					gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
					gEngfuncs.pTriAPI->Vertex3f( Drip->origin[0]-toPlayer.y - shiftX, Drip->origin[1]+toPlayer.x - shiftY,Drip->origin[2] + DRIP_SPRITE_HALFHEIGHT );

					gEngfuncs.pTriAPI->TexCoord2f( 0.5, 1 );
					gEngfuncs.pTriAPI->Vertex3f( Drip->origin[0] + shiftX, Drip->origin[1] + shiftY, Drip->origin[2]-DRIP_SPRITE_HALFHEIGHT );

					gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
					gEngfuncs.pTriAPI->Vertex3f( Drip->origin[0]+toPlayer.y - shiftX, Drip->origin[1]-toPlayer.x - shiftY, Drip->origin[2]+DRIP_SPRITE_HALFHEIGHT);

				gEngfuncs.pTriAPI->End();
			// --- draw triangle end ----------------------

				Drip = nextdDrip;
			}
		}
		break;

		case WEATHER_SNOW:
		{
			Vector normal;
			gEngfuncs.GetViewAngles((float*)normal);

			float matrix[3][4];
			AngleMatrix (normal, matrix);	// calc view matrix

			while (Drip != NULL)
			{
				cl_drip* nextdDrip = Drip->p_Next;

				matrix[0][3] = Drip->origin[0]; // write origin to matrix
				matrix[1][3] = Drip->origin[1];
				matrix[2][3] = Drip->origin[2];

				// apply start fading effect
				float alpha = (Drip->origin[2] <= visibleHeight) ? Drip->alpha : ((Rain.globalHeight - Drip->origin[2]) / (float)SNOWFADEDIST) * Drip->alpha;

			// --- draw quad --------------------------
				gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, alpha );
				gEngfuncs.pTriAPI->Begin( TRI_QUADS );

					gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
					SetPoint(0, SNOW_SPRITE_HALFSIZE ,SNOW_SPRITE_HALFSIZE, matrix);

					gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
					SetPoint(0, SNOW_SPRITE_HALFSIZE ,-SNOW_SPRITE_HALFSIZE, matrix);

					gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
					SetPoint(0, -SNOW_SPRITE_HALFSIZE ,-SNOW_SPRITE_HALFSIZE, matrix);

					gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
					SetPoint(0, -SNOW_SPRITE_HALFSIZE ,SNOW_SPRITE_HALFSIZE, matrix);

				gEngfuncs.pTriAPI->End();
			// --- draw quad end ----------------------

				Drip = nextdDrip;
			}
		}
		break;

		case WEATHER_DUST:
		{
			Vector normal;
			gEngfuncs.GetViewAngles((float*)normal);

			float matrix[3][4];
			AngleMatrix (normal, matrix);	// calc view matrix

			while (Drip != NULL)
			{
				cl_drip* nextdDrip = Drip->p_Next;

				matrix[0][3] = Drip->origin[0]; // write origin to matrix
				matrix[1][3] = Drip->origin[1];
				matrix[2][3] = Drip->origin[2];

				// apply start fading effect
				float alpha = (Drip->origin[2] <= visibleHeight) ? Drip->alpha : ((Rain.globalHeight - Drip->origin[2]) / (float)SNOWFADEDIST) * Drip->alpha;

			// --- draw quad --------------------------
				gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, alpha );
				gEngfuncs.pTriAPI->Begin( TRI_QUADS );

					gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
					SetPoint(0, DUST_SPRITE_HALFSIZE ,DUST_SPRITE_HALFSIZE, matrix);

					gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
					SetPoint(0, DUST_SPRITE_HALFSIZE ,-DUST_SPRITE_HALFSIZE, matrix);

					gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
					SetPoint(0, -DUST_SPRITE_HALFSIZE ,-DUST_SPRITE_HALFSIZE, matrix);

					gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
					SetPoint(0, -DUST_SPRITE_HALFSIZE ,DUST_SPRITE_HALFSIZE, matrix);

				gEngfuncs.pTriAPI->End();
			// --- draw quad end ----------------------

				Drip = nextdDrip;
			}
		}
		break;
	}
}

/*
=================================
DrawFXObjects
=================================
*/
extern cl_rainfx FirstChainFX;

void DrawFXObjects( void )
{
	if (FirstChainFX.p_Next == NULL)
		return; // no objects to draw

	float curtime = gEngfuncs.GetClientTime();

	// go through objects list
	cl_rainfx* curFX = FirstChainFX.p_Next;
	while (curFX != NULL)
	{
		cl_rainfx* nextFX = curFX->p_Next;

		// usual triapi stuff
		V_HSPRITE hsprTexture;
		const model_s *pTexture;
		if ( curFX->inWater )
			hsprTexture = LoadSprite( RANDOM_LONG(0, 1) == 1 ? "sprites/effects/rain_splash_01.spr" : "sprites/effects/rain_splash_02.spr" );
		else
			hsprTexture = LoadSprite( "sprites/effects/rainsplash.spr" );
		pTexture = gEngfuncs.GetSpritePointer( hsprTexture );
		gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)pTexture, 0 );
		gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd );
		gEngfuncs.pTriAPI->CullFace( TRI_NONE );

		// fadeout
		float alpha = ((curFX->birthTime + curFX->life - curtime) / curFX->life) * curFX->alpha;
		float size = (curtime - curFX->birthTime) * MAXRINGHALFSIZE;

		// --- draw quad --------------------------
		gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, alpha );
		gEngfuncs.pTriAPI->Begin( TRI_QUADS );

			gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin[0] - size, curFX->origin[1] - size, curFX->origin[2]);

			gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin[0] - size, curFX->origin[1] + size, curFX->origin[2]);

			gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin[0] + size, curFX->origin[1] + size, curFX->origin[2]);

			gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin[0] + size, curFX->origin[1] - size, curFX->origin[2]);

		gEngfuncs.pTriAPI->End();
		// --- draw quad end ----------------------

		curFX = nextFX;
	}
}

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void DLLEXPORT HUD_DrawNormalTriangles()
{
	//	RecClDrawNormalTriangles();

	gHUD.m_Spectator.DrawOverview();
	gFog.HUD_DrawNormalTriangles();
}


/*
=================
HUD_DrawTransparentTriangles

Render any triangles with transparent rendermode needs here
=================
*/
void DLLEXPORT HUD_DrawTransparentTriangles()
{
	//	RecClDrawTransparentTriangles();
	gFog.HUD_DrawTransparentTriangles();

	if (g_pParticleMan)
		g_pParticleMan->Update();

	ProcessFXObjects();
	ProcessRain();

	DrawRain();
	DrawFXObjects();
}
