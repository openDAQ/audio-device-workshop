#include <audio_device_module/audio_channel_impl.h>
#include <opendaq/signal_factory.h>
#include <opendaq/packet_factory.h>
#include <opendaq/range_factory.h>

BEGIN_NAMESPACE_AUDIO_DEVICE_MODULE

AudioChannelImpl::AudioChannelImpl(const ContextPtr& ctx, const ComponentPtr& parent, const StringPtr& localId)
    : ChannelImpl(FunctionBlockType("AudioChannel", "Audio", ""), ctx, parent, localId)
    , samplesGenerated(0)
{
    loggerComponent = context.getLogger().getOrAddComponent("AudioDeviceModule");

    // LockingStrategy::InheritLock means that the methods such as `getAcquisitionLock2` will lock
    // the parent's mutex instead of the components own.
    objPtr.asPtr<IPropertyObjectInternal>().setLockingStrategy(LockingStrategy::InheritLock);

    initSignals();
}

AudioChannelImpl::~AudioChannelImpl() = default;

/*
 * Sets the time signal description for its sample rate to match that of the miniaudio device.
 * Also configures the output signal name.
 */
void AudioChannelImpl::configure(const ma_device& device)
{
    this->name = device.capture.name;

    // TASK: - Set output signal name
    //       - Set time signal descriptor:
    //         - Int64 sample type,
    //         - Tick resolution 1 / device.sampleRate,
    //         - Unit "s"
    //         - Linear rule delta=1, start=0
}

void AudioChannelImpl::reset()
{
    samplesGenerated = 0;
}

/*
 * Creates time and value packets containing the timestamps and the data received from the miniaudio
 * device. The packets are sent through the value and time signals; the samplesGenerated variable
 * is updated with the sample count.
 */
void AudioChannelImpl::generatePackets(const void* /*data*/, size_t /*sampleCount*/)
{
    try
    {
        auto lock = getAcquisitionLock2();

        // TASK: - Create domain and data packets. 
        //       - Use samplesGenerated for the domain packet offset.
        //       - Copy `data` to the data packet's raw data (Memcpy command: std::memcpy(packetData, data, sampleCount * sizeof(float));)
        //       - Send the time and value packets
        //       - Increment samplesGenerated

    }
    catch (const std::exception& e)
    {
        LOG_W("Miniaudio device failed to generate packets: {}", e.what());
        throw;
    }
}

uint64_t AudioChannelImpl::getSamplesGenerated()
{
    auto lock = getAcquisitionLock2();
    return samplesGenerated;
}

/*
 * Creates two signals. The value signal carrying audio data, and the time signal that carries timestamp
 * data. The value signals is always set to carry 32-bit floating point data, whereas the time signal
 * is configured dynamically in the `configure` method, as its sampling rate can change based on user
 * input.
 */
void AudioChannelImpl::initSignals()
{
    // TASK: - Create Audio Signal with a data descriptor (Float32-type data, -1.0, 1.0 value range)
    //       - Create an invisible Audio Time Signal with no descriptor configured
    //       - Set the domain signal reference
    //
    // NOTE: Use the `createAndAddSignal` helper method.

    timeSignal = nullptr;
    outputSignal = nullptr;
}

END_NAMESPACE_AUDIO_DEVICE_MODULE
