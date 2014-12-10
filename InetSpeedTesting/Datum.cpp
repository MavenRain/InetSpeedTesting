#include "pch.h"
#include "Datum.h"
InetSpeedTesting::Datum::Datum(float32 num, float32 rtt, float32 upload, float32 download) :TestNum(num), RTT(rtt), UploadBW(upload), DownloadBW(download){}