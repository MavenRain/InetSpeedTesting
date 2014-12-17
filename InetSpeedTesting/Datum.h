namespace InetSpeedTesting
{
	public ref class Datum sealed
	{
		uint16 TestNum;
		float32 RTT;
		float32 UploadBW;
		float32 DownloadBW;
	public:
		Datum(uint16, float32, float32, float32);
	};
}

using namespace InetSpeedTesting;