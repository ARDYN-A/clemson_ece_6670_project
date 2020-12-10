/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DSP_GradProject_FilterAudioProcessor::DSP_GradProject_FilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    state = new juce::AudioProcessorValueTreeState(*this, nullptr);

    juce::NormalisableRange<float> cutoffRange(20.0f, 20000.0f, 1.0f);
    juce::NormalisableRange<float> resRange(1.0f, 5.0f);
    juce::NormalisableRange<float> menuItemRange(0, 2);

    cutoffRange.setSkewForCentre(1000.0f);

    state->createAndAddParameter("cutoff", "Cutoff", "cutoff", cutoffRange, 20000.0f, nullptr, nullptr);
    state->createAndAddParameter("resonance", "Resonance", "resonance", resRange, 1.0f, nullptr, nullptr);
    state->createAndAddParameter("menuItem", "MenuItem", "menuItem", menuItemRange, 0, nullptr, nullptr);

    state->state = juce::ValueTree("cutoff");
    state->state = juce::ValueTree("resonance");
    state->state = juce::ValueTree("menuItem");
}

DSP_GradProject_FilterAudioProcessor::~DSP_GradProject_FilterAudioProcessor()
{
}

//==============================================================================
const juce::String DSP_GradProject_FilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DSP_GradProject_FilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DSP_GradProject_FilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DSP_GradProject_FilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DSP_GradProject_FilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DSP_GradProject_FilterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DSP_GradProject_FilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DSP_GradProject_FilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DSP_GradProject_FilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void DSP_GradProject_FilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DSP_GradProject_FilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();

    svf.reset();
    svf.prepare(spec);
}

void DSP_GradProject_FilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DSP_GradProject_FilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DSP_GradProject_FilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    float cutoff = *state->getRawParameterValue("cutoff");
    float resonance = *state->getRawParameterValue("resonance");
    int menuItem = (int)(*state->getRawParameterValue("menuItem"));

    switch (menuItem)
    {
    case 0:
        svf.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
        svf.state->setCutOffFrequency(lastSampleRate, cutoff, resonance);
        break;
    case 1:
        svf.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
        svf.state->setCutOffFrequency(lastSampleRate, cutoff, resonance);
        break;
    case 2:
        svf.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::bandPass;
        svf.state->setCutOffFrequency(lastSampleRate, cutoff, resonance);
        break;
    default:
        svf.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
        svf.state->setCutOffFrequency(lastSampleRate, cutoff, resonance);
    }

    juce::dsp::AudioBlock<float> block(buffer);

    svf.process(juce::dsp::ProcessContextReplacing<float>(block));
}

juce::AudioProcessorValueTreeState& DSP_GradProject_FilterAudioProcessor::getState()
{
    return *state;
}

//==============================================================================
bool DSP_GradProject_FilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DSP_GradProject_FilterAudioProcessor::createEditor()
{
    return new DSP_GradProject_FilterAudioProcessorEditor (*this);
}

//==============================================================================
void DSP_GradProject_FilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, false);
    state->state.writeToStream(stream);
}

void DSP_GradProject_FilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid())
    {
        state->state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DSP_GradProject_FilterAudioProcessor();
}
