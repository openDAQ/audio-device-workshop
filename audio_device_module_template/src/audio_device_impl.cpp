#include <audio_device_module/audio_device_impl.h>
#include <audio_device_module/audio_channel_impl.h>
#include <opendaq/device_info_factory.h>
#include <opendaq/device_type_factory.h>
#include <opendaq/device_domain_factory.h>
#include <miniaudio/miniaudio.h>

BEGIN_NAMESPACE_AUDIO_DEVICE_MODULE

AudioDeviceImpl::AudioDeviceImpl(const std::shared_ptr<ma_utils::MiniaudioContext>& maContext,
                                 const ma_device_id& id,
                                 const ContextPtr& ctx,
                                 const ComponentPtr& parent,
                                 const StringPtr& localId)
    : GenericDevice(ctx, parent, localId)
    , maId(id)
    , maContext(maContext)
    , started(false)
{
    loggerComponent = context.getLogger().getOrAddComponent("AudioDeviceModule");

    initProperties();
    createAudioChannel();

	// TASK: Uncomment `start()` once channel is created.
	// start();

    setDeviceInfo();

    // TASK: Create device domain with a tick resolution of 1 / maDevice.sampleRate and seconds as the unit.
    DeviceDomainPtr deviceDomain;
    setDeviceDomain(deviceDomain);
}

AudioDeviceImpl::~AudioDeviceImpl()
{
    auto lock = getAcquisitionLock2();
    stop();
}

/*
 * Creates a Device type that is used by the module manager to link connection strings to modules.
 */
DeviceTypePtr AudioDeviceImpl::CreateType()
{
    // TASK: Create an audio device type with prefix "miniaudio", ID "AudioDevice" and name "Audio Capture Device".

    DeviceTypePtr audioDeviceType;
    return audioDeviceType;
}

/*
 * Creates a device information object that carries metadata of the device. Device information objects
 * are used during discovery.
 */
DeviceInfoPtr AudioDeviceImpl::CreateDeviceInfo(const std::shared_ptr<ma_utils::MiniaudioContext>& /*maContext*/, const ma_device_info& /*deviceInfo*/)
{
    //auto connectionString = ma_utils::getConnectionStringFromId(maContext->getPtr()->backend, deviceInfo.id);
    //auto name = deviceInfo.name;
    
    // TASK: Create a device info object and populate its connection string, name, and device type.
    //       Use the commented fields above.

    DeviceInfoConfigPtr info;
    return info;
}

/*
 * Returns the number of ticks (time units) that have passed since the device's epoch. The
 * audio device only has a relative time, so there is no origin (epoch).
 */
uint64_t AudioDeviceImpl::onGetTicksSinceOrigin()
{
    // TASK: Call and return the output of the Audio Channel's `getSamplesGenerated` method.

    return 0;
}

/*
 * Configures a set of properties that can be configured by users of the device. When changed,
 * each property triggers an `onWrite` event that can be subscribed to.
 */
void AudioDeviceImpl::initProperties()
{
    // TASK: - Create an Integer selection property named "SampleRate" with unit "Hz".
    //       - Call `sampleRateChanged` in the `onPropertyValueWrite` event.
    
    auto availableSRSelectionList = List<IInteger>(11025, 22050, 44100);
    PropertyPtr sampleRateProperty;

    //sampleRateProperty.getOnPropertyValueWrite() +=
    //    [this](PropertyObjectPtr&, PropertyValueEventArgsPtr& args)
    //    {
    //    };
    //objPtr.addProperty(sampleRateProperty);
    
    this->sampleRate = availableSRSelectionList[0];
}

/*
 * Creates an audio channel IO object that holds the value and time signals of the audio device.
 */
void AudioDeviceImpl::createAudioChannel()
{
    // TASK: Create and add audio channel using `createAndAddChannel`.
    //       Cache the newly created channel in the `channel` variable.

    channel = nullptr;
}

/*
 * Each device has cached information about itself that can be retrieved through the API. This
 * method sets the internal information object.
 */
void AudioDeviceImpl::setDeviceInfo()
{
    //ma_device_info info;
    //ma_utils::getMiniAudioDeviceInfo(&maDevice, &info);
    
    // TASK: Configure internal `this->deviceInfo` variable with the Audio Device info, replacing the temporary one.

    this->deviceInfo = DeviceInfo("temp");
}

void AudioDeviceImpl::sampleRateChanged(uint32_t sampleRate)
{
    stop();
    this->sampleRate = sampleRate;
    start();
}

/*
 * Callback triggered by the miniaudio library when audio device data is available.
 */
static void MiniaudioDataCallback(ma_device* /*pDevice*/, void*, const void* /*pInput*/, ma_uint32 /*frameCount*/)
{
    //auto audioChannel = static_cast<AudioChannelImpl*>(pDevice->pUserData);

    // TASK: Call the Audio Channel method generatePackets with pInput and frameCount arguments.
}

/*
 * Initializes and starts the miniaudio device, setting up the data available callback. Prepares
 * the internal audio device channel configuration.
 */
void AudioDeviceImpl::start()
{
    if (started || disposeCalled)
        return;
    
    ma_device_config devConfig = ma_device_config_init(ma_device_type_capture);
    devConfig.capture.pDeviceID = &maId;
    devConfig.capture.channels = 1;
    devConfig.capture.format = ma_format_f32;
    devConfig.sampleRate = sampleRate;
    devConfig.dataCallback = MiniaudioDataCallback;
    devConfig.pUserData = reinterpret_cast<void*>(channel.getObject());

    if (!ma_utils::initAudioDevice(maContext->getPtr(), &devConfig, &maDevice, loggerComponent))
        return;

    // TASK: Call Audio Channel methods `reset` and `configure`.

    // auto audioChannel = channel.asPtr<IAudioChannel>();

    if (!ma_utils::startAudioDevice(&maDevice, loggerComponent))
        return;

    started = true;
}

void AudioDeviceImpl::stop()
{
    if (!started)
        return;

    ma_utils::uninitializeDevice(&maDevice);
    started = false;
}

END_NAMESPACE_AUDIO_DEVICE_MODULE
