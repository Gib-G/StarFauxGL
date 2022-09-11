#pragma once
#include "Types.h"

bool	isPathExist(const string& pathname);
bool	isFileExist(const string& filename);

bool	loadFile(const string& path, vector<uint8_t>& buffer);										// binary version
bool	loadFile(const string& path, string&          buffer, bool ignoreCrLfConversion = false);	// string version
bool	saveFile(const string& path, void* data, int lengthInByte,  bool bAppendMode = false);		// raw    version (binary write, no CRLF translation)
bool	saveFile(const string& path, const vector<uint8_t>& buffer, bool bAppendMode = false);		// vector version (binary write, no CRLF translation)
bool	saveFile(const string& path, const string&          buffer, bool bAppendMode = false);		// string version (binary write, no CRLF translation)

bool	renameFile(const string& oldName, const string& newName);
bool	deleteFile(const string& path);
bool	copyFile(const string& srcPath, const string& dstPath);
s64		getFileSize(const string& filename);

string	getCurrentDirectory();
bool	setCurrentDirectory(const string& dir);
bool	createDirectory(const string& dir);
bool	deleteDirectory(const string& dir);
bool	renameDirectory(const string& oldName, const string& newName);

bool	delDirectoryTree(const string& rootDir);
bool	getDirectoryFileList(const string& rootDir, vector<string>& fileList, string ext = "");
bool	getDirectoryDirList(const string& rootDir, vector<string>& dirList);
void	getFullRecursiveList(const string& rootDir, vector<pair<string, bool>>& liste);

string	makeCorrectPath    (const string& originalPath, bool bMustSetTrailingSlash = true);
string	makeCorrectFilePath(const string& originalPath);
bool	isFilenameValid    (const string& filename);

bool getFileDateTime(const string& filename, int& y,int& M,int& d, int& h,int& m,int& s);
