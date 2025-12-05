#define MA_COINIT_VALUE COINIT_MULTITHREADED
#define MA_NO_NODE_GRAPH
#define MA_NO_RESOURCE_MANAGER
#define MINIAUDIO_IMPLEMENTATION

#include <audio_device_module/miniaudio_utils.h>
#include <coretypes/exceptions.h>
#include <opendaq/custom_log.h>
#include <iterator>
#include <string_view>

#ifdef MA_WIN32
    #include <Windows.h>
#endif

BEGIN_NAMESPACE_AUDIO_DEVICE_MODULE

namespace ma_utils
{

class ComGuard
{
public:
    ComGuard()
    {
#ifdef MA_WIN32
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        shouldUninit = (hr == S_OK);
#endif
    }

    ~ComGuard()
    {
#ifdef MA_WIN32
        if (shouldUninit)
            CoUninitialize();
#endif
    }

private:
    bool shouldUninit = false;
};

MiniaudioContext::MiniaudioContext()
{
    if (ma_context_init(nullptr, 0, nullptr, &context) != MA_SUCCESS)
        throw std::runtime_error("Failed to initialize miniaudio context");
}

MiniaudioContext::~MiniaudioContext()
{
    ma_context_uninit(&context);
}

ma_context* MiniaudioContext::getPtr()
{
    return &context;
}

namespace
{

#ifdef MA_WIN32
std::wstring utf8ToWide(std::string_view input)
{
    if (input.empty())
        return {};

    const int size = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        input.data(),
        static_cast<int>(input.size()),
        nullptr,
        0);

    if (size <= 0)
        DAQ_THROW_EXCEPTION(InvalidParameterException, "Invalid UTF-8 in WASAPI device id.");

    std::wstring output(size, L'\0');

    const int converted = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        input.data(),
        static_cast<int>(input.size()),
        output.data(),
        size);

    if (converted != size)
        DAQ_THROW_EXCEPTION(InvalidParameterException, "Failed to decode WASAPI device id.");

    return output;
}

std::string wideToUtf8(const wchar_t* input)
{
    if (input == nullptr || input[0] == L'\0')
        return {};

    const int inputSize = static_cast<int>(std::wcslen(input));

    const int size = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        input,
        inputSize,
        nullptr,
        0,
        nullptr,
        nullptr);

    if (size <= 0)
        DAQ_THROW_EXCEPTION(GeneralErrorException, "Failed to encode WASAPI device id.");

    std::string output(size, '\0');

    const int converted = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        input,
        inputSize,
        output.data(),
        size,
        nullptr,
        nullptr);

    if (converted != size)
        DAQ_THROW_EXCEPTION(GeneralErrorException, "Failed to encode WASAPI device id.");

    return output;
}
#endif
}

ma_device_id getIdFromConnectionString(const std::string& connectionString)
{
    const std::string prefix = "miniaudio://";

    if (auto pos = connectionString.find(prefix); pos == std::string::npos)
        DAQ_THROW_EXCEPTION(InvalidParameterException, "Miniaudio device connection strings must have the \"miniaudio://\" prefix.");

    auto rest = connectionString.substr(prefix.size(), std::string::npos);
    auto pos = rest.find("/");
    if (pos == std::string::npos)
        DAQ_THROW_EXCEPTION(InvalidParameterException, "Miniaudio connection string lacks a \"/\" delimiter.");

    auto backendId = rest.substr(0, pos);
    auto id = rest.substr(pos + 1, std::string::npos);

    ma_device_id deviceId{};
    if (backendId == "wasapi")
    {
#ifdef MA_WIN32
        std::wstring wasapiId = utf8ToWide(id);
        if (wasapiId.size() >= std::size(deviceId.wasapi))
            DAQ_THROW_EXCEPTION(InvalidParameterException, "WASAPI id too long.");

        std::memcpy(deviceId.wasapi, wasapiId.c_str(), (wasapiId.size() + 1) * sizeof(wchar_t));
#else
        DAQ_THROW_EXCEPTION(InvalidParameterException, "WASAPI ids are only supported on Windows.");
#endif
    }
    else if (backendId == "dsound")
    {
        for (size_t i = 0; i < 16; ++i)
        {
            auto subStr = id.substr(i * 2, 2);
            deviceId.dsound[i] = static_cast<ma_uint8>(std::stoul(subStr, nullptr, 16));
        }
    }
    else if (backendId == "winmm")
    {
        deviceId.winmm = std::stoul(id, nullptr, 16);
    }
    else if (backendId == "coreaudio")
    {
        std::strcpy(deviceId.coreaudio, id.c_str());
    }
    else if (backendId == "alsa")
    {
        std::strcpy(deviceId.alsa, id.c_str());
    }
    else if (backendId == "pulseaudio")
    {
        std::strcpy(deviceId.pulse, id.c_str());
    }
    else if (backendId == "jack")
    {
        deviceId.jack = std::stoul(id, nullptr, 16);
    }

    return deviceId;
}

std::string getConnectionStringFromId(ma_backend backend, ma_device_id id)
{
    std::string connectionString = "miniaudio://";

    switch (backend)
    {        
    	case ma_backend_wasapi:
            connectionString += "wasapi/";
#ifdef MA_WIN32
            connectionString += wideToUtf8(id.wasapi);
#else
            DAQ_THROW_EXCEPTION(GeneralErrorException, "WASAPI backend is only supported on Windows.");
#endif
            break;

        case ma_backend_dsound:
            connectionString += "dsound/";
            for (unsigned char& i : id.dsound)
                connectionString += fmt::format("{:02x}", i);
            break;

        case ma_backend_winmm:
            connectionString += "winmm/";
            connectionString += fmt::format("{:x}", id.winmm);
            break;

        case ma_backend_coreaudio:
            connectionString += "coreaudio/";
            connectionString += id.coreaudio;
            break;

        case ma_backend_sndio:
            connectionString += "sndio/";
            break;

        case ma_backend_audio4:
            connectionString += "audio4/";
            break;

        case ma_backend_oss:
            connectionString += "oss/";
            break;

        case ma_backend_pulseaudio:
            connectionString += "pulseaudio/";
            connectionString += id.pulse;
            break;

        case ma_backend_alsa:
            connectionString += "alsa/";
            connectionString += id.alsa;
            break;

        case ma_backend_jack:
            connectionString += "jack/";
            connectionString += fmt::format("{:x}", id.jack);
            break;

        case ma_backend_aaudio:
            connectionString += "aaudio/";
            break;

        case ma_backend_opensl:
            connectionString += "opensl/";
            break;

        default:
            connectionString += "unknown/";
    }

    return connectionString;
}

void getMiniAudioDevices(ma_device_info** ppCaptureDeviceInfos, ma_uint32* pCaptureDeviceCount, ma_context* maContext)
{
    ComGuard guard;
    ma_result result = ma_context_get_devices(maContext, nullptr, nullptr, ppCaptureDeviceInfos, pCaptureDeviceCount);
    if (result != MA_SUCCESS)
        DAQ_THROW_EXCEPTION(GeneralErrorException, "Miniaudio failed to retrieve device information: {}", ma_result_description(result));
}

void getMiniAudioDeviceInfo(ma_device* pDevice, ma_device_info* pDeviceInfo)
{
    ComGuard guard;
    ma_result result = ma_device_get_info(pDevice, ma_device_type_capture, pDeviceInfo);
    if (result != MA_SUCCESS)
        DAQ_THROW_EXCEPTION(CreateFailedException, "Failed to get Miniaudio device information: {}", ma_result_description(result));
}

bool initAudioDevice(ma_context* pContext, const ma_device_config* pConfig, ma_device* pDevice, const LoggerComponentPtr& loggerComponent)
{
    ComGuard guard;
    ma_result result;
    if ((result = ma_device_init(pContext, pConfig, pDevice)) != MA_SUCCESS)
    {
        LOG_W("Miniaudio device init failed: {}", ma_result_description(result));
        return false;
    }

    return true;
}

void uninitializeDevice(ma_device* pDevice)
{
    ComGuard guard;
    ma_device_uninit(pDevice);
}

bool startAudioDevice(ma_device* pDevice, const LoggerComponentPtr& loggerComponent)
{
    ComGuard guard;
    ma_result result;
    if ((result = ma_device_start(pDevice)) != MA_SUCCESS)
    {
        LOG_W("Miniaudio device start failed: {}", ma_result_description(result));
        ma_device_uninit(pDevice);
        return false;
    }

    return true;
}

}

END_NAMESPACE_AUDIO_DEVICE_MODULE
