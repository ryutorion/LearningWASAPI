#include "AudioEndpointDeviceNotificationClient.h"
#include <functiondiscoverykeys_devpkey.h>
#include <wrl/client.h>
#include <sstream>
#include <iostream>

using namespace std;
using namespace Microsoft::WRL;

extern "C" PROPERTYKEY PKEY_MonitorOutput;

namespace
{
	wstring to_wstring(ComPtr<IMMDevice> & p_device)
	{
		wstring result;

		ComPtr<IPropertyStore> p_property_store;
		HRESULT hr = p_device->OpenPropertyStore(STGM_READ, &p_property_store);
		if(FAILED(hr))
		{
			cerr << "オーディオエンドポイントデバイスのプロパティ取得に失敗" << endl;
			return result;
		}

		PROPVARIANT property_value;
		PropVariantInit(&property_value);

		hr = p_property_store->GetValue(PKEY_Device_FriendlyName, &property_value);
		if(FAILED(hr))
		{
			cerr << "オーディオエンドポイントデバイスのプロパティ値取得に失敗" << endl;
			return result;
		}

		result = property_value.pwszVal;

		PropVariantClear(&property_value);

		return result;
	}
}

HRESULT STDMETHODCALLTYPE AudioEndpointDeviceNotificationClient::QueryInterface(REFIID riid, void ** ppvObject)
{
	if(riid == __uuidof(IUnknown))
	{
		AddRef();
		*ppvObject = static_cast<IUnknown *>(this);
	}
	else if(riid == __uuidof(IMMNotificationClient))
	{
		AddRef();
		*ppvObject = static_cast<IMMNotificationClient *>(this);
	}
	else
	{
		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG STDMETHODCALLTYPE AudioEndpointDeviceNotificationClient::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

ULONG STDMETHODCALLTYPE AudioEndpointDeviceNotificationClient::Release(void)
{
	auto count = InterlockedDecrement(&mReferenceCount);
	if(count == 0)
	{
		delete this;
	}

	return count;
}

HRESULT STDMETHODCALLTYPE AudioEndpointDeviceNotificationClient::OnDeviceStateChanged(
	_In_ LPCWSTR pwstrDeviceId,
	_In_ DWORD dwNewState
)
{
	if(!mpEnumerator)
	{
		return S_OK;
	}

	ComPtr<IMMDevice> p_device;
	mpEnumerator->GetDevice(pwstrDeviceId, &p_device);

	wcout << L"State Changed : " << to_wstring(p_device) << endl;
	wcout << "\t";

	struct { DWORD state; const wchar_t * const name; } states[]
	{
		{ DEVICE_STATE_ACTIVE, L"ACTIVE" },
		{ DEVICE_STATE_DISABLED, L"DISABLED" },
		{ DEVICE_STATE_NOTPRESENT, L"NOTPRESENT" },
		{ DEVICE_STATE_UNPLUGGED, L"UNPLUGGED" },
	};
	for(auto & s : states)
	{
		if(dwNewState & s.state)
		{
			wcout << s.name << " ";
		}
	}

	wcout << endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioEndpointDeviceNotificationClient::OnDeviceAdded(
	_In_ LPCWSTR pwstrDeviceId
)
{
	if(!mpEnumerator)
	{
		return S_OK;
	}

	ComPtr<IMMDevice> p_device;
	mpEnumerator->GetDevice(pwstrDeviceId, &p_device);

	wcout << L"Added : " << to_wstring(p_device) << endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioEndpointDeviceNotificationClient::OnDeviceRemoved(
	_In_ LPCWSTR pwstrDeviceId
)
{
	if(!mpEnumerator)
	{
		return S_OK;
	}

	ComPtr<IMMDevice> p_device;
	mpEnumerator->GetDevice(pwstrDeviceId, &p_device);

	wcout << L"Removed : " << to_wstring(p_device) << endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioEndpointDeviceNotificationClient::OnDefaultDeviceChanged(
	_In_ EDataFlow flow,
	_In_ ERole role,
	_In_ LPCWSTR pwstrDefaultDeviceId
)
{
	if(!mpEnumerator)
	{
		return S_OK;
	}
	ComPtr<IMMDevice> p_device;
	mpEnumerator->GetDevice(pwstrDefaultDeviceId, &p_device);

	switch(flow)
	{
	case eRender:
		wcout << L"Render ";
		break;
	case eCapture:
		wcout << L"Capture ";
		break;
	case eAll:
		wcout << L"All ";
		break;
	default:
		wcout << L"Unknown ";
		break;
	}

	switch(role)
	{
	case eConsole:
		wcout << "Console ";
		break;
	case eMultimedia:
		wcout << "Multimedia ";
		break;
	case eCommunications:
		wcout << "Communication ";
		break;
	default:
		wcout << "Unknown ";
		break;
	}

	wcout << to_wstring(p_device) << endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE AudioEndpointDeviceNotificationClient::OnPropertyValueChanged(
	_In_ LPCWSTR pwstrDeviceId,
	_In_ const PROPERTYKEY key
)
{
	if(!mpEnumerator)
	{
		return S_OK;
	}

	wchar_t buffer[256];
	swprintf_s(
		buffer,
		_countof(buffer),
		L"{%8.8x-%4.4x-%4.4x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x}#%8.8x",
		key.fmtid.Data1,
		key.fmtid.Data2,
		key.fmtid.Data3,
		key.fmtid.Data4[0],
		key.fmtid.Data4[1],
		key.fmtid.Data4[2],
		key.fmtid.Data4[3],
		key.fmtid.Data4[4],
		key.fmtid.Data4[5],
		key.fmtid.Data4[6],
		key.fmtid.Data4[7],
		key.pid
	);

	ComPtr<IMMDevice> p_device;
	mpEnumerator->GetDevice(pwstrDeviceId, &p_device);

	wcout << "Property Changed : " << to_wstring(p_device) << endl;
	wcout << "\t" << buffer << endl;

	return S_OK;
}

void AudioEndpointDeviceNotificationClient::SetDeviceEnumerator(Microsoft::WRL::ComPtr<IMMDeviceEnumerator>& p_enumerator)
{
	mpEnumerator = p_enumerator;
}
