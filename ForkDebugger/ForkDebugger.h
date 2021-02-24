#include <iostream>
#include <cstdint>
#include <memory>
#include <mutex>
#include "ADAM_AOController.h"
#include "ADAM_ROController.h"


class ForkDebugger
{
    enum ControlState { WaitingForTrigger, Triggering, Successful, FailRejection, FailWindowTime, End };

public:
	ForkDebugger();
	~ForkDebugger();
	void run();

private:
    void SetupMiddleWare();
    void doLowerFork();
    void doRaiseFork();
    void doStopFork();
    void doReadFork();

private:
    std::shared_ptr<DBWPalletCommon::Adam_AOController> sptr_ADAM_AOController_Fork_Ctrl;
    std::shared_ptr<DBWPalletCommon::Adam_ROController> sptr_ADAM_ROController_Fork_Ctrl;
};
