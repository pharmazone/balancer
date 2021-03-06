#pragma once

#include "flock.h"
#include "fstat.h"
#include "defaults.h"

#include <cstdio>

#include <util/generic/ptr.h>
#include <util/generic/stroka.h>
#include <util/generic/noncopyable.h>

#if defined(_win_)
    typedef void* FHANDLE;
    #define INVALID_FHANDLE ((FHANDLE)(long)-1)
#elif defined(_unix_)
    typedef int FHANDLE;
    #define INVALID_FHANDLE -1
#else
#   error
#endif

enum OpenMode {
    OpenExisting  = 0, // Opens a file. It fails if the file does not exist.
    TruncExisting = 1, // Opens a file and truncates it to zero size. It fails if the file does not exist.
    OpenAlways    = 2, // Opens a file, always. If a file does not exist, it creates a file.
    CreateNew     = 3, // Creates a new file. It fails if a specified file exists.
    CreateAlways  = 4, // Creates a new file, always. If a file exists, it overwrites the file.
    MaskCreation  = 7,

    RdOnly = 8,  // open for reading only
    WrOnly = 16, // open for writing only
    RdWr   = 24, // open for reading and writing
    MaskRW = 24,

    Seq      = 0x20, // file access is primarily sequential
    Direct   = 0x40, // file is being opened with no system caching
    Temp     = 0x80, // avoid writing data back to disk if sufficient cache memory is available
    MaskMisc = 0xE0,

    ForAppend = 256,
    //actually, temporary file - 'delete on close' for windows, unlink after creation for unix
    Transient = 512,

    AXOther = 0x00010000,
    AWOther = 0x00020000,
    AROther = 0x00040000,
    AXGroup = 0x00100000,
    AWGroup = 0x00200000,
    ARGroup = 0x00400000,
    AXUser  = 0x01000000,
    AWUser  = 0x02000000,
    ARUser  = 0x04000000,
    AX = AXUser | AXGroup | AXOther,
    AW = AWUser | AWGroup,
    AR = ARUser | ARGroup | AROther,
    ARW = AR | AW,
    AMask = 0x0FFF0000,
};

Stroka DecodeOpenMode(ui32 openMode);

enum SeekDir {
    sSet = 0,
    sCur = 1,
    sEnd = 2,
};

class TFileHandle: public TNonCopyable {
public:
    /// Warning: takes ownership of fd, so closes it in destructor.
    inline TFileHandle(FHANDLE fd) throw ()
        : Fd_(fd)
    {
    }

    TFileHandle(const Stroka& fName, ui32 oMode) throw();

    inline ~TFileHandle() throw () {
        Close();
    }

    bool Close() throw ();

    inline FHANDLE Release() throw () {
        FHANDLE ret = Fd_;
        Fd_ = INVALID_FHANDLE;
        return ret;
    }

    inline void Swap(TFileHandle& r) throw () {
        DoSwap(Fd_, r.Fd_);
    }

    inline operator FHANDLE() const throw () {
        return Fd_;
    }

    inline bool IsOpen() const throw () {
        return Fd_ != INVALID_FHANDLE;
    }

    i64 GetPosition() const throw ();
    i64 GetLength() const throw ();

    i64 Seek(i64 offset, SeekDir origin) throw ();
    bool Resize(i64 length) throw ();
    bool Reserve(i64 length) throw ();
    bool Flush() throw ();
    i32 Read(void* buffer, ui32 byteCount) throw ();
    i32 Write(const void* buffer, ui32 byteCount) throw ();
    i32 Pread(void* buffer, ui32 byteCount, i64 offset) const throw ();
    i32 Pwrite(const void* buffer, ui32 byteCount, i64 offset) const throw ();
    int Flock(int op) throw ();

    FHANDLE Duplicate() const throw();

    //dup2 - like semantics, return true on success
    bool LinkTo(const TFileHandle& fh) const throw ();

private:
    FHANDLE Fd_;
};

class TFile {
public:
    TFile();
    /// Takes ownership of handle, so closes it when the last holder of descriptor dies.
    explicit TFile(FHANDLE fd);
    TFile(const Stroka& fName, ui32 oMode);
    ~TFile() throw ();

    void Close();

    const Stroka& GetName() const throw ();
    i64 GetPosition() const throw ();
    i64 GetLength() const throw ();
    bool IsOpen() const throw ();
    FHANDLE GetHandle() const throw ();

    i64 Seek(i64 offset, SeekDir origin);
    void Resize(i64 length);
    void Reserve(i64 length);
    void Flush();

    void LinkTo(const TFile& f) const;
    TFile Duplicate() const;
    TFileStat GetStat() const;

    size_t Read(void* buf, size_t len);
    void Load(void* buf, size_t len);
    void Write(const void* buf, size_t len);
    size_t Pread(void* buf, size_t len, i64 offset) const;
    void Pload(void* buf, size_t len, i64 offset) const;
    void Pwrite(const void* buf, size_t len, i64 offset) const;
    void Flock(int op);

    static TFile Temporary(const Stroka& prefix);

private:
    class TImpl;
    TSimpleIntrusivePtr<TImpl> Impl_;
};

TFile Duplicate(FILE*);
TFile Duplicate(int);
