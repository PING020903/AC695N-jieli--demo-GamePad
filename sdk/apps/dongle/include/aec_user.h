#ifndef _AEC_USER_H_
#define _AEC_USER_H_

#include "generic/typedef.h"
#include "user_cfg.h"
#include "commproc.h"

#define AEC_DEBUG_ONLINE	0
#define AEC_READ_CONFIG		0

#define AEC_EN				BIT(0)
#define NLP_EN				BIT(1)
#define	ANS_EN				BIT(2)

/*aec module enable bit define*/
#define AEC_MODE_ADVANCE	(AEC_EN | NLP_EN | ANS_EN)
#define AEC_MODE_REDUCE		(NLP_EN | ANS_EN)
#define AEC_MODE_SIMPLEX	(ANS_EN)

extern struct aec_s_attr aec_param;
extern const u8 CONST_AEC_SIMPLEX;

struct aec_s_attr *aec_param_init(u16 sr);
s8 aec_debug_online(void *buf, u16 size);
void aec_cfg_fill(AEC_CONFIG *cfg);

int audio_aec_init(u16 sample_rate);
void audio_aec_close(void);
void audio_aec_inbuf(s16 *buf, u16 len);
void audio_aec_refbuf(s16 *buf, u16 len);

#endif/*_AEC_USER_H_*/
