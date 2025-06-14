#pragma once

#undef _DLL

#include <string>
#include <stdint.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <mutex>
#include "Resource.h"

namespace LUS {
struct File;

#if defined(__ANDROID__)
// ========== ANDROID DUMMY STUB CLASS ==========

class Archive : public std::enable_shared_from_this<Archive> {
public:
    Archive(const std::string&, bool) {}
    Archive(const std::string&, const std::string&, const std::unordered_set<uint32_t>&, bool, bool = true) {}
    Archive(const std::vector<std::string>&, const std::unordered_set<uint32_t>&, bool, bool = true) {}
    ~Archive() {}

    static std::shared_ptr<Archive> CreateArchive(const std::string&, size_t) { return nullptr; }

    bool IsMainMPQValid() { return false; }
    std::shared_ptr<File> LoadFile(const std::string&, bool = true) { return nullptr; }
    bool AddFile(const std::string&, uintptr_t, int) { return false; }
    bool RemoveFile(const std::string&) { return false; }
    bool RenameFile(const std::string&, const std::string&) { return false; }
    std::shared_ptr<std::vector<std::string>> ListFiles(const std::string&) { return nullptr; }
    bool HasFile(const std::string&) { return false; }
    const std::string* HashToString(uint64_t) const { return nullptr; }
    std::vector<uint32_t> GetGameVersions() { return {}; }
    void PushGameVersion(uint32_t) {}

protected:
    // No protected functions for Android
    // (if you ever need, add empty stubs here)

private:
    // Only add private members if actually needed for build
};

#else
// ========== DESKTOP/STORMLIB VERSION ==========

#include <StormLib.h>

class Archive : public std::enable_shared_from_this<Archive> {
public:
    Archive(const std::string& mainPath, bool enableWriting);
    Archive(const std::string& mainPath, const std::string& patchesPath,
            const std::unordered_set<uint32_t>& validHashes, bool enableWriting, bool generateCrcMap = true);
    Archive(const std::vector<std::string>& fileList, const std::unordered_set<uint32_t>& validHashes,
            bool enableWriting, bool generateCrcMap = true);
    ~Archive();

    static std::shared_ptr<Archive> CreateArchive(const std::string& archivePath, size_t fileCapacity);

    bool IsMainMPQValid();
    std::shared_ptr<File> LoadFile(const std::string& filePath, bool includeParent = true);
    bool AddFile(const std::string& filePath, uintptr_t fileData, DWORD fileSize);
    bool RemoveFile(const std::string& filePath);
    bool RenameFile(const std::string& oldFilePath, const std::string& newFilePath);
    std::shared_ptr<std::vector<std::string>> ListFiles(const std::string& fileSearchMask);
    bool HasFile(const std::string& fileSearchMask);
    const std::string* HashToString(uint64_t hash) const;
    std::vector<uint32_t> GetGameVersions();
    void PushGameVersion(uint32_t newGameVersion);

protected:
    std::shared_ptr<std::vector<SFILE_FIND_DATA>> FindFiles(const std::string& fileSearchMask);
    bool Load(bool enableWriting, bool generateCrcMap);
    bool Unload();

private:
    std::string mMainPath;
    std::string mPatchesPath;
    std::vector<std::string> mOtrArchives;
    std::unordered_set<uint32_t> mValidHashes;
    std::map<std::string, HANDLE> mMpqHandles;
    std::vector<std::string> mAddedFiles;
    std::vector<uint32_t> mGameVersions;
    std::unordered_map<uint64_t, std::string> mHashes;
    HANDLE mMainMpq;
    std::mutex mMutex;

    bool LoadMainMPQ(bool enableWriting, bool generateCrcMap);
    bool LoadPatchMPQs();
    bool LoadPatchMPQ(const std::string& otrPath, bool validateVersion = false);
    void GenerateCrcMap();
    bool ProcessOtrVersion(HANDLE mpqHandle = nullptr);
    std::shared_ptr<File> LoadFileFromHandle(const std::string& filePath, bool includeParent = true,
                                             HANDLE mpqHandle = nullptr);
};

#endif // __ANDROID__
} // namespace LUS
