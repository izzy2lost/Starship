#include "AssetArrayFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>

#define FORMAT_HEX(ptr) (ptr)

ExportResult AssetArrayHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto data = std::static_pointer_cast<AssetArrayData>(raw);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern " << data->mType << "* " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult AssetArrayCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    auto data = std::static_pointer_cast<AssetArrayData>(raw);

    write << "static " << data->mType << "* " << symbol << "[] = {\n";
    for (auto ptr : data->mPtrs) {
        write << fourSpaceTab;
        if (ptr == 0) {
            write << "NULL,\n";
        } else {
            auto dec = Companion::Instance->GetNodeByAddr(ptr);
            if (dec.has_value()) {
                auto node = std::get<1>(dec.value());
                auto assetSymbol = GetSafeNode<std::string>(node, "symbol");
                write << "&" << assetSymbol << ",\n";
            } else {
                write << FORMAT_HEX(ptr) << ",\n";
            }
        }
    }
    write << "};\n";

    size_t size = (data->mPtrs.size()) * sizeof(uint32_t);

    return offset + size;
}

ExportResult AssetArrayBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<AssetArrayData>(raw);

    WriteHeader(writer, Torch::ResourceType::AssetArray, 0);

    writer.Write((uint32_t)data->mPtrs.size());

    for (auto ptr : data->mPtrs) {
        if (ptr == 0) {
            writer.Write((uint64_t)0);
            continue;
        }

        auto dec = Companion::Instance->GetNodeByAddr(ptr);
        if (dec.has_value()) {
            uint64_t hash = CRC64(std::get<0>(dec.value()).c_str());
            SPDLOG_INFO("Found Asset: 0x{:X} Hash: 0x{:X} Path: {}", ptr, hash, std::get<0>(dec.value()));
            writer.Write(hash);
        } else {
            SPDLOG_WARN("Could not find Asset at 0x{:X}", ptr);
        }
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> AssetArrayFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    std::vector<uint32_t> ptrs;
    auto assetType = GetSafeNode<std::string>(node, "assetType");
    auto factoryType = GetSafeNode<std::string>(node, "factoryType");
    const auto count = GetSafeNode<uint32_t>(node, "count");
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    for (uint32_t i = 0; i < count; ++i) {
        auto ptr = reader.ReadUInt32();

        if (ptr != 0) {
            YAML::Node assetNode;
            assetNode["type"] = factoryType;
            assetNode["offset"] = ptr;
            Companion::Instance->AddAsset(assetNode);
        }

        ptrs.emplace_back(ptr);
    }

    return std::make_shared<AssetArrayData>(ptrs, assetType);
}
