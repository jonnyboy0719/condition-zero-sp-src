
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "triggers.h"
#include "filesystem_utils.h"
#include "TriggerScript.h"
#include <sstream>
#include <iostream>
#include <list>

class CTriggerScript : public CTriggerOnce
{
public:
	bool KeyValue(KeyValueData* pkvd) override;
	void Spawn() override;

	int ObjectCaps() override { return CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	static TYPEDESCRIPTION m_SaveData[];

	void EXPORT ScriptTouch(CBaseEntity* pOther);
};
LINK_ENTITY_TO_CLASS(trigger_script, CTriggerScript);

bool CTriggerScript::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "function"))
	{
		pev->message = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	return CTriggerOnce::KeyValue(pkvd);
}

void CTriggerScript::Spawn()
{
	CTriggerOnce::Spawn();
	SetTouch(&CTriggerScript::ScriptTouch);
}

void CTriggerScript::ScriptTouch(CBaseEntity* pOther)
{
	// TODO: Call angelscript for our stuff.
	if (!FStringNull(pev->message))
	{
		// STRING(pev->message)
	}

	// Delete this trigger on touch
	SetTouch(NULL);
	pev->nextthink = gpGlobals->time + 0.1;
	SetThink(&CBaseTrigger::SUB_Remove);
}

extern int gmsgScriptedSequence;

// CZero support
class CTriggerSequence : public CPointEntity
{
public:
	void Spawn( void );
	bool KeyValue( KeyValueData* pkvd ) override;
    void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	string_t m_file;
};
LINK_ENTITY_TO_CLASS(trigger_sequence, CTriggerSequence);

bool CTriggerSequence::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "sequence_id"))
	{
		pev->message = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "sequence_file"))
	{
		m_file = ALLOC_STRING(pkvd->szValue);
		return true;
	}
	return CPointEntity::KeyValue(pkvd);
}

void CTriggerSequence::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	SetUse(NULL);

	char text_file[64];
	char text_message[64];
	sprintf(text_file, "%s", STRING(m_file));
	sprintf(text_message, "%s", STRING(pev->message));

	MESSAGE_BEGIN(MSG_ALL, gmsgScriptedSequence);
	WRITE_STRING(text_file);
	WRITE_STRING(text_message);
	MESSAGE_END();

	pev->nextthink = gpGlobals->time + 0.1;
	SetThink(&CPointEntity::SUB_Remove);
}

void CTriggerSequence::Spawn()
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
}

void UTIL_STDReplaceString(std::string& path, std::string search, std::string replace)
{
	size_t pos = 0;
	while ((pos = path.find(search, pos)) != std::string::npos) {
		path.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

bool UTIL_STDFindString(std::string input, const std::string& search)
{
	std::size_t found = input.rfind( search );
	if ( found != std::string::npos )
		return true;
	return false;
}

enum FoundSentences_t
{
	STATE_NO = 0,
	STATE_YES,
	STATE_BEGIN,
	STATE_END
};

void SequenceScripting::PrecacheSentences()
{
	// make sure the directories have been made
	g_pFileSystem->CreateDirHierarchy("sequences", "GAMECONFIG");

	// Load the file, and read !sentences, precache em
	const std::string fileName{std::string{"sequences/"} + (char*)STRING(gpGlobals->mapname) + ".seq"};

	const auto buffer = FileSystem_LoadFileIntoBuffer(fileName.c_str(), FileContentFormat::Text, "GAMECONFIG");
	if (buffer.empty()) return;

	std::string s( reinterpret_cast< char const* >(buffer.data()) );

	// Create the string views with the lines
	char* start{ s.data() };
	char* end{ start };
	FoundSentences_t foundSentences = FoundSentences_t::STATE_NO;

	std::list<string_t> s_list;
	for (const char c : s )
	{
	    ++end;
	    if (c == '\n')
		{
			std::string tmp = { start, end };
			UTIL_STDReplaceString(tmp, "\n", "");
			UTIL_STDReplaceString(tmp, "\r", "");
			switch (foundSentences)
			{
				case STATE_NO:
				{
					if (UTIL_STDFindString(tmp.c_str(), "!sentences"))
						foundSentences = FoundSentences_t::STATE_YES;
				}
				break;
				case STATE_YES:
				{
					if (UTIL_STDFindString(tmp.c_str(), "{"))
						foundSentences = FoundSentences_t::STATE_BEGIN;
				}
				break;
				case STATE_BEGIN:
				{
					if (UTIL_STDFindString(tmp.c_str(), "}"))
						foundSentences = FoundSentences_t::STATE_END;
					else
					{
						// Is Tabbed
						if (UTIL_STDFindString(tmp, "\t"))
						{
							std::istringstream find(tmp);
							std::string str;
							int iLine = 0;
							while (std::getline(find, str, '\t'))
							{
								if (str.empty()) continue;
								if (iLine > 1) break;
								if (iLine == 1)
									s_list.push_back(ALLOC_STRING(str.c_str()));
								iLine++;
							}
						}
						else
						{
							std::istringstream find(tmp);
							std::string str;
							int iLine = 0;
							while (std::getline(find, str, ' '))
							{
								if (str.empty()) continue;
								if (iLine > 1) break;
								if (iLine == 1)
									s_list.push_back(ALLOC_STRING(str.c_str()));
								iLine++;
							}
						}
					}
				}
				break;
			}
	        start = end;
	    }
	}
	s.clear();

	for each (auto var in s_list)
	{
		char* szSoundFile = (char*)STRING(var);
		if (!FStringNull(var) && strlen(szSoundFile) > 0)
		{
			ALERT(at_console, "Precached Sequence Sound \"%s\"!\n", szSoundFile);
			PRECACHE_SOUND(szSoundFile);
		}
	}
}
