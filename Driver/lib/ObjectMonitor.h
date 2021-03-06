#include <wdm.h>
/*
POB_PRE_OPERATION_CALLBACK PobPreOperationCallbackProcess;
POB_POST_OPERATION_CALLBACK PobPostOperationCallbackProcess;


POB_PRE_OPERATION_CALLBACK PobPreOperationCallbackThread;
POB_POST_OPERATION_CALLBACK PobPostOperationCallbackThread;

POB_PRE_OPERATION_CALLBACK PobPreOperationCallbackDesktop;
POB_POST_OPERATION_CALLBACK PobPostOperationCallbackDesktop;
*/

OB_PREOP_CALLBACK_STATUS PobPreOperationCallbackProcess(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);
void PobPostOperationCallbackProcess(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation);

OB_PREOP_CALLBACK_STATUS PobPreOperationCallbackThread(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);
void PobPostOperationCallbackThread(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation);

OB_PREOP_CALLBACK_STATUS PobPreOperationCallbackDesktop(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);
void PobPostOperationCallbackDesktop(PVOID RegistrationContext, POB_POST_OPERATION_INFORMATION OperationInformation);

typedef struct RegistrationContextStruct {

	short dummy;

} RegistrationContextStruct, *PRegistrationContextStruct;
