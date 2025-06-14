
// Archive.android_stub.cpp -- Android no-op stub for Archive class
#include "Archive.h"

namespace LUS {

Archive::Archive(const std::string&, bool) {}
Archive::Archive(const std::string&, const std::string&, const std::unordered_set<uint32_t>&, bool, bool) {}
Archive::Archive(const std::vector<std::string>&, const std::unordered_set<uint32_t>&, bool, bool) {}
Archive::~Archive() {}

std::shared_ptr<Archive> Archive::CreateArchive(const std::string&, size_t) { return nullptr; }

bool Archive::IsMainMPQValid() { return false; }
std::shared_ptr<File> Archive::LoadFile(const std::string&, bool) { return nullptr; }
bool Archive::AddFile(const std::string&, uintptr_t, unsigned int) { return false; }
bool Archive::RemoveFile(const std::string&) { return false; }
bool Archive::RenameFile(const std::string&, const std::string&) { return false; }
std::shared_ptr<std::vector<std::string>> Archive::ListFiles(const std::string&) { return nullptr; }
bool Archive::HasFile(const std::string&) { return false; }
const std::string* Archive::HashToString(uint64_t) const { return nullptr; }
std::vector<uint32_t> Archive::GetGameVersions() { return {}; }
void Archive::PushGameVersion(uint32_t) {}

std::shared_ptr<std::vector<void*>> Archive::FindFiles(const std::string&) { return nullptr; }
bool Archive::Load(bool, bool) { return false; }
bool Archive::Unload() { return false; }
bool Archive::LoadMainMPQ(bool, bool) { return false; }
bool Archive::LoadPatchMPQs() { return false; }
bool Archive::LoadPatchMPQ(const std::string&, bool) { return false; }
void Archive::GenerateCrcMap() {}
bool Archive::ProcessOtrVersion(int) { return false; }
std::shared_ptr<File> Archive::LoadFileFromHandle(const std::string&, bool, int) { return nullptr; }

} // namespace LUS
