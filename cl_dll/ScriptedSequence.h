/***	Copyright (c) 2023, Johan Ehrendahl. All rights reserved.	***/

#ifndef CLIENT_SCRIPTED_SEQUENCE_H
#define CLIENT_SCRIPTED_SEQUENCE_H

#include "Sequence.h"
#include <string>

namespace ScriptedSequence
{
	enum SequenceState
	{
		State_None,
		State_Read,
		State_Done
	};

	void SetCommandLine( const char *file, const char *seq );
	void Clear();
	void Think();

	void ReplaceString( std::string &path, std::string search, std::string replace );
	bool FStrEq(const char *sz1, const char *sz2);
}

#endif