using namespace System;

#include "Form1.h"

using namespace System::Windows::Forms;
[STAThread]


int main() {	
	bool createdNew;
	System::Threading::Mutex^ mutex = gcnew System::Threading::Mutex(true, "IL-2 Dials Server.exe", createdNew);
    if (createdNew) {
        Application::EnableVisualStyles();
        Application::SetCompatibleTextRenderingDefault(false);
        Application::Run(gcnew Il2Dials::Form1());
        //Garbage collection will get rid of mutex reference if we don't use this line
        GC::KeepAlive(mutex);
    }
    else {

        MessageBox::Show("IL-2 Dials Server already running!");
       
    }

	return 0;
}
