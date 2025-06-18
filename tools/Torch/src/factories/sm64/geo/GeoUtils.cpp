#include "GeoUtils.h"

#define next_s16_in_geo_script(src) (int16_t) BSWAP16((*(*src)++))

int16_t* read_vec3s_to_vec3f(Vec3f& dst, int16_t *src) {
    dst.x = next_s16_in_geo_script(&src);
    dst.y = next_s16_in_geo_script(&src);
    dst.z = next_s16_in_geo_script(&src);
    return src;
}

int16_t* read_vec3s(Vec3s& dst, int16_t *src) {
    dst.x = next_s16_in_geo_script(&src);
    dst.y = next_s16_in_geo_script(&src);
    dst.z = next_s16_in_geo_script(&src);
    return src;
}

int16_t* read_vec3s_angle(Vec3s& dst, int16_t *src) {
    dst.x = next_s16_in_geo_script(&src);
    dst.y = next_s16_in_geo_script(&src);
    dst.z = next_s16_in_geo_script(&src);
    return src;
}