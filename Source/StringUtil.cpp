#include "StringUtil.h"


/**************************************************************************\
*                                                                          *
*  Right trimming of a string.                                             *
*                                                                          *
\**************************************************************************/
string stringTrimRight(const string& s, const char* t /*= " \t\r\n"*/)
{
	string::size_type i = s.find_last_not_of(t);

	if (i == string::npos)
	{
		return string();
	}

	return s.substr(0, i+1); 
}
wstring stringTrimRight(const wstring& s, const wchar_t* t /*= L" \t\r\n"*/)
//------------------------------------------------------------------------------
{
	wstring::size_type i = s.find_last_not_of(t);

	if (i == wstring::npos)
	{
		return wstring();
	}

	return s.substr(0, i+1); 
}

/**************************************************************************\
*                                                                          *
*   Left trimming of a string.                                             *
*                                                                          *
\**************************************************************************/
string stringTrimLeft(const string& s, const char* t /*= " \t\r\n"*/) 
{
	string::size_type i = s.find_first_not_of(t);

	if (i == string::npos)
	{
		return string();
	}

	return s.substr(i);
}
wstring stringTrimLeft(const wstring& s, const wchar_t* t /*= L" \t\r\n"*/) 
//------------------------------------------------------------------------------
{
	wstring::size_type i = s.find_first_not_of(t);

	if (i == wstring::npos)
	{
		return wstring();
	}

	return s.substr(i);
}

/**************************************************************************\
*                                                                          *
*  Left and right trimming of a string.                                    *
*                                                                          *
\**************************************************************************/
string stringTrim(const string& s, const char* t /*= " \t\r\n"*/)
{
	string::size_type i = s.find_first_not_of(t);

	if (i == string::npos)
	{
		return string();
	}

	string::size_type j = s.find_last_not_of(t);
	return s.substr(i, j+1-i); 
}
wstring stringTrim(const wstring& s, const wchar_t* t /*= L" \t\r\n"*/)
//------------------------------------------------------------------------------
{
	wstring::size_type i = s.find_first_not_of(t);

	if (i == wstring::npos)
	{
		return wstring();
	}

	wstring::size_type j = s.find_last_not_of(t);
	return s.substr(i, j+1-i); 
}

/**************************************************************************\
*                                                                          *
* Gets the right part of the string after the last marker (not included).  *
*                                                                          *
\**************************************************************************/
string stringAfterLast(const string& s, const string& marker)
{
	string::size_type i = s.rfind(marker);

	if (i == string::npos)
	{
		return string();
	}

	return s.substr(i+marker.length());
}

/**************************************************************************\
*                                                                          *
*  Gets the right part of the string after the 1st marker (not included).  *
*                                                                          *
\**************************************************************************/
string stringAfterFirst(const string& s, const string& marker)
{
	string::size_type i = s.find(marker);

	if (i == string::npos)
	{
		return string();
	}

	return s.substr(i+marker.length());
}

/**************************************************************************\
*                                                                          *
*  Gets the left part of the string before the 1st marker (not included).  *
*                                                                          *
\**************************************************************************/
string stringBeforeFirst(const string& s, const string& marker)
{
	string::size_type i = s.find(marker);

	if (i == string::npos)
	{
		return string();
	}

	return s.substr(0,i);
}

/**************************************************************************\
*                                                                          *
*  Gets the left part of the string before the last marker (not included). *
*                                                                          *
\**************************************************************************/
string stringBeforeLast(const string& s, const string& marker)
{
	string::size_type i = s.rfind(marker);

	if (i == string::npos)
	{
		return string();
	}

	return s.substr(0,i);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
size_t stringFind(const string& s, char c, size_t offset/*=0*/)
{
	return s.find(c,offset);		// Returns string::npos if not found
}
size_t stringFind(const wstring& s, wchar_t c, size_t offset/*=0*/)
//------------------------------------------------------------------------------
{
	return s.find(c,offset);		// Returns string::npos if not found
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
size_t stringFind(const string& s, const string& token, size_t offset/*=0*/)
{
	return s.find(token,offset);	// Returns string::npos if not found
}
size_t stringFind(const wstring& s, const wstring& token, size_t offset/*=0*/)
//------------------------------------------------------------------------------
{
	return s.find(token,offset);	// Returns string::npos if not found
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
size_t stringFindOneOf(const string& s, const string& charsList, size_t offset/*=0*/)
{
	return s.find_first_of(charsList,offset);	// Returns string::npos if not found
}
size_t stringFindOneOf(const wstring& s, const wstring& charsList, size_t offset/*=0*/)
//------------------------------------------------------------------------------
{
	return s.find_first_of(charsList,offset);	// Returns string::npos if not found
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
size_t stringReverseFind(const string& s, char c, size_t offset/*=0*/)
{
	return s.rfind(c,offset);		// Returns string::npos if not found
}
size_t stringReverseFind(const wstring& s, wchar_t c, size_t offset/*=0*/)
//------------------------------------------------------------------------------
{
	return s.rfind(c,offset);		// Returns string::npos if not found
}

/**************************************************************************\
*                                                                          *
*  Replace char by another one.                                            *
*                                                                          *
\**************************************************************************/
void stringReplaceChar(string& str, char OldChar, char NewChar)
{
	std::replace(str.begin(), str.end(), OldChar, NewChar);
}
void stringReplaceChar(wstring& str, wchar_t OldChar, wchar_t NewChar)
//------------------------------------------------------------------------------
{
	std::replace(str.begin(), str.end(), OldChar, NewChar);
}
void stringReplaceChar(char* str, char OldChar, char NewChar)
//------------------------------------------------------------------------------
{
	int i = 0;

	while (str[i])
	{
		if (str[i] == OldChar)
		{
			str[i] = NewChar;
		}
		i++;
	}
}
void stringReplaceChar(wchar_t* str, wchar_t OldChar, wchar_t NewChar)
//------------------------------------------------------------------------------
{
	int i = 0;

	while (str[i])
	{
		if (str[i] == OldChar)
		{
			str[i] = NewChar;
		}
		i++;
	}
}

/**************************************************************************\
*                                                                          *
*  Remove a specific char.                                                 *
*                                                                          *
\**************************************************************************/
void stringRemoveChar(string& str, char c)
{
	string::iterator end_pos = std::remove(str.begin(), str.end(), c);
	str.erase(end_pos, str.end());
}
void stringRemoveChar(wstring& str, wchar_t c)
//------------------------------------------------------------------------------
{
	wstring::iterator end_pos = std::remove(str.begin(), str.end(), c);
	str.erase(end_pos, str.end());
}

/**************************************************************************\
*                                                                          *
*  Returns a lower case version of the string.                             *
*                                                                          *
\**************************************************************************/
string stringToLower(const string& s)
{
	string d(s);

	transform(d.begin(), d.end(), d.begin(),(int(*)(int)) tolower);
	return d;
}
wstring stringToLower(const wstring& s)
//------------------------------------------------------------------------------
{
	wstring d(s);

	transform(d.begin(), d.end(), d.begin(),(wint_t(*)(wint_t)) towlower);
	return d;
}

/**************************************************************************\
*                                                                          *
*  Returns an upper case version of the string.                            *
*                                                                          *
\**************************************************************************/
string stringToUpper(const string& s)
{
	string d(s);

	transform(d.begin(), d.end(), d.begin(),(int(*)(int)) toupper);
	return d;
}
wstring stringToUpper(const wstring& s)
//------------------------------------------------------------------------------
{
	wstring d(s);

	transform(d.begin(), d.end(), d.begin(),(wint_t(*)(wint_t)) towupper);
	return d;
}

/**************************************************************************\
*                                                                          *
*  Returns true if the string ends with the token.                         *
*                                                                          *
\**************************************************************************/
bool isStringEndsWith(const string& s, const string& token)
{
	size_t pos = s.rfind(token);
	if (pos != string::npos)
	{
		return (pos+token.length() == s.length());
	}
	return false;
}

/**************************************************************************\
*                                                                          *
*  Returns true if the string starts with the token.                       *
*                                                                          *
\**************************************************************************/
bool isStringStartsWith(const string& s, const string& token)
{
	return (strncmp(s.c_str(), token.c_str(), token.length()) == 0);
}

/**************************************************************************\
*                                                                          *
*  Returns true if the string contains the token.                          *
*                                                                          *
\**************************************************************************/
bool isStringContains(const string& s, const string& token)
{
	return (s.find(token) != string::npos);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
bool isStringInList(const string& s, const vector<string>& list, int& outIndex)
{
	return isStringInList(s.c_str(),list,outIndex);
}
bool isStringInList(const char* s, const vector<string>& list, int& outIndex)
//------------------------------------------------------------------------------
{
	for (size_t i=0,size=list.size(); i<size; i++)
	{
		if (strcmp(s,list[i].c_str()) == 0)
		{
			outIndex = (int)i;
			return true;
		}
	}
	outIndex = -1;
	return false;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
string stringLeft(const string& s, size_t nbChar)
{
	return s.substr(0,nbChar);
}
wstring stringLeft(const wstring& s, size_t nbChar)
//------------------------------------------------------------------------------
{
	return s.substr(0,nbChar);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
string stringRight(const string& s, size_t nbChar)
{
	if (nbChar >= s.length())
	{
		return s;
	}
	return s.substr(s.length()-nbChar);
}
wstring stringRight(const wstring& s, size_t nbChar)
//------------------------------------------------------------------------------
{
	if (nbChar >= s.length())
	{
		return s;
	}
	return s.substr(s.length()-nbChar);
}

/**************************************************************************\
*                                                                          *
*  Finds a string between 2 given tokens. Returns "" if not found, or if   *
*  tokenEnd not reached. If found, tokenStart and tokenEnd are not         *
*  included in the returned value. tokenStart & tokenEnd may be the same.  *
*                                                                          *
\**************************************************************************/
string getStringBetween(const string& s, const string& tokenStart, const string& tokenEnd)
{
	size_t posBeg = s.find(tokenStart);
	if (posBeg == string::npos)
	{
		return string();
	}

	size_t posEnd = s.find(tokenEnd, posBeg+tokenStart.length());
	if (posEnd == string::npos)
	{
		return string();
	}

	return s.substr(posBeg+tokenStart.length(), posEnd-posBeg-tokenStart.length());
}
wstring getStringBetween(const wstring& s, const wstring& tokenStart, const wstring& tokenEnd)
{
	size_t posBeg = s.find(tokenStart);
	if (posBeg == wstring::npos)
	{
		return wstring();
	}

	size_t posEnd = s.find(tokenEnd, posBeg+tokenStart.length());
	if (posEnd == wstring::npos)
	{
		return wstring();
	}

	return s.substr(posBeg+tokenStart.length(), posEnd-posBeg-tokenStart.length());
}

/**************************************************************************\
*                                                                          *
*  Replaces a string between the first 2 given tokens. Returns "" if at    *
*  least one token is not found.                                           *
*                                                                          *
\**************************************************************************/
string stringReplaceBetween(const string& s, const string& tokenStart, const string& tokenEnd, const string& newStr)
{
	size_t posBeg = s.find(tokenStart);
	if (posBeg == string::npos)
	{
		return string();
	}

	size_t posEnd = s.find(tokenEnd, posBeg+tokenStart.length());
	if (posEnd == string::npos)
	{
		return string();
	}

	return s.substr(0,posBeg+tokenStart.length()) + newStr + s.substr(posEnd);
}
wstring stringReplaceBetween(const wstring& s, const wstring& tokenStart, const wstring& tokenEnd, const wstring& newStr)
{
	size_t posBeg = s.find(tokenStart);
	if (posBeg == wstring::npos)
	{
		return wstring();
	}

	size_t posEnd = s.find(tokenEnd, posBeg+tokenStart.length());
	if (posEnd == wstring::npos)
	{
		return wstring();
	}

	return s.substr(0,posBeg+tokenStart.length()) + newStr + s.substr(posEnd);
}

/**************************************************************************\
*                                                                          *
*  Replaces a string token with newStr. Returns s if token is not found.   *
*                                                                          *
\**************************************************************************/
string stringReplaceToken(const string& s, const string& token, const string& newStr)
{
	size_t posBeg = s.find(token);
	if (posBeg == string::npos)
	{
		return s;
	}

	return s.substr(0,posBeg)+newStr+s.substr(posBeg+token.length());
}
wstring stringReplaceToken(const wstring& s, const wstring& token, const wstring& newStr)
{
	size_t posBeg = s.find(token);
	if (posBeg == wstring::npos)
	{
		return s;
	}

	return s.substr(0,posBeg)+newStr+s.substr(posBeg+token.length());
}

/**************************************************************************\
*                                                                          *
*  Replaces all string token with newStr. Returns s if token is not found. *
*                                                                          *
\**************************************************************************/
string stringReplaceAllTokens(string s, const string& token, const string& newStr)
{
	size_t posBeg = s.find(token);
	while (posBeg != string::npos)
	{
		s = s.substr(0,posBeg)+newStr+s.substr(posBeg+token.length());
		posBeg = s.find(token,posBeg+newStr.length());
	}
	return s;
}
wstring stringReplaceAllTokens(wstring s, const wstring& token, const wstring& newStr)
{
	size_t posBeg = s.find(token);
	while (posBeg != string::npos)
	{
		s = s.substr(0,posBeg)+newStr+s.substr(posBeg+token.length());
		posBeg = s.find(token);
	}
	return s;
}

/**************************************************************************\
*                                                                          *
*  Count occurences of a token in a string.                                *
*                                                                          *
\**************************************************************************/
size_t stringCountToken(const string& str, const string& token)
{
	size_t last  = 0;
	size_t count = 0;
	size_t tokenLen = token.length();

	for(;;)
	{
		size_t i = str.find(token, last);
		if (i == string::npos)
		{
			break;
		}
		count++;
		last = i+tokenLen;
	}

	return count;
}
size_t stringCountToken(const wstring& str, const wstring& token)
{
	size_t last  = 0;
	size_t count = 0;
	size_t tokenLen = token.length();

	for(;;)
	{
		size_t i = str.find(token, last);
		if (i == wstring::npos)
		{
			break;
		}
		count++;
		last = i+tokenLen;
	}

	return count;
}

/**************************************************************************\
*                                                                          *
*  Converts lines to a single text string (adds spacer).                   *
*                                                                          *
\**************************************************************************/
string vectorToString(const vector<string>& v, const string& spacer)
{
	string text;
	for (size_t i=0,size=v.size(); i<size; i++)
	{
		text += v[i] + spacer;
	}
	return text;
}
wstring vectorToString(const vector<wstring>& v, const wstring& spacer)
//------------------------------------------------------------------------------
{
	wstring text;
	for (size_t i=0,size=v.size(); i<size; i++)
	{
		text += v[i] + spacer;
	}
	return text;
}

/**************************************************************************\
*                                                                          *
*  Split a string in a vector.                                             *
*                                                                          *
\**************************************************************************/
void stringToVector(string s, vector<string>& v, char delim/*= ' '*/, bool trim_spaces/* = true*/)
{
	// We assume an empty string (after trimming spaces) will give an empty vector.
	// However, a non-empty string (with no delimiter) will give one item
	// After that, you get an item per delimiter, plus 1.
	// eg.  ""      => empty
	//      "a"     => 1 item
	//      "a,b"   => 2 items
	//      "a,b,"  => 3 items(last one empty)

	v.clear();

	// start with initial string, trimmed of leading/trailing spaces if required
	if (trim_spaces)
	{
		s = stringTrim(s);
	}

	// no string ? no element
	if (s.empty())
 	{
 		return;
 	}
	const char* pStr = s.c_str();

	// add to vector while we have a delimiter
	for(;;)
	{
		const char* pStrFind = strchr(pStr,delim);
		if (pStrFind == NULL)
		{
			// End
			v.push_back(trim_spaces ? stringTrim(string(pStr)) : string(pStr));
			return;
		}
		
		v.push_back(trim_spaces ? stringTrim(string(pStr,pStrFind)) : string(pStr,pStrFind));
		pStr = pStrFind+1;
	}
}
void stringToVector(wstring s, vector<wstring>& v, wchar_t delim/*= L' '*/, bool trim_spaces/* = true*/)
//------------------------------------------------------------------------------
{
	v.clear();

	// start with initial string, trimmed of leading/trailing spaces if required
	if (trim_spaces)
	{
		s = stringTrim(s);
	}

	// no string ? no element
	if (s.empty())
 	{
 		return;
 	}
	const wchar_t* pStr = s.c_str();

	// add to vector while we have a delimiter
	for(;;)
	{
		const wchar_t* pStrFind = wcschr(pStr,delim);
		if (pStrFind == NULL)
		{
			// End
			v.push_back(trim_spaces ? stringTrim(wstring(pStr)) : wstring(pStr));
			return;
		}
		
		v.push_back(trim_spaces ? stringTrim(wstring(pStr,pStrFind)) : wstring(pStr,pStrFind));
		pStr = pStrFind+1;
	}
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
float stringToFloat(const string& s, bool& outIsOk)
{
	stringstream	ss(s);
	float			f;

	ss >> f;
	if (ss.fail())
	{
		outIsOk = false;
		return 0.0f;
	}

	outIsOk = true;
	return f;
}
float stringToFloat(const wstring& s, bool& outIsOk)
//------------------------------------------------------------------------------
{
	wstringstream	ss(s);
	float			f;

	ss >> f;
	if (ss.fail())
	{
		outIsOk = false;
		return 0.0f;
	}

	outIsOk = true;
	return f;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
double stringToDouble(const string& s, bool& outIsOk)
{
	stringstream	ss(s);
	double			d;

	ss >> d;
	if (ss.fail())
	{
		outIsOk = false;
		return 0.0;
	}

	outIsOk = true;
	return d;
}
double stringToDouble(const wstring& s, bool& outIsOk)
//------------------------------------------------------------------------------
{
	wstringstream	ss(s);
	double			d;

	ss >> d;
	if (ss.fail())
	{
		outIsOk = false;
		return 0.0;
	}

	outIsOk = true;
	return d;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
int stringToInt(const string& s, bool& outIsOk)
{
	stringstream	ss(s);
	int				i;

	ss >> i;
	if (ss.fail())
	{
		outIsOk = false;
		return 0;
	}

	outIsOk = true;
	return i;
}
int stringToInt(const wstring& s, bool& outIsOk)
//------------------------------------------------------------------------------
{
	wstringstream	ss(s);
	int				i;

	ss >> i;
	if (ss.fail())
	{
		outIsOk = false;
		return 0;
	}

	outIsOk = true;
	return i;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
int64_t stringToInt64(const string& s, bool& outIsOk)
{
	stringstream	ss(s);
	int64_t			i;

	ss >> i;
	if (ss.fail())
	{
		outIsOk = false;
		return 0;
	}

	outIsOk = true;
	return i;
}
int64_t stringToInt64(const wstring& s, bool& outIsOk)
//------------------------------------------------------------------------------
{
	wstringstream	ss(s);
	int64_t			i;

	ss >> i;
	if (ss.fail())
	{
		outIsOk = false;
		return 0;
	}

	outIsOk = true;
	return i;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
uint64_t stringToUInt64(const string& s, bool& outIsOk)
{
	stringstream	ss(s);
	uint64_t		i;

	ss >> i;
	if (ss.fail())
	{
		outIsOk = false;
		return 0;
	}

	outIsOk = true;
	return i;
}
uint64_t stringToUInt64(const wstring& s, bool& outIsOk)
//------------------------------------------------------------------------------
{
	wstringstream	ss(s);
	uint64_t		i;

	ss >> i;
	if (ss.fail())
	{
		outIsOk = false;
		return 0;
	}

	outIsOk = true;
	return i;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
uint64_t stringHexToUInt64(const string& s, bool& outIsOk)
{
	stringstream	ss(s);
	uint64_t		i;

	ss >> std::hex >> i;
	if (ss.fail())
	{
		outIsOk = false;
		return 0;
	}

	outIsOk = true;
	return i;
}
uint64_t stringHexToUInt64(const wstring& s, bool& outIsOk)
//------------------------------------------------------------------------------
{
	wstringstream	ss(s);
	uint64_t		i;

	ss >> std::hex >> i;
	if (ss.fail())
	{
		outIsOk = false;
		return 0;
	}

	outIsOk = true;
	return i;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
bool stringToBool(const string& s, bool& outIsOk)
{
	stringstream	ss(s);
	bool			b;

	ss >> b;
	if (ss.fail())
	{
		outIsOk = false;
		return false;
	}

	outIsOk = true;
	return b;
}
bool stringToBool(const wstring& s, bool& outIsOk)
//------------------------------------------------------------------------------
{
	wstringstream	ss(s);
	bool			b;

	ss >> b;
	if (ss.fail())
	{
		outIsOk = false;
		return false;
	}

	outIsOk = true;
	return b;
}

/**************************************************************************\
*                                                                          *
*  Convert a string like "FFCD34" to 16764212. Must not include the "#".   *
*                                                                          *
\**************************************************************************/
uint32_t stringToHexRGB(const string& s, bool& outIsOk)
{
	unsigned int val = 0;
	outIsOk = (sscanf(s.c_str(), "%x", &val) == 1);
	return val;
}
uint32_t stringToHexRGB(const wstring& s, bool& outIsOk)
//------------------------------------------------------------------------------
{
	unsigned int val = 0;
	outIsOk = (swscanf(s.c_str(), L"%x", &val) == 1);
	return val;
}

/**************************************************************************\
*                                                                          *
*  Convert a string like "FFCD34" (or "0xFFCD32") to 16764212.             *
*                                                                          *
\**************************************************************************/
int64_t hexToInt64(const string& s, bool& outIsOk)
{
	int64_t v;
	outIsOk = true;
	try
	{
		v = std::stoll(s, nullptr, 16);
	}
	catch(...)
	{
		outIsOk = false;
		v = 0;
	}
	return v;
}
int64_t hexToInt64(const wstring& s, bool& outIsOk)
{
	int64_t v;
	outIsOk = true;
	try
	{
		v = std::stoll(s, nullptr, 16);
	}
	catch(...)
	{
		outIsOk = false;
		v = 0;
	}
	return v;
}
uint64_t hexToUInt64(const string& s, bool& outIsOk)
{
	int64_t v;
	outIsOk = true;
	try
	{
		v = std::stoull(s, nullptr, 16);
	}
	catch(...)
	{
		outIsOk = false;
		v = 0;
	}
	return v;
}
uint64_t hexToUInt64(const wstring& s, bool& outIsOk)
{
	int64_t v;
	outIsOk = true;
	try
	{
		v = std::stoull(s, nullptr, 16);
	}
	catch(...)
	{
		outIsOk = false;
		v = 0;
	}
	return v;
}


/**************************************************************************\
*                                                                          *
*  printf-like for std::string.                                            *
*                                                                          *
\**************************************************************************/
string stringFormat(const char* fmt, ...)
{
	va_list	argptr;
	char	buf[4096];

	va_start(argptr,fmt);
	/*int needed =*/ vsnprintf(buf, sizeof(buf)/sizeof(char), fmt, argptr);
	va_end(argptr);

// 	assert(needed < (int)sizeof(buf));
	return string(buf);
}
wstring stringFormat(const wchar_t* fmt, ...)
//------------------------------------------------------------------------------
{
	va_list	argptr;
	wchar_t	buf[4096];

	va_start(argptr,fmt);
	/*int needed =*/ __vswprintf(buf, sizeof(buf)/sizeof(wchar_t), fmt, argptr);
	va_end(argptr);

// 	assert(needed < (int)sizeof(buf));
	return buf;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
string toString(double d, int nbDecimal/*=7*/) // Equivalent to stringFormat("%.nf",d);  where n is the # of decimals
//------------------------------------------------------------------------------
{
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(nbDecimal) << d;
	return stream.str();
}
wstring toWString(double d, int nbDecimal/*=7*/)	// Equivalent to stringFormat(L"%.nf",d);  where n is the # of decimals  
//------------------------------------------------------------------------------
{
	std::wostringstream stream;
	stream << std::fixed << std::setprecision(nbDecimal) << d;
	return stream.str();
}
string toString(float f)			// Equivalent to stringFormat("%f",f);  
//------------------------------------------------------------------------------
{
	return to_string(f);
}
wstring toWString(float f)			// Equivalent to stringFormat(L"%f",f);  
//------------------------------------------------------------------------------
{
	return to_wstring(f);
}
string toString(int i)				// Equivalent to stringFormat("%d",i);  
//------------------------------------------------------------------------------
{
	return to_string(i);
}
wstring toWString(int i)			// Equivalent to stringFormat(L"%d",i);  
//------------------------------------------------------------------------------
{
	return to_wstring(i);
}
string toString(int64_t i)			// Equivalent to stringFormat("%lld",i);  
//------------------------------------------------------------------------------
{
	return to_string(i);
}
wstring toWString(int64_t i)		// Equivalent to stringFormat(L"%lld",i);  
//------------------------------------------------------------------------------
{
	return to_wstring(i);
}
string toString(size_t i)			// Equivalent to stringFormat("%u",i);  
//------------------------------------------------------------------------------
{
	return to_string(i);
}
wstring toWString(size_t i)			// Equivalent to stringFormat(L"%u",i);  
//------------------------------------------------------------------------------
{
	return to_wstring(i);
}
string toString(bool b)				// Equivalent to stringFormat("%s",b?"true":"false");
//------------------------------------------------------------------------------
{
	return b ? "true":"false";
}
wstring toWString(bool b)			// Equivalent to stringFormat(L"%s",b?"true":"false");
//------------------------------------------------------------------------------
{
	return b ? L"true":L"false";
}
string toStringHex(int i, int width)
//------------------------------------------------------------------------------
{
	// Equivalent to StringFormat("%nX",i) with n=width 

	std::ostringstream stream;

	stream.flags(ios::right | ios::hex | ios::uppercase);
	stream.fill ('0');
	stream.width(width);
	stream << i;
	return stream.str();
}
wstring toWStringHex(int i, int width)
//------------------------------------------------------------------------------
{
	// Equivalent to StringFormat("%nX",i) with n=width 

	std::wostringstream stream;

	stream.flags(ios::right | ios::hex | ios::uppercase);
	stream.fill ('0');
	stream.width(width);
	stream << i;
	return stream.str();
}
string toStringHexRGB(u8 r, u8 g, u8 b)
//------------------------------------------------------------------------------
{
	char buf[10];
	sprintf(buf,"%02X%02X%02X",r,g,b);
	return string(buf);
}
wstring toWStringHexRGB(u8 r, u8 g, u8 b)
//------------------------------------------------------------------------------
{
	return StringToWString(toStringHexRGB(r,g,b));
}
string toStringHexARGB(u8 a, u8 r, u8 g, u8 b)
//------------------------------------------------------------------------------
{
	char buf[10];
	sprintf(buf,"%02X%02X%02X%02X",a,r,g,b);
	return string(buf);
}
wstring toWStringHexARGB(u8 a, u8 r, u8 g, u8 b)
//------------------------------------------------------------------------------
{
	return StringToWString(toStringHexARGB(a,r,g,b));
}

/**************************************************************************\
*                                                                          *
*   int to ascii string                                                    *
*                                                                          *
\**************************************************************************/
char* __itoa(int32_t value, char* str, int radix)
{
	return __i64toa((int64_t)value,str,radix);
}
char* __i64toa(int64_t value, char* str, int radix)
//------------------------------------------------------------------------------
{
	if (radix < 2 || radix > 36)
	{
		*str = '\0';
		return str;
	}

	static const char digit[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	bool bIsNegative = (radix == 10 && value < 0);
	if (bIsNegative)
	{
		value = -value;
	}

	char buf[64+1+1+1];
	int n = 0;
	uint64_t v = (uint64_t)value;
	do
	{
		buf[n++] = digit[v%radix];
		v /= radix;
	}
	while (v);

	if (bIsNegative)
	{
		buf[n++] = '-';
	}
	buf[n] = '\0';

	for (int i=n-1; i>=0; i--)
	{
		*str++ = buf[i];
	}
	*str++ = '\0';
	return str;
}

/**************************************************************************\
*                                                                          *
*   unsigned int to ascii string                                           *
*                                                                          *
\**************************************************************************/
char* __uitoa(uint32_t value, char* str, int radix)
{
	return __ui64toa((uint64_t)value,str,radix);
}
char* __ui64toa(uint64_t value, char* str, int radix)
//------------------------------------------------------------------------------
{
	if (radix < 2 || radix > 36)
	{
		*str = '\0';
		return str;
	}

	static const char digit[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	char buf[64+1+1+1];
	int n = 0;
	do
	{
		buf[n++] = digit[value%radix];
		value /= radix;
	}
	while (value);
	buf[n] = '\0';

	for (int i=n-1; i>=0; i--)
	{
		*str++ = buf[i];
	}
	*str++ = '\0';
	return str;
}

/**************************************************************************\
*                                                                          *
*  Convert wstring to string. Some extended chars may be converted to "?". *
*                                                                          *
\**************************************************************************/
string WStringToString(const wstring& wstr)
{
	string	str;
	for (int i=0; i<(int)wstr.length(); i++)
	{
		char ch[10];
		int  res = wctomb(ch,wstr[i]);
		if (res == 1)
		{
			str += ch[0];
		}
		else
		{
			str += '?';
			assert(0);
		}
	}
	return str;
}

/**************************************************************************\
*                                                                          *
*  Convert string to wstring.                                              *
*                                                                          *
\**************************************************************************/
wstring StringToWString(const string& str)
{
	wstring wstr(str.length(), L' ');
	if (mbstowcs((wchar_t*)wstr.c_str(), str.c_str(), str.length()) == (uint32_t)-1)
	{
		assert(0); // problem with a "strange" character ?
	}
	return wstr;
}

/**************************************************************************\
*                                                                          *
*  Escape these chars : \a, \b, \e, \f, \n, \r, \t, \v, \, ', ", ?.        *
*  Octal, hexa and unicode format not yet supported.                       *
*                                                                          *
\**************************************************************************/
string stringEscapeC(const string& str)
{
	const char* escapeSequence = "\a\b\f\n\r\t\v\\\'\"\?";
	const char* escapeCodes    = "abfnrtv\\'\"?";

	size_t len = str.length();
	string res;
	res.reserve(len*2);

	for (size_t i=0; i<len; i++)
	{
		const char* pos = strchr(escapeSequence,str[i]);
		if (pos)
		{
			res.push_back('\\');
			res.push_back(escapeCodes[pos-escapeSequence]);
		}
		else
		{
			res.push_back(str[i]);
		}
	}
	return res;
}

/**************************************************************************\
*                                                                          *
*  Unescape a C string.                                                    *
*                                                                          *
\**************************************************************************/
string stringUnescapeC(const string& str)
{
	const char* escapeSequence = "\a\b\f\n\r\t\v\\\'\"\?";
	const char* escapeCodes    = "abfnrtv\\'\"?";

	size_t len = str.length();
	string res;
	res.reserve(len);

	for (size_t i=0; i<len; )
	{
		char c = str[i++];
		if (c == '\\')
		{
			if (i<len)
			{
				char code = str[i++];

				const char* pos = strchr(escapeCodes,code);
				if (pos)
				{
					res.push_back(escapeSequence[pos-escapeCodes]);
				}
				else
				{
					// Escape sequence not found
					res.push_back('\\');
					res.push_back(code);
				}
			}
			else
			{
				// end of string, unfinished string
				break;
			}
		}
		else
		{
			res.push_back(c);
		}
	}
	return res;
}
