#include "AEConfig.h"

#ifndef AE_OS_WIN
	#include "AE_General.r"
#endif

resource 'PiPL' (16000, "AEPathLink", purgeable) {
	{
		Kind { AEGP },
		Name { "AEPathLink" },
		Category { "General Plugin" },
		Version { 0x30000 },  // 3.0.0
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 { "EntryPointFunc" },
	#endif
#else
	#ifdef AE_OS_MAC
		CodeMacIntel64 { "EntryPointFunc" },
		CodeMacARM64 { "EntryPointFunc" },
	#endif
#endif
		AE_PiPL_Version { 2, 0 },
		AE_Effect_Global_OutFlags { 0x02000000 }  // Panel flag
	}
};