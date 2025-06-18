#pragma once

namespace Torch {

enum class ResourceType {
    // Not set
    None = 0x00000000,

    // Common
    Archive = 0x4F415243,      // OARC (UNUSED)
    DisplayList = 0x4F444C54,  // ODLT
    Vertex = 0x4F565458,       // OVTX
    Matrix = 0x4F4D5458,       // OMTX
    Float = 0x4F464C54,        // OFLT
    Array = 0x4F415252,        // OARR
    Blob = 0x4F424C42,         // OBLB
    Texture = 0x4F544558,      // OTEX
    Lights = 0x46669697,       // LGTS
    Vec3f = 0x56433346,        // VC3F
    Vec3s = 0x56433353,        // VC3S
    GenericArray = 0x47415252, // GARR
    AssetArray = 0x41415252,   // AARR
    Viewport = 0x4F565054,     // OVPT

    // SM64
    Anim = 0x414E494D,           // ANIM
    BehaviorScript = 0x42485653, // BHVS
    SDialog = 0x53444C47,        // SDLG
    Dictionary = 0x44494354,     // DICT
    GeoLayout = 0x47454F20,      // GEO
    Collision = 0x434F4C20,      // COL
    LevelScript = 0x4C564C53,    // LVLS 
    MacroObject = 0x4D41434F,    // MACO
    Movtex = 0x4D4F5654,         // MOVT
    MovtexQuad = 0x4D4F5651,     // MOVQ
    Painting = 0x504E5420,       // PNT
    PaintingData = 0x504E5444,   // PNTD
    Trajectory = 0x5452414A,     // TRAJ
    WaterDroplet = 0x57545244,   // WTRD

    // MK64
    CourseProperties = 0x43505459, // CPTY
    CourseVertex = 0x43565458,     // CVTX
    TrackSection = 0x5343544E,     // SCTN
    Waypoints = 0x57505453,        // WPTS
    Metadata  = 0x4D444154,        // MDAT
    SpawnData = 0x53444154,        // SDAT
    UnkSpawnData = 0x55534454,     // USDT
    DrivingBehaviour = 0x44424856, // DBHV

    // SF64
    AnimData = 0x414E494D,     // ANIM
    ColPoly = 0x43504C59,      // CPLY
    Environment = 0x454E5653,  // ENVS
    Limb = 0x4C494D42,         // LIMB
    Message = 0x4D534720,      // MSG
    MessageTable = 0x4D534754, // MSGT
    Skeleton = 0x534B454C,     // SKEL
    Script = 0x53435250,       // SCRP
    ScriptCmd = 0x53434D44,    // SCMD
    Hitbox = 0x48544258,       // HTBX
    ObjectInit = 0x4F42494E,   // OBIN

    // F-ZERO X
    CourseData = 0x58435253,   // XCRS
    GhostRecord = 0x58475244,  // XGRD

    // NAudio v0
    Bank = 0x42414E4B,         // BANK
    Sample = 0x41554643,       // AIFC
    Sequence = 0x53455143,     // SEQC

    // NAudio v1
    SoundFont = 0x53464E54,    // SFNT
    Drum = 0x4452554D,         // DRUM
    Instrument = 0x494E5354,   // INST
    AdpcmLoop = 0x4150434C,    // APCL
    AdpcmBook = 0x41504342,    // APCB
    Envelope = 0x45564C50,     // EVLP
    AudioTable = 0x4154424C    // ATBL
};
} // namespace LUS
