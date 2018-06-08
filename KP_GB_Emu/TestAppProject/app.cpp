#include "app.h"

[System::STAThreadAttribute]
void Main(array<System::String^>^ args)
{
	System::Windows::Forms::Application::EnableVisualStyles();
	System::Windows::Forms::Application::SetCompatibleTextRenderingDefault(true);
	TestAppProject::app form;
	System::Windows::Forms::Application::Run(%form);
}
