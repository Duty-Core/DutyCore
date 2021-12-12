#include "script.h"
#include "dllmain.h"
#include "MinLog.h"

namespace callofduty {
	CommonFunctions* script::GetCommonFunctions() {
		return ((CommonFunctions*)(dutycore::game::ModuleBase + 0x33477a0));
	}

	char* script::Scr_GetString(scriptInstance_t inst, unsigned int index) {
		return ((tScr_GetString)(dutycore::game::ModuleBase + 0x12ebaa0))(inst, index);
	}

	void script::Cbuf_AddText(LocalClientNum_t localClientNum, const char* text) {
		((tCbuf_AddText)(dutycore::game::ModuleBase + 0x20ec8b0))(localClientNum, text);
	}

	void script::GScr_AddDebugCommand(scriptInstance_t inst)
	{
		auto str = Scr_GetString(inst, 0);
		dutycore::main::Log << "Add Debug Command: " << str << "\n";
		Cbuf_AddText(LocalClientNum_t::LOCAL_CLIENT_0, str);
	}
}
