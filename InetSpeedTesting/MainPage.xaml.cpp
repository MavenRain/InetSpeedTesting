//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#define DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION ";"
using namespace InetSpeedTesting;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	InitializeComponent();
	DataContext = resultsVector;
}


void InetSpeedTesting::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Begin setup
	//------------------------------------------------------------------------------------------
	auto setup = [&]()
	{
		currentSpeed = 0.0;
		currentUploadBandwidth = 0.0;
		currentDownloadBandwidth = 0.0;
		payloadLength = 1024;
		retries = 4;
		resultsVector = ref new Vector<Datum^>();
		task_timeout_ms = 400;
		std::chrono::milliseconds timeout(task_timeout_ms);

		String^ _payloadChoices = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		aggregate = "";
		auto it = _payloadChoices->Begin();
		for (int i = 0; i < payloadLength; i++, aggregate += it[rand() % _payloadChoices->Length()]);

		_socketTcpWellKnownHostNames[0] = "google.com";
		_socketTcpWellKnownHostNames[1] = "bing.com";
		_socketTcpWellKnownHostNames[2] = "facebook.com";
		_socketTcpWellKnownHostNames[3] = "yahoo.com";
		
		for (auto i = 0; i < 4; i++)
		{
			clientSocket[i] = ref new StreamSocket();
			clientSocket[i]->Control->NoDelay = true;
			clientSocket[i]->Control->QualityOfService = SocketQualityOfService::LowLatency;
			clientSocket[i]->Control->KeepAlive = false;
		};

	};	
	
	//------------------------------------------------------------------------------------
	// End setup

	//Prepare continuation flow using several lambdas
	//-------------------------------------------------------------------------------------
	
	
	auto handleVoidException = [this](task<void> previousTask)
	{

		try { previousTask.get(); }
		catch (Platform::COMException^ e)
		{
			currentSpeed = 0.0;
			//retries--;
		}
		catch (task_canceled&)
		{
			currentSpeed = 0.0;
			//retries--;
		}
	};

	auto handleIntException = [this](task<unsigned int> previousTask)
	{
		try { previousTask.get(); }
		catch (Platform::COMException^ e)
		{
			currentSpeed = 0.0;
			retries--;
		}
		catch (task_canceled&)
		{
			currentSpeed = 0.0;
			retries--;
		}
	};

	auto procession = [&]()
	{
		setup();
		//_testList->ItemsSource = resultsVector;
		StreamSocket^ streamSocket = ref new StreamSocket();
		HostName^ hostName = ref new HostName("google.com");
		streamSocket->ConnectAsync(hostName, "80", SocketProtectionLevel::PlainSocket);
		DataWriter^ dataWriter = ref new DataWriter(streamSocket->OutputStream);
		dataWriter->WriteString(aggregate);
		create_task(dataWriter->StoreAsync()).then([](task<unsigned int> previousTask)
		{ try{ previousTask.get(); }
		catch (COMException^ ex) {}
	
		},task_continuation_context::use_current());
		/*
		for (; retries > 0;)
		{
			//tcs[retries - 1].cancel(timeout);
			HostName^ server = ref new HostName(_socketTcpWellKnownHostNames[retries - 1]);
			create_task(clientSocket[retries - 1]->ConnectAsync(server, "80"))
				.then([&]()
			{
				auto outputStream = clientSocket[retries - 1]->OutputStream;
				writer[retries - 1] = ref new DataWriter(outputStream);
				create_task(create_async([this]{writer[retries - 1]->WriteString(aggregate); })).then([this]{writer[retries - 1]->StoreAsync(); });
				currentSpeed += clientSocket[retries - 1]->Information->RoundTripTimeStatistics.Min / 1000000.0;
				currentUploadBandwidth += clientSocket[retries - 1]->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024;
				currentDownloadBandwidth += clientSocket[retries - 1]->Information->BandwidthStatistics.InboundBitsPerSecond / 1024;
				Datum^ datum = ref new Datum(resultsVector->Size, currentSpeed, currentUploadBandwidth, currentDownloadBandwidth);
				resultsVector->Append(datum);
			}).then(handleException);
			retries--;
		}
		*/
	};
	//---------------------------------------------------------------------------------------------------------------------------------------------
	//Lambdas created

	//Main execution flow
	//--------------------------
	procession();
}
