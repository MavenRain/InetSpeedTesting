//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "pplpp.h"
#include "Datum.h"

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace pplpp;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

namespace InetSpeedTesting
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
		property ListView^ TestList {ListView^ get(void); void set(ListView^); }

	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		bool _canceled = false;
		int retries = 4;
		long long task_timeout_ms = 400;
		Vector<Datum^>^ _resultsVector;
		String^ _payloadChoices;
	};
}
