#pragma once

namespace Palleon
{
	class CDx11ContextManager
	{
	public:
		typedef Framework::Win32::CComPtr<ID3D11DeviceContext> DeviceContextPtr;

								CDx11ContextManager();
		virtual					~CDx11ContextManager();

		ID3D11DeviceContext*	GetCurrentDeviceContext() const;
		void					SetCurrentDeviceContext(const DeviceContextPtr&);

	private:
		typedef std::map<DWORD, DeviceContextPtr> DeviceContextMap;

		DeviceContextMap		m_deviceContexts;
	};
}
