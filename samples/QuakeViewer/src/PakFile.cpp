#include "PakFile.h"
#include "AthenaEngine.h"
#include "StdStream.h"

CPakFile::CPakFile(const char* path)
: m_archiveStream(NULL)
, m_archiveReader(NULL)
{
	m_archiveStream = new Framework::CStdStream(path, "rb");
	m_archiveReader = new Framework::CZipArchiveReader(*m_archiveStream);
}

CPakFile::~CPakFile()
{
	if(m_archiveReader != NULL)
	{
		delete m_archiveReader;
		m_archiveReader = NULL;
	}

	if(m_archiveStream != NULL)
	{
		delete m_archiveStream;
		m_archiveStream = NULL;
	}
}

bool CPakFile::ReadFile(const char* filePath, uint8** dataPtr, uint32* dataSize)
{
	const Framework::Zip::ZIPDIRFILEHEADER* dirFileHeader = m_archiveReader->GetFileHeader(filePath);
	if(dirFileHeader == NULL)
	{
		return false;
	}
	(*dataPtr) = new uint8[dirFileHeader->uncompressedSize];
	(*dataSize) = dirFileHeader->uncompressedSize;
	Framework::CZipArchiveReader::StreamPtr stream = m_archiveReader->BeginReadFile(filePath);
	stream->Read((*dataPtr), dirFileHeader->uncompressedSize);
	return true;
}

bool CPakFile::TryCompleteFileName(const char* fileName, std::string& completeFileName) const
{
	for(auto fileHeaderIterator(std::begin(m_archiveReader->GetFileHeaders())); 
		fileHeaderIterator != std::end(m_archiveReader->GetFileHeaders()); fileHeaderIterator++)
	{
		const std::string headerFileName(fileHeaderIterator->first);
		if(strstr(headerFileName.c_str(), fileName))
		{
			completeFileName = headerFileName;
			return true;
		}
	}
	return false;
}

CPakFile::FileNameList CPakFile::GetFileNamesMatching(const char* pattern) const
{
	FileNameList result;
	for(auto fileHeaderIterator(std::begin(m_archiveReader->GetFileHeaders()));
		fileHeaderIterator != std::end(m_archiveReader->GetFileHeaders()); fileHeaderIterator++)
	{
		const std::string headerFileName(fileHeaderIterator->first);
		if(strstr(headerFileName.c_str(), pattern))
		{
			result.push_back(headerFileName);
		}
	}
	return result;
}
