#include <windows.h>
#include <dbt.h>
#include <tchar.h>
#include <mmsystem.h>
#include <string>

// Generic low-level GUID for USB devices.
// Any device that communicates over USB (phone, mouse, flash drive, etc.)
// can trigger this interface notification.
const GUID GUID_USB_DEVICE = { 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };

const TCHAR g_szClassName[] = _T("USBSoundNotifierWindowClass");
const TCHAR g_ConfigFileName[] = _T("config.ini");
TCHAR g_SoundFilePath[MAX_PATH];

ULONGLONG g_LastPlayTime = 0; 
const ULONGLONG COOLDOWN_MS = 1000; 

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ReadConfiguration();
void PlayNotificationSound();

bool IsAbsolutePath(const std::wstring& path)
{
    return path.size() > 2 && path[1] == _T(':');
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    ReadConfiguration();

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = g_szClassName;
    if (!RegisterClassEx(&wc)) return 0;

    HWND hwnd = CreateWindowEx(0, g_szClassName, _T("USB Monitor"), 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
    if (!hwnd) return 0;

    // Register with the system so we receive notifications for low-level USB
    // device plug and unplug events.
    DEV_BROADCAST_DEVICEINTERFACE filter = {0};
    filter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter.dbcc_classguid = GUID_USB_DEVICE; 
    
    HDEVNOTIFY hDeviceNotify = RegisterDeviceNotification(hwnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

    if (hDeviceNotify == NULL) {
        MessageBox(NULL, _T("fail to register device notification!"), _T("Error"), MB_OK);
        return 0;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Unregister the device notification before the application exits.
    UnregisterDeviceNotification(hDeviceNotify);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DEVICECHANGE)
    {
        if (wParam == DBT_DEVICEARRIVAL)
        {
            // A non-null lParam indicates that device information is available.
            if (lParam != 0) 
            {
                PDEV_BROADCAST_HDR pHeader = (PDEV_BROADCAST_HDR)lParam;
                
                // Filter on DEVICEINTERFACE notifications instead of VOLUME
                // notifications so all USB device classes can be detected.
                if (pHeader->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                {
                    ULONGLONG currentTime = GetTickCount64();
                    if (currentTime - g_LastPlayTime > COOLDOWN_MS)
                    {
                        g_LastPlayTime = currentTime; 
                        
                        // Uncomment the MessageBox below if you want a simple
                        // popup while testing device detection.
                        // MessageBox(NULL, _T("detect anyt USB device to being inserted！"), _T("Notification"), MB_OK);
                        
                        PlayNotificationSound(); 
                    }
                }
            }
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ReadConfiguration()
{
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    std::wstring path = exePath;
    std::wstring exeDirectory = path.substr(0, path.find_last_of(_T("\\")) + 1);

    path = exeDirectory + g_ConfigFileName;
    GetPrivateProfileString(_T("Settings"), _T("SoundFile"), _T("usb.wav"), g_SoundFilePath, MAX_PATH, path.c_str());

    std::wstring configuredPath = g_SoundFilePath;
    if (!IsAbsolutePath(configuredPath))
    {
        configuredPath = exeDirectory + configuredPath;
        lstrcpyn(g_SoundFilePath, configuredPath.c_str(), MAX_PATH);
    }
}

void PlayNotificationSound()
{
    PlaySound(g_SoundFilePath, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}
