#include "config.h"

#ifdef HAVE_ARM_NEON_H
#include <arm_neon.h>
#endif

#include "AL/al.h"
#include "AL/alc.h"
#include "alMain.h"
#include "alu.h"


static __inline void ApplyCoeffsStep(ALuint Offset, ALfloat (*RESTRICT Values)[2],
                                     ALfloat (*RESTRICT Coeffs)[2],
                                     ALfloat (*RESTRICT CoeffStep)[2],
                                     ALfloat left, ALfloat right)
{
    ALuint c;
    for(c = 0;c < HRIR_LENGTH;c++)
    {
        const ALuint off = (Offset+c)&HRIR_MASK;
        Values[off][0] += Coeffs[c][0] * left;
        Values[off][1] += Coeffs[c][1] * right;
        Coeffs[c][0] += CoeffStep[c][0];
        Coeffs[c][1] += CoeffStep[c][1];
    }
}

static __inline void ApplyCoeffs(ALuint Offset, ALfloat (*RESTRICT Values)[2],
                                 ALfloat (*RESTRICT Coeffs)[2],
                                 ALfloat left, ALfloat right)
{
    ALuint c;
    float32x4_t leftright4;
    {
        float32x2_t leftright2 = vdup_n_f32(0.0);
        leftright2 = vset_lane_f32(left, leftright2, 0);
        leftright2 = vset_lane_f32(right, leftright2, 1);
        leftright4 = vcombine_f32(leftright2, leftright2);
    }
    for(c = 0;c < HRIR_LENGTH;c += 2)
    {
        const ALuint o0 = (Offset+c)&HRIR_MASK;
        const ALuint o1 = (o0+1)&HRIR_MASK;
        float32x4_t vals = vcombine_f32(vld1_f32((float32_t*)&Values[o0][0]),
                                        vld1_f32((float32_t*)&Values[o1][0]));
        float32x4_t coefs = vld1q_f32((float32_t*)&Coeffs[c][0]);

        vals = vmlaq_f32(vals, coefs, leftright4);

        vst1_f32((float32_t*)&Values[o0][0], vget_low_f32(vals));
        vst1_f32((float32_t*)&Values[o1][0], vget_high_f32(vals));
    }
}


static __inline void ApplyValue(ALfloat *RESTRICT Output, ALfloat value, const ALfloat *DrySend)
{
    ALuint c;
    for(c = 0;c < MaxChannels;c++)
        Output[c] += value*DrySend[c];
}


#define SUFFIX Neon
#define Sampler point32
#include "mixer_inc.c"
#undef Sampler
#define Sampler lerp32
#include "mixer_inc.c"
#undef Sampler
#define Sampler cubic32
#include "mixer_inc.c"
#undef Sampler
#undef SUFFIX
