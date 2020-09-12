#include "pch.hpp"
#include "include/Error/Win32Error.hpp"
#include "include/Compression/Compressor.hpp"

namespace Boring32::Compression
{
	void Compressor::Close()
	{
		if (m_compressor != nullptr)
		{
			CloseCompressor(m_compressor);
			m_type = CompressionType::NotSet;
			m_compressor = nullptr;
		}
	}

	Compressor::~Compressor()
	{
		Close();
	}

	Compressor::Compressor(const CompressionType type)
	:	m_type(type),
		m_compressor(nullptr)
	{
		Create();
	}

	Compressor::Compressor(const Compressor& other)
	:	m_type(CompressionType::NotSet),
		m_compressor(nullptr)
	{
		Copy(other);
	}

	Compressor& Compressor::operator=(const Compressor other)
	{
		Copy(other);
		return *this;
	}

	void Compressor::Copy(const Compressor& other)
	{
		Close();
		m_type = other.m_type;
		Create();
	}

	Compressor::Compressor(Compressor&& other) noexcept
	:	m_type(CompressionType::NotSet),
		m_compressor(nullptr)
	{
		Move(other);
	}

	Compressor& Compressor::operator=(Compressor&& other) noexcept
	{
		Move(other);
		return *this;
	}

	void Compressor::Move(Compressor& other) noexcept
	{
		try
		{
			Close();
			m_type = other.m_type;
			m_compressor = other.m_compressor;
			other.m_compressor = nullptr;
		}
		catch (const std::exception& ex)
		{
			std::wcerr << ex.what() << std::endl;
		}
	}

	size_t Compressor::GetCompressedSize(std::vector<std::byte>& buffer)
	{
		if (m_compressor == nullptr)
			throw std::runtime_error("Compressor::GetCompressedSize(): compressor handle is null");
		if (buffer.size() == 0)
			throw std::runtime_error("Compressor::GetCompressedSize(): buffer is empty");

		size_t compressedBufferSize = 0;
		// https://docs.microsoft.com/en-us/windows/win32/api/compressapi/nf-compressapi-compress
		bool succeeded = Compress(
			m_compressor,               //  Compressor Handle
			&buffer[0],             //  Input buffer, Uncompressed data
			buffer.size(),          //  Uncompressed data size
			nullptr,                //  Compressed Buffer
			0,                      //  Compressed Buffer size
			&compressedBufferSize);	//  Compressed Data size
		if (succeeded == false)
			throw Error::Win32Error("Compressor::GetCompressedSize(): Compress() failed", GetLastError());
		return compressedBufferSize;
	}
	
	CompressionType Compressor::GetType()
	{
		return m_type;
	}

	void Compressor::Create()
	{
		if (m_compressor == nullptr)
			throw std::runtime_error("Compressor::Create(): compressor handle is null");

		// https://docs.microsoft.com/en-us/windows/win32/api/compressapi/nf-compressapi-createcompressor
		bool succeeded = CreateCompressor(
			(DWORD)m_type,	// Algorithm
			nullptr,		// AllocationRoutines
			&m_compressor		// CompressorHandle
		);
		if (succeeded == false)
			throw Error::Win32Error("Compressor::Create(): CreateCompressor() failed", GetLastError());
	}
}