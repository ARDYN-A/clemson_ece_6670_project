/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DSP_GradProject_FilterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DSP_GradProject_FilterAudioProcessorEditor (DSP_GradProject_FilterAudioProcessor&);
    ~DSP_GradProject_FilterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DSP_GradProject_FilterAudioProcessor& audioProcessor;

    juce::ScopedPointer<juce::Slider> cutoffKnob;
    juce::ScopedPointer<juce::Slider> resKnob;
    juce::ScopedPointer<juce::ComboBox> menu;

    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> resAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::ComboBoxAttachment> menuAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DSP_GradProject_FilterAudioProcessorEditor)
};
