namespace InetSpeedTesting
{
	public ref class Datum sealed
	{
		uint8 TestNum;
		float32 RTT;
		float32 UploadBW;
		float32 DownloadBW;
	public:
		Datum(float32, float32, float32, float32);
	};
}

using namespace InetSpeedTesting;