/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DSP_GradProject_FilterAudioProcessorEditor::DSP_GradProject_FilterAudioProcessorEditor (DSP_GradProject_FilterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(cutoffKnob = new juce::Slider("Cutoff"));
    cutoffKnob->setSliderStyle(juce::Slider::Rotary);
    cutoffKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    cutoffKnob->setPopupDisplayEnabled(true, true, this);

    addAndMakeVisible(resKnob = new juce::Slider("Resonance"));
    resKnob->setSliderStyle(juce::Slider::Rotary);
    resKnob->setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);
    resKnob->setPopupDisplayEnabled(true, true, this);

    addAndMakeVisible(menu = new juce::ComboBox("MenuItem"));
    menu->setJustificationType(juce::Justification::centred);
    menu->addItem("Low Pass", 1);
    menu->addItem("High Pass", 2);
    menu->addItem("Band Pass", 3);

    cutoffAttachment = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "cutoff", *cutoffKnob);
    resAttachment = new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "resonance", *resKnob);
    menuAttachment = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(p.getState(), "menuItem", *menu);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 400);
}

DSP_GradProject_FilterAudioProcessorEditor::~DSP_GradProject_FilterAudioProcessorEditor()
{
}

//==============================================================================
void DSP_GradProject_FilterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::white);

    g.setColour(juce::Colours::black);
    g.setFont(15.0f);

    juce::Rectangle<int> topArea = getLocalBounds();
    juce::Rectangle<int> bottomArea = topArea.removeFromBottom(topArea.getHeight() / 2);
    juce::Rectangle<int> r = bottomArea.removeFromRight(bottomArea.getWidth() / 2);
    juce::Rectangle<int> c = bottomArea;

    g.drawText("Cutoff", c.removeFromBottom(50), juce::Justification::centred, false);
    g.drawText("Resonance", r.removeFromBottom(50), juce::Justification::centred, false);
    g.drawText("Filter", topArea.removeFromBottom(50), juce::Justification::centred, false);
}

void DSP_GradProject_FilterAudioProcessorEditor::resized()
{
    int border = 10;
    juce::Rectangle<int> topArea = getLocalBounds();
    juce::Rectangle<int> bottomArea = topArea.removeFromBottom(topArea.getHeight() / 2);

    cutoffKnob->setBounds(bottomArea.removeFromLeft(bottomArea.getWidth() / 2));
    resKnob->setBounds(bottomArea);
    menu->setBounds(topArea);

    cutoffKnob->setBounds(cutoffKnob->getBounds().reduced(border));
    resKnob->setBounds(resKnob->getBounds().reduced(border));
    menu->setBounds(menu->getBounds().reduced(8*border));
}
