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
		std::wstring wString(_trialsBox->Text->Data());
		std::wstringstream wss;
		wss << wString;
		unsigned int _trials;
		wss >> _trials;
		Array<StreamSocket^>^ streamSocket = ref new Array<StreamSocket^>(_trials);
		for (unsigned int i = 0; i < streamSocket->Length; i++) streamSocket[i] = ref new StreamSocket();
		Array<String^>^ hostNames = ref new Array<String^>{ "google.com","abc.com","bing.com","msn.com"};
		Vector<Datum^>^ resultsVector = ref new Vector<Datum^>();
		_testList->ItemsSource = resultsVector;
		auto procession = [streamSocket, resultsVector, hostNames, _trials, this]() mutable
		{
			return create_task(streamSocket[0]->ConnectAsync(ref new HostName(hostNames[0]), ref new String(L"80")))
				.then([streamSocket, resultsVector, hostNames, _trials, this]() mutable
			{
				for (unsigned int i = 1; i < streamSocket->Length; i++) streamSocket[i]->ConnectAsync(ref new HostName(hostNames[(i+1) % hostNames->Length]), ref new String(L"80"));
				String^ possible("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
				String^ aggregate = "";
				unsigned int payloadSize = 9600;
				for (unsigned int i = 0; i < payloadSize; i++, aggregate += possible->Begin()[rand() % possible->Length()]);
				Array<DataWriter^>^ writer = ref new Array<DataWriter^>(_trials);
				for (unsigned int i = 0; i < writer->Length; i++)
				{
					writer[i] = ref new DataWriter(streamSocket[i]->OutputStream);
					writer[i]->WriteString(aggregate);
					//if (i!=0) writer[i]->StoreAsync();
				}
				auto beginTime = clock();
				create_task(writer[0]->StoreAsync()).then([beginTime, streamSocket, writer, resultsVector, payloadSize, _trials, this](unsigned int previousTask) mutable
				{
					auto endTime = clock();
					auto uploadBandwidth = payloadSize / (float32)(endTime - beginTime) / 10 * CLOCKS_PER_SEC / 1024;
					float32 speedTotal = streamSocket[0]->Information->RoundTripTimeStatistics.Min / 1000000.0F;
					if (streamSocket[0]->Information->BandwidthStatistics.OutboundBandwidthPeaked == true) resultsVector->Append(ref new Datum(1, speedTotal, streamSocket[0]->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024.0F));
					else resultsVector->Append(ref new Datum(1, speedTotal, uploadBandwidth));
					for (unsigned int i = 1; i < streamSocket->Length; i++)
					{
						speedTotal += streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F;
						resultsVector->Append(ref new Datum(i + 1, streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F, streamSocket[i]->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024.0F));
					}
					create_task(KnownFolders::PicturesLibrary->CreateFileAsync(L"sample.txt",CreationCollisionOption::ReplaceExisting))
						.then([speedTotal, streamSocket, _trials, this](StorageFile^ storageFile)
					{
						String^ statistics = "Mean Round Trip Time: " + speedTotal / (float32)_trials + "\r\n";
						for (unsigned int i = 0; i < _trials; i++) statistics += "Trial #" + i + ": " + streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F + "\r\n";
						FileIO::WriteTextAsync(storageFile, statistics);
						_meanRTT->Text = (speedTotal / (float32)_trials).ToString();
					});
				}).then([](task<void> previousTask) {try { previousTask.get(); } catch (COMException^ ex) { }});
				delete writer;
			});
		};
		procession();
	};
	// Main execution flow
	//--------------------
	flow();
}


