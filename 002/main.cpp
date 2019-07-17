#include <Windows.h>
#include <wrl/client.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <endpointvolume.h>
#include <cstdint>
#include <clocale>
#include <iostream>

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

	ComPtr<IMMDevice> p_device;
	hr = p_enumerator->GetDefaultAudioEndpoint(
		eRender,
		eConsole,
		&p_device
	);
	if(FAILED(hr))
	{
		cerr << "デフォルトオーディオエンドポイントデバイスの取得失敗" << endl;
		return ;
	}
	cout << "デフォルトオーディオエンドポイントデバイスの取得成功" << endl;

	ComPtr<IPropertyStore> p_property_store;
	hr = p_device->OpenPropertyStore(STGM_READ, &p_property_store);
	if(FAILED(hr))
	{
		cerr << "オーディオエンドポイントデバイスのプロパティ取得に失敗" << endl;
		return ;
	}

	PROPVARIANT property_value;
	PropVariantInit(&property_value);

	hr = p_property_store->GetValue(PKEY_Device_FriendlyName, &property_value);
	if(FAILED(hr))
	{
		cerr << "オーディオエンドポイントデバイスのプロパティ値取得に失敗" << endl;
	}

	wcout << property_value.pwszVal << endl;

	PropVariantClear(&property_value);

	ComPtr<IAudioEndpointVolume> p_volume;
	hr = p_device->Activate(__uuidof(*&p_volume), CLSCTX_ALL, nullptr, &p_volume);
	if(FAILED(hr))
	{
		cerr << "オーディオエンドポイントボリュームの取得失敗" << endl;
		return ;
	}

	float min_db = 0.0f;
	float max_db = 0.0f;
	float inc_db = 0.0f;
	hr = p_volume->GetVolumeRange(&min_db, &max_db, &inc_db);
	if(FAILED(hr))
	{
		cerr << "ボリュームレンジの取得失敗" << endl;
		return ;
	}

	cout << "Min: " << min_db << endl;
	cout << "Max: " << max_db << endl;
	cout << "Inc: " << inc_db << endl;

	float master_db = 0.0f;
	hr = p_volume->GetMasterVolumeLevel(&master_db);
	if(FAILED(hr))
	{
		cerr << "マスターボリュームレベル(dB)の取得失敗" << endl;
		return ;
	}

	float master_level = 0.0f;
	hr = p_volume->GetMasterVolumeLevelScalar(&master_level);
	if(FAILED(hr))
	{
		cerr << "マスターボリュームレベルの取得失敗" << endl;
		return ;
	}

	cout << "Master(dB): " << master_db << endl;
	cout << "Master    : " << master_level << endl;
}
