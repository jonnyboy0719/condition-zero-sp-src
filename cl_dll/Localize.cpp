/***	Copyright (c) 2023, Johan Ehrendahl. All rights reserved.	***/

#include "hud.h"
#include "cl_util.h"
#include "pm_shared.h"
#include "Localize.h"
#include <list>

// Should be in the header file, but since we only use it on the CPP file, it's kinda pointless.
enum TranslationLoadedState
{
	k_eNotLoaded = 0,
	k_eLoaded,
	k_eLoadFailed
};

static TranslationLoadedState s_LoadedTranslationFile = TranslationLoadedState::k_eNotLoaded;
static std::string s_Language = "english";

// GoldSource does not have KeyValues support, so let's simply use std::list instead since its similar to it.
struct TranslationData
{
	std::string m_Key;
	std::string m_Data;
};
static std::list<TranslationData> s_LangData;

bool Localize::IsDebug(int level)
{
	if ( !cl_lang_debug ) return false;
	if (level > 0)
		return cl_lang_debug->value >= level ? true : false;
	return cl_lang_debug->value > 0 ? true : false;
}

void Localize::Reload()
{
	s_LoadedTranslationFile = TranslationLoadedState::k_eNotLoaded;
	s_LangData.clear();
	Localize::SetLanguage( cl_lang->string );
	ReadManifestFile();
}

void Localize::SetLanguage( const char* szLang )
{
	if ( !szLang ) return;
	if (IsDebug())
	{
		gEngfuncs.Con_Printf("----: SetLanguage :----\n");
		gEngfuncs.Con_Printf("previous: %s\n", s_Language.c_str());
		gEngfuncs.Con_Printf("new: %s\n", szLang);
	}
	s_Language = szLang;
}

void Localize::ReadManifestFile()
{
	if ( s_LoadedTranslationFile == TranslationLoadedState::k_eLoaded ) return;
	if ( IsDebug() )
		gEngfuncs.Con_Printf("----: ReadManifestFile :----\n");

	char filepath[128];
	sprintf( filepath, "translations/%s.txt", s_Language.c_str() );

	if ( IsDebug(2) )
		gEngfuncs.Con_Printf("Loading translation file \"%s\"\n", filepath);

	char* pfile = (char*)gEngfuncs.COM_LoadFile( filepath, 5, NULL);
	if ( !pfile )
	{
		gEngfuncs.Con_Printf( "Unable to load %s!\n", filepath );
		s_LoadedTranslationFile = TranslationLoadedState::k_eLoadFailed;
		return;
	}

	int y = 0;
	char token[1024];
	pfile = gEngfuncs.COM_ParseFile( pfile, token );
	while ( pfile )
	{
		char cKey[32] = "";
		char cText[128] = "";

		strncpy( cKey, token, 32 );
		cKey[31] = '\0';
		pfile = gEngfuncs.COM_ParseFile( pfile, token );

		strncpy( cText, token, 128 );
		cText[127] = '\0';

		if ( IsDebug() )
		{
			gEngfuncs.Con_Printf("\t%i >> [%s] = \"%s\"\n", y, cKey, cText);
			y++;
		}

		// Add to our std::list
		TranslationData data;
		data.m_Key = cKey;
		data.m_Data = cText;
		s_LangData.push_back( data );

		pfile = gEngfuncs.COM_ParseFile( pfile, token );
	}

	gEngfuncs.COM_FreeFile( pfile );

	s_LoadedTranslationFile = TranslationLoadedState::k_eLoaded;
}

void Localize::UTIL_STDReplaceString(std::string& path, std::string search, std::string replace)
{
	size_t pos = 0;
	while ((pos = path.find(search, pos)) != std::string::npos) {
		path.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

std::string Localize::GetLocalizedTextv2( const char* szText )
{
	ReadManifestFile();
	if ( IsDebug() )
		gEngfuncs.Con_Printf("----: GetLocalizedText[%s] :----\n", szText);
	for ( auto iter = s_LangData.begin(); iter != s_LangData.end(); ++iter )
	{
		auto find = std::string( szText );
		UTIL_STDReplaceString( find, "#", "" );
		auto search = iter->m_Key;
		if (IsDebug(2))
		{
			gEngfuncs.Con_Printf("\tfind: [%s]\n", find.c_str());
			gEngfuncs.Con_Printf("\tsearch: [%s]\n", search.c_str());
		}
		if ( (strcmp(find.c_str(), search.c_str()) == 0) )
			return iter->m_Data;
	}
	return szText;
}

const char *Localize::GetLocalizedText( const char* szText )
{
	ReadManifestFile();
	if ( IsDebug() )
		gEngfuncs.Con_Printf("----: GetLocalizedText[%s] :----\n", szText);

	for ( auto iter = s_LangData.begin(); iter != s_LangData.end(); ++iter )
	{
		auto find = std::string( szText );
		UTIL_STDReplaceString( find, "#", "" );
		UTIL_STDReplaceString( find, "\r", "" );
		UTIL_STDReplaceString( find, "\n", "" );
		auto search = iter->m_Key;
		if (IsDebug(2))
		{
			gEngfuncs.Con_Printf("\tfind: [%s]\n", find.c_str());
			gEngfuncs.Con_Printf("\tsearch: [%s]\n", search.c_str());
			gEngfuncs.Con_Printf("\tEquals?: [%s]\n", StringEquals(find.c_str(), search.c_str()) ? "Y" : "N");
		}
		if ( (strcmp(find.c_str(), search.c_str()) == 0) )
			return iter->m_Data.c_str();
		//if ( StringEquals( find.c_str(), search.c_str() ) )
		//	return iter->m_Data.c_str();
	}

	return szText;
}
