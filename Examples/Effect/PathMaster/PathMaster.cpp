#include "PathMaster.h"

static PF_Err 
About (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err err = PF_Err_NONE;
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,
								"%s v%d.%d\r%s",
								STR(StrID_Name), 
								MAJOR_VERSION, 
								MINOR_VERSION, 
								STR(StrID_Description));
	return err;
}

static PF_Err 
GlobalSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data)
{	
	PF_Err err = PF_Err_NONE;

	out_data->my_version = PF_VERSION(	MAJOR_VERSION, 
										MINOR_VERSION,
										BUG_VERSION, 
										STAGE_VERSION, 
										BUILD_VERSION);
		
	out_data->out_flags = 	PF_OutFlag_SEQUENCE_DATA_NEEDS_FLATTENING	|
							PF_OutFlag_DEEP_COLOR_AWARE;

	out_data->out_flags2 =	PF_OutFlag2_SUPPORTS_THREADED_RENDERING |
							PF_OutFlag2_SUPPORTS_GET_FLATTENED_SEQUENCE_DATA;

	return err;
}

static PF_Err 
SequenceSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data)
{
	PF_Err				err = PF_Err_NONE,
						err2 = PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	//	Here's how to get and set the color of each mask attached to a layer.
	if (!err) {
		AEGP_LayerH 	layerH 		= NULL;
		A_long 			mask_countL = 0;
		AEGP_MaskRefH 	mask_refH 	= NULL;
		AEGP_ColorVal   old_color, new_color;

		ERR(suites.PFInterfaceSuite1()->AEGP_GetEffectLayer(in_data->effect_ref, &layerH));

		ERR(suites.MaskSuite5()->AEGP_GetLayerNumMasks(layerH, &mask_countL));

		for (A_long iL = 0; !err && (iL < mask_countL); ++iL){
			ERR(suites.MaskSuite5()->AEGP_GetLayerMaskByIndex(layerH, iL, &mask_refH));

			if (mask_refH){
				ERR(suites.MaskSuite5()->AEGP_GetMaskColor(mask_refH, &old_color));
				if (!err){
	        		new_color.redF		= 1 - old_color.greenF;
	        		new_color.greenF	= 1 - old_color.blueF;
	        		new_color.blueF		= 1 - old_color.redF;

					// This seems to cause recursion when called from an effect like this
	        		ERR(suites.MaskSuite5()->AEGP_SetMaskColor(mask_refH, &new_color));
				}

	       		ERR2(suites.MaskSuite5()->AEGP_DisposeMask(mask_refH));
			}
		}
	}

	// Create sequence data
	PF_Handle			seq_dataH =	suites.HandleSuite1()->host_new_handle(sizeof(Unflat_Seq_Data));

	if (seq_dataH){
		Unflat_Seq_Data	*seqP = reinterpret_cast<Unflat_Seq_Data*>(suites.HandleSuite1()->host_lock_handle(seq_dataH));

		if (seqP){
			AEFX_CLR_STRUCT(*seqP);

			// We need to allocate some non-flat memory for this arbitrary string.
			seqP->stringP = new A_char[strlen(STR(StrID_Really_Long_String)) + 1];

			//	WARNING: The following assignments are not safe for cross-platform use.
			//	Production code would enforce byte order consistency, across platforms.

			if (seqP->stringP){
				suites.ANSICallbacksSuite1()->strcpy(seqP->stringP, STR(StrID_Really_Long_String));
			}
		
			seqP->fixed_valF		= 123;
			seqP->flatB				= FALSE;
			out_data->sequence_data = seq_dataH;

			suites.HandleSuite1()->host_unlock_handle(seq_dataH);
		}
	} else {	// whoa, we couldn't allocate sequence data; bail!
		err = PF_Err_OUT_OF_MEMORY;
	}
	return err;
}

static PF_Err
ParamsSetup(
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[])
{
	PF_Err		err		= PF_Err_NONE;
	PF_ParamDef	def;	

	AEFX_CLR_STRUCT(def);
	def.param_type = PF_Param_PATH;
	def.uu.id = PATH_DISK_ID;
	PF_STRCPY(def.name, STR(StrID_PathName)); 
	def.u.path_d.dephault	= 0;

	PF_ADD_PARAM(in_data, -1, &def);

	AEFX_CLR_STRUCT(def);
	PF_ADD_COLOR(	STR(StrID_ColorName), 
					PF_MAX_CHAN8,	// Red channel set to max
					0,
					0,
					COLOR_DISK_ID);
	
	AEFX_CLR_STRUCT(def);						
  	PF_ADD_FIXED (	STR(StrID_X_Feather),
  					FEATHER_VALID_MIN,
  					FEATHER_VALID_MAX,
  					FEATHER_MIN,
  					FEATHER_MAX,
  					FEATHER_DFLT,
  					1,
  					0,
  					0,
  					X_FEATHER_DISK_ID);
					
	AEFX_CLR_STRUCT(def);
  	PF_ADD_FIXED (	STR(StrID_Y_Feather),
  					FEATHER_VALID_MIN,
  					FEATHER_VALID_MAX,
  					FEATHER_MIN,
  					FEATHER_MAX,
  					FEATHER_DFLT,
  					1,
  					0,
  					0,
  					Y_FEATHER_DISK_ID);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOX(	STR(StrID_CheckName), 
						STR(StrID_CheckDetail), 
						FALSE,
						0, 
						DOWNSAMPLE_DISK_ID);
	
	AEFX_CLR_STRUCT(def);
  	PF_ADD_FIXED (	STR(StrID_OpacityName),
  					OPACITY_VALID_MIN,
  					OPACITY_VALID_MAX,
  					OPACITY_VALID_MIN,
  					OPACITY_VALID_MAX,
  					OPACITY_DFLT,
  					OPACITY_PRECISION,
  					PF_ValueDisplayFlag_PERCENT,
  					0,
  					OPACITY_DISK_ID);

	AEFX_CLR_STRUCT(def);
	PF_ADD_COLOR(STR(StrID_OutputColorName),
		0,		// Default to black
		0,
		0,
		OUTPUT_COLOR_DISK_ID);
	
	out_data->num_params = PATHMASTER_NUM_PARAMS;

	return err;
}

static PF_Err
Render(
	PF_InData* in_data,
	PF_OutData* out_data,
	PF_ParamDef* params[],
	PF_LayerDef* output)
{
	PF_Err				err = PF_Err_NONE;
	PF_EffectWorld* inputP = NULL;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	// Get input layer
	inputP = &params[PATHMASTER_INPUT]->u.ld;

	// Simply copy input to output without any modifications
	if (in_data->quality == PF_Quality_HI) {
		ERR(suites.WorldTransformSuite1()->copy_hq(in_data->effect_ref,
			inputP,
			output,
			0,
			0));
	}
	else {
		ERR(suites.WorldTransformSuite1()->copy(in_data->effect_ref,
			inputP,
			output,
			0,
			0));
	}

	return err;
}

extern "C" DllExport
PF_Err PluginDataEntryFunction2(
	PF_PluginDataPtr inPtr,
	PF_PluginDataCB2 inPluginDataCallBackPtr,
	SPBasicSuite* inSPBasicSuitePtr,
	const char* inHostName,
	const char* inHostVersion)
{
	PF_Err result = PF_Err_INVALID_CALLBACK;

	result = PF_REGISTER_EFFECT_EXT2(
		inPtr,
		inPluginDataCallBackPtr,
		"PathMaster", // Name
		"ADBE PathMaster", // Match Name
		"Sample Plug-ins", // Category
		AE_RESERVED_INFO, // Reserved Info
		"EffectMain",	// Entry point
		"https://www.adobe.com");	// support URL

	return result;
}


PF_Err
EffectMain(
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extra)
{
	PF_Err		err = PF_Err_NONE;		
	return err;
}
