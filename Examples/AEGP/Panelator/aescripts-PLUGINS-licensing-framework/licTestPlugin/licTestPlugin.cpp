#include <string>
#include <sstream>
#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "licTestPlugin.h"
// ********************* aescripts licensing specific code start *********************

// change the following settings for your own plugin!!!

// this is the name of your plugin
#define LIC_PRODUCT_NAME "License Framework v4 Test Plugin"

// this is the private number - it is unique to every plugin/product and issued by aescripts
#define LIC_PRIVATE_NUM 11223344

// the product ID is a string also issued by aescripts that identifies your plugin
#define LIC_PRODUCT_ID "AESLT4"

// this is the filename (extension .lic gets added automatically) under which the license is stored on the disk
// you must share the license filename with the aescripts admin so that it is entered in the main database
#define LIC_FILENAME "AESLT4"

// set this define if your plugin is compiled for beta-testers, it will then accept BTA type licenses
#define LIC_BETA

// include the aescripts licensing API (should be done *after* including the Adobe AE headers!)
#include "../include/aescriptsLicensing.h"
// include the aescripts licensing Adobe helper API (should be done *after* including the Adobe AE headers and the licensing API!)
#include "../include/aescriptsLicensing_AdobeHelpers.h"

// ********************* aescripts licensing specific code end *********************

// function for drawing an "X" across the rendered output in trial mode
inline bool checkCross(A_long x, A_long y, const RenderData* rendP) {
	float ypr = y / (float)rendP->inputP->height;
	A_long w = 1 + rendP->inputP->width / 250;
	return (abs(x-(A_long)(ypr * rendP->inputP->width)) < w || abs(x-(rendP->inputP->width-(A_long)(ypr * rendP->inputP->width))) < w);
}

// demo process: blend with grayscale version of layer!

// 8bpc
void ProcessPixel8(A_long x, A_long y, PF_Pixel* col, RenderData* rendP) {
	float mix = (PF_FpShort)rendP->params[FXID_MIX].u.fs_d.value / 100.0f;
	
	col->red = A_u_char((1.0f - mix) * col->red + mix * col->green);
	col->blue = A_u_char((1.0f - mix) * col->blue + mix * col->green);
	if (aescripts::licenseData.registered && aescripts::licenseData.overused != 1 && aescripts::licenseData.renderOK) return;
	if (checkCross(x, y, rendP)) { col->alpha = 150 + rand() % 100; col->red = rand() % 255; col->green = rand() % 255; col->blue = rand() % 255; }
}

// 16bpc
void ProcessPixel16(A_long x, A_long y, PF_Pixel16* col, RenderData* rendP) {
	float mix = (PF_FpShort)rendP->params[FXID_MIX].u.fs_d.value / 100.0f;

	col->red = A_u_short((1.0f - mix) * col->red + mix * col->green);
	col->blue = A_u_short((1.0f - mix) * col->blue + mix * col->green);
	if (aescripts::licenseData.registered && aescripts::licenseData.overused != 1 && aescripts::licenseData.renderOK) return;
	if (checkCross(x, y, rendP)) { col->alpha = 65000 + rand() % 500; col->red = rand() % 65535; col->green = rand() % 65535; col->blue = rand() % 65535; }
}

// 32bpc
void ProcessPixel32(A_long x, A_long y, PF_PixelFloat* col, RenderData* rendP) {
	float mix = (PF_FpShort)rendP->params[FXID_MIX].u.fs_d.value / 100.0f;

	col->red = (1.0f - mix) * col->red + mix * col->green;
	col->blue = (1.0f - mix) * col->blue + mix * col->green;
	if (aescripts::licenseData.registered && aescripts::licenseData.overused != 1 && aescripts::licenseData.renderOK) return;
	if (checkCross(x, y, rendP)) { col->alpha = (rand() % 1000) / 1000.0f; col->red = (rand() % 1000) / 1000.0f; col->green = (rand() % 1000) / 1000.0f; col->blue = (rand() % 1000) / 1000.0f; }
}

// special iteration loop for Premiere Pro BGRA 32bit pixel format
static PF_Err ProcessFloat_BGRA(
	PF_InData			*in_data,
	long				progress_base,
	long				progress_final,
	PF_EffectWorld		*src,
	AE_REFCON			refcon,
	PF_EffectWorld		*dst) {
	PF_Err err = PF_Err_NONE;
	char *localSrc, *localDst;
	localSrc = reinterpret_cast<char*>(src->data);
	localDst = reinterpret_cast<char*>(dst->data);
	register RenderData *rendP = reinterpret_cast<RenderData*>(refcon);

	A_long width = src->width;
	if (dst->width < src->width) width = dst->width;
	A_long height = src->height;
	if (dst->height < src->height) height = dst->height;

	for (int y = (int)progress_base; y < (int)progress_final; y++) {
		for (int x = 0; x < width; x++) {
			PF_PixelFloat_BGRA* inBGRA = reinterpret_cast<PF_PixelFloat_BGRA*>(localSrc);
			PF_PixelFloat_BGRA* outBGRA = reinterpret_cast<PF_PixelFloat_BGRA*>(localDst);
			
			PF_PixelFloat col;
			convertFormat(inBGRA, &col);
			ProcessPixel32(x, y, &col, rendP);
			convertFormat(&col, outBGRA);

			localSrc += sizeof(PF_PixelFloat_BGRA);
			localDst += sizeof(PF_PixelFloat_BGRA);
		}
		localSrc += (src->rowbytes - width * sizeof(PF_PixelFloat_BGRA));
		localDst += (dst->rowbytes - width * sizeof(PF_PixelFloat_BGRA));
	}
	return err;
}

// special iteration loop for Premiere Pro BGRA 8bit pixel format
static PF_Err Process8_BGRA(
	PF_InData			*in_data,
	long				progress_base,
	long				progress_final,
	PF_EffectWorld		*src,
	AE_REFCON			refcon,
	PF_EffectWorld		*dst) {
	PF_Err err = PF_Err_NONE;
	char *localSrc, *localDst;
	localSrc = reinterpret_cast<char*>(src->data);
	localDst = reinterpret_cast<char*>(dst->data);
	register RenderData *rendP = reinterpret_cast<RenderData*>(refcon);

	A_long width = src->width;
	if (dst->width < src->width) width = dst->width;
	A_long height = src->height;
	if (dst->height < src->height) height = dst->height;

	for (int y = (int)progress_base; y < (int)progress_final; y++) {
		for (int x = 0; x < width; x++) {
			PF_Pixel_BGRA* inBGRA = reinterpret_cast<PF_Pixel_BGRA*>(localSrc);
			PF_Pixel_BGRA* outBGRA = reinterpret_cast<PF_Pixel_BGRA*>(localDst);

			PF_PixelFloat col;
			convertFormat(inBGRA, &col);
			ProcessPixel32(x, y, &col, rendP);
			convertFormat(&col, outBGRA);

			localSrc += sizeof(PF_Pixel_BGRA);
			localDst += sizeof(PF_Pixel_BGRA);
		}
		localSrc += (src->rowbytes - width * sizeof(PF_Pixel_BGRA));
		localDst += (dst->rowbytes - width * sizeof(PF_Pixel_BGRA));
	}
	return err;
}

static PF_Err ParamsSetup(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*outputP) {
	PF_Err err = PF_Err_NONE;

	PF_ParamDef def; 
	AEFX_CLR_STRUCT(def);
	PF_ADD_FLOAT_SLIDERX(
		"Mix", // name
		0, 100, //valid min, max values
		0, 100, //slider min, max values
		100, //default value
		2, //precision
		PF_ValueDisplayFlag_PERCENT, //display flags
		0, //flags
		FXID_MIX //id
		);
	out_data->num_params = NUM_PARAMS;
	return err;
}


static PF_Err GlobalSetup(
	PF_InData		*in_data, 
	PF_OutData		*out_data, 
	PF_ParamDef		*params[], 
	PF_LayerDef		*outputP) {
	PF_Err err = PF_Err_NONE;

	out_data->my_version = PF_VERSION(MAJOR_VERSION, MINOR_VERSION, BUG_VERSION, STAGE_VERSION, BUILD_VERSION);
	out_data->out_flags = FX_OUT_FLAGS;
	out_data->out_flags2 = FX_OUT_FLAGS2;

	// Premiere Pro/Elements does not support this suite
	if (in_data->appl_id != 'PrMr') {
		PF_EffectUISuite1 *effect_ui_suiteP = NULL;
		in_data->pica_basicP->AcquireSuite(kPFEffectUISuite, kPFEffectUISuiteVersion1, (const void **)&effect_ui_suiteP);
		if (effect_ui_suiteP) {
			effect_ui_suiteP->PF_SetOptionsButtonName(in_data->effect_ref, "Register");
			in_data->pica_basicP->ReleaseSuite(kPFEffectUISuite, kPFEffectUISuiteVersion1);
		}
	}
	
	// support Premiere Pro formats
	if (in_data->appl_id == 'PrMr') {
		out_data->out_flags &= ~PF_OutFlag_DEEP_COLOR_AWARE;
		out_data->out_flags2 &= ~PF_OutFlag2_FLOAT_COLOR_AWARE;

		PF_PixelFormatSuite1 *pfS = NULL;
		in_data->pica_basicP->AcquireSuite(kPFPixelFormatSuite, kPFPixelFormatSuiteVersion1, (const void **)&pfS);
		if (pfS) {
			pfS->ClearSupportedPixelFormats(in_data->effect_ref);
			pfS->AddSupportedPixelFormat(in_data->effect_ref, PrPixelFormat_BGRA_4444_8u);
			in_data->pica_basicP->ReleaseSuite(kPFPixelFormatSuite, kPFPixelFormatSuiteVersion1);
		}
	}

	return err;
}

static PF_Err ProcessFloat(
	AE_REFCON		refcon, 
	A_long 			xL, 
	A_long 			yL,
	PF_PixelFloat 	*inP,
	PF_PixelFloat 	*outP) {
	register RenderData *rendP = reinterpret_cast<RenderData*>(refcon);
	PF_Err err = PF_Err_NONE;
	A_long i;
	for (i = xL; i < rendP->outputP->width + xL; i++) {
		*outP = *inP;
		ProcessPixel32(i, yL, outP, rendP);
		++inP;
		++outP;
	}
	return err;
}

static PF_Err Process8(
	AE_REFCON	refcon, 
	A_long 		xL, 
	A_long 		yL,
	PF_Pixel 	*inP, 
	PF_Pixel 	*outP) {
	register RenderData *rendP = reinterpret_cast<RenderData*>(refcon);
	PF_Err err = PF_Err_NONE;
	A_long i;
	for (i = xL; i < rendP->outputP->width + xL; i++) {
		*outP = *inP;
		ProcessPixel8(i, yL, outP, rendP);
		++inP;
		++outP;
	}
	return err;
}
 
static PF_Err Process16(
	AE_REFCON	refcon, 
	A_long 		xL, 
	A_long 		yL, 
	PF_Pixel16 	*inP, 
	PF_Pixel16 	*outP) {
	register RenderData *rendP = reinterpret_cast<RenderData*>(refcon);
	PF_Err err = PF_Err_NONE;
	A_long i;
	for (i = xL; i < rendP->outputP->width + xL; i++) {
		*outP = *inP;
		ProcessPixel16(i, yL, outP, rendP);
		++inP;
		++outP;
	}
	return err;
}

static inline PF_Err DoRender(
	PF_InData		*in_data,
	PF_EffectWorld	*inputP,
	RenderData		*rendP,
	PF_OutData      *out_data,
	PF_LayerDef		*outputP) { 
	PF_Err err = PF_Err_NONE;
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	PF_WorldSuite2 *wsP = NULL;
	err = in_data->pica_basicP->AcquireSuite(kPFWorldSuite, kPFWorldSuiteVersion2, (const void **)&wsP);
	if (!err) {
		PF_Point origin;
		PF_Rect areaR;
		origin.h = (A_short)(in_data->output_origin_x);
		origin.v = (A_short)(in_data->output_origin_y);
		areaR.top = areaR.left = 0;
		areaR.right = 1;
		areaR.bottom = (A_short)outputP->height;

		// do high bit depth rendering in Premiere Pro
		if (in_data->appl_id == 'PrMr') {
			// get the Premiere pixel format suite
			PF_PixelFormatSuite1 *pfS = NULL;
			err = in_data->pica_basicP->AcquireSuite(kPFPixelFormatSuite, kPFPixelFormatSuiteVersion1, (const void **)&pfS);
			PrPixelFormat format = PrPixelFormat_BGRA_4444_8u;
			if (pfS) {
				pfS->GetPixelFormat(outputP, &format);
				A_long linesL = outputP->extent_hint.bottom - outputP->extent_hint.top;
				if (format == PrPixelFormat_BGRA_4444_8u) {
					rendP->format = PF_PixelFormat_ARGB32;
					rendP->bitDepth = 8;
					Process8_BGRA(in_data, 0, linesL, inputP, (AE_REFCON)rendP, outputP);
				} else if (format == PrPixelFormat_BGRA_4444_32f
					|| format == PrPixelFormat_BGRA_4444_32f_Linear
					) {
					// Premiere doesn't support IterateFloatSuite1, so we've rolled our own
					rendP->format = PF_PixelFormat_ARGB128;
					rendP->bitDepth = 32;
					ProcessFloat_BGRA(in_data, 0, linesL, inputP, (AE_REFCON)rendP, outputP);
				} else {
					//	Return error, because we don't know how to handle the specified pixel type
					return PF_Err_UNRECOGNIZED_PARAM_TYPE;
				}
				in_data->pica_basicP->ReleaseSuite(kPFPixelFormatSuite, kPFPixelFormatSuiteVersion1);
			}
		} else {
			// determine pixel format
			wsP->PF_GetPixelFormat(outputP, &rendP->format);
			switch (rendP->format) {
			case PF_PixelFormat_ARGB128:
				{
				rendP->bitDepth = 32;
				ERR(suites.IterateFloatSuite1()->iterate_origin(in_data, 0, outputP->height, inputP, 
					&areaR, &origin, (AE_REFCON)rendP, ProcessFloat, outputP));
				}
				break;
			case PF_PixelFormat_ARGB64:
				{
				rendP->bitDepth = 16;
				ERR(suites.Iterate16Suite1()->iterate_origin(in_data, 0, outputP->height, inputP, 
					&areaR, &origin, (AE_REFCON)rendP, Process16, outputP));
				}
				break;
			case PF_PixelFormat_ARGB32:
				{
				rendP->bitDepth = 8;
				ERR(suites.Iterate8Suite1()->iterate_origin(in_data, 0, outputP->height, inputP, 
					&areaR, &origin, (AE_REFCON)rendP, Process8, outputP));
				}
				break;
			default:
				err = PF_Err_BAD_CALLBACK_PARAM;
				break;
			}
		}
	}
	
	if (wsP) in_data->pica_basicP->ReleaseSuite(kPFWorldSuite, kPFWorldSuiteVersion2);
	
	return err;
}

static PF_Err Render(	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*outputP) {
	PF_Err err = PF_Err_NONE, err2 = PF_Err_NONE;


	PF_EffectWorld *inputP = &(params[FXID_INPUT]->u.ld);
	if (!inputP) return PF_Err_INTERNAL_STRUCT_DAMAGED;
	RenderData rd;
	rd.params.resize(NUM_PARAMS);

	// checkout the required params
	for (int i = 1; i < NUM_PARAMS; i++) {
		AEFX_CLR_STRUCT(rd.params[i]);
		PF_CHECKOUT_PARAM(in_data, i, in_data->current_time, in_data->time_step, in_data->time_scale, &rd.params[i]);
	}


	rd.in_data = in_data;
	rd.inputP = inputP;
	rd.outputP = outputP;
	rd.samp_pb.src = inputP;



	ERR(DoRender(in_data, inputP, &rd, out_data, outputP));

	// Always check in, no matter what the error condition!
	for (int i = 1; i < NUM_PARAMS; i++) {
		ERR2(PF_CHECKIN_PARAM(in_data, &rd.params[i]));
	}
	return err;
}

static PF_Err PreRender(
	PF_InData			*in_data,
	PF_OutData			*out_data,
	PF_PreRenderExtra	*extra) {
	PF_Err err = PF_Err_NONE;
	PF_RenderRequest req = extra->input->output_request;
	PF_CheckoutResult in_result;
	ERR(extra->cb->checkout_layer(in_data->effect_ref, FXID_INPUT, FXID_INPUT, 
		&req, in_data->current_time, in_data->time_step, in_data->time_scale, &in_result));
	UnionLRect(&in_result.result_rect, &extra->output->result_rect);
	UnionLRect(&in_result.max_result_rect, &extra->output->max_result_rect);						
	return err;
}

static PF_Err SmartRender(
	PF_InData				*in_data,
	PF_OutData				*out_data,
	PF_SmartRenderExtra		*extra) {
	PF_Err err = PF_Err_NONE, err2 = PF_Err_NONE;

	PF_EffectWorld *inputP = NULL, *outputP = NULL;

	// checkout input & output layers
	ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, FXID_INPUT, &inputP));
	if (err) return err;

	ERR(extra->cb->checkout_output(in_data->effect_ref, &outputP));
	if (err) return err;
	
	RenderData rd;
	rd.params.resize(NUM_PARAMS);

	// checkout the required params
	for (int i = 1; i < NUM_PARAMS; i++) {
		AEFX_CLR_STRUCT(rd.params[i]);
		PF_CHECKOUT_PARAM(in_data, i, in_data->current_time, in_data->time_step, in_data->time_scale, &rd.params[i]);
	}

	rd.in_data = in_data;
	rd.inputP = inputP;
	rd.outputP = outputP;
	rd.samp_pb.src = inputP;

	ERR(DoRender(in_data, inputP, &rd, out_data, outputP));

	// Always check in, no matter what the error condition!
	for (int i = 1; i < NUM_PARAMS; i++) {
		ERR2(PF_CHECKIN_PARAM(in_data, &rd.params[i]));
	}
	return err;
}

static std::string intToStr(const int i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}

static PF_Err About(
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*outputP) {
	PF_Err err = PF_Err_NONE;
	if (in_data->appl_id == 'PrMr') return err;
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	std::string desc = FX_DESCRIPTION;
// ********************* aescripts licensing specific code start *********************
	std::string licString = aescripts::getLicenseDataAsString(aescripts::licenseData);
	licString += "\rDays since first use: " + intToStr(aescripts::licenseData.numberOfDaysSinceFirstStart);
	if (strlen(aescripts::licenseData.startDate) > 3) {
		licString += "\rTime limited license only valid from " + std::string(aescripts::licenseData.startDate) + " to " + std::string(aescripts::licenseData.endDate);
	}
	suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg, "%s, v%d.%d\r%s\rRegistered to: %s", 
		FX_NAME, MAJOR_VERSION, MINOR_VERSION, desc.c_str(), licString.c_str());
// ********************* aescripts licensing specific code end *********************
	return err;
}

static PF_Err Register(	
	PF_InData		*in_data,
	PF_OutData		*out_data) {
	PF_Err err = PF_Err_NONE;
// ********************* aescripts licensing specific code start *********************
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	// determine product version
	std::string sProductVersion = aescripts::intToString(MAJOR_VERSION) + "." + aescripts::intToString(MINOR_VERSION) + "." + aescripts::intToString(BUG_VERSION);
	int reg_result = aescripts::showRegistrationDialog(&suites, aescripts::licenseData, in_data->appl_id != 'PrMr', sProductVersion.c_str());
// ********************* aescripts licensing specific code end *********************
	return err;
}

extern "C" DllExport PF_Err PluginDataEntryFunction(
	PF_PluginDataPtr inPtr,
	PF_PluginDataCB inPluginDataCallBackPtr,
	SPBasicSuite* inSPBasicSuitePtr,
	const char* inHostName,
	const char* inHostVersion) {

	PF_Err result = PF_Err_INVALID_CALLBACK;

	result = PF_REGISTER_EFFECT(
		inPtr,
		inPluginDataCallBackPtr,
		FX_NAME,
		FX_UNIQUEID,
		FX_CATEGORY,
		AE_RESERVED_INFO); // Reserved Info

	return result;
}

// uncomment the following line to write license info to a logfile on the desktop (useful for debugging purposes)
// #define AESCRIPTS_LIC_LOG

extern "C" DllExport PF_Err EffectMain(
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*outputP,
	void			*extraP) {

	PF_Err err = PF_Err_NONE;

	// ********************* aescripts licensing specific code start *********************
	if (aescripts::checkLicenseAE(cmd, in_data, out_data, aescripts::licenseData) != 0) return err;
	#ifdef AESCRIPTS_LIC_LOG
		aescripts::writeLicenseInfoToLogfile(in_data, cmd);
	#endif
	// ********************* aescripts licensing specific code end *********************

	try {
		switch (cmd) {

			case PF_Cmd_ABOUT: {
				err = About(in_data, out_data, params, outputP);
				break;
				}
			case PF_Cmd_GLOBAL_SETUP: {
				err = GlobalSetup(in_data, out_data, params, outputP);
				break;
				}
			case PF_Cmd_PARAMS_SETUP: {
				err = ParamsSetup(in_data, out_data, params, outputP);
				break;
				}
			case PF_Cmd_RENDER: {
				err = Render(in_data, out_data, params, outputP);
				break;
				}
			case PF_Cmd_SMART_PRE_RENDER: {
				err = PreRender(in_data, out_data, reinterpret_cast<PF_PreRenderExtra*>(extraP));
				break;
				}
			case PF_Cmd_SMART_RENDER: {
				err = SmartRender(in_data, out_data, reinterpret_cast<PF_SmartRenderExtra*>(extraP));
				break;
				}
			case PF_Cmd_DO_DIALOG: {
				err = Register(in_data, out_data);
				break;
				}
		}
	} catch(PF_Err &thrown_err) { 
		err = thrown_err; 
	} catch(...) { 
		err = PF_Err_INTERNAL_STRUCT_DAMAGED; 
	}
	return err;
}
