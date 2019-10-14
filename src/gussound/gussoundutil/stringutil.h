#ifndef GUS_LIB_STRINGUTIL_H
#define GUS_LIB_STRINGUTIL_H

//   This file is part of the guslib library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2011  Augustin Preda (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//   
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//   
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//
//   String utility
//
//	 Last change:	$LastChangedDate: 2013-06-02 23:28:45 +0300 (D, 02 iun. 2013) $
//	 Revision:		$Revision: 563 $

#include <string>

namespace guslib
{
	typedef enum
	{
		SentenceCase,	// Sentencecase
		LowerCase,		// lowercase
		UpperCase,		// ALL_CAPS
		CamelCase,		// camelCase
		UpperCamelCase,	// CamelCase
		NumLetterCases
	}LetterCase;



	namespace stringutil
	{
		
		// Prototype for conversion functions
		std::wstring StringToWString (const std::string& s);
		std::string WStringToString (const std::wstring& s);

		bool StringToBool (const std::string& s);
		std::string BoolToString (bool value, LetterCase format = SentenceCase);

		int StringToInt (const std::string & s);
		std::string IntToString (int value);

		long long StringToLongLong (const std::string & s);
		std::string LongLongToString (long long value);

		unsigned long long StringToUnsignedLongLong (const std::string & s);
		std::string UnsignedLongLongToString (unsigned long long value);

		double StringToDouble (const std::string & s);
		std::string DoubleToString (double value);

		std::string getExtensionFromFileName (const std::string &fileName);
	}
}

#endif // GUS_LIB_STRINGUTIL_H