/***	Copyright (c) 2023, Johan Ehrendahl. All rights reserved.	***/

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "pm_shared.h"

#include "ScriptedSequence.h"
#include "Localize.h"
#include <sstream>
#include <fstream>

static sequenceCommandLine_s* m_CurrentCommandLine = nullptr;
static float m_Delay = 0.0f;
static std::string m_sSpeaker = "";
static std::string m_sListener = "";

void ScriptedSequence::SetCommandLine( const char *file, const char *seq )
{
	// This may contain some memory leaks.
	//gEngfuncs.Con_Printf( "File: %s!\n", file );
	//gEngfuncs.Con_Printf( "Seq: %s!\n", seq );

#if 0
	char filepath[128];
	sprintf( filepath, "csz/sequences/%s", file );

	std::ifstream seqfile( filepath );
	std::string line;
	int y = 0;
	bool bFoundData = false;
	bool found_sequence = false;
	SequenceState _SequenceState = State_None;
	std::string sequence = "%" + std::string( seq );
	while ( std::getline( seqfile, line ) )
	{
		// Stop reading the file
		if ( bFoundData )
			break;

		std::string stdbuffer = line;
		ReplaceString( stdbuffer, "\r", "" );
		ReplaceString( stdbuffer, "\n", "" );

		if ( found_sequence )
		{
			switch ( _SequenceState )
			{
				case State_None:
				{
					if ( line[0] == '{' )
						_SequenceState = State_Read;
				}
				break;

				case State_Read:
				{
					if ( line[0] == '}' )
					{
						_SequenceState = State_Done;
						bFoundData = true;
					}
					else
					{
						gEngfuncs.Con_Printf( "\t%i >> \"%s\"\n", y, line.c_str() );
						y++;
					}
				}
				break;

				case State_Done:
				{
					bFoundData = true;
				}
				break;
			}
		}
		else
		{
			// Search for our sequence
			if ( FStrEq( sequence.c_str(), stdbuffer.c_str() ) )
				found_sequence = true;
		}
	}

	m_CurrentCommandLine = nullptr;
#else
	auto m_scriptedSequence = gEngfuncs.pfnSequenceGet( file, seq );
	if ( !m_scriptedSequence ) return;
	m_CurrentCommandLine = m_scriptedSequence->firstCommand;
#endif
	m_Delay = gEngfuncs.GetClientTime() + 0.1f;
	m_sSpeaker.clear();
	m_sListener.clear();
}

void ScriptedSequence::Clear()
{
	m_CurrentCommandLine = nullptr;
	m_Delay = -1;
	m_sSpeaker.clear();
	m_sListener.clear();
}

bool IsValidString(const std::string &input)
{
	if (input.empty()) return false;
	if (input == "none") return false;
	if ((strcmp(input.c_str(), "none") == 0)) return false;
	return true;
}

void ScriptedSequence::Think()
{
	if ( !m_CurrentCommandLine ) return;
	if ( m_Delay > gEngfuncs.GetClientTime() ) return;

	if (m_CurrentCommandLine->speakerName)
		m_sSpeaker = m_CurrentCommandLine->speakerName;

	if (m_CurrentCommandLine->listenerName)
		m_sListener = m_CurrentCommandLine->listenerName;

	if (m_CurrentCommandLine->soundFileName)
	{
		char string[64];
		int iszItem = 0;
		auto sentence = gEngfuncs.pfnSequencePickSentence( m_CurrentCommandLine->soundFileName, 0, &iszItem );
		if (IsValidString(m_sSpeaker))
		{
			if ( sentence )
				sprintf(string, "npcspk %s \"%s\" \"%s\"\n", m_sSpeaker.c_str(), sentence->data, m_sListener.c_str());
			else
				sprintf(string, "npcspk %s \"%s\" \"%s\"\n", m_sSpeaker.c_str(), m_CurrentCommandLine->soundFileName, m_sListener.c_str());
			//gEngfuncs.Con_Printf("Sequence Fired [SND::SPEAK::%s]: %s\n", m_sSpeaker.c_str(), string);
			m_sSpeaker.clear();
			m_sListener.clear();
		}
		else
		{
			if ( sentence )
				sprintf(string, "spk %s\n", sentence->data);
			else
				sprintf(string, "spk %s\n", m_CurrentCommandLine->soundFileName);
			//gEngfuncs.Con_Printf("Sequence Fired [SND]: %s\n", string);
		}
		gEngfuncs.pfnClientCmd(string);
	}

	if (m_CurrentCommandLine->fireTargetNames)
	{
		char string[64];
		sprintf(string, "ent_fire %s\n", m_CurrentCommandLine->fireTargetNames);
		//gEngfuncs.Con_Printf("Sequence Fired [TRGT]: %s\n", string);
		gEngfuncs.pfnServerCmd(string);
	}

	if (m_CurrentCommandLine->killTargetNames)
	{
		char string[64];
		sprintf(string, "ent_kill %s\n", m_CurrentCommandLine->killTargetNames);
		//gEngfuncs.Con_Printf("Sequence Fired [KILL]: %s\n", string);
		gEngfuncs.pfnServerCmd(string);
	}

	//gEngfuncs.Con_Printf("Sequence Fired [TEXT CHANNEL]: %i\n", m_CurrentCommandLine->textChannel);

	if ( m_CurrentCommandLine->clientMessage.pMessage )
	{
		client_textmessage_t *TempMessage = &m_CurrentCommandLine->clientMessage;
		// if -1, then become 10.0f
		if ( TempMessage->holdtime == -1 )
			TempMessage->holdtime = 10.0f;

#if 0
		gEngfuncs.Con_Printf("Sequence Fired [HUDMSG]: %s\n", TempMessage->pMessage);
		gEngfuncs.Con_Printf("----: HUD_MSG_DEBUG :----\n");
		gEngfuncs.Con_Printf("textChannel: %i\n", m_CurrentCommandLine->textChannel);
		gEngfuncs.Con_Printf("modifierBitField: %i\n", m_CurrentCommandLine->modifierBitField);
		gEngfuncs.Con_Printf("pMessage: %s\n", TempMessage->pMessage);
		gEngfuncs.Con_Printf("pName: %s\n", TempMessage->pName);
		gEngfuncs.Con_Printf("fxtime: %f\n", TempMessage->fxtime);
		gEngfuncs.Con_Printf("holdtime: %f\n", TempMessage->holdtime);
		gEngfuncs.Con_Printf("fadeout: %f\n", TempMessage->fadeout);
		gEngfuncs.Con_Printf("fadein: %f\n", TempMessage->fadein);
		gEngfuncs.Con_Printf("y: %f\n", TempMessage->y);
		gEngfuncs.Con_Printf("x: %f\n", TempMessage->x);
		gEngfuncs.Con_Printf("effect: %i\n", TempMessage->effect);
		gEngfuncs.Con_Printf("color: (%i, %i, %i)\n", TempMessage->r1, TempMessage->g1, TempMessage->b1);
		gEngfuncs.Con_Printf("color2: (%i, %i, %i)\n", TempMessage->r2, TempMessage->g2, TempMessage->b2);
#endif

		// Translate text if we find any.
		//if ( TempMessage->pMessage[0] == '#' )
		//	TempMessage->pMessage = Localize::GetLocalizedText( msg.pMessage );

		gHUD.m_Message.MessageSetChannel( m_CurrentCommandLine->textChannel, m_CurrentCommandLine->modifierBitField, TempMessage );
	}

	m_Delay = gEngfuncs.GetClientTime() + m_CurrentCommandLine->delay;
	m_CurrentCommandLine = m_CurrentCommandLine->nextCommandLine;
}

void ScriptedSequence::ReplaceString( std::string& path, std::string search, std::string replace )
{
	size_t pos = 0;
	while ((pos = path.find(search, pos)) != std::string::npos)
	{
		path.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

bool ScriptedSequence::FStrEq( const char* sz1, const char* sz2 )
{
	return ( sz1 == sz2 || stricmp(sz1, sz2) == 0 );
}
