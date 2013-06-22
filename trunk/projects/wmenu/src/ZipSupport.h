#pragma once

#include "stdafx.h"

// Zip File System Namespace
namespace zip_support {

class ZipSupport;



// Zip file headers
enum headerid {	LOCALFILE	= 0x04034b50,
				FILE		= 0x02014b50,
				ENDOFDIR	= 0x06054b50, 
				UNKNOWN, 
				READERROR
};



// Zip file "local file" header class
struct local_file_header
{
	bool ReadHeader(std::istream & File);

	static const uint_least32_t m_ConstSign= LOCALFILE;

	uint_least32_t		m_Signature;
	uint_least16_t		m_VersionExtract;
	uint_least16_t		m_GeneralFlags;
	uint_least16_t		m_CompMethod;
	uint_least16_t		m_Time;
	uint_least16_t		m_Date;
	uint_least32_t		m_CRC32;
	uint_least32_t		m_CompSize;
	uint_least32_t		m_UncompSize;
	uint_least16_t		m_FilenameSize;
	uint_least16_t		m_FieldSize;

	std::vector<char>	m_Filename;
	std::vector<char>	m_ExtraField;
};



// Zip file "file header" header class
struct file_header
{
	bool ReadHeader(std::istream & File);

	static const headerid m_ConstSign = FILE;

	uint_least32_t		m_Signature;
	uint_least16_t		m_VersionMade;
	uint_least16_t		m_VersionExtract;
	uint_least16_t		m_GeneralFlags;
	uint_least16_t		m_CompMethod;
	uint_least16_t		m_Time;
	uint_least16_t		m_Date;
	uint_least32_t		m_CRC32;
	uint_least32_t		m_CompSize;
	uint_least32_t		m_UncompSize;
	uint_least16_t		m_FilenameSize;
	uint_least16_t		m_FieldSize;
	uint_least16_t		m_CommentSize;
	uint_least16_t		m_DiskNb;
	uint_least16_t		m_IntAttrib;
	uint_least32_t		m_ExtAttrib;
	uint_least32_t		m_RelOffset;

	std::vector<char>	m_Filename;
	std::vector<char>	m_ExtraField;
	std::vector<char>	m_Comment;
};



// Zip file "end of central dir" header class
struct eofcd_header
{
	bool ReadHeader(std::istream & File);

	static const headerid m_ConstSign = ENDOFDIR;

	uint_least32_t		m_Signature;
	uint_least16_t		m_NbDisks;
	uint_least16_t		m_DirDisk;
	uint_least16_t		m_LocalEntries;
	uint_least16_t		m_TotalEntries;
	uint_least32_t		m_DirSize;
	uint_least32_t		m_Offset;
	uint_least16_t		m_CommentSize;

	std::vector<char>	m_Comment;
};



// Zip File System central class
class ZipSupport 
{
public:
	class limited_file_info
	{
	public:
		limited_file_info() :  m_Offset(0), m_Size(0) { }
		limited_file_info(size_t Offset, size_t Size) :
			m_Offset(Offset), m_Size(Size) { }

		size_t	m_Offset;
		size_t	m_Size;
	};

    static bool PreloadZip(const char * Filename, std::map<std::string, limited_file_info>& target);
    static std::streamoff SkipLFHdr(std::istream & File, std::streamoff LFHdrPos);

    
protected:



	// Zip file info class
	class zipfile_info
	{
	public:
		zipfile_info() : m_NbEntries(0), m_FilesSize(0), m_FilesCompSize(0) { }

		std::string	m_Filename;
		size_t		m_NbEntries;
		size_t		m_FilesSize;
		size_t		m_FilesCompSize;
	};

	// Class for file path string comparaison
	struct lt_path
	{
		bool operator() (const std::string & s1, const std::string & s2) const;
	};


	// Protected member functions
	// Zip file format related functions
	static std::streamoff CentralDir(std::istream & File);
    static std::streamoff CentralDirZipped(std::istream & File, std::streamoff begin, std::size_t size);
	static headerid NextHeader(std::istream & File);

};


}	// namespace zip_file_system

