#include <iostream>
#include <Windows.h>

#include <dshow.h>

#include <atlbase.h>
#include <atlcom.h>
#include <ks.h>
#include <ksproxy.h>
#include <ksmedia.h>

using namespace std;

int main()
{
    HRESULT hr = S_OK;

    KSPROPERTY_CAMERACONTROL_S cameraControl = { 0 };
    KSPROPERTY ksProp = { 0 };

    hr = CoInitialize(NULL);

    ICreateDevEnum* pSysDevEnum = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pSysDevEnum));
    if (FAILED(hr)) {
        if (hr == REGDB_E_CLASSNOTREG) cout << "REGDB_E_CLASSNOTREG";
        if (hr == CLASS_E_NOAGGREGATION) cout << "CLASS_E_NOAGGREGATION";
        if (hr == E_NOINTERFACE) cout << "E_NOINTERFACE";
        if (hr == E_POINTER) cout << "E_POINTER";
        CoUninitialize();
        return 0;
    }

    IEnumMoniker* pEnumCat = NULL;
    //AM_KSCATEGORY_CAPTURE CLSID_VideoInputDeviceCategory
    hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

    if (SUCCEEDED(hr)) {
        IMoniker* pMoniker = NULL;
        ULONG cFetch = 0;
        while (pEnumCat->Next(1, &pMoniker, &cFetch) == S_OK) {
            IPropertyBag* pPropBag = NULL;
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropBag);
            if (SUCCEEDED(hr)) {
                VARIANT varName;
                VariantInit(&varName);
                
                hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                if (SUCCEEDED(hr)) {
                    cout << "varname: " << wstring(varName.bstrVal, SysStringLen(varName.bstrVal)).c_str() << endl;
                }
                VariantClear(&varName);

                IBaseFilter* baseFilter = NULL;
                hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&baseFilter);
                if (SUCCEEDED(hr)) {
                    cout << "IBaseFilter" << endl;
                    IAMCameraControl* pICameraControl = NULL;
                    hr = baseFilter->QueryInterface(IID_IAMCameraControl, (void**)&pICameraControl);
                    if (SUCCEEDED(hr)) {
                        cout << "IAMCameraControl" << endl;
                        ULONG isSupported = 0;
                        LONG v =0 ;
                        LONG f =0;
                        LONG min, max, step, def, caps;
                        hr = pICameraControl->GetRange(KSPROPERTY_CAMERACONTROL_PRIVACY, &min, &max, &step, &def, &caps);
                        hr = pICameraControl->Get(KSPROPERTY_CAMERACONTROL_PRIVACY, &v, &f);
                        if (SUCCEEDED(hr)) {
                            cout << "Value: " << v << endl;
                            cout << "min: " << min << " max: " << max << endl;
                        }
                        pICameraControl->Release();
                    }

                    IKsPropertySet* pIKsProp = NULL;
                    hr = baseFilter->QueryInterface(IID_IKsPropertySet, (void**)&pIKsProp);
                    if (SUCCEEDED(hr)) {
                        cout << "IKsPropertySet" << endl;
                        DWORD supp = 0;
                        
                        DWORD dCamControl = KSPROPERTY_CAMERACONTROL_PAN;
                        while (dCamControl <= KSPROPERTY_CAMERACONTROL_PANTILT) {
                            hr = pIKsProp->QuerySupported(PROPSETID_VIDCAP_CAMERACONTROL, dCamControl, &supp);
                            if (SUCCEEDED(hr)) {
                                if (dCamControl == KSPROPERTY_CAMERACONTROL_PAN) cout << "KSPROPERTY_CAMERACONTROL_PAN" << endl;
                                else if (dCamControl == KSPROPERTY_CAMERACONTROL_TILT) cout << "KSPROPERTY_CAMERACONTROL_TILT" << endl;
                                else if (dCamControl == KSPROPERTY_CAMERACONTROL_ROLL) cout << "KSPROPERTY_CAMERACONTROL_ROLL" << endl;
                                else if (dCamControl == KSPROPERTY_CAMERACONTROL_ZOOM) cout << "KSPROPERTY_CAMERACONTROL_ZOOM" << endl;
                                else if (dCamControl == KSPROPERTY_CAMERACONTROL_EXPOSURE) cout << "KSPROPERTY_CAMERACONTROL_EXPOSURE" << endl;
                                else if (dCamControl == KSPROPERTY_CAMERACONTROL_IRIS) cout << "KSPROPERTY_CAMERACONTROL_IRIS" << endl;
                                else if (dCamControl == KSPROPERTY_CAMERACONTROL_FOCUS) cout << "KSPROPERTY_CAMERACONTROL_FOCUS" << endl;
                                else if (dCamControl == KSPROPERTY_CAMERACONTROL_SCANMODE) cout << "KSPROPERTY_CAMERACONTROL_SCANMODE" << endl;
                                else if (dCamControl == KSPROPERTY_CAMERACONTROL_PRIVACY) cout << "KSPROPERTY_CAMERACONTROL_PRIVACY" << endl;
                                else if (dCamControl == KSPROPERTY_CAMERACONTROL_PANTILT) cout << "KSPROPERTY_CAMERACONTROL_PANTILT" << endl;

                                ksProp.Set = PROPSETID_VIDCAP_CAMERACONTROL;
                                ksProp.Id = dCamControl;
                                ksProp.Flags = KSPROPERTY_TYPE_GET;

                                cameraControl.Property = ksProp;
                                cameraControl.Value = 0;
                                cameraControl.Flags = 0;
                                cameraControl.Capabilities = 0;

                                DWORD propRet = 0;
                                hr = pIKsProp->Get(PROPSETID_VIDCAP_CAMERACONTROL, dCamControl,
                                    NULL, 0,
                                    &cameraControl, sizeof(KSPROPERTY_CAMERACONTROL_S), &propRet);
                                if (SUCCEEDED(hr)) {
                                    cout << "Value: " << cameraControl.Value << endl;
                                }
                            }
                            dCamControl++;
                        }
                        pIKsProp->Release();
                    }
  
                    baseFilter->Release();
                }
                pPropBag->Release();
            }
            pMoniker->Release();
        }
        pEnumCat->Release();
    }

    pSysDevEnum->Release();
    CoUninitialize();
    system("pause");
}
