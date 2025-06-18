#include "SkeletonFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"

#include "archive/SWrapper.h"

#define NUM(x, w) std::dec << std::setfill(' ') << std::setw(w) << x
// #define NUM_JOINT(x) std::dec << std::setfill(' ') << std::setw(5) << x
#define FLOAT(x, w, p) std::dec << std::setfill(' ') << std::setw(w) << std::fixed << std::setprecision(p) << x

SF64::LimbData::LimbData(uint32_t addr, uint32_t dList, Vec3f trans, Vec3s rot, uint32_t sibling, uint32_t child, int index): mAddr(addr), mDList(dList), mTrans(trans), mRot(rot), mSibling(sibling), mChild(child), mIndex(index) {

}

ExportResult SF64::SkeletonHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto skeleton = std::static_pointer_cast<SF64::SkeletonData>(raw);
    auto limbCount = skeleton->mSkeleton.size();

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Limb* " << symbol << "[" << std::dec << limbCount + 1 << "];\n";
    return std::nullopt;
}

ExportResult SF64::SkeletonCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto skeleton = std::static_pointer_cast<SF64::SkeletonData>(raw);
    auto limbs = skeleton->mSkeleton;
    std::sort(limbs.begin(), limbs.end(), [](SF64::LimbData a, SF64::LimbData b) {return a.mAddr < b.mAddr;});
    std::unordered_map<uint32_t, std::string> limbDict;

    limbDict[0] = "NULL";
    for(SF64::LimbData limb : limbs) {
        std::ostringstream limbDefaultName;
        auto limbOffset = ASSET_PTR(limb.mAddr);
        limbDefaultName << symbol << "_limb_" << std::dec << limb.mIndex << "_" << std::uppercase << std::hex << limbOffset;
        limbDict[limb.mAddr] = limbDefaultName.str();
    }

    for(SF64::LimbData limb : limbs) {
        write << "Limb " << limbDict[limb.mAddr] << " = {\n";
        write << fourSpaceTab;
        if(limb.mDList == 0) {
            write << "NULL, ";
        } else {
            auto dec = Companion::Instance->GetNodeByAddr(limb.mDList);
            if(dec.has_value()){
                auto node = std::get<1>(dec.value());
                auto symbol = GetSafeNode<std::string>(node, "symbol");
                write << symbol << ", ";
            } else {
                write << "0x" << std::uppercase << std::hex << limb.mDList << ", ";
            }
        }
        write << FLOAT(limb.mTrans, 0, limb.mTrans.precision()) << ", ";
        write << std::dec << limb.mRot << ", ";
        write << ((limb.mSibling != 0) ? "&" : "") << limbDict[limb.mSibling] << ", ";
        write << ((limb.mChild != 0) ? "&" : "") << limbDict[limb.mChild] << ",\n";
        write << "};\n\n";
    }

    write << "Limb* " << symbol << "[] = {";
    for(int i = 0; i <= skeleton->mSkeleton.size(); i++) {
        if ((i % 4) == 0) {
            write << "\n" << fourSpaceTab;
        }
        if (i == skeleton->mSkeleton.size()) {
            write << "NULL, ";
        } else {
            write << "&" << limbDict[skeleton->mSkeleton[i].mAddr] << ", ";
        }
    }
    write << "\n};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// Limbs: " << std::dec << skeleton->mSkeleton.size() << "\n";
    }

    return OffsetEntry {
        limbs[0].mAddr,
        static_cast<uint32_t>(limbs[0].mAddr + skeleton->mSkeleton.size() * 0x24 + 4)
    };
}

ExportResult SF64::SkeletonBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto skeletonWriter = LUS::BinaryWriter();
    auto skeleton = std::static_pointer_cast<SF64::SkeletonData>(raw);
    auto limbs = skeleton->mSkeleton;
    std::unordered_map<uint32_t, uint64_t> limbDict;

    // Populate map of limbs with hashes
    for (auto &limb : limbs) {
        std::ostringstream limbDefaultName;
        limbDefaultName << entryName << "_limb_" << std::dec << limb.mIndex;
        limbDict[limb.mAddr] = CRC64(limbDefaultName.str().c_str());
    }

    // Export Each Limb
    for (auto &limb : limbs) {
        auto wrapper = Companion::Instance->GetCurrentWrapper();
        std::ostringstream stream;

        auto limbWriter = LUS::BinaryWriter();
        WriteHeader(limbWriter, Torch::ResourceType::Limb, 0);

        if(limb.mDList != 0){
            auto dec = Companion::Instance->GetNodeByAddr(limb.mDList);
            if (dec.has_value()){
                std::string path = std::get<0>(dec.value());
                limbWriter.Write(CRC64(path.c_str()));
                SPDLOG_INFO("Found display list: 0x{:X} at {} with size {}", limb.mDList, path, path.size());
            } else {
                SPDLOG_WARN("Could not find dlist at 0x{:X}", limb.mDList);
                throw std::runtime_error("Could not find dlist at 0x" + std::to_string(limb.mDList));
            }
        } else {
            limbWriter.Write((uint64_t) 0);
        }
        auto [transX, transY, transZ] = limb.mTrans;
        limbWriter.Write(transX);
        limbWriter.Write(transY);
        limbWriter.Write(transZ);
        auto [rotX, rotY, rotZ] = limb.mRot;
        limbWriter.Write(rotX);
        limbWriter.Write(rotY);
        limbWriter.Write(rotZ);
        uint64_t sibling = (limb.mSibling != 0) ? limbDict.at(limb.mSibling) : 0;
        limbWriter.Write(sibling);
        uint64_t child = (limb.mChild != 0) ? limbDict.at(limb.mChild) : 0;
        limbWriter.Write(child);
        limbWriter.Finish(stream);

        auto data = stream.str();
        wrapper->AddFile(entryName + "_limb_" + std::to_string(limb.mIndex), std::vector(data.begin(), data.end()));
    }

    // Export Skeleton
    WriteHeader(skeletonWriter, Torch::ResourceType::Skeleton, 0);
    skeletonWriter.Write((uint32_t) limbs.size());
    for (auto &limb : limbs) {
        skeletonWriter.Write(limbDict.at(limb.mAddr));
    }

    skeletonWriter.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SF64::SkeletonFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    std::vector<SF64::LimbData> skeleton;
    auto [root, segment] = Decompressor::AutoDecode(node, buffer, 0x1000);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);
    auto limbAddr = reader.ReadUInt32();
    auto limbIndex = 0;

    while(limbAddr != 0) {
        YAML::Node limbNode;
        Vec3f trans;
        Vec3s rot;
        limbNode["offset"] = limbAddr;
        DecompressedData limbDataRaw = Decompressor::AutoDecode(limbNode, buffer, 0x20);
        LUS::BinaryReader limbReader(limbDataRaw.segment.data, limbDataRaw.segment.size);
        limbReader.SetEndianness(Torch::Endianness::Big);

        auto dListAddr = limbReader.ReadUInt32();
        trans.x = limbReader.ReadFloat();
        trans.y = limbReader.ReadFloat();
        trans.z = limbReader.ReadFloat();
        rot.x = limbReader.ReadInt16();
        rot.y = limbReader.ReadInt16();
        rot.z = limbReader.ReadInt16();
        auto rw = limbReader.ReadInt16();
        auto siblingAddr = limbReader.ReadUInt32();
        auto childAddr = limbReader.ReadUInt32();

        if(dListAddr != 0 && (SEGMENT_NUMBER(dListAddr) == SEGMENT_NUMBER(limbAddr))) {
            YAML::Node dListNode;
            dListNode["type"] = "GFX";
            dListNode["offset"] = dListAddr;
            Companion::Instance->AddAsset(dListNode);
        }

        skeleton.push_back(LimbData(limbAddr, dListAddr, trans, rot, siblingAddr, childAddr, limbIndex));
        limbAddr = reader.ReadUInt32();
        limbIndex++;
    }

    return std::make_shared<SF64::SkeletonData>(skeleton);
}
