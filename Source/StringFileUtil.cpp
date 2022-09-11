#include "StringUtil.h"
#include "StringFileUtil.h"


/**************************************************************************\
*                                                                          *
*  Removes the directories path in a full or relative path with filename.  *
*                                                                          *
\**************************************************************************/
string stringRemovePath(const string& path)
{
	string d(path); 
	stringReplaceChar(d, '\\', '/');

	string::size_type i = d.find_last_of("/");
	if (i == string::npos)
	{
		return d;
	}

	return d.erase(0, i+1);
}

/**************************************************************************\
*                                                                          *
*  Removes filename in a path, keeps the last slash. Converts '\' to '/'.  *
*                                                                          *
\**************************************************************************/
string stringRemoveFilename(const string& path)
{
	string d(path);
	stringReplaceChar(d, '\\', '/');

	string::size_type i = d.find_last_of("/");
	if (i == string::npos)
	{
		return string();
	}

	return d.erase(i+1);
}

/**************************************************************************\
*                                                                          *
* Removes the filename extension, including the point. Converts '\' to '/' *
*                                                                          *
\**************************************************************************/
string stringRemoveFileExt(const string& path)
{
	string d(path);
	stringReplaceChar(d, '\\', '/');

	string::size_type i = path.find_last_of(".");
	if (i == string::npos)
	{
		return d;
	}
	
	return d.erase(i);
}

/**************************************************************************\
*                                                                          *
*  Get only the directories part of the path without filename.             *
*                                                                          *
\**************************************************************************/
string stringGetPath(const string& path)
{
	string d(path); 
	stringReplaceChar(d, '\\', '/');

	string::size_type i = d.find_last_of("/");
	if (i == string::npos)
	{
		return d;
	}

	return d.substr(0, i+1);
}

/**************************************************************************\
*                                                                          *
*  Get only the filename part of the path.                                 *
*                                                                          *
\**************************************************************************/
string stringGetFilename(const string& path)
{
	string d(path);
	stringReplaceChar(d, '\\', '/');

	string::size_type i = d.find_last_of("/");
	if (i == string::npos)
	{
		return d;
	}

	return d.substr(i+1);
}

/**************************************************************************\
*                                                                          *
*  Get only the filename extension (with point part) of the path.          *
*                                                                          *
\**************************************************************************/
string stringGetFileExt(const string& path)
{
	string::size_type i = path.find_last_of(".");
	if (i == string::npos)
	{
		return string();
	}

	return path.substr(i);
}

