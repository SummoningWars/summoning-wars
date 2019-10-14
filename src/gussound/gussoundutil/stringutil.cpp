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
//	 Last change:	$LastChangedDate: 2013-06-20 22:36:33 +0200 (J, 20 iun. 2013) $
//	 Revision:		$Revision: 573 $

#include <gussoundutil/stringutil.h>
#include <sstream>

namespace guslib
{
	namespace stringutil
	{
		std::wstring StringToWString (const std::string& s)
		{
			std::wstring temp (s.length (),L' ');
			std::copy (s.begin (), s.end (), temp.begin ());
			return temp;
		}

		std::string WStringToString(const std::wstring& s)
		{
			if (s.length () == 0)
			{
				return std::string ();
			}
			std::string temp (s.length (), L' ');
			std::copy (s.begin (), s.end (), temp.begin ());
			return temp;
		}

		bool StringToBool (const std::string& s)
		{
			if (s.length () <= 0)
			{
				return false;
			}

			if (s[0] == 'T' || s[0] == 't' || s[0] == 'y' || s[0] == 'Y' || s[0] == '1')
			{
				return true;
			}

			return false;
		}


		std::string BoolToString (bool value, LetterCase format)
		{
			if (value)
			{
				switch (format)
				{
				case LowerCase:
					return "true";
				case UpperCase:
					return "TRUE";
				case SentenceCase:
				default:
					return "True";
				}
			}

			switch (format)
			{
			case LowerCase:
				return "false";
			case UpperCase:
				return "FALSE";
			} // fall through
			return "False";
		}


		int StringToInt (const std::string & s)
		{
			int ret;
			std::istringstream (s) >> ret;
			return ret;
		}

		std::string IntToString (int value)
		{
			std::stringstream ss;
			ss << value;
			return ss.str ();
		}

		long long StringToLongLong (const std::string & s)
		{
			long long ret;
			std::istringstream (s) >> ret;
			return ret;
		}

		std::string LongLongToString (long long value)
		{
			std::stringstream ss;
			ss << value;
			return ss.str ();
		}

		unsigned long long StringToUnsignedLongLong (const std::string & s)
		{
			unsigned long long ret;
			std::istringstream (s) >> ret;
			return ret;
		}

		std::string UnsignedLongLongToString (unsigned long long value)
		{
			std::stringstream ss;
			ss << value;
			return ss.str ();
		}

		double StringToDouble (const std::string & s)
		{
			double ret;
			std::istringstream (s) >> ret;
			return ret;
		}

		std::string DoubleToString (double value)
		{
			std::stringstream ss;
			ss << value;
			return ss.str ();
		}
		

		std::string getExtensionFromFileName (const std::string &fileName)
		{
			std::string result;
			unsigned foundPos = fileName.find_last_of (".");
			if (foundPos != std::string::npos)
			{
				// Name
				//result = result.substr (0, foundPos);
				// Extension
				result = fileName.substr (foundPos + 1);
			}

			return result;
		}

	}
}