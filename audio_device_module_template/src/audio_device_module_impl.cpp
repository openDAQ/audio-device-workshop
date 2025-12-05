#include <audio_device_module/audio_device_module_impl.h>
#include <audio_device_module/audio_device_impl.h>
#include <audio_device_module/version.h>
#include <coretypes/version_info_factory.h>
#include <miniaudio/miniaudio.h>
#include <opendaq/custom_log.h>

BEGIN_NAMESPACE_AUDIO_DEVICE_MODULE

AudioDeviceModule::AudioDeviceModule(const ContextPtr& context)
    : Module("AudioDeviceModule",
             VersionInfo(AUDIO_DEVICE_MODULE_MAJOR_VERSION, AUDIO_DEVICE_MODULE_MINOR_VERSION, AUDIO_DEVICE_MODULE_PATCH_VERSION),
             context,
             "AudioDeviceModule")
      , maContext(std::make_shared<ma_utils::MiniaudioContext>())
      , deviceIndex(0)
{
}

/*
 * Searches for available audio devices using the miniaudio library.
 * Returns the list available device information objects, representing the
 * available audio capture devices.
 */
ListPtr<IDeviceInfo> AudioDeviceModule::onGetAvailableDevices()
{
    ma_device_info* pCaptureDeviceInfos;
    ma_uint32 captureDeviceCount;
    
    std::scoped_lock lock(sync);
    ma_utils::getMiniAudioDevices(&pCaptureDeviceInfos, &captureDeviceCount, maContext->getPtr());

    // TASK: - Populate available devices      
    //       - Use AudioDeviceImpl::CreateDeviceInfo (must first be implemented)
    
    auto availableDevices = List<IDeviceInfo>();
    return availableDevices;
}

/*
 * Devices created with this module all share the same device type. There can be
 * multiple instances of a single device type created by a module. This function
 * returns the miniaudio device type that has a "miniaudio://" prefix.
 */
DictPtr<IString, IDeviceType> AudioDeviceModule::onGetAvailableDeviceTypes()
{
    // TASK: - Populate available device types
    //       - Use AudioDeviceImpl::CreateType (must first be implemented)
    
    auto availableDeviceTypes = Dict<IString, IDeviceType>();
    return availableDeviceTypes;
}

/*
 * When creating devices with a given connection string, the module manager asks
 * the module that owns a device type with the connection string prefix to create
 * a device of said type, given a connection string. The connection strings of the
 * miniaudio device module are in the format of "miniaudio://audio_device_id".
 */
DevicePtr AudioDeviceModule::onCreateDevice(const StringPtr& connectionString,
                                            const ComponentPtr& /*parent*/,
                                            const PropertyObjectPtr& /*config*/)
{
    auto id = ma_utils::getIdFromConnectionString(connectionString);

    std::scoped_lock lock(sync);
    std::string localId = fmt::format("MiniAudioDev{}", deviceIndex++);

    // TASK: - Create audio device using `createWithImplementation<IDevice, AudioDeviceImpl>(...)`
    //         - createWithImplementation forwards the arguments to the AudioDeviceImpl constructor method
    //       - Store the created device in `devicePtr` and return it
    
    DevicePtr devicePtr;
    return devicePtr;
}

END_NAMESPACE_AUDIO_DEVICE_MODULE
