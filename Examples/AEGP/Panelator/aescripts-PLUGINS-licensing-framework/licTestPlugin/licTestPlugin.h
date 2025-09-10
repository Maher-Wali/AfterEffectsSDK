#pragma once
#ifndef licTestPlugin_H
#define licTestPlugin_H

// name and description for the effect
#define FX_NAME "License Test Plugin"
#define FX_DESCRIPTION "This is a simple plugin (color to grayscale) to test the aescripts.com licensing framework"
#define FX_CATEGORY "aescripts + aeplugins"
#define FX_UNIQUEID "AESC licTestPlugin"

// global output flags
// concatenate only using the + operator (not |), else the PIPL compiler will choke!
#define FX_OUT_FLAGS (PF_OutFlag_USE_OUTPUT_EXTENT + PF_OutFlag_PIX_INDEPENDENT + PF_OutFlag_DEEP_COLOR_AWARE + PF_OutFlag_I_DO_DIALOG + PF_OutFlag_SEND_UPDATE_PARAMS_UI)
#define FX_OUT_FLAGS2 (PF_OutFlag2_SUPPORTS_SMART_RENDER + PF_OutFlag2_FLOAT_COLOR_AWARE + PF_OutFlag2_SUPPORTS_THREADED_RENDERING)

// version information
#define	MAJOR_VERSION		4
#define	MINOR_VERSION		0
#define	BUG_VERSION			8
#define	STAGE_VERSION		0
#define	BUILD_VERSION		1

#ifndef PIPL

// the parameter IDs for the effect
enum {
	FXID_INPUT = 0,
	FXID_MIX,
	NUM_PARAMS
};

#undef AE_64
#undef AE_32

#if defined(_M_AMD64) || defined(_AMD64_) || defined(_M_IA64) || defined(_IA64_) || defined(_WIN64) || defined(__x86_64__) || defined(__LP64__)
#define AE_64
#undef AE_32
#else
#define AE_32
#undef AE_64
#endif

#define PF_DEEP_COLOR_AWARE 1

#include "AEConfig.h"
#include "entry.h"
#ifdef AE_32
#include "PF_Suite_Helper.h"
#else
#include "AEFX_SuiteHelper.h"
#endif

#include "AE_Effect.h"
#include "PrSDKAESupport.h"

#include "AE_EffectCB.h"
#include "AE_EffectCBSuites.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"
#include "String_Utils.h"
#include "Param_Utils.h"
#include "Smart_Utils.h"

#include <vector>
using namespace std;

#ifdef AE_OS_WIN
#include <Windows.h>

#endif
#ifdef AE_32
// safer newer version
#define PF_ADD_FLOAT_SLIDERX(NAME, VALID_MIN, VALID_MAX, SLIDER_MIN, SLIDER_MAX, DFLT, PREC, DISP, FLAGS, ID)	\
	do {																										\
		def.flags = (FLAGS); \
		PF_ADD_FLOAT_SLIDER(NAME, VALID_MIN, VALID_MAX, SLIDER_MIN, SLIDER_MAX, 0, DFLT, PREC, DISP, 0, ID);	\
	} while (0)
#endif
#ifdef AE_32
#define AE_REFCON A_long
#else
#define AE_REFCON void*
#endif

class RenderData {
public:
	PF_InData *in_data;
	PF_EffectWorld *inputP;
	PF_LayerDef *outputP;
	PF_SampPB samp_pb;
	PF_PixelFormat format;
	A_long bitDepth;
	vector<PF_ParamDef> params;
	RenderData() {
		in_data = NULL;
		inputP = NULL;
		outputP = NULL;
		bitDepth = 8;
		format = PF_PixelFormat_ARGB32;
	}
};

void ProcessPixel8(A_long x, A_long y, PF_Pixel* col, RenderData* rendP);
void ProcessPixel16(A_long x, A_long y, PF_Pixel16* col, RenderData* rendP);
void ProcessPixel32(A_long x, A_long y, PF_PixelFloat* col, RenderData* rendP);
static PF_Err ParamsSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *outputP);
static PF_Err ParamsAssign(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_EffectWorld *inputP, PF_LayerDef *outputP, RenderData* rendP);

extern "C" {
DllExport PF_Err 
EntryPointFunc (
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extraP);
}

typedef struct {
	A_u_char	blue, green, red, alpha;
} PF_Pixel_BGRA;

typedef struct {
	PF_FpShort	blue, green, red, alpha;
} PF_PixelFloat_BGRA;

const float IMAX8 = 1.0f / (float)(PF_MAX_CHAN8);
const float IMAX16 = 1.0f / (float)(PF_MAX_CHAN16);

static inline void convertFormat(PF_PixelFloat_BGRA* i, PF_PixelFloat* o)
{
	o->alpha = i->alpha;
	o->blue = i->blue;
	o->red = i->red;
	o->green = i->green;
}
static inline void convertFormat(PF_Pixel_BGRA* i, PF_PixelFloat* o)
{
	o->red = i->red * IMAX8;
	o->green = i->green * IMAX8;
	o->blue = i->blue * IMAX8;
	o->alpha = i->alpha * IMAX8;
}
static inline void convertFormat(PF_PixelFloat* p, PF_PixelFloat_BGRA* outP)
{
	outP->alpha = p->alpha;
	outP->blue = p->blue;
	outP->red = p->red;
	outP->green = p->green;
}
static inline void convertFormat(PF_PixelFloat* p, PF_Pixel_BGRA* outP)
{
	outP->red = static_cast<A_u_char>(p->red * PF_MAX_CHAN8);
	outP->green = static_cast<A_u_char>(p->green * PF_MAX_CHAN8);
	outP->blue = static_cast<A_u_char>(p->blue * PF_MAX_CHAN8);
	outP->alpha = static_cast<A_u_char>(p->alpha * PF_MAX_CHAN8);
}

#endif // PIPL

#endif // licTestPlugin_H