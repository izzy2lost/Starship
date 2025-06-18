#include "TrajectoryFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"
#include "utils/TorchUtils.h"
#include <regex>

ExportResult SM64::TrajectoryHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Trajectory " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SM64::TrajectoryCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    auto trajectoryData = std::static_pointer_cast<SM64::TrajectoryData>(raw)->mTrajectoryData;

    write << "const Trajectory " << symbol << "[] = {\n";

    for (auto &trajectory : trajectoryData) {
        write << fourSpaceTab;
        write << "TRAJECTORY_POS(";
        write << trajectory.trajId << ", " << trajectory.posX << ", " << trajectory.posY << ", " << trajectory.posZ << "),\n";
    }

    write << fourSpaceTab << "TRAJECTORY_END(),";

    write << "\n};\n";

    size_t size = (trajectoryData.size()) * sizeof(Trajectory) + 1;

    return offset + size;
}

ExportResult SM64::TrajectoryBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto trajectoryData = std::static_pointer_cast<SM64::TrajectoryData>(raw)->mTrajectoryData;

    WriteHeader(writer, Torch::ResourceType::Trajectory, 0);

    writer.Write((uint32_t)trajectoryData.size());

    for (auto &trajectory : trajectoryData) {
        writer.Write(trajectory.trajId);
        writer.Write(trajectory.posX);
        writer.Write(trajectory.posY);
        writer.Write(trajectory.posZ);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SM64::TrajectoryFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    std::vector<Trajectory> trajectoryData;
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);

    while (true) {
        auto trajId = reader.ReadInt16();
        if (trajId == -1) {
            break;
        }
        auto posX = reader.ReadInt16();
        auto posY = reader.ReadInt16();
        auto posZ = reader.ReadInt16();
        trajectoryData.emplace_back(trajId, posX, posY, posZ);
    }

    return std::make_shared<SM64::TrajectoryData>(trajectoryData);
}
