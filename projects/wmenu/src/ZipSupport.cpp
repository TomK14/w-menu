

#include "stdafx.h"
#include "ZipSupport.h"


#include "fileio.h"		// I/O facilities


namespace zip_support {

using namespace std;



//////////////////////////////////////////////////////////////////////
// Zip Header Classes Member Functions
//////////////////////////////////////////////////////////////////////

bool local_file_header::ReadHeader(std::istream & File)
{
	using io_facilities::readvar;

	// quick check about char size
	//static_assert(CHAR_BIT == 8);

	if (! readvar(File, m_Signature, 4)) return false;
	if (! readvar(File, m_VersionExtract, 2)) return false;
	if (! readvar(File, m_GeneralFlags, 2)) return false;
	if (! readvar(File, m_CompMethod, 2)) return false;
	if (! readvar(File, m_Time, 2)) return false;
	if (! readvar(File, m_Date, 2)) return false;
	if (! readvar(File, m_CRC32, 4)) return false;
	if (! readvar(File, m_CompSize, 4)) return false;
	if (! readvar(File, m_UncompSize, 4)) return false;
	if (! readvar(File, m_FilenameSize, 2)) return false;
	if (! readvar(File, m_FieldSize, 2)) return false;

	m_Filename.resize(m_FilenameSize + 1);
	m_ExtraField.resize(m_FieldSize + 1);

	if (! File.read(&(m_Filename[0]), m_FilenameSize)) return false;
	if (! File.read(&(m_ExtraField[0]), m_FieldSize)) return false;

	m_Filename[m_FilenameSize] = '\0';
	m_ExtraField[m_FieldSize] = '\0';

	return true;
}



bool file_header::ReadHeader(std::istream & File)
{
	using io_facilities::readvar;

	if (! readvar(File, m_Signature, 4)) return false;
	if (! readvar(File, m_VersionMade, 2)) return false;
	if (! readvar(File, m_VersionExtract, 2)) return false;
	if (! readvar(File, m_GeneralFlags, 2)) return false;
	if (! readvar(File, m_CompMethod, 2)) return false;
	if (! readvar(File, m_Time, 2)) return false;
	if (! readvar(File, m_Date, 2)) return false;
	if (! readvar(File, m_CRC32, 4)) return false;
	if (! readvar(File, m_CompSize, 4)) return false;
	if (! readvar(File, m_UncompSize, 4)) return false;
	if (! readvar(File, m_FilenameSize, 2)) return false;
	if (! readvar(File, m_FieldSize, 2)) return false;
	if (! readvar(File, m_CommentSize, 2)) return false;
	if (! readvar(File, m_DiskNb, 2)) return false;
	if (! readvar(File, m_IntAttrib, 2)) return false;
	if (! readvar(File, m_ExtAttrib, 4)) return false;
	if (! readvar(File, m_RelOffset, 4)) return false;

	m_Filename.resize(m_FilenameSize + 1);
	m_ExtraField.resize(m_FieldSize + 1);
	m_Comment.resize(m_CommentSize + 1);

	if (! File.read(&(m_Filename[0]), m_FilenameSize)) return false;
	if (! File.read(&(m_ExtraField[0]), m_FieldSize)) return false;
	if (! File.read(&(m_Comment[0]), m_CommentSize)) return false;

	m_Filename[m_FilenameSize] = '\0';
	m_ExtraField[m_FieldSize] = '\0';
	m_Comment[m_CommentSize] = '\0';

	return true;
}



bool eofcd_header::ReadHeader(std::istream & File)
{
	using io_facilities::readvar;

	if (! readvar(File, m_Signature, 4)) return false;
	if (! readvar(File, m_NbDisks, 2)) return false;
	if (! readvar(File, m_DirDisk, 2)) return false;
	if (! readvar(File, m_LocalEntries, 2)) return false;
	if (! readvar(File, m_TotalEntries, 2)) return false;
	if (! readvar(File, m_DirSize, 4)) return false;
	if (! readvar(File, m_Offset, 4)) return false;
	if (! readvar(File, m_CommentSize, 2)) return false;

	m_Comment.resize(m_CommentSize + 1);

	if (! File.read(&(m_Comment[0]), m_CommentSize)) return false;

	m_Comment[m_CommentSize] = '\0';

	return true;
}




bool ZipSupport::PreloadZip(const char * Filename, map<string, limited_file_info>& target)
{
	zipfile_info ZipInfo;

	// Open zip
	ifstream File(Filename, ios::binary);

	if (! File)
		return false;


	if (! File.seekg(CentralDir(File)))
    {
        File.close();
        return false;
    }

	// Check every headers within the zip file
	file_header FileHdr;

	while ((NextHeader(File) == FILE) && (FileHdr.ReadHeader(File))) {

		// Include files into Files map
		const char * Name = &(* FileHdr.m_Filename.begin());
		if (FileHdr.m_FilenameSize != 0) {

            // The zip in zip method only supports stored Zips because of JFileSystem limitations
            if ((FileHdr.m_UncompSize != FileHdr.m_CompSize))
                continue;

			target[Name] = limited_file_info(
				FileHdr.m_RelOffset,					// "Local File" header offset position
				FileHdr.m_UncompSize					// File Size
			);
		}
	}

	File.close();
    return (target.size() ? true : false);
    


}





//////////////////////////////////////////////////////////////////////
// "Less Than" Comparaison lt_path_str Member Functions
//////////////////////////////////////////////////////////////////////

bool ZipSupport::lt_path::operator () (const string & s1, const  string & s2) const
{
	const char * A = s1.c_str();
	const char * B = s2.c_str();

	for (size_t i = 0; ; ++i) {

		if ((A[i] == '\0') && (B[i] == '\0'))
			return false;

		// case insensitive and '/' is the same as '\'
		if (! (
				(A[i] == B[i] + ('a' - 'A'))		||
				(A[i] == B[i] - ('a' - 'A'))		||
				(A[i] == B[i])						||
				((A[i] == '\\') && (B[i] == '/'))	||
				((A[i] == '/') && (B[i] == '\\'))
			)) {

			if ((A[i] == '\0') || (A[i] < B[i]))
				return true;
			else
				return false;
		}
	}
}



//////////////////////////////////////////////////////////////////////
// Zip Header Classes Related Member Functions
//////////////////////////////////////////////////////////////////////


streamoff ZipSupport::CentralDir(istream & File)
{
	using io_facilities::readvar;

	// Look for the "end of central dir" header. Start minimum 22 bytes before end.
	if (! File.seekg(-22, ios::end)) return -1;

	streamoff EndPos;
	streamoff StartPos = File.tellg();

	if (StartPos == streamoff(0)) return -1;

	if (StartPos <= streamoff(65536))
		EndPos = 1;
	else
		EndPos = StartPos - streamoff(65536);

	// Start the scan
	do {
		unsigned int RawSignature;

		if (! readvar(File, RawSignature, 4)) return -1;

		eofcd_header Header;
		streampos Pos = File.tellg();

		// Found a potential "eofcd" header?
		if ((RawSignature == ENDOFDIR) && (File.seekg(-4, ios::cur)) && (Header.ReadHeader(File))) {

			// Check invariant values (1 disk only)
			if ((Header.m_NbDisks == 0) && (0 == Header.m_DirDisk) && (Header.m_LocalEntries == Header.m_TotalEntries)) {

				// Check comment ends at eof
				if (! File.seekg(-1, ios::end)) return -1;
				if ((File.tellg() + streamoff(1)) == (Pos + streamoff(Header.m_CommentSize + 22 - 4))) {

					// Check the start offset leads to a correct directory/file header;
					if (! File.seekg(Header.m_Offset)) return -1;
					if (! readvar(File, RawSignature, 4)) return -1;
					if (RawSignature == FILE)
						return Header.m_Offset;
				}
			}
		}

		File.seekg(Pos);

	} while ((File.seekg(-5, ios::cur)) && (File.tellg() > EndPos));

    return -1;
}



streamoff ZipSupport::SkipLFHdr(istream & File, streamoff LFHdrPos)
{
	using io_facilities::readvar;

	unsigned short NameSize;
	unsigned short FieldSize;
	unsigned int RawSignature;

	// verify it's a local header
	if (! File.seekg(LFHdrPos)) return -1;
	if (! readvar(File, RawSignature, 4)) return -1;
	if (RawSignature != LOCALFILE) return -1;

	// Skip and go directly to comment/field size
	if (! File.seekg(22, ios::cur)) return -1;
	if (! readvar(File, NameSize, 2)) return -1;
	if (! readvar(File, FieldSize, 2)) return -1;

	// Skip comment and extra field
	if (! File.seekg(NameSize + FieldSize, ios::cur)) return -1;

	// Now we are at the compressed data position
	return (File.tellg());
}



headerid ZipSupport::NextHeader(istream & File)
{
	using io_facilities::readvar;

	unsigned int RawSignature;

	if (! readvar(File, RawSignature, 4))
		return READERROR;

	if (! File.seekg(-4, ios::cur))
		return READERROR;

	headerid Signature = headerid(RawSignature);

	switch (Signature) {
	case FILE:
	case LOCALFILE:
	case ENDOFDIR:
		return Signature;
	default:
		return UNKNOWN;
	}
}




} // namespace zip_file_system
