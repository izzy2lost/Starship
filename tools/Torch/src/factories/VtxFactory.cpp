#include "VtxFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"

#define NUM(x) std::dec << std::setfill(' ') << std::setw(6) << x
#define COL(c) std::dec << std::setfill(' ') << std::setw(3) << c

ExportResult VtxHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto vtx = std::static_pointer_cast<VtxData>(raw)->mVtxs;
    const auto offset = GetSafeNode<uint32_t>(node, "offset");

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    const auto searchTable = Companion::Instance->SearchTable(offset);

    if(searchTable.has_value()){
        const auto [name, start, end, mode, index_size] = searchTable.value();
        // We will ignore the overriden index_size for now...

        if(start != offset){
            return std::nullopt;
        }

        write << "extern Vtx " << name << "[][" << vtx.size() << "];\n";
    } else {
        write << "extern Vtx " << symbol << "[];\n";
    }

    return std::nullopt;
}

ExportResult VtxCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto vtx = std::static_pointer_cast<VtxData>(raw)->mVtxs;
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    const auto searchTable = Companion::Instance->SearchTable(offset);

    if(searchTable.has_value()){
        const auto [name, start, end, mode, index_size] = searchTable.value();


        if(start == offset){
            write << "Vtx " << name << "[][" << vtx.size() << "] = {\n";
        }

        write << fourSpaceTab << "{";

        for (int i = 0; i < vtx.size(); ++i) {
            auto v = vtx[i];

            auto x = v.ob[0];
            auto y = v.ob[1];
            auto z = v.ob[2];

            auto flag = v.flag;

            auto tc1 = v.tc[0];
            auto tc2 = v.tc[1];

            auto c1 = (uint16_t) v.cn[0];
            auto c2 = (uint16_t) v.cn[1];
            auto c3 = (uint16_t) v.cn[2];
            auto c4 = (uint16_t) v.cn[3];

            if(i <= vtx.size() - 1) {
                write << "\n" << fourSpaceTab;
            }

            // {{{ x, y, z }, f, { tc1, tc2 }, { c1, c2, c3, c4 }}}
            write << fourSpaceTab << "{{{" << NUM(x) << ", " << NUM(y) << ", " << NUM(z) << "}, " << flag << ", {" << NUM(tc1) << ", " << NUM(tc2) << "}, {" << COL(c1) << ", " << COL(c2) << ", " << COL(c3) << ", " << COL(c4) << "}}},";
        }
        write << "\n" << fourSpaceTab << "},\n";

        if(end == offset){
            write << "};\n\n";
        }
    } else {

        write << "Vtx " << symbol << "[] = {\n";

        for (int i = 0; i < vtx.size(); ++i) {
            auto v = vtx[i];

            auto x = v.ob[0];
            auto y = v.ob[1];
            auto z = v.ob[2];

            auto flag = v.flag;

            auto tc1 = v.tc[0];
            auto tc2 = v.tc[1];

            auto c1 = (uint16_t) v.cn[0];
            auto c2 = (uint16_t) v.cn[1];
            auto c3 = (uint16_t) v.cn[2];
            auto c4 = (uint16_t) v.cn[3];

            if(i <= vtx.size() - 1) {
                write << fourSpaceTab;
            }

            // {{{ x, y, z }, f, { tc1, tc2 }, { c1, c2, c3, c4 }}}
            write << "{{{" << NUM(x) << ", " << NUM(y) << ", " << NUM(z) << "}, " << flag << ", {" << NUM(tc1) << ", " << NUM(tc2) << "}, {" << COL(c1) << ", " << COL(c2) << ", " << COL(c3) << ", " << COL(c4) << "}}},\n";
        }

        write << "};\n";

        if (Companion::Instance->IsDebug()) {
            write << "// count: " << std::to_string(vtx.size()) << " Vtxs\n";
        } else {
            write << "\n";
        }
    }

    return offset + vtx.size() * sizeof(VtxRaw);
}

ExportResult VtxBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto vtx = std::static_pointer_cast<VtxData>(raw);
    auto writer = LUS::BinaryWriter();

    WriteHeader(writer, Torch::ResourceType::Vertex, 0);
    writer.Write((uint32_t) vtx->mVtxs.size());
    for(auto v : vtx->mVtxs) {
        if(Companion::Instance->GetConfig().gbi.useFloats){
            writer.Write((float) v.ob[0]);
            writer.Write((float) v.ob[1]);
            writer.Write((float) v.ob[2]);
        } else {
            writer.Write((int16_t) v.ob[0]);
            writer.Write((int16_t) v.ob[1]);
            writer.Write((int16_t) v.ob[2]);
        }
        writer.Write(v.flag);
        writer.Write(v.tc[0]);
        writer.Write(v.tc[1]);
        writer.Write(v.cn[0]);
        writer.Write(v.cn[1]);
        writer.Write(v.cn[2]);
        writer.Write(v.cn[3]);
    }

    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> VtxFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto count = GetSafeNode<size_t>(node, "count");

    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, count * sizeof(VtxRaw));

    reader.SetEndianness(Torch::Endianness::Big);
    std::vector<VtxRaw> vertices;

    for(size_t i = 0; i < count; i++) {
        auto x = reader.ReadInt16();
        auto y = reader.ReadInt16();
        auto z = reader.ReadInt16();
        auto flag = reader.ReadUInt16();
        auto tc1 = reader.ReadInt16();
        auto tc2 = reader.ReadInt16();
        auto cn1 = reader.ReadUByte();
        auto cn2 = reader.ReadUByte();
        auto cn3 = reader.ReadUByte();
        auto cn4 = reader.ReadUByte();

        vertices.push_back(VtxRaw({
           {x, y, z}, flag, {tc1, tc2}, {cn1, cn2, cn3, cn4}
       }));
    }

    return std::make_shared<VtxData>(vertices);
}
