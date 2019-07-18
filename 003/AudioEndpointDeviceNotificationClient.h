#ifndef AUDIO_ENDPOINT_DEVICE_NOTIFICATION_CLIENT_H_INCLUDED
#define AUDIO_ENDPOINT_DEVICE_NOTIFICATION_CLIENT_H_INCLUDED

#include <mmdeviceapi.h>
#include <wrl/client.h>

class AudioEndpointDeviceNotificationClient: public IMMNotificationClient
{
public:
	// IUnknown—R—ˆ
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) override;
	ULONG STDMETHODCALLTYPE AddRef(void) override;
	ULONG STDMETHODCALLTYPE Release(void) override;

	// IMMNotificationClient—R—ˆ
	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(_In_  LPCWSTR pwstrDeviceId, _In_  DWORD dwNewState) override;
	HRESULT STDMETHODCALLTYPE OnDeviceAdded(_In_  LPCWSTR pwstrDeviceId) override;
	HRESULT STDMETHODCALLTYPE OnDeviceRemoved(_In_  LPCWSTR pwstrDeviceId) override;
	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(_In_  EDataFlow flow, _In_  ERole role, _In_  LPCWSTR pwstrDefaultDeviceId) override;
	HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(_In_  LPCWSTR pwstrDeviceId, _In_  const PROPERTYKEY key) override;

public:

	void SetDeviceEnumerator(Microsoft::WRL::ComPtr<IMMDeviceEnumerator> & p_enumerator);

private:
	ULONG mReferenceCount = 0;
	Microsoft::WRL::ComPtr<IMMDeviceEnumerator> mpEnumerator;
};

#endif // AUDIO_ENDPOINT_DEVICE_NOTIFICATION_CLIENT_H_INCLUDED
