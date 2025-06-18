#include "MtxFactory.h"
#include "spdlog/spdlog.h"

#include "Companion.h"
#include "utils/Decompressor.h"

#define NUM(x) std::dec << std::setfill(' ') << std::setw(6) << x
#define COL(c) std::dec << std::setfill(' ') << std::setw(3) << c

ExportResult MtxHeaderExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement) {
    const auto symbol = GetSafeNode(node, "symbol", entryName);

    if(Companion::Instance->IsOTRMode()){
        write << "static const ALIGN_ASSET(2) char " << symbol << "[] = \"__OTR__" << (*replacement) << "\";\n\n";
        return std::nullopt;
    }

    write << "extern Mtx " << symbol << ";\n";
    return std::nullopt;
}

ExportResult MtxCodeExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto m = std::static_pointer_cast<MtxData>(raw)->mMtxs;
    const auto symbol = GetSafeNode(node, "symbol", entryName);
    auto offset = GetSafeNode<uint32_t>(node, "offset");

    if (IS_SEGMENTED(offset)) {
        offset = SEGMENT_OFFSET(offset);
    }

    if (Companion::Instance->IsDebug()) {
        if (IS_SEGMENTED(offset)) {
            offset = SEGMENT_OFFSET(offset);
        }
        write << "// 0x" << std::hex << std::uppercase << offset << "\n";
    }

    #define fiveFourSpaceTabs fourSpaceTab << fourSpaceTab << fourSpaceTab << fourSpaceTab << fourSpaceTab << "   "

    /**
     * toFixedPointMatrix(1.0, 0.0, 0.0, 0.0,
     *                    0.0, 1.0, 0.0, 0.0,
     *                    0.0, 0.0, 1.0, 0.0,
     *                    0.0, 0.0, 0.0, 1.0);
     */

    write << "Mtx " << symbol << " = {\n";

    for (int i = 0; i < m.size(); ++i) {

        write << fourSpaceTab << "toFixedPointMatrix(";

        for (int j = 0; j < 16; ++j) {

            // Turn 1, 3, and 6 into 1.0, 3.0, and 6.0. Unless it has a decimal number then leave it alone.
                SPDLOG_INFO(m[i].mtx[j]);
            if (std::abs(m[i].mtx[j] - static_cast<int>(m[i].mtx[j])) < 1e-6) {
                write << std::fixed << std::setprecision(1) << m[i].mtx[j];
            } else {
                // Stupid hack to get matching precision so this value outputs 0.0000153 instead.
                if (std::fabs(m[i].mtx[j] - 0.000015) < 0.000001) {
                    write << std::fixed << std::setprecision(7) << m[i].mtx[j];
                } else {
                    write << std::fixed << std::setprecision(6) << m[i].mtx[j];
                }
            }

            // Add comma for all but the last arg
            if (j < 15) {
                write << ", ";
            }

            // Add closing bracket for last arg in matrix
            if (j == 15) {
                write << "),\n";

                // Do not add an extra \n on the last iteration.
                if (i < m.size() - 1) {
                    write << "\n";
                }
                break;
            }

            // Add lots of spaces for start of a new line
            if ((j + 1) % 4 == 0) {
                write << "\n" << fiveFourSpaceTabs;
            }
        }
    }

    write << "};\n";

    if (Companion::Instance->IsDebug()) {
        write << "// count: " << std::to_string(m.size()) << " Mtxs\n";
        write << "// 0x" << std::hex << std::uppercase << (offset + (sizeof(MtxRaw) * m.size())) << "\n";
    }

    write << "\n";

    #undef fiveFourSpaceTabs

    return offset + sizeof(MtxRaw);
}

ExportResult MtxBinaryExporter::Export(std::ostream &write, std::shared_ptr<IParsedData> raw, std::string& entryName, YAML::Node &node, std::string* replacement ) {
    auto mtx = std::static_pointer_cast<MtxData>(raw);
    auto writer = LUS::BinaryWriter();
    auto floats = Companion::Instance->GetConfig().gbi.useFloats;

    WriteHeader(writer, Torch::ResourceType::Matrix, 0);

    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){
            if(floats){
                writer.Write(mtx->mMtxs[0].mtx[i * 4 + j]);
            } else {
                writer.Write(mtx->mMtxs[0].mt.mint[i][j]);
            }
        }
    }
    writer.Finish(write);
    return std::nullopt;
}

std::optional<std::shared_ptr<IParsedData>> MtxFactory::parse(std::vector<uint8_t>& buffer, YAML::Node& node) {
    //auto count = GetSafeNode<size_t>(node, "count");

    auto [_, segment] = Decompressor::AutoDecode(node, buffer);
    LUS::BinaryReader reader(segment.data, 1 * sizeof(MtxRaw));

    reader.SetEndianness(Torch::Endianness::Big);
    std::vector<MtxRaw> matrix;

    #define FIXTOF(x)      ((float)((x) / 65536.0f))

    // Reads the inteer portion, the fractional portion, puts each together into a fixed-point value, and finally converts to float.
    for(size_t i = 0; i < 1; i++) {

        // Read the integer portion of the fixed-point value (ex. 4)
        auto i1  = reader.ReadUInt16();
        auto i2  = reader.ReadUInt16();
        auto i3  = reader.ReadUInt16();
        auto i4  = reader.ReadUInt16();
        auto i5  = reader.ReadUInt16();
        auto i6  = reader.ReadUInt16();
        auto i7  = reader.ReadUInt16();
        auto i8  = reader.ReadUInt16();
        auto i9  = reader.ReadUInt16();
        auto i10 = reader.ReadUInt16();
        auto i11 = reader.ReadUInt16();
        auto i12 = reader.ReadUInt16();
        auto i13 = reader.ReadUInt16();
        auto i14 = reader.ReadUInt16();
        auto i15 = reader.ReadUInt16();
        auto i16 = reader.ReadUInt16();

        // Read the fractional portion of the fixed-point value (ex. 0.45)
        auto f1  = reader.ReadUInt16();
        auto f2  = reader.ReadUInt16();
        auto f3  = reader.ReadUInt16();
        auto f4  = reader.ReadUInt16();
        auto f5  = reader.ReadUInt16();
        auto f6  = reader.ReadUInt16();
        auto f7  = reader.ReadUInt16();
        auto f8  = reader.ReadUInt16();
        auto f9  = reader.ReadUInt16();
        auto f10 = reader.ReadUInt16();
        auto f11 = reader.ReadUInt16();
        auto f12 = reader.ReadUInt16();
        auto f13 = reader.ReadUInt16();
        auto f14 = reader.ReadUInt16();
        auto f15 = reader.ReadUInt16();
        auto f16 = reader.ReadUInt16();

        // Place the integer and fractional portions together (ex 4.45) and convert to floating-point
        auto m1  = FIXTOF( (int32_t) ( (i1 << 16) | f1 ) );
        auto m2  = FIXTOF( (int32_t) ( (i2 << 16) | f2 ) );
        auto m3  = FIXTOF( (int32_t) ( (i3 << 16) | f3 ) );
        auto m4  = FIXTOF( (int32_t) ( (i4 << 16) | f4 ) );
        auto m5  = FIXTOF( (int32_t) ( (i5 << 16) | f5 ) );
        auto m6  = FIXTOF( (int32_t) ( (i6 << 16) | f6 ) );
        auto m7  = FIXTOF( (int32_t) ( (i7 << 16) | f7 ) );
        auto m8  = FIXTOF( (int32_t) ( (i8 << 16) | f8 ) );
        auto m9  = FIXTOF( (int32_t) ( (i9 << 16) | f9 ) );
        auto m10 = FIXTOF( (int32_t) ( (i10 << 16) | f10 ) );
        auto m11 = FIXTOF( (int32_t) ( (i11 << 16) | f11 ) );
        auto m12 = FIXTOF( (int32_t) ( (i12 << 16) | f12 ) );
        auto m13 = FIXTOF( (int32_t) ( (i13 << 16) | f13 ) );
        auto m14 = FIXTOF( (int32_t) ( (i14 << 16) | f14 ) );
        auto m15 = FIXTOF( (int32_t) ( (i15 << 16) | f15 ) );
        auto m16 = FIXTOF( (int32_t) ( (i16 << 16) | f16 ) );

        matrix.push_back(MtxRaw({
            .mtx = {
                m1, m2, m3, m4,
                m5, m6, m7, m8,
                m9, m10, m11, m12,
                m13, m14, m15, m16,
            },
            .mt = MtxS {{
                {
                    { i1,  i2,  i3,  i4 },
                    { i5,  i6,  i7,  i8 },
                    { i9,  i10, i11, i12 },
                    { i13, i14, i15, i16 },
                },
                {
                    { f1,  f2,  f3,  f4 },
                    { f5,  f6,  f7,  f8 },
                    { f9,  f10, f11, f12 },
                    { f13, f14, f15, f16 },
                }
            }}
        }));
    }

    #undef FIXTOF

    return std::make_shared<MtxData>(matrix);
}
