// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once

typedef struct MEMORY_MAPPED_BUFFER
{
    wil::unique_handle FileMapping;
    unique_viewoffile Map1;
    unique_viewoffile Map2;
}MEMORY_MAPPED_BUFFER, *PMEMORY_MAPPED_BUFFER;

typedef struct MEMORY_MAPPED_DATA
{
    PBYTE BufferAddress{ nullptr };
    ULONG BufferSize{ 0 };
}MEMORY_MAPPED_DATA, * PMEMORY_MAPPED_DATA;

typedef struct MEMORY_MAPPED_REGISTERS
{
    PULONG WritePosition{ nullptr };
    PULONG ReadPosition{ nullptr };
} MEMORY_MAPPED_REGISTERS, * PMEMORY_MAPPED_REGISTERS;

typedef struct MEMORY_MAPPED_PIPE
{
    std::unique_ptr<MEMORY_MAPPED_BUFFER> DataBuffer;
    std::unique_ptr<MEMORY_MAPPED_BUFFER> RegistersBuffer;

    MEMORY_MAPPED_DATA Data;
    MEMORY_MAPPED_REGISTERS Registers;
    wil::unique_event_nothrow WriteEvent;
} MEMORY_MAPPED_PIPE, *PMEMORY_MAPPED_PIPE;

HRESULT GetRequiredBufferSize(_In_ ULONG&);

HRESULT MapBuffer(_In_ BOOL,
                    _In_ ULONG,
                    _In_ PMEMORY_MAPPED_BUFFER);
HRESULT CreateMappedBuffer(_In_ BOOL,
                            _In_ ULONG,
                            _In_ PMEMORY_MAPPED_BUFFER);
HRESULT CreateMappedDataBuffer(_In_ ULONG,
                                _In_ PMEMORY_MAPPED_BUFFER,
                                _In_ PMEMORY_MAPPED_DATA);
HRESULT CreateMappedRegisters(_In_ PMEMORY_MAPPED_BUFFER,
                                _In_ PMEMORY_MAPPED_REGISTERS);

HRESULT DisableMmcss(_In_ unique_mmcss_handle& MmcssHandle);
HRESULT EnableMmcss(_In_ unique_mmcss_handle& MmcssHandle,
                        _In_ DWORD& MmcssTaskId);

class CMidiXProc
{
public:
    CMidiXProc() {}
    ~CMidiXProc();

    HRESULT Initialize(_In_ DWORD*,
                          _In_ std::unique_ptr<MEMORY_MAPPED_PIPE>&,
                          _In_ std::unique_ptr<MEMORY_MAPPED_PIPE>&,
                          _In_opt_ IMidiCallback *);

    HRESULT Cleanup();

    HRESULT SendMidiMessage(
        _In_ void *,
        _In_ UINT32,
        _In_ LONGLONG);

private:

    wil::com_ptr_nothrow<IMidiCallback> m_MidiInCallback;

    std::unique_ptr<MEMORY_MAPPED_PIPE> m_MidiIn;
    std::unique_ptr<MEMORY_MAPPED_PIPE> m_MidiOut;

    static DWORD WINAPI MidiInWorker(_In_ LPVOID);

    HRESULT ProcessMidiIn();

    unique_mmcss_handle m_MmcssHandle;
    DWORD m_MmcssTaskId {0};

    wil::unique_event_nothrow m_ThreadTerminateEvent;
    wil::unique_event_nothrow m_ThreadStartedEvent;
    wil::unique_handle m_ThreadHandle;
};

