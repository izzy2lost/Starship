#include <cstdint>
#include <vector>
#include <cmath>
#include "endianness.h"
#include "AudioDecompressor.h"

#define C_M_PI 3.14159265358979323846f

struct StupidDMAStruct {
    int16_t* unk_0;
    int32_t unk_4;
    int32_t unk_8;
    int16_t* unk_C;
    int16_t unk18;
};

int16_t DFT_8014C1B4 = 0x1000;
float DFT_80145D48[256];
float DFT_80146148[256];
float DFT_80146548[515];
float DFT_80146D54;
float DFT_80146D58;
float DFT_80146D5C;
float DFT_80146D60;
float DFT_80146D64;
float DFT_80146D68;
float DFT_80146D6C;
float DFT_80146D70;

void func_80009124(int16_t** arg0) {
    int16_t* var_a1;
    int32_t temp_a0;
    uint8_t temp_s0;
    uint8_t temp_s1;
    uint8_t temp_u1;
    int32_t temp_t5_4;
    int32_t temp_v0;
    uint8_t temp_v1;
    int32_t var_s1;
    int32_t var_t2;
    int32_t var_v1;
    uint16_t var_s0;
    uint32_t var_t3;
    int32_t i;
    int32_t j;
    int16_t new_var2;

    var_a1 = *arg0;

    for (i = 255; i >= 0; i--) {
        DFT_80145D48[i] = 0.0f;
    }
    temp_v0 = *var_a1++;
    var_t3 = temp_v0 << 0x10;
    temp_v0 = *var_a1++;
    var_t3 |= temp_v0;

    for (var_t2 = 0; var_t2 < 4; var_t2++) {
        var_v1 = var_t2 * 0x40;
        temp_s0 = var_t3 >> 0x18;

        var_t3 <<= 8;

        temp_v1 = ((temp_s0 >> 4) & 0xF);
        temp_a0 = temp_s0 & 0xF;
        if (temp_v1 == 0) {
            continue;
        }
        switch (temp_v1) {
            case 1:
                while (true) {
                    var_s0 = *var_a1++;
                    for (var_s1 = 0; var_s1 < 4; var_s1++) {
                        temp_u1 = (var_s0 >> 0xC) & 0xF;
                        var_s0 <<= 4;
                        DFT_80145D48[var_v1++] = ((temp_u1 & 7) - 4) << temp_a0;
                        if (temp_u1 >= 8) {
                            goto case_1_break;
                        }
                    }
                }
            case_1_break:
                break;
            case 2:
                for (var_s1 = 0; var_s1 < 16; var_s1++) {
                    var_s0 = *var_a1++;
                    for (i = 0; i < 4; i++) {
                        temp_u1 = (var_s0 >> 0xC) & 0xF;
                        var_s0 <<= 4;
                        DFT_80145D48[var_v1++] = (temp_u1 - 8) << temp_a0;
                    }
                }
                break;
            case 6:
                while (true) {
                    var_s0 = *var_a1++;
                    temp_u1 = (var_s0 >> 8) & 0xFF;
                    temp_t5_4 = temp_u1 >> 6;
                    DFT_80145D48[var_v1] = ((temp_u1 & 0x3F) - 0x20) << temp_a0;
                    if (temp_t5_4 == 0) {
                        break;
                    }
                    var_v1 += temp_t5_4;
                    temp_u1 = var_s0 & 0xFF;
                    temp_t5_4 = temp_u1 >> 6;
                    DFT_80145D48[var_v1] = ((temp_u1 & 0x3F) - 0x20) << temp_a0;
                    if (temp_t5_4 == 0) {
                        break;
                    }
                    var_v1 += temp_t5_4;
                }
                break;
            case 3:
                while (true) {
                    var_s0 = *var_a1++;
                    temp_u1 = (var_s0 >> 8) & 0xFF;

                    DFT_80145D48[var_v1++] = ((temp_u1 & 0x7F) - 0x40) << temp_a0;

                    if (temp_u1 >= 0x80) {
                        break;
                    }
                    temp_u1 = var_s0 & 0xFF;
                    DFT_80145D48[var_v1++] = ((temp_u1 & 0x7F) - 0x40) << temp_a0;
                    if (temp_u1 >= 0x80) {
                        break;
                    }
                }
                break;
            case 4:
                while (true) {
                    var_s0 = *var_a1++;
                    temp_t5_4 = var_s0 >> 0xC;
                    DFT_80145D48[var_v1] = ((var_s0 & 0xFFF) - 0x800) << temp_a0;
                    if (temp_t5_4 == 0) {
                        break;
                    }
                    var_v1 += temp_t5_4;
                }
                break;
            case 5:
                while (true) {
                    var_s0 = *var_a1++;
                    temp_t5_4 = var_s0 >> 0xF;
                    DFT_80145D48[var_v1] = ((var_s0 & 0x7FFF) - 0x4000) << temp_a0;
                    if (temp_t5_4 == 1) {
                        break;
                    }
                    var_v1++;
                }
                break;
        }
    }
    *arg0 = var_a1;
}

void AudioSynth_HartleyTransform(float* arg0, int32_t arg1, float* arg2) {
    int32_t length;
    int32_t spD0;
    int32_t spCC;
    int32_t spC8;
    int32_t var_a0;
    int32_t spC0;
    int32_t spBC;
    int32_t pad;
    int32_t spB4;
    int32_t sp58;
    int32_t sp50;
    int32_t spA8;
    float var_fs0;
    float temp_fa0;
    float temp_fv1;
    float* temp_a0;
    float* temp_a1;
    float* temp_a2;
    float* temp_a3;
    float* temp_b0;
    float* temp_b1;
    float* temp_b2;
    float* temp_b3;
    float* var_s0;
    float* var_s1;
    float* var_s2;
    float* var_s3;

    length = 1 << arg1;
    spBC = length * 2;
    sp58 = (length / 8) - 1;
    switch (length) {
        case 1:
            break;
        case 2:
            temp_fa0 = arg0[1];
            temp_fv1 = arg0[0];

            arg0[0] = (temp_fa0 + temp_fv1) * 0.707107f;
            arg0[1] = (temp_fv1 - temp_fa0) * 0.707107f;
            break;
        case 4:
            temp_fv1 = arg0[0];
            arg0[0] = (arg0[2] + temp_fv1) / 2.0f;
            arg0[2] = (temp_fv1 - arg0[2]) / 2.0f;

            temp_fa0 = arg0[1];
            arg0[1] = (arg0[3] + temp_fa0) / 2.0f;
            arg0[3] = (temp_fa0 - arg0[3]) / 2.0f;

            temp_fv1 = arg0[0];
            temp_fa0 = arg0[1];
            arg0[0] = arg0[1] + temp_fv1;
            arg0[1] = arg0[3] + arg0[2];
            arg0[3] = arg0[2] - arg0[3];
            arg0[2] = temp_fv1 - temp_fa0;
            break;
        default:
            if (length != (int32_t) *arg2) {
                *arg2 = length;

                var_s0 = &arg2[1];
                var_s1 = &var_s0[sp58];
                var_s2 = &var_s1[sp58];
                var_s3 = &var_s2[sp58];

                var_fs0 = 6.283186f / length;
                for (spCC = 0; spCC < sp58; spCC++) {
                    *var_s0++ = cosf(var_fs0);
                    *var_s1++ = sinf(var_fs0);
                    *var_s2++ = cosf(3.0f * var_fs0);
                    *var_s3++ = sinf(3.0f * var_fs0);
                    var_fs0 += 6.283186f / length;
                }
            }
            spC0 = 1;
            for (spD0 = 0; spD0 < arg1 - 1; spD0++) {
                spA8 = spBC;
                spBC >>= 1;
                spB4 = spBC >> 3;
                sp50 = spBC >> 2;
                var_a0 = 1;
                do {
                    for (spCC = var_a0 - 1; spCC < length; spCC += spA8) {
                        // if (0) { }
                        temp_a0 = arg0 + spCC;
                        temp_a1 = temp_a0 + sp50;
                        temp_a2 = temp_a1 + sp50;
                        temp_a3 = temp_a2 + sp50;

                        DFT_80146D54 = *temp_a0;
                        *temp_a0 = *temp_a2 + DFT_80146D54;
                        DFT_80146D58 = *temp_a1;
                        *temp_a1 = *temp_a1 + *temp_a3;
                        DFT_80146D5C = *temp_a2;
                        *temp_a2 = DFT_80146D54 - DFT_80146D5C + DFT_80146D58 - *temp_a3;
                        *temp_a3 = DFT_80146D54 - DFT_80146D5C - DFT_80146D58 + *temp_a3;
                        if (sp50 > 1) {
                            temp_a0 = arg0 + spCC + spB4;
                            temp_a1 = temp_a0 + sp50;
                            temp_a2 = temp_a1 + sp50;
                            temp_a3 = temp_a2 + sp50;

                            DFT_80146D54 = *temp_a0;
                            *temp_a0 = *temp_a2 + DFT_80146D54;
                            DFT_80146D58 = *temp_a1;
                            *temp_a1 = *temp_a3 + DFT_80146D58;
                            *temp_a2 = (DFT_80146D54 - *temp_a2) * 1.414214f;
                            *temp_a3 = (DFT_80146D58 - *temp_a3) * 1.414214f;

                            var_s0 = &arg2[spC0];
                            var_s1 = &var_s0[sp58];
                            var_s2 = &var_s1[sp58];
                            var_s3 = &var_s2[sp58];
                            for (spC8 = 1; spC8 < spB4; spC8++) {
                                temp_a0 = arg0 + spCC + spC8;
                                temp_a1 = temp_a0 + sp50;
                                temp_a2 = temp_a1 + sp50;
                                temp_a3 = temp_a2 + sp50;

                                temp_b0 = arg0 + spCC + sp50 - spC8;
                                temp_b1 = temp_b0 + sp50;
                                temp_b2 = temp_b1 + sp50;
                                temp_b3 = temp_b2 + sp50;

                                DFT_80146D54 = *temp_a0;
                                DFT_80146D58 = *temp_a1;
                                DFT_80146D5C = *temp_a2;
                                DFT_80146D60 = *temp_a3;

                                DFT_80146D64 = *temp_b0;
                                DFT_80146D68 = *temp_b1;
                                DFT_80146D6C = *temp_b2;
                                DFT_80146D70 = *temp_b3;

                                *temp_a0 = DFT_80146D54 + DFT_80146D5C;
                                *temp_a1 = DFT_80146D58 + DFT_80146D60;
                                *temp_a2 = ((DFT_80146D54 - DFT_80146D5C + DFT_80146D64 - DFT_80146D6C) * *var_s0) +
                                           ((DFT_80146D60 - DFT_80146D58 + DFT_80146D68 - DFT_80146D70) * *var_s1);
                                *temp_a3 = ((DFT_80146D54 - DFT_80146D5C - DFT_80146D64 + DFT_80146D6C) * *var_s2) -
                                           ((DFT_80146D60 - DFT_80146D58 - DFT_80146D68 + DFT_80146D70) * *var_s3);
                                *temp_b0 = DFT_80146D64 + DFT_80146D6C;
                                *temp_b1 = DFT_80146D68 + DFT_80146D70;
                                *temp_b2 = ((DFT_80146D54 - DFT_80146D5C + DFT_80146D64 - DFT_80146D6C) * *var_s1) -
                                           ((DFT_80146D60 - DFT_80146D58 + DFT_80146D68 - DFT_80146D70) * *var_s0);
                                *temp_b3 = ((DFT_80146D54 - DFT_80146D5C - DFT_80146D64 + DFT_80146D6C) * *var_s3) +
                                           ((DFT_80146D60 - DFT_80146D58 - DFT_80146D68 + DFT_80146D70) * *var_s2);
                                var_s0 += spC0;
                                var_s1 += spC0;
                                var_s2 += spC0;
                                var_s3 += spC0;
                            }
                        }
                    }
                    var_a0 = ((spA8 * 2) - spBC) + 1;
                    spA8 *= 4;
                } while (var_a0 < length);
                spC0 = spC0 * 2;
                temp_a0 = arg0;
                for (spC8 = 0; spC8 < length; spC8++, temp_a0++) {
                    *temp_a0 /= 1.414214f;
                }
            }
            var_a0 = 1;
            spA8 = 4;

            do {
                for (spCC = var_a0 - 1; spCC < length; spCC += spA8) {
                    DFT_80146D54 = arg0[spCC];
                    arg0[spCC] = arg0[spCC + 1] + DFT_80146D54;
                    arg0[spCC + 1] = DFT_80146D54 - arg0[spCC + 1];
                }
                var_a0 = (spA8 * 2) - 1;
                spA8 *= 4;
            } while (var_a0 < length);
            temp_a0 = arg0;
            for (spC8 = 0; spC8 < length; spC8++) {
                *temp_a0++ /= 1.414214f;
            }
            spB4 = 1;
            temp_a0 = arg0;

            for (spC8 = 1; spC8 < length; spC8++) {
                if (spC8 < spB4) {
                    DFT_80146D54 = arg0[spB4 - 1];
                    arg0[spB4 - 1] = *temp_a0;
                    *temp_a0 = DFT_80146D54;
                }
                temp_a0++;
                spC0 = length >> 1;
                while (spC0 < spB4) {
                    spB4 -= spC0;
                    spC0 >>= 1;
                }
                spB4 += spC0;
            }
            break;
    }
}

void AudioSynth_InverseDiscreteCosineTransform(float* buffer0, float* buffer1, int32_t length, float* buffer2) {
    float temp_ft0;
    float var_fs0;
    float* buff0fromStart;
    float* buf2half2;
    float* buf2half3;
    float* buff1half1;
    float* buff0FromEnd;
    float* buff1half2;
    int32_t half;
    int32_t i;
    int32_t size;

    size = 1 << length;
    half = size >> 1;

    // Initialize buffer 2 if it is the wrong size for this calculation
    if (size != (int32_t) buffer2[0]) {
        buf2half2 = &buffer2[half];
        buf2half3 = &buf2half2[half];
        var_fs0 = 0.0f;
        temp_ft0 = C_M_PI / (float) (2 * size);
        for (i = 0; i < half; i++) {
            *buf2half2++ = (cosf(var_fs0) - sinf(var_fs0)) * 0.707107f;
            *buf2half3++ = (cosf(var_fs0) + sinf(var_fs0)) * 0.707107f;
            var_fs0 += temp_ft0;
        }
    }

    // reset the buffer pointers
    buf2half2 = &buffer2[half];
    buf2half3 = &buf2half2[half];
    buff1half1 = buffer1;
    buff0fromStart = buffer0;

    // handle i = 0 case
    buffer1[0] = buffer0[0];
    buffer1[half] = buffer0[half];

    // advance buffer pointers
    buf2half2++;
    buf2half3++;
    buff0fromStart++;
    buff0FromEnd = &buffer0[size - 1];
    buff1half1++;
    buff1half2 = &buffer1[size - 1];

    // convert to real amplitudes
    for (i = 1; i < half; i++) {
        *buff1half1++ = (*buf2half2 * *buff0fromStart) + (*buf2half3 * *buff0FromEnd);
        *buff1half2-- = (*buf2half3 * *buff0fromStart) - (*buf2half2 * *buff0FromEnd);
        buff0fromStart++;
        buf2half3++;
        buf2half2++;
        buff0FromEnd--;
    }

    // FFT buffer 1 using buffer 2
    AudioSynth_HartleyTransform(buffer1, length, buffer2);

    buff0fromStart = buffer0;
    buff0FromEnd = &buffer0[size - 1];
    buff1half1 = buffer1;
    buff1half2 = &buffer1[half];

    // Copy even entries of buffer 0 into the first half of buffer 1. Copy odd entries into the second half in reverse
    // order
    for (i = 0; i < half; i++) {
        *buff0fromStart = *buff1half1++;
        *buff0FromEnd = *buff1half2++;
        buff0fromStart += 2;
        buff0FromEnd -= 2;
    }
}

void func_80009504(int16_t* arg0, StupidDMAStruct* arg1) {
    int32_t i;

    if (arg1->unk_0 != nullptr) {
        arg1->unk_C = arg1->unk_0;
        arg1->unk_0 = 0;
    }

    arg1->unk18 += DFT_8014C1B4;

    while (arg1->unk18 > 0x1000) {
        func_80009124(&arg1->unk_C);
        arg1->unk18 -= 0x1000;
    }

    AudioSynth_InverseDiscreteCosineTransform(DFT_80145D48, DFT_80146148, 8, DFT_80146548);

    for (i = 0; i < 256; i++) {
        if (DFT_80145D48[i] > 32767.0f) {
            DFT_80145D48[i] = 32767.0f;
        }
        if (DFT_80145D48[i] < -32767.0f) {
            DFT_80145D48[i] = -32767.0f;
        }
    }

    for (i = 0; i < 0x100; i++, arg0++) {
        *arg0 = (int16_t) DFT_80145D48[i];
    }
}

int32_t func_8000967C(int32_t length, int16_t* inputAddr, int16_t* ramAddr, StupidDMAStruct* arg2) {
    int32_t pad;
    int32_t temp_t0;
    int32_t i;
    int32_t var_s1;
    int16_t* temp_t7 = inputAddr;

    for (i = 0; i < arg2->unk_4; i++) {
        ramAddr[i] = temp_t7[i];
    }

    var_s1 = arg2->unk_4;
    temp_t0 = (length - arg2->unk_4 + 0xFF) / 256;
    arg2->unk_4 = (temp_t0 * 256) + arg2->unk_4 - length;

    for (i = 0; i < temp_t0; i++) {
        func_80009504(&ramAddr[var_s1], arg2);
        var_s1 += 0x100;
    }

    for (i = 0; i < arg2->unk_4; i++) {
        temp_t7[i] = ramAddr[length + i];
    }
    return temp_t0;
}

void SF64::DecompressAudio(std::vector<uint8_t> data, int16_t* output) {
    StupidDMAStruct arg3 = {};
    arg3.unk_0 = (int16_t*) data.data();
    arg3.unk_4 = 0;
    arg3.unk_8 = 0;
    arg3.unk18 = 0;

    for(int i = 0; i < data.size() / 2; i++){
        arg3.unk_0[i] = BSWAP16(arg3.unk_0[i]);
    }

    func_8000967C((int32_t) data.size(), arg3.unk_0, (int16_t*) output, &arg3);
}