#pragma once

#include <string>

void SetPortNumber(int t);
int StartServerUDP(System::ComponentModel::BackgroundWorker^ worker, bool localIP);


