#include "ObjInitFactory.h"
#include "utils/Decompressor.h"
#include "Companion.h"
#include <tinyxml2.h>

#define NUM(x, w) std::dec << std::setfill(' ') << std::setw(w) << x
#define FLOAT(x, w) std::dec << std::setfill(' ') << std::setw(w) << std::fixed << std::setprecision(1) << x << "f"

ExportResult SF64::ObjInitHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern ObjectInit " << symbol << "[];\n";
    return std::nullopt;
}

ExportResult SF64::ObjInitCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto symbol = GetSafeNode(node, "symbol", entryName);
    auto offset = GetSafeNode<uint32_t>(node, "offset");
    auto objs = std::static_pointer_cast<ObjInitData>(raw)->mObjInit;

    write << "ObjectInit " << symbol << "[] = {\n";
    for(auto& obj : objs) {
        auto enumName = Companion::Instance->GetEnumFromValue("ObjectId", obj.id).value_or(std::to_string(obj.id));
        if(obj.id >= 1000) {
            enumName = "ACTOR_EVENT_ID + " + std::to_string(obj.id - 1000);
        }
        write << fourSpaceTab << "{ ";
        write << FLOAT(obj.zPos1, 8) << ", ";
        write << NUM(obj.zPos2, 7) << ", ";
        write << NUM(obj.xPos, 7) << ", ";
        write << NUM(obj.yPos, 7) << ", ";
        write << NUM(obj.rot, 3) << ", ";
        write << enumName << " },\n";
    }
    write << "};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// count: " << std::to_string(objs.size()) << " ObjectInits\n";
    }

    return offset + sizeof(ObjectInit) * objs.size();
}

ExportResult SF64::ObjInitBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto writer = LUS::BinaryWriter();
    auto data = std::static_pointer_cast<ObjInitData>(raw)->mObjInit;

    WriteHeader(writer, Torch::ResourceType::ObjectInit, 0);
    auto count = data.size();
    writer.Write((uint32_t) count);
    for(size_t i = 0; i < data.size(); i++) {
        writer.Write(data[i].zPos1);
        writer.Write(data[i].zPos2);
        writer.Write(data[i].xPos);
        writer.Write(data[i].yPos);
        writer.Write(data[i].rot.x);
        writer.Write(data[i].rot.y);
        writer.Write(data[i].rot.z);
        writer.Write(data[i].id);
    }
    writer.Finish(write);
    return std::nullopt;
}

ExportResult SF64::ObjInitXMLExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto data = std::static_pointer_cast<ObjInitData>(raw)->mObjInit;

    tinyxml2::XMLPrinter printer;
    tinyxml2::XMLDocument objects;
    tinyxml2::XMLElement* root = objects.NewElement("ObjectList");

    *replacement += ".meta";

    for(auto & i : data) {
        tinyxml2::XMLElement* obj = root->InsertNewChildElement("ObjInit");
        auto enumName = Companion::Instance->GetEnumFromValue("ObjectId", i.id).value_or(std::to_string(i.id));

        obj->SetAttribute("ID", enumName.c_str());
        obj->SetAttribute("xPos", i.xPos);
        obj->SetAttribute("yPos", i.yPos);
        obj->SetAttribute("zPos1", i.zPos1);
        obj->SetAttribute("zPos2", i.zPos2);
        obj->SetAttribute("xRot", i.rot.x);
        obj->SetAttribute("yRot", i.rot.y);
        obj->SetAttribute("zRot", i.rot.z);
        root->InsertEndChild(obj);
    }

    objects.InsertEndChild(root);
    objects.Accept(&printer);
    write.write(printer.CStr(), printer.CStrSize() - 1);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> SF64::ObjInitFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    auto [_, segment] = Decompressor::AutoDecode(node, buffer);

    LUS::BinaryReader reader(segment.data, segment.size);
    reader.SetEndianness(Torch::Endianness::Big);
    std::vector<ObjectInit> objects;
    bool terminator = false;

    bool processing = true;
    while(processing) {
        float zPos1 = reader.ReadFloat();
        int16_t zPos2 = reader.ReadInt16();
        int16_t xPos = reader.ReadInt16();
        int16_t yPos = reader.ReadInt16();
        int16_t rotX = reader.ReadInt16();
        int16_t rotY = reader.ReadInt16();
        int16_t rotZ = reader.ReadInt16();
        int16_t id = reader.ReadInt16();
        reader.ReadInt16();

        if(id == -1) {
            terminator = true;
        }
        if(terminator && ((zPos1*zPos2*xPos*yPos*rotX*rotY*rotZ) != 0 || id != -1)) {
            processing = false;
        } else {
            objects.push_back({ zPos1, zPos2, xPos, yPos, {rotX, rotY, rotZ}, id});
        }
    }

    return std::make_shared<ObjInitData>(objects);
}