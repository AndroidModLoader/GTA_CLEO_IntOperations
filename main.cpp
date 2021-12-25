#include <mod/amlmod.h>
#include <mod/logger.h>

#include "icleo.h"
ICLEO* cleo = nullptr;

MYMOD(net.alexblade.rusjj.intops, CLEO IntOperations, 1.0, Alexander Blade & RusJJ)
BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.cleolib, 2.0.1.1)
END_DEPLIST()

#define __print_to_log(__str)	cleo->PrintToCleoLog(__str); logger->Info(__str)
#define __reg_opcode	cleo->RegisterOpcode
#define __reg_func		cleo->RegisterOpcodeFunction

#define __handler_params void *handle, uint32_t *ip, uint16_t opcode, const char *name
#define __op_name_match(x) opcode == OP_##x || strcmp(name, NAME_##x) == 0

#define __fatal_unk_op_or_func() fatal("%s: unknown opcode %04X, name '%s'", __FUNCTION__, opcode, name);

#define __if_func_verify_list() \
	if (opcode == 0x1000) {						\
		if (cleo->IsParamListEnd(*ip))			\
			cleo->SkipParamListEnd(ip);			\
		else									\
			fatal("%s unexpected param", name); \
	}
#define __reg_op_func(x, h) __reg_opcode(OP_##x, h); __reg_func(NAME_##x, h);



const uint16_t OP_INTOPS_AND	 = 0x0B10;			// 0B10=3,%3d% = %1d% AND %2d%	
const uint16_t OP_INTOPS_OR		 = 0x0B11;			// 0B11=3,%3d% = %1d% OR %2d%	
const uint16_t OP_INTOPS_XOR	 = 0x0B12;			// 0B12=3,%3d% = %1d% XOR %2d%	
const uint16_t OP_INTOPS_NOT	 = 0x0B13;			// 0B13=2,%2d% = NOT %1d%		
const uint16_t OP_INTOPS_MOD	 = 0x0B14;			// 0B14=3,%3d% = %1d% MOD %2d%	
const uint16_t OP_INTOPS_SHR	 = 0x0B15;			// 0B15=3,%3d% = %1d% SHR %2d%	
const uint16_t OP_INTOPS_SHL	 = 0x0B16;			// 0B16=3,%3d% = %1d% SHL %2d%	
const uint16_t OP_INTOPS_INP_AND = 0x0B17;			// 0B17=2,%1d% _&= %2d%			
const uint16_t OP_INTOPS_INP_OR	 = 0x0B18;			// 0B18=2,%1d% |= %2d%				
const uint16_t OP_INTOPS_INP_XOR = 0x0B19;			// 0B19=2,%1d% ^= %2d%				
const uint16_t OP_INTOPS_INP_NOT = 0x0B1A;			// 0B1A=1,~ %1d%					
const uint16_t OP_INTOPS_INP_MOD = 0x0B1B;			// 0B1B=2,%1d% %= %2d%				
const uint16_t OP_INTOPS_INP_SHR = 0x0B1C;			// 0B1C=2,%1d% >>= %2d%			
const uint16_t OP_INTOPS_INP_SHL = 0x0B1D;			// 0B1D=2,%1d% <<= %2d%			

const char *NAME_INTOPS_AND		= "Intops::And";	// 1000: "Intops::And" i32 _& i32 to resi32
const char *NAME_INTOPS_OR		= "Intops::Or";		// 1000: "Intops::Or" i32 | i32 to resi32
const char *NAME_INTOPS_XOR		= "Intops::Xor";	// 1000: "Intops::Xor" i32 ^ i32 to resi32
const char *NAME_INTOPS_NOT		= "Intops::Not";	// 1000: "Intops::Not" i32 to resi32
const char *NAME_INTOPS_MOD		= "Intops::Mod";	// 1000: "Intops::Mod" i32 % i32 to resi32
const char *NAME_INTOPS_SHR		= "Intops::Shr";	// 1000: "Intops::Shr" i32 >> i32 to resi32
const char *NAME_INTOPS_SHL		= "Intops::Shl";	// 1000: "Intops::Shl" i32 << i32 to resi32
const char *NAME_INTOPS_INP_AND	= "Intops::&=";		// 1000: "Intops::&=" resi32 with i32
const char *NAME_INTOPS_INP_OR	= "Intops::|=";		// 1000: "Intops::|=" resi32 with i32
const char *NAME_INTOPS_INP_XOR	= "Intops::^=";		// 1000: "Intops::^=" resi32 with i32
const char *NAME_INTOPS_INP_NOT	= "Intops::~";		// 1000: "Intops::~" resi32
const char *NAME_INTOPS_INP_MOD	= "Intops::%=";		// 1000: "Intops::%=" resi32 with i32
const char *NAME_INTOPS_INP_SHR	= "Intops::>>=";	// 1000: "Intops::>>=" resi32 with i32
const char *NAME_INTOPS_INP_SHL	= "Intops::<<=";	// 1000: "Intops::<<=" resi32 with i32

void fatal(const char *fmt, ...)
{
	char str[256];
	va_list lst;
	va_start(lst, fmt);
	vsprintf(str, fmt, lst);
	va_end(lst);
	__print_to_log(str);
	exit(1);
}

void intops__i32_i32_resi32(__handler_params)
{
	int param1 = cleo->ReadParam(handle)->i;
	int param2 = cleo->ReadParam(handle)->i;
	int *result = &cleo->GetPointerToScriptVar(handle)->i;
	if (__op_name_match(INTOPS_AND)) *result = param1 & param2; else
	if (__op_name_match(INTOPS_OR))  *result = param1 | param2; else
	if (__op_name_match(INTOPS_XOR)) *result = param1 ^ param2; else
	if (__op_name_match(INTOPS_MOD)) *result = param1 % param2; else
	if (__op_name_match(INTOPS_SHR)) *result = param1 >> param2; else
	if (__op_name_match(INTOPS_SHL)) *result = param1 << param2; else
		__fatal_unk_op_or_func();
	__if_func_verify_list();
}

void intops__i32_resi32(__handler_params)
{
	int param = cleo->ReadParam(handle)->i;
	int *result = &cleo->GetPointerToScriptVar(handle)->i;
	if (__op_name_match(INTOPS_NOT)) *result = ~param; else
		__fatal_unk_op_or_func();
	__if_func_verify_list();
}

void intops__resi32_i32(__handler_params)
{
	int *var = &cleo->GetPointerToScriptVar(handle)->i;
	int param = cleo->ReadParam(handle)->i;
	if (__op_name_match(INTOPS_INP_AND)) *var &= param; else
	if (__op_name_match(INTOPS_INP_OR))  *var |= param; else
	if (__op_name_match(INTOPS_INP_XOR)) *var ^= param; else
	if (__op_name_match(INTOPS_INP_MOD)) *var %= param; else
	if (__op_name_match(INTOPS_INP_SHR)) *var >>= param; else
	if (__op_name_match(INTOPS_INP_SHL)) *var <<= param; else
		__fatal_unk_op_or_func();
	__if_func_verify_list();
}

void intops__resi32(__handler_params)
{
	int *var = &cleo->GetPointerToScriptVar(handle)->i;
	if (__op_name_match(INTOPS_INP_NOT)) *var = ~*var; else
		__fatal_unk_op_or_func();
	__if_func_verify_list();
}

extern "C" void OnModLoad()
{
    logger->SetTag("[CLEO] IntOperations");
    if(!(cleo = (ICLEO*)GetInterface("CLEO")))
    {
        logger->Error("CLEO is not installed!");
        return;
    }

    __reg_op_func(INTOPS_AND, intops__i32_i32_resi32);	
	__reg_op_func(INTOPS_OR,  intops__i32_i32_resi32);
	__reg_op_func(INTOPS_XOR, intops__i32_i32_resi32);

	__reg_op_func(INTOPS_NOT, intops__i32_resi32);

	__reg_op_func(INTOPS_MOD, intops__i32_i32_resi32);
	__reg_op_func(INTOPS_SHR, intops__i32_i32_resi32);
	__reg_op_func(INTOPS_SHL, intops__i32_i32_resi32);

	__reg_op_func(INTOPS_INP_AND, intops__resi32_i32);
	__reg_op_func(INTOPS_INP_OR,  intops__resi32_i32);
	__reg_op_func(INTOPS_INP_XOR, intops__resi32_i32);

	__reg_op_func(INTOPS_INP_NOT, intops__resi32);

	__reg_op_func(INTOPS_INP_MOD, intops__resi32_i32);
	__reg_op_func(INTOPS_INP_SHR, intops__resi32_i32);
	__reg_op_func(INTOPS_INP_SHL, intops__resi32_i32);
	
	__print_to_log("IntOperations has been loaded!");
}