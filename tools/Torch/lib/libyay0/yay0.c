#include "yay0.h"
#include "libmio0/utils.h"
#include <string.h>
#include <stdlib.h>

uint8_t* yay0_decode(const uint8_t* in_buf, uint32_t* out_size){

    const uint8_t* in = in_buf;
    const char* magic = (const char*) in;
    if(!strncmp(magic, "PERS-SZP", 8)){
        uint32_t header_size = read_u32_be(in + 8);
        magic += header_size;
        in += header_size;
    }

    if(strncmp(magic, "Yay0", 4) != 0){
        return NULL;
    }

    uint32_t decompressed_size = read_u32_be(in + 4);
    uint32_t link_table_offset = read_u32_be(in + 8);
    uint32_t chunk_offset      = read_u32_be(in + 12);

    uint32_t link_table_idx = link_table_offset;
    uint32_t chunk_idx = chunk_offset;
    uint32_t other_idx = 16;

    uint32_t mask_bit_counter = 0;
    uint32_t current_mask = 0;
    uint32_t idx = 0;

    uint8_t* out = malloc(decompressed_size);
    memset(out, 0, decompressed_size);
    *out_size = decompressed_size;

    while(idx < decompressed_size){
        if(mask_bit_counter == 0){
            current_mask = read_u32_be(in + other_idx);
            other_idx += 4;
            mask_bit_counter = 32;
        }

        if(current_mask & 0x80000000){
            out[idx] = in[chunk_idx];
            idx++;
            chunk_idx++;
        } else {
            uint16_t link = read_u16_be(in + link_table_idx);
            link_table_idx += 2;
            uint32_t offset = idx - (link & 0xFFF);
            uint32_t count = link >> 12;

            if(count == 0){
                uint8_t count_modifier = in[chunk_idx];
                chunk_idx++;
                count = count_modifier + 18;
            } else {
                count += 2;
            }

            for(size_t i = 0; i < count; i++){
                out[idx] = out[offset + i - 1];
                idx++;
            }
        }

        current_mask <<= 1;
        mask_bit_counter--;
    }

    return out;
}