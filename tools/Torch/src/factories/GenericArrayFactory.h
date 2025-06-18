#pragma once

#include "BaseFactory.h"
#include <vector>
#include <variant>
#include <types/Vec3D.h>

typedef std::variant<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, float, double, Vec2f, Vec3f, Vec3s, Vec3i, Vec3iu, Vec4f, Vec4s> ArrayDatum;

enum class ArrayType {
    u8, s8, u16, s16, u32, s32, u64, f32, f64, Vec2f, Vec3f, Vec3s, Vec3i, Vec3iu, Vec4f, Vec4s,
};

class GenericArray : public IParsedData {
public:
    uint32_t mMaxWidth;
    uint32_t mMaxPrec;
    std::vector<ArrayDatum> mData;

    explicit GenericArray(std::vector<ArrayDatum> data);
};

class ArrayCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ArrayHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class ArrayBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class GenericArrayFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Header, ArrayHeaderExporter)
            REGISTER(Binary, ArrayBinaryExporter)
            REGISTER(Code, ArrayCodeExporter)
        };
    }
};
