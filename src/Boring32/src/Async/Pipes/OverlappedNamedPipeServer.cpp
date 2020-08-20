#include "pch.hpp"
#include <stdexcept>
#include "include/Error/Win32Exception.hpp"
#include "include/Async/Pipes/OverlappedNamedPipeServer.hpp"

namespace Boring32::Async
{
    OverlappedNamedPipeServer::~OverlappedNamedPipeServer()
    {
        Close();
    }

    OverlappedNamedPipeServer::OverlappedNamedPipeServer(
        const std::wstring& pipeName,
        const DWORD size,
        const DWORD maxInstances,
        const std::wstring& sid,
        const bool isInheritable,
        const bool isLocalPipe
    )
    : NamedPipeServerBase(
        pipeName,
        size,
        maxInstances,
        sid,
        isInheritable,
        PIPE_ACCESS_DUPLEX 
            | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE           // message type pipe 
            | PIPE_READMODE_MESSAGE     // message-read mode
            | PIPE_WAIT
            | (isLocalPipe
                ? PIPE_REJECT_REMOTE_CLIENTS
                : PIPE_ACCEPT_REMOTE_CLIENTS)
    )
    {
        InternalCreatePipe();
    }

    OverlappedNamedPipeServer::OverlappedNamedPipeServer(
        const std::wstring& pipeName,
        const DWORD size,
        const DWORD maxInstances, // PIPE_UNLIMITED_INSTANCES
        const std::wstring& sid,
        const bool isInheritable,
        const DWORD openMode,
        const DWORD pipeMode
    )
    :   NamedPipeServerBase(
            pipeName,
            size,
            maxInstances,
            sid,
            isInheritable,
            openMode,
            pipeMode
        )
    {
        m_openMode |= FILE_FLAG_OVERLAPPED; // Ensure we're overlapped
        InternalCreatePipe();
    }

    OverlappedNamedPipeServer::OverlappedNamedPipeServer(const OverlappedNamedPipeServer& other)
    :   NamedPipeServerBase(other)
    {
        InternalCreatePipe();
    }

    void OverlappedNamedPipeServer::operator=(const OverlappedNamedPipeServer& other)
    {
        Copy(other);
        InternalCreatePipe();
    }

    OverlappedNamedPipeServer::OverlappedNamedPipeServer(OverlappedNamedPipeServer&& other) noexcept
    :   NamedPipeServerBase(other)
    {
        InternalCreatePipe();
    }

    void OverlappedNamedPipeServer::operator=(OverlappedNamedPipeServer&& other) noexcept
    {
        Move(other);
        InternalCreatePipe();
    }

    OverlappedOp OverlappedNamedPipeServer::Connect()
    {
        if (m_pipe == nullptr)
            throw std::runtime_error("No valid pipe handle to connect");
        OverlappedOp oio;
        oio.CallReturnValue = ConnectNamedPipe(m_pipe.GetHandle(), &oio.IoOverlapped);
        oio.LastErrorValue = GetLastError();
        if(oio.CallReturnValue == false && oio.LastErrorValue != ERROR_IO_PENDING)
            throw Error::Win32Exception("Failed to connect named pipe", oio.LastErrorValue);
        return oio;
    }

    OverlappedIo OverlappedNamedPipeServer::Write(const std::wstring& msg)
    {
        if (m_pipe == nullptr)
            throw std::runtime_error("No pipe to write to");

        OverlappedIo oio;
        oio.IoHandle = m_pipe;
        DWORD bytesWritten = 0;
        oio.CallReturnValue = WriteFile(
            m_pipe.GetHandle(),     // handle to pipe 
            &msg[0],                // buffer to write from 
            msg.size() * sizeof(wchar_t), // number of bytes to write 
            &bytesWritten,          // number of bytes written 
            &oio.IoOverlapped       // overlapped I/O
        );
        oio.LastErrorValue = GetLastError();
        if (oio.CallReturnValue == false && oio.LastErrorValue != ERROR_IO_PENDING)
            throw Error::Win32Exception("Failed to read pipe", oio.LastErrorValue);

        return oio;
    }

    OverlappedIo OverlappedNamedPipeServer::Read()
    {
        if (m_pipe == nullptr)
            throw std::runtime_error("No pipe to read from");

        constexpr DWORD blockSize = 1024;
        OverlappedIo oio;
        oio.IoBuffer.resize(blockSize);

        oio.IoHandle = m_pipe;
        DWORD totalBytesRead = 0;
        bool continueReading = true;
        while (continueReading)
        {
            DWORD currentBytesRead = 0;
            oio.CallReturnValue = ReadFile(
                m_pipe.GetHandle(),    // pipe handle 
                &oio.IoBuffer[0],    // buffer to receive reply 
                oio.IoBuffer.size() * sizeof(TCHAR),  // size of buffer 
                &currentBytesRead,  // number of bytes read 
                &oio.IoOverlapped);    // overlapped
            totalBytesRead += currentBytesRead;
            oio.LastErrorValue = GetLastError();
            if (oio.LastErrorValue == ERROR_MORE_DATA)
                oio.IoBuffer.resize(oio.IoBuffer.size() + blockSize);
            else if (oio.LastErrorValue != ERROR_IO_PENDING)
                throw Error::Win32Exception("Failed to read from pipe", oio.LastErrorValue);
        }

        if (totalBytesRead > 0)
            oio.IoBuffer.resize(totalBytesRead / sizeof(wchar_t));

        return oio;
    }
}