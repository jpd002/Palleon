#ifndef _PAKFILERESOURCEPROVIDER_H_
#define _PAKFILERESOURCEPROVIDER_H_

#include "Types.h"
#include "Stream.h"
#include "zip/ZipArchiveReader.h"

class CPakFile
{
public:
	typedef Framework::CZipArchiveReader::FileNameList FileNameList;

										CPakFile(const char*);
	virtual								~CPakFile();

	bool								ReadFile(const char*, uint8**, uint32*);
	bool								TryCompleteFileName(const char*, std::string&) const;
	FileNameList						GetFileNamesMatching(const char*) const;

private:
	Framework::CStream*					m_archiveStream;
	Framework::CZipArchiveReader*		m_archiveReader;
};

#endif
