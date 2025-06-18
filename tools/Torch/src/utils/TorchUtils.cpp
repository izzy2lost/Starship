#include "TorchUtils.h"

#include <stack>
#include <Companion.h>
#include <factories/BaseFactory.h>
#include "spdlog/spdlog.h"

namespace fs = std::filesystem;

uint32_t Torch::translate(const uint32_t offset) {
    if(SEGMENT_NUMBER(offset) > 0x01) {
        auto segment = SEGMENT_NUMBER(offset);
        const auto addr = Companion::Instance->GetFileOffsetFromSegmentedAddr(segment);
        if(!addr.has_value()) {
            SPDLOG_ERROR("Segment data missing from game config\nPlease add an entry for segment {}", segment);
            throw std::runtime_error("Failed to find offset");
        }

        return addr.value() + SEGMENT_OFFSET(offset);
    }

    return offset;
}

int getFileDepth(const fs::path& base, const fs::path& p) {
    return std::distance(base.begin(), p.begin());
}

std::vector<fs::directory_entry> Torch::getRecursiveEntries(const fs::path baseDir) {
    std::set<fs::directory_entry> result;

    for (const auto& entry : fs::recursive_directory_iterator(baseDir)) {
        result.insert(entry);
    }

    std::vector<fs::directory_entry> sortedEntries(result.begin(), result.end());
    return sortedEntries;
}