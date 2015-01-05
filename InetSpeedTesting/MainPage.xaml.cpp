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
		if (_trials < 1) _trials = 1;
		Array<StreamSocket^>^ streamSocket = ref new Array<StreamSocket^>(_trials);
		for (unsigned int i = 0; i < _trials; i++) streamSocket[i] = ref new StreamSocket();
		Array<String^>^ hostNames = ref new Array<String^>{ "google.com","abc.com","bing.com","msn.com"};
		Vector<Datum^>^ resultsVector = ref new Vector<Datum^>();
		_testList->ItemsSource = resultsVector;
		auto procession = [streamSocket, resultsVector, hostNames, _trials, this]() mutable
		{
			return create_task(streamSocket[0]->ConnectAsync(ref new HostName(hostNames[0]), ref new String(L"80")))
				.then([streamSocket, resultsVector, hostNames, _trials, this]() mutable
			{
				for (unsigned int i = 1; i < _trials; i++) streamSocket[i]->ConnectAsync(ref new HostName(hostNames[(i+1) % hostNames->Length]), ref new String(L"80"));
				String^ possible("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
				String^ aggregate = "";
				unsigned int payloadSize = 1400;
				for (unsigned int i = 0; i < payloadSize; i++, aggregate += possible->Begin()[rand() % possible->Length()]);
				Array<DataWriter^>^ writer = ref new Array<DataWriter^>(_trials);
				for (unsigned int i = 0; i < _trials; i++)
				{
					writer[i] = ref new DataWriter(streamSocket[i]->OutputStream);
					writer[i]->WriteString(aggregate);
					if (i!=0) writer[i]->StoreAsync();
				}
				create_task(writer[0]->StoreAsync()).then([streamSocket, writer, resultsVector, payloadSize, _trials, this](unsigned int previousTask) mutable
				{
					float32 speedTotal = 0;
					float32 variance = 0;
					for (unsigned int i = 0; i < _trials; i++)
					{
						speedTotal += streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F;
						resultsVector->Append(ref new Datum(i + 1, streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F, streamSocket[i]->Information->BandwidthStatistics.OutboundBitsPerSecond / 1024.0F / 1024.0F));
					}
					for (unsigned int i = 0; i < _trials; i++) variance += (streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F - speedTotal / _trials) * (streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F - speedTotal / _trials);
					variance /= _trials - 1;
					create_task(KnownFolders::PicturesLibrary->CreateFileAsync(L"sample.txt",CreationCollisionOption::ReplaceExisting))
						.then([speedTotal, variance, streamSocket, _trials, this](StorageFile^ storageFile)
					{
						String^ statistics = "Mean Round Trip Time: " + speedTotal / (float32)_trials + "\r\n" + "Round Trip Time Variance: " + variance + "\r\n";
						for (unsigned int i = 0; i < _trials; i++) statistics += "Trial #" + i + ": " + streamSocket[i]->Information->RoundTripTimeStatistics.Min / 1000000.0F + "\r\n";
						FileIO::WriteTextAsync(storageFile, statistics);
						_meanRTT->Text = (speedTotal / (float32)_trials).ToString();
						_varianceRTT->Text = variance.ToString();
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


