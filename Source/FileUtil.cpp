#include "FileUtil.h"
#include "StringUtil.h"

/**************************************************************************\
*                                                                          *
*  True if directory exists. False is not or if it's a file.               *
*                                                                          *
\**************************************************************************/
bool isPathExist(const string& pathname)
{
	string path = makeCorrectPath(pathname, false);

	struct __sstat64 status;
	if (__stat64(path.c_str(), &status) == -1)
	{
		return false;
	}

	if (status.st_mode & S_IFDIR)
	{
		return true;
	}

	return false;
}

/**************************************************************************\
*                                                                          *
*  True if file exists. False is not or if it's a directory.               *
*                                                                          *
\**************************************************************************/
bool isFileExist(const string& filename)
{
	struct __sstat64 status;

	if (__stat64(filename.c_str(), &status) == -1)
	{
		return false;
	}

	if (status.st_mode & S_IFREG)
	{
		return true;
	}

	return false;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
bool loadFile(const string& path, vector<uint8_t>& buffer)
{
	ifstream			f;
	struct __sstat64	results;

	buffer.clear();

	if (__stat64(path.c_str(), &results) != 0)
	{
		return false;
	}

	f.open(path.c_str(), ios::in | ios::binary);
	if (f.is_open() == false)
	{
		return false;
	}

	uint32_t lu = 0;
	buffer.resize((size_t)results.st_size);
	if (buffer.size() > 0)
	{
		f.read((char*)&buffer.front(), (int)buffer.size());
		lu = (uint32_t)f.gcount();
	}
	f.close();

	return (lu == (uint32_t)results.st_size);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
bool loadFile(const string& path, string& buffer, bool ignoreCrLfConversion/*=false*/)
{
	ifstream			f;
	struct __sstat64	results;

	buffer.clear();

	if (__stat64(path.c_str(), &results) != 0)
	{
		return false;
	}

	f.open(path.c_str(), ios::in|ios::binary);
	if (f.is_open() == false)
	{
		return false;
	}

	buffer.resize((size_t)results.st_size);
	f.read((char*)buffer.data(), (int)results.st_size);
	if (ignoreCrLfConversion == false)
	{
		buffer = stringReplaceAllTokens(buffer,"\r\n","\n");
	}
	bool isBad = f.bad();
	f.close();

	return (isBad == false);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
bool saveFile(const string& path, void* data, int lengthInByte, bool bAppendMode/*=false*/)
{
	ofstream	f;
	bool		isBad = false;

	f.open(path.c_str(), (bAppendMode ? ios::out|ios::binary|ios::app : ios::out|ios::binary));
	if (f.is_open() == false)
	{
		return false;
	}

	if (lengthInByte)
	{
		f.write((char*)data, lengthInByte);
		isBad = f.bad();
	}
	f.close();

	return (isBad == false);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
bool saveFile(const string& path, const vector<uint8_t>& buffer, bool bAppendMode/*=false*/)
{
	if (buffer.size() == 0)
	{
		return true;
	}
	return saveFile(path, (void*)&buffer.front(), (int)buffer.size(), bAppendMode);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
bool saveFile(const string& path, const string& buffer, bool bAppendMode/*=false*/)
{
	if (buffer.empty())
	{
		return true;
	}
	return saveFile(path, (void*)buffer.data(), (int)buffer.length(), bAppendMode);
}

/**************************************************************************\
*                                                                          *
*  Renames (or moves if supported by the OS) a file.                       *
*                                                                          *
\**************************************************************************/
bool renameFile(const string& oldName, const string& newName)
{
	return renameDirectory(oldName, newName);
}

/**************************************************************************\
*                                                                          *
*  Deletes a file. Returns true if success.                                *
*                                                                          *
\**************************************************************************/
bool deleteFile(const string& path)
{
	if (isFileExist(path) == false)
	{
		return true;
	}
	return (remove(path.c_str()) == 0);
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
bool copyFile(const string& srcPath, const string& dstPath)
{
	FILE* fi = fopen(srcPath.c_str(), "rb");
	if (!fi)
	{
		return false;
	}

	FILE* fo = fopen(dstPath.c_str(), "wb");
	if (!fo)
	{
		fclose(fi);
		return false;
	}

	uint8_t	buffer[4096];
	bool	retCode = true;


	for (;;)
	{
		int nbRead = (int)fread(buffer, 1, sizeof(buffer), fi);
		if (nbRead > 0)
		{
			int nbWrite = (int)fwrite(buffer, 1, nbRead, fo);
			if (nbWrite != nbRead)
			{
				retCode = false;
				break;
			}
		}
		else
		{
			retCode = (feof(fi) != 0);
			break;
		}
	}

	fclose(fi);
	fclose(fo);
	return retCode;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
s64 getFileSize(const string& filename)
{
	struct __sstat64 status;

	if (__stat64(filename.c_str(), &status) != -1)
	{
		return status.st_size;
	}

	return -1;
}

/**************************************************************************\
*                                                                          *
*  Get the current path (slashes are OS dependent).                        *
*                                                                          *
\**************************************************************************/
string getCurrentDirectory()
{
	char buffer[3000] = {0};
	__getcwd(buffer, sizeof(buffer));
	string path(buffer);
	return makeCorrectPath(path);
}

/**************************************************************************\
*                                                                          *
*  Set the current path.                                                   *
*                                                                          *
\**************************************************************************/
bool setCurrentDirectory(const string& dir)
{
	string pathname = makeCorrectPath(dir, false);
	return (__chdir(pathname.c_str()) == 0);
}

/**************************************************************************\
*                                                                          *
*  Create a directory, doesn't fail if it already exists.                  *
*                                                                          *
\**************************************************************************/
bool createDirectory(const string& dir)
{
	string pathname = makeCorrectPath(dir, false);
	if (!isPathExist(pathname))
	{
		__mkdir(pathname.c_str());
	}
	return isPathExist(pathname);
}

/**************************************************************************\
*                                                                          *
*  Delete an ***EMPTY*** directory, doesn't fail if it doesn't exists.     *
*                                                                          *
\**************************************************************************/
bool deleteDirectory(const string& dir)
{
	string pathname = makeCorrectPath(dir, false);

	if (isPathExist(pathname))
	{
		__rmdir(pathname.c_str());
	}
	return !isPathExist(pathname);
}

/**************************************************************************\
*                                                                          *
*  Renames (or moves if supported by the OS) a directory.                  *
*                                                                          *
\**************************************************************************/
bool renameDirectory(const string& oldName, const string& newName)
{
	return (rename(oldName.c_str(), newName.c_str()) == 0);
}

/**************************************************************************\
*                                                                          *
*  Recursively delete a directory tree (directories and files).            *
*    Returns true if all is ok, if false: may be partially deleted or not. *
*    TODO: check if it may fail because rootDir is the current directory.  *
*                                                                          *
\**************************************************************************/
bool delDirectoryTree(const string& rootDir)
{
	if (isPathExist(rootDir))
	{
		vector<string>	fileList;
		vector<string>	dirList;
		bool			hasError = false;
		string			curDir = makeCorrectPath(rootDir);

		// 1st : get directory listing
		getDirectoryFileList(rootDir, fileList);
		getDirectoryDirList(rootDir, dirList);

		// 2nd : erase files
		for (size_t i = 0, size = fileList.size(); i<size; i++)
		{
			hasError |= !deleteFile(curDir + fileList[i]);
		}

		// 3rd : recursion on directories
		for (size_t i = 0, size = dirList.size(); i<size; i++)
		{
			hasError |= !delDirectoryTree(curDir + dirList[i]);
		}

		// 4th : erase current dir
		hasError |= !deleteDirectory(rootDir);

		return !hasError;
	}
	return true;
}

/**************************************************************************\
*                                                                          *
*  Get a list of directories in a directory. Returns true if no error.     *
*  You can specify a file extension to filter results (case sensitive).    *
*  If so, don't add the point (ex: "bmp", "conf", "txt", ...).             *
*                                                                          *
\**************************************************************************/
bool getDirectoryFileList(const string& rootDir, vector<string>& fileList, string ext/*=""*/)
{
	fileList.clear();

	if (isPathExist(rootDir))
	{
#if defined(UNIX)
		DIR* directory = opendir(makeCorrectPath(rootDir, false).c_str());
		if (directory == NULL)
		{
//			ConsoleWriteErr("getDirectoryFileList(..) failed : %s", getErrno().c_str());
			return false;
		}

		struct dirent* dirEntry = readdir(directory);
		while (dirEntry)
		{
			if (dirEntry->d_type == DT_REG  &&  strlen(dirEntry->d_name)>0)
			{
				string filename = dirEntry->d_name;
				if (ext.empty() || isStringEndsWith(filename, ext))
				{
					fileList.push_back(filename);
				}
			}
			dirEntry = readdir(directory);
		}
		closedir(directory);
#elif defined(WIN32)
		_finddata_t	c_file;
		intptr_t	hFile;
		string		mask = makeCorrectPath(rootDir, false) + "\\*";
		hFile = _findfirst(mask.c_str(), &c_file);
		if (hFile == -1L)
		{
//			ConsoleWriteErr("getDirectoryFileList(..) failed : %s", getErrno().c_str());
			return false;	// maybe the directory is empty ?
		}

		do
		{
			if (!(c_file.attrib & _A_SUBDIR))
			{
				string filename = c_file.name;
				if (ext.empty() || isStringEndsWith(filename, ext))
				{
					fileList.push_back(filename);
				}
			}
		}
		while (_findnext(hFile, &c_file) == 0);
		_findclose(hFile);
#endif
		return true;
	}
	return false;
}

/**************************************************************************\
*                                                                          *
*  Get a list of files in a directory. Returns true if no error.           *
*                                                                          *
\**************************************************************************/
bool getDirectoryDirList(const string& rootDir, vector<string>& dirList)
{
	dirList.clear();

	if (isPathExist(rootDir))
	{
#if defined(UNIX)
		DIR* directory = opendir(makeCorrectPath(rootDir, false).c_str());
		if (directory == NULL)
		{
//			ConsoleWriteErr("getDirectoryDirList(..) failed : %s", getErrno().c_str());
			return false;
		}

		struct dirent* dirEntry = readdir(directory);
		while (dirEntry)
		{
			if (dirEntry->d_type == DT_DIR  &&  strlen(dirEntry->d_name))
			{
				if (strcmp(dirEntry->d_name, ".") && strcmp(dirEntry->d_name, ".."))
				{
					dirList.push_back(dirEntry->d_name);
				}
			}
			dirEntry = readdir(directory);
		}
		closedir(directory);
#elif defined(WIN32)
		_finddata_t	c_file;
		intptr_t	hFile;
		string		mask = makeCorrectPath(rootDir, false) + "\\*";
		hFile = _findfirst(mask.c_str(), &c_file);
		if (hFile == -1L)
		{
//			ConsoleWriteErr("getDirectoryDirList(..) failed : %s", getErrno().c_str());
			return false; // maybe the directory is empty ?
		}

		do
		{
			if (c_file.attrib & _A_SUBDIR)
			{
				if (strcmp(c_file.name, ".") && strcmp(c_file.name, ".."))
				{
					dirList.push_back(c_file.name);
				}
			}
		}
		while (_findnext(hFile, &c_file) == 0);
		_findclose(hFile);
#endif
		return true;
	}
	return false;
}

/**************************************************************************\
*                                                                          *
*  Sub function of getFullRecursiveList. Don't call it directly.           *
*                                                                          *
\**************************************************************************/
static void getFullRecursiveListSub2(const string& rootDir, vector<pair<string, bool>>& liste)
{
	if (isPathExist(rootDir))
	{
		vector<string>	fileList;
		vector<string>	dirList;
		string			curDir = makeCorrectPath(rootDir);

		// 1st : get directory listing
		getDirectoryFileList(rootDir, fileList);
		getDirectoryDirList(rootDir, dirList);

		// 2nd : add files
		for (size_t i = 0, size = fileList.size(); i<size; i++)
		{
			liste.push_back(make_pair<string, bool>(curDir + fileList[i], true));
		}

		// 3rd : add and recurse on directories
		for (size_t i = 0, size = dirList.size(); i<size; i++)
		{
			liste.push_back(make_pair<string, bool>(curDir + dirList[i], false));
			getFullRecursiveListSub2(liste.back().first, liste);
		}
	}
}

/**************************************************************************\
*                                                                          *
*  Get an vector of pair<filename,isFile_flag>.                            *
*                                                                          *
\**************************************************************************/
void getFullRecursiveList(const string& rootDir, vector<pair<string, bool>>& liste)
{
	liste.clear();
	getFullRecursiveListSub2(rootDir, liste);
}

/**************************************************************************\
*                                                                          *
*  Add or remove trailing slash or backslash to a path.                    *
*                                                                          *
\**************************************************************************/
string makeCorrectPath(const string& originalPath, bool bMustSetTrailingSlash/*=true*/)
{
	string str(originalPath);
//#if defined(UNIX)
	std::replace(str.begin(), str.end(), '\\', '/');
//#else
//	std::replace(str.begin(), str.end(), '/', '\\');
//#endif

	size_t length = str.length();
	if (length > 0)
	{
		bool hasSlash = (str[length-1] == '/') || (str[length-1] == '\\');
		if (hasSlash && !bMustSetTrailingSlash)
		{
			return str.substr(0, length-1);
		}
		else if (!hasSlash && bMustSetTrailingSlash)
		{
//#if defined(UNIX)
			return str + '/';
//#else
//			return str + '\\';
//#endif
		}
		// other cases : hasSlash && mustSet, !hasSlash && !mustSet -> no change
	}

	return str;
}

/**************************************************************************\
*                                                                          *
*  Converts / or \ to the correct direction for a filename+path.           *
*                                                                          *
\**************************************************************************/
string makeCorrectFilePath(const string& originalPath)
{
	string str(originalPath);
#if defined(UNIX)
	std::replace(str.begin(), str.end(), '\\', '/');
#else
	std::replace(str.begin(), str.end(), '/', '\\');
#endif

	return str;
}

/**************************************************************************\
*                                                                          *
*                                                                          *
*                                                                          *
\**************************************************************************/
bool getFileDateTime(const string& filename, int& y,int& M,int& d, int& h,int& m,int& s)
{
	struct __sstat64 status;

	if (__stat64(filename.c_str(), &status) == -1)
	{
		return false;
	}

	if (status.st_mode & S_IFREG)
	{
		struct tm* temps = gmtime(&status.st_mtime);
		y = temps->tm_year + 1900;
		M = temps->tm_mon;
		d = temps->tm_mday;
		h = temps->tm_hour;
		m = temps->tm_min;
		s = temps->tm_sec;
		return true;
	}

	return false;
}

/**************************************************************************\
*                                                                          *
*  Check name validity for the filesystem (common rules for win/linux)     *
*  Fails in case of \, /, :, *, ", <, >, |, and ascii char < 32.           *
*  Fails also if it's one of : CON, PRN, AUX, NUL, COMx, LPTx (x=[1-9]).   *
*                                                                          *
\**************************************************************************/
bool isFilenameValid(const string& filename)
{
	static const string kCON("CON");
	static const string kPRN("PRN");
	static const string kAUX("AUX");
	static const string kNUL("NUL");
	static const string kCOM("COM");
	static const string kLPT("LPT");


	if (filename.length() == 0)
	{
		return false;
	}

	if (filename.back() == ' ' || filename.back() == '.')	// Windows Specific
	{
		return false;
	}

	if (filename.find_first_of("\\/:*\"<>|") != string::npos)
	{
		return false;
	}

	if (std::any_of(filename.begin(), filename.end(), [](int i){return i<32/* || i>127*/;}))
	{
		return false;
	}

	if (filename == kCON || filename == kPRN || 
		filename == kAUX || filename == kNUL || 
		filename.find(kCOM) == 0 || filename.find(kLPT) == 0)
	{
		return false;
	}

	return true;
}
