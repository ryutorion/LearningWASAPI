#include <Windows.h>
#include <wrl/client.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <endpointvolume.h>
#include <cstdint>
#include <clocale>
#include <iostream>
#include "AudioEndpointDeviceNotificationClient.h"

using namespace std;
using namespace Microsoft::WRL;

void sample();

int main(int argc, char * argv[])
{
	// オーディオエンドポイントデバイス名にASCII以外が含まれる場合に
	// 正しく表示するためのロケール設定
	setlocale(LC_ALL, ".OCP");

	cout << "START " << TARGET_NAME << endl;

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if(FAILED(hr))
	{
		cerr << "COMの初期化失敗" << endl;
		return -1;
	}
	cout << "COMの初期化成功" << endl;

	sample();

	CoUninitialize();

	cout << "END " << TARGET_NAME << endl;

	return 0;
}

void sample()
{
	ComPtr<IMMDeviceEnumerator> p_enumerator;
	HRESULT hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		nullptr,
		CLSCTX_ALL,
		IID_PPV_ARGS(&p_enumerator)
	);
	if(FAILED(hr))
	{
		cerr << "MMDeviceEnumeratorの生成失敗" << endl;
		return ;
	}
	cout << "MMDeviceEnumeratorの生成成功" << endl;

	ComPtr<AudioEndpointDeviceNotificationClient> p_notification(new AudioEndpointDeviceNotificationClient);
	p_notification->SetDeviceEnumerator(p_enumerator);

	p_enumerator->RegisterEndpointNotificationCallback(p_notification.Get());

	while(true)
	{
		Sleep(100);
	}

	p_enumerator->UnregisterEndpointNotificationCallback(p_notification.Get());
}
