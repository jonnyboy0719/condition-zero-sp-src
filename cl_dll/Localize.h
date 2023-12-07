/***	Copyright (c) 2023, Johan Ehrendahl. All rights reserved.	***/

#ifndef CLIENT_LOCALIZE_H
#define CLIENT_LOCALIZE_H

#include <string>

namespace Localize
{
	bool IsDebug(int level = 0);
	void Reload();
	void SetLanguage( const char *szLang );
	void ReadManifestFile();
	std::string GetLocalizedTextv2( const char* szText );
	const char *GetLocalizedText( const char* szText );
	void UTIL_STDReplaceString( std::string &path, std::string search, std::string replace );
	inline bool StringEquals(const char* sz1, const char* sz2)
	{
		return (strcmp(sz1, sz2) == 0);
	}
}

extern cvar_t* cl_lang;
extern cvar_t* cl_lang_debug;

#endif