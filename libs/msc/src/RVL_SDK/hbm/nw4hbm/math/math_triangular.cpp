#include "revolution/hbm/nw4hbm/math/arithmetic.h"
#include "revolution/types.h"

typedef struct {
    /* 0x00 */ f32 sin_val;
    /* 0x04 */ f32 cos_val;
    /* 0x08 */ f32 sin_delta;
    /* 0x0C */ f32 cos_delta;
} SinCosSample; // size = 0x10

typedef struct {
    /* 0x00 */ f32 atan_val;
    /* 0x04 */ f32 atan_delta;
} ArcTanSample; // size = 0x08

namespace nw4hbm {
namespace math {
namespace {
f32 AtanFIdx_(f32 x);

// DUMP_DATA sSinCosTbl__Q36nw4hbm4math29@unnamed@math_triangular_cpp@ {f32,f32,f32,f32}
static SinCosSample sSinCosTbl[] = {
#include "dumpdata/sSinCosTbl__Q36nw4hbm4math29@unnamed@math_triangular_cpp@.inc"
};

// DUMP_DATA sArcTanTbl__Q36nw4hbm4math29@unnamed@math_triangular_cpp@ {f32,f32}
static ArcTanSample sArcTanTbl[] = {
#include "dumpdata/sArcTanTbl__Q36nw4hbm4math29@unnamed@math_triangular_cpp@.inc"
};

} // namespace

namespace {

f32 AtanFIdx_(f32 x) {
    u16 idx;
    f32 val;
    f32 r;

    x *= 32.0f;

    idx = F32ToU16(x);
    r = x - U16ToF32(idx);
    val = sArcTanTbl[idx].atan_val + r * sArcTanTbl[idx].atan_delta;

    return val;
}

} // unnamed namespace

f32 SinFIdx(f32 fidx) {
    f32 abs_fidx = FAbs(fidx);
    f32 val;
    u16 idx;
    f32 r;

    while (abs_fidx >= 65536.0f) {
        abs_fidx -= 65536.0f;
    }

    idx = F32ToU16(abs_fidx);
    r = abs_fidx - U16ToF32(idx);

    idx &= 0xff;
    val = sSinCosTbl[idx].sin_val + r * sSinCosTbl[idx].sin_delta;

    return fidx < 0.0f ? -val : val;
}

f32 CosFIdx(f32 fidx) {
    u16 idx;
    f32 r;

    fidx = FAbs(fidx);
    while (fidx >= 65536.0f) {
        fidx -= 65536.0f;
    }

    idx = F32ToU16(fidx);
    r = fidx - U16ToF32(idx);

    idx &= 0xff;

    return sSinCosTbl[idx].cos_val + r * sSinCosTbl[idx].cos_delta;
}

void SinCosFIdx(f32* pSin, f32* pCos, f32 fidx) {
    f32 abs_fidx = FAbs(fidx);

    while (abs_fidx >= 65536.0f) {
        abs_fidx -= 65536.0f;
    }

    u16 idx = F32ToU16(abs_fidx);
    f32 r = abs_fidx - U16ToF32(idx);

    f32 sin = sSinCosTbl[idx & 255].sin_val + r * sSinCosTbl[idx & 255].sin_delta;

    f32 cos = sSinCosTbl[idx & 255].cos_val + r * sSinCosTbl[idx & 255].cos_delta;

    *pSin = (fidx < 0.0f) ? -sin : sin;
    *pCos = cos;
}

f32 AtanFIdx(f32 x) {
    if (x >= 0.0f) {
        if (x > 1.0f) {
            return 64.0f - AtanFIdx_(1.0f / x);
        } else {
            return AtanFIdx_(x);
        }
    } else {
        if (x < -1.0f) {
            return AtanFIdx_(-1.0f / x) + -64.0f;
        } else {
            return -AtanFIdx_(-x);
        }
    }
}

f32 Atan2FIdx(f32 y, f32 x) {
    f32 a;
    f32 b;
    f32 c;
    bool minus;

    if (x == 0.0f && y == 0.0f) {
        return 0.0f;
    }

    if (x >= 0.0f) {
        if (y >= 0.0f) {
            if (x >= y) {
                a = x;
                b = y;
                c = 0.0f;

                minus = false;
            } else {
                a = y;
                b = x;
                c = 64.0f;

                minus = true;
            }
        } else {
            if (x >= -y) {
                a = x;
                b = -y;
                c = 0.0f;

                minus = true;
            } else {
                a = -y;
                b = x;
                c = -64.0f;

                minus = false;
            }
        }
    } else {
        if (y >= 0.0f) {
            if (-x >= y) {
                a = -x;
                b = y;
                c = 128.0f;

                minus = true;
            } else {
                a = y;
                b = -x;
                c = 64.0f;

                minus = false;
            }
        } else {
            if (-x >= -y) {
                a = -x;
                b = -y;
                c = -128.0f;

                minus = false;
            } else {
                a = -y;
                b = -x;
                c = -64.0f;

                minus = true;
            }
        }
    }

    return minus ? c - AtanFIdx_(b / a) : c + AtanFIdx_(b / a);
}

} // namespace math
} // namespace nw4hbm
