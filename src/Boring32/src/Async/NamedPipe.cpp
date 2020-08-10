#include "pch.hpp"
#include <stdexcept>
#include "include/Async/NamedPipe.hpp"

namespace Boring32::Async
{
	NamedPipe::~NamedPipe()
	{
        Close();
    }
    
    void NamedPipe::Close()
    {
        m_pipe.Close();
    }

	NamedPipe::NamedPipe(
        const std::wstring& pipeName, 
        const DWORD size,
        const DWORD maxInstances
    )
    :   m_pipeName(pipeName),
        m_size(size),
        m_maxInstances(maxInstances),
        m_isConnected(false)
	{
        m_pipe = CreateNamedPipeW(
            m_pipeName.c_str(),             // pipe name 
            PIPE_ACCESS_DUPLEX,             // read/write access 
            PIPE_TYPE_MESSAGE |             // message type pipe 
                PIPE_READMODE_MESSAGE |     // message-read mode 
                PIPE_WAIT,                  // blocking mode 
            m_maxInstances,                 // max. instances  
            m_size,                         // output buffer size 
            m_size,                         // input buffer size 
            0,                              // client time-out 
            nullptr);
        if (m_pipe == nullptr)
            throw std::runtime_error("Failed to create named pipe");
	}

    NamedPipe::NamedPipe(const NamedPipe& other)
    :   m_size(0),
        m_isConnected(false)
    {
        Copy(other);
    }

    void NamedPipe::operator=(const NamedPipe& other)
    {
        Copy(other);
    }

    void NamedPipe::Copy(const NamedPipe& other)
    {
        Close();
        m_pipe = other.m_pipe;
        m_pipeName = other.m_pipeName;
        m_size = other.m_size;
        m_maxInstances = other.m_maxInstances;
        m_isConnected = other.m_isConnected;
    }

    NamedPipe::NamedPipe(NamedPipe&& other) noexcept
    :   m_size(0)
    {
        Move(other);
    }

    void NamedPipe::operator=(NamedPipe&& other) noexcept
    {
        Move(other);
    }

    void NamedPipe::Move(NamedPipe& other) noexcept
    {
        Close();
        m_pipeName = std::move(other.m_pipeName);
        m_size = other.m_size;
        m_maxInstances = other.m_maxInstances;
        m_isConnected = other.m_isConnected;
        if (other.m_pipe != nullptr)
            m_pipe = std::move(other.m_pipe);
    }

    void NamedPipe::Connect()
    {
        if (m_pipe == nullptr)
            throw std::runtime_error("No valid pipe handle to connect");

        if (ConnectNamedPipe(m_pipe.GetHandle(), nullptr) == false)
            throw std::runtime_error("Failed to connect named pipe");
    }

    void NamedPipe::Disconnect()
    {
        if (m_pipe == nullptr)
            throw std::runtime_error("No valid pipe handle to disconnect");
        if (m_isConnected)
        {
            DisconnectNamedPipe(m_pipe.GetHandle());
            m_isConnected = false;
        }
    }

    void NamedPipe::Write(const std::wstring& msg)
    {
        if (m_pipe == nullptr)
            throw std::runtime_error("No pipe to write to");

        DWORD bytesWritten = 0;
        bool success = WriteFile(
            m_pipe.GetHandle(),     // handle to pipe 
            &msg[0],                // buffer to write from 
            msg.size() * sizeof(wchar_t), // number of bytes to write 
            &bytesWritten,          // number of bytes written 
            nullptr                 // not overlapped I/O
        );               
        if (success == false)
            throw std::runtime_error("Failed to read pipe");
    }

    std::wstring NamedPipe::Read()
    {
        if (m_pipe == nullptr)
            throw std::runtime_error("No pipe to read from");

        std::wstring data;
        data.resize(m_size * sizeof(wchar_t));
        DWORD bytesRead = 0;
        bool success = ReadFile(
            m_pipe.GetHandle(),             // handle to pipe 
            &data[0],                       // buffer to receive data 
            data.size() * sizeof(wchar_t),    // size of buffer 
            &bytesRead,                     // number of bytes read 
            nullptr                         // not overlapped I/O
        );
        if (success == false)
            throw std::runtime_error("Failed to read pipe");

        return data;
    }

    Raii::Win32Handle& NamedPipe::GetInternalHandle()
    {
        return m_pipe;
    }

    std::wstring NamedPipe::GetName() const
    {
        return m_pipeName;
    }

    DWORD NamedPipe::GetSize() const
    {
        return m_size;
    }

    DWORD NamedPipe::GetMaxInstances() const
    {
        return m_maxInstances;
    }

    bool NamedPipe::IsConnected() const
    {
        return m_isConnected;
    }
}