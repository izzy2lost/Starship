// struct Painting ttm_slide_painting = {
//     /* id */ 0x0000,
//     /* Image Count */ 0x02,
//     /* Texture Type */ PAINTING_IMAGE,
//     /* Floor Status */ 0x00, 0x00, 0x00 /* which of the painting's nearby special floors Mario's on */,
//     /* Ripple Status */ 0x00,
//     /* Rotation */    0.0f,   90.0f,
//     /* Position */ 3072.0f, 921.6f, -819.2f,
//     /*                         curr   passive     entry */
//     /* Ripple Magnitude */     0.0f,    20.0f,    80.0f,
//     /* Ripple Decay */         1.0f,  0.9608f,  0.9524f,
//     /* Ripple Rate */          0.0f,    0.24f,    0.14f,
//     /* Ripple Dispersion */    0.0f,    40.0f,    30.0f,
//     /* Curr Ripple Timer */    0.0f,
//     /* Curr Ripple x, y */     0.0f,    0.0f,
//     /* Normal DList */ ttm_seg7_painting_dl_07012E98,
//     /* Texture Maps */ ttm_seg7_painting_texture_maps_07012E88,
//     /* Textures */     ttm_seg7_painting_textures_07012EF8,
//     /* Texture w, h */ 64, 32,
//     /* Ripple DList */ ttm_seg7_painting_dl_07012430,
//     /* Ripple Trigger */ RIPPLE_TRIGGER_PROXIMITY,
//     /* Alpha */ 0xFF,
//     /* Mario Below */  0x00, 0x00, 0x00, /* Whether or not Mario is below the painting */
//     /* Size */  460.8f,
// };

#pragma once

#include "factories/BaseFactory.h"

#define PAINTING_IMAGE 0
#define PAINTING_ENV_MAP 1

#define RIPPLE_TRIGGER_PROXIMITY 10
#define RIPPLE_TRIGGER_CONTINUOUS 20

namespace SM64 {

class Painting : public IParsedData {
public:
    int16_t id;
    int8_t imageCount;
    int8_t textureType;
    int8_t lastFloor;
    int8_t currFloor;
    int8_t floorEntered;
    int8_t state;
    float pitch;
    float yaw;
    float posX;
    float posY;
    float posZ;
    float currRippleMag;
    float passiveRippleMag;
    float entryRippleMag;
    float rippleDecay;
    float passiveRippleDecay;
    float entryRippleDecay;
    float currRippleRate;
    float passiveRippleRate;
    float entryRippleRate;
    float dispersionFactor;
    float passiveDispersionFactor;
    float entryDispersionFactor;
    float rippleTimer;
    float rippleX;
    float rippleY;
    uint32_t normalDisplayList;
    uint32_t textureMaps;
    uint32_t textureArray;
    int16_t textureWidth;
    int16_t textureHeight;
    uint32_t rippleDisplayList;
    int8_t rippleTrigger;
    uint8_t  alpha;
    int8_t marioWasUnder;
    int8_t marioIsUnder;
    int8_t marioWentUnder;
    float size;

    Painting(
        int16_t id,
        int8_t imageCount,
        int8_t textureType,
        int8_t lastFloor, int8_t currFloor, int8_t floorEntered,
        int8_t state,
        float pitch, float yaw,
        float posX, float posY, float posZ,
        float currRippleMag, float passiveRippleMag, float entryRippleMag,
        float rippleDecay, float passiveRippleDecay, float entryRippleDecay,
        float currRippleRate, float passiveRippleRate, float entryRippleRate,
        float dispersionFactor, float passiveDispersionFactor, float entryDispersionFactor,
        float rippleTimer,
        float rippleX, float rippleY,
        uint32_t normalDisplayList,
        uint32_t textureMaps,
        uint32_t textureArray,
        int16_t textureWidth, int16_t textureHeight,
        uint32_t rippleDisplayList,
        int8_t rippleTrigger,
        uint8_t  alpha,
        int8_t marioWasUnder, int8_t marioIsUnder, int8_t marioWentUnder,
        float size
    ) :
        id(id),
        imageCount(imageCount),
        textureType(textureType),
        lastFloor(lastFloor), currFloor(currFloor), floorEntered(floorEntered),
        state(state),
        pitch(pitch), yaw(yaw),
        posX(posX), posY(posY), posZ(posZ),
        currRippleMag(currRippleMag), passiveRippleMag(passiveRippleMag), entryRippleMag(entryRippleMag),
        rippleDecay(rippleDecay), passiveRippleDecay(passiveRippleDecay), entryRippleDecay(entryRippleDecay),
        currRippleRate(currRippleRate), passiveRippleRate(passiveRippleRate), entryRippleRate(entryRippleRate),
        dispersionFactor(dispersionFactor), passiveDispersionFactor(passiveDispersionFactor), entryDispersionFactor(entryDispersionFactor),
        rippleTimer(rippleTimer),
        rippleX(rippleX), rippleY(rippleY),
        normalDisplayList(normalDisplayList),
        textureMaps(textureMaps),
        textureArray(textureArray),
        textureWidth(textureWidth), textureHeight(textureHeight),
        rippleDisplayList(rippleDisplayList),
        rippleTrigger(rippleTrigger),
        alpha(alpha),
        marioWasUnder(marioWasUnder), marioIsUnder(marioIsUnder), marioWentUnder(marioWentUnder),
        size(size)
    {}
};

class PaintingHeaderExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class PaintingBinaryExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class PaintingCodeExporter : public BaseExporter {
    ExportResult Export(std::ostream& write, std::shared_ptr<IParsedData> data, std::string& entryName, YAML::Node& node, std::string* replacement) override;
};

class PaintingFactory : public BaseFactory {
public:
    std::optional<std::shared_ptr<IParsedData>> parse(std::vector<uint8_t>& buffer, YAML::Node& data) override;
    inline std::unordered_map<ExportType, std::shared_ptr<BaseExporter>> GetExporters() override {
        return {
            REGISTER(Code, PaintingCodeExporter)
            REGISTER(Header, PaintingHeaderExporter)
            REGISTER(Binary, PaintingBinaryExporter)
        };
    }
};
}
