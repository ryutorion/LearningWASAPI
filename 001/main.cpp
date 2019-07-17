#include <Windows.h>
#include <wrl/client.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
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

	ComPtr<IMMDeviceCollection> p_collection;
	hr = p_enumerator->EnumAudioEndpoints(
		eRender,
		DEVICE_STATE_ACTIVE,
		&p_collection
	);
	if(FAILED(hr))
	{
		cerr << "オーディオエンドポイントデバイスの列挙失敗" << endl;
		return ;
	}
	cout << "オーディオエンドポイントデバイスの列挙成功" << endl;

	uint32_t count = 0;
	hr = p_collection->GetCount(&count);
	if(FAILED(hr))
	{
		cerr << "オーディオエンドポイントデバイス数の取得失敗" << endl;
		return ;
	}
	cout << count << "個のオーディオエンドポイントデバイスが見つかりました．" << endl;

	for(uint32_t i = 0; i < count; ++i)
	{
		ComPtr<IMMDevice> p_device;
		hr = p_collection->Item(i, &p_device);
		if(FAILED(hr))
		{
			cerr << i << "番目のオーディオエンドポイントデバイス取得に失敗" << endl;
			return ;
		}

		ComPtr<IPropertyStore> p_property_store;
		hr = p_device->OpenPropertyStore(STGM_READ, &p_property_store);
		if(FAILED(hr))
		{
			cerr << i << "番目のオーディオエンドポイントデバイスのプロパティ取得に失敗" << endl;
			return ;
		}

		PROPVARIANT property_value;
		PropVariantInit(&property_value);

		hr = p_property_store->GetValue(PKEY_Device_FriendlyName, &property_value);
		if(FAILED(hr))
		{
			cerr << i << "番目のオーディオエンドポイントデバイスのプロパティ値取得に失敗" << endl;
		}

		wcout << "[" << i << "]\t" << property_value.pwszVal << endl;

		PropVariantClear(&property_value);
	}
}
