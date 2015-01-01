//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	InitializeComponent();
}


void InetSpeedTesting::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	
	auto flow = [this]
	{
		StreamSocket^ socket = ref new StreamSocket();
		Array<String^>^ hostNames = ref new Array<String^>{ "google.com","bing.com","facebook.com","yahoo.com" };
		unsigned int hostNamesSize = 4;
		Vector<Datum^>^ resultsVector = ref new Vector<Datum^>();
		_testList->ItemsSource = resultsVector;
		auto procession = [socket, resultsVector, hostNames](unsigned int hosts) mutable
		{
			return create_task(socket->ConnectAsync(ref new HostName(hostNames[hosts]), ref new String(L"80")))
				.then([socket, resultsVector, hosts]() mutable
			{
				float32 speed = socket->Information->RoundTripTimeStatistics.Min / 1000000.0F;
				String^ possible("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
				String^ aggregate = "";
				unsigned int payloadSize = 4800;
				for (unsigned int i = 0; i < payloadSize; i++, aggregate += possible->Begin()[rand() % possible->Length()]);
				DataWriter^ dataWriter = ref new DataWriter(socket->OutputStream);
				dataWriter->WriteString(aggregate);
				auto beginTime = clock();
				create_task(dataWriter->StoreAsync()).then([beginTime, speed, socket, resultsVector, payloadSize, hosts](unsigned int previousTask) mutable
				{
					auto endTime = clock();
					auto uploadBandwidth = payloadSize / (float32)(endTime - beginTime) / 10 * CLOCKS_PER_SEC / 1024;
					if (socket->Information->BandwidthStatistics.OutboundBandwidthPeaked == true) resultsVector->Append(ref new Datum(hosts, speed, socket->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024.0F));
					else resultsVector->Append(ref new Datum(hosts, speed, uploadBandwidth));
				});
			});
		};
		//procession(0);
		procession(2);
	};

	// Main execution flow
	//--------------------
	flow();
}


