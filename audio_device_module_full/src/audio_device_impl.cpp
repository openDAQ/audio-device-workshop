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

	start();
   
    setDeviceInfo();
    setDeviceDomain(DeviceDomain(Ratio(1, maDevice.sampleRate), "", Unit("s", -1, "seconds", "time")));
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
    return DeviceType("AudioDevice", "Audio capture device", "", "miniaudio");
}

/*
 * Creates a device information object that carries metadata of the device. Device information objects
 * are used during discovery.
 */
DeviceInfoPtr AudioDeviceImpl::CreateDeviceInfo(const std::shared_ptr<ma_utils::MiniaudioContext>& maContext, const ma_device_info& deviceInfo)
{
    auto connectionString = ma_utils::getConnectionStringFromId(maContext->getPtr()->backend, deviceInfo.id);
    DeviceInfoConfigPtr info = DeviceInfo(connectionString, deviceInfo.name);
    info.setDeviceType(CreateType());
    return info;
}

/*
 * Returns the number of ticks (time units) that have passed since the device's epoch. The
 * audio device only has a relative time, so there is no origin (epoch).
 */
uint64_t AudioDeviceImpl::onGetTicksSinceOrigin()
{
    return channel.asPtr<IAudioChannel>()->getSamplesGenerated();
}

/*
 * Configures a set of properties that can be configured by users of the device. When changed,
 * each property triggers an `onWrite` event that can be subscribed to.
 */
void AudioDeviceImpl::initProperties()
{
    auto availableSRSelection = List<IInteger>(11025, 22050, 44100);
    auto selectionProperty = IntPropertyBuilder("SampleRate", 11025)
                             .setUnit(Unit("Hz"))
                             .setIsIntegerValueSelection(true)
                             .setSelectionValues(availableSRSelection)
                             .build();

    selectionProperty.getOnPropertyValueWrite() += 
        [this](PropertyObjectPtr& /*obj*/, PropertyValueEventArgsPtr& args) 
        {
            this->sampleRateChanged(args.getValue());
        };

    objPtr.addProperty(selectionProperty);
    this->sampleRate = availableSRSelection[0];
}

/*
 * Creates an audio channel IO object that holds the value and time signals of the audio device.
 */
void AudioDeviceImpl::createAudioChannel()
{
    channel = createAndAddChannel<AudioChannelImpl>(this->ioFolder, "AudioChannel");
}

/*
 * Each device has cached information about itself that can be retrieved through the API. This
 * method sets the internal information object.
 */
void AudioDeviceImpl::setDeviceInfo()
{
    ma_device_info info;
    ma_utils::getMiniAudioDeviceInfo(&maDevice, &info);
    this->deviceInfo = CreateDeviceInfo(maContext, info);
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
static void MiniaudioDataCallback(ma_device* pDevice, void*, const void* pInput, ma_uint32 frameCount)
{
    auto audioChannel = static_cast<AudioChannelImpl*>(pDevice->pUserData);
    audioChannel->generatePackets(pInput, frameCount);
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
    
    channel.asPtr<IAudioChannel>()->reset();
    channel.asPtr<IAudioChannel>()->configure(maDevice);

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
