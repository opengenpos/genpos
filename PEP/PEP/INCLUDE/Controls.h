/*Controls.h
*/

#ifdef _cplusplus
	extern "C" {
#endif

#if defined(PEP_CONTROLS)
#define CONTROL_DECLSPEC __declspec(dllexport)
#else
#define CONTROL_DECLSPEC __declspec(dllimport)
#endif

#ifdef _cplusplus
	}
#endif

#ifdef __cplusplus
	#define CONTROL_EXTERN_C extern "C"
#else
	#define CONTROL_EXTERN_C
#endif

CONTROL_EXTERN_C CONTROL_DECLSPEC LONG CheckLicense(VOID);
