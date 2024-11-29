/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AmpOSCReceiver.h"
#include <stdio.h>
#include <fstream>
#include <iostream>

//==============================================================================
NeuralPiAudioProcessorEditor::NeuralPiAudioProcessorEditor (NeuralPiAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to
    setLookAndFeel(&PiPedalLookAndFeel);

    // Connected light


    //addAndMakeVisible(modelKnob);
    //ampGainKnob.setLookAndFeel(&ampSilverKnobLAF);
    modelKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    modelKnob.setNumDecimalPlacesToDisplay(1);
    modelKnob.addListener(this);
    //modelKnob.setRange(0, processor.jsonFiles.size() - 1);
    modelKnob.setRange(0.0, 1.0);
    modelKnob.setValue(0.0);
    modelKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    modelKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    modelKnob.setNumDecimalPlacesToDisplay(1);
    modelKnob.setDoubleClickReturnValue(true, 0.0);

    auto modelValue = getParameterValue(modelName);
    Slider& modelSlider = getModelSlider();
    modelSlider.setValue(modelValue, NotificationType::dontSendNotification);

    modelKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getModelSlider().getValue());
        const float modelValue = getParameterValue(modelName);

        if (!approximatelyEqual(modelValue, sliderValue))
        {
            setParameterValue(modelName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getModelSlider().getValue());

            if (!oscSender.send(modelAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + modelAddressPattern);
            }
        }
    };
    
    
    addAndMakeVisible(modelSelect);
    modelSelect.setColour(juce::Label::textColourId, juce::Colours::black);
    int c = 1;
    for (const auto& jsonFile : processor.jsonFiles) {
        modelSelect.addItem(jsonFile.getFileNameWithoutExtension(), c);
        c += 1;
    }
    modelSelect.onChange = [this] {modelSelectChanged(); };
    modelSelect.setSelectedItemIndex(processor.current_model_index, juce::NotificationType::dontSendNotification);
    modelSelect.setScrollWheelEnabled(true);

    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Import");
    loadButton.setColour(juce::Label::textColourId, juce::Colours::black);
    loadButton.addListener(this);


    //addAndMakeVisible(irKnob);
    //irKnob.setLookAndFeel(&ampSilverKnobLAF);
    irKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    irKnob.setNumDecimalPlacesToDisplay(1);
    irKnob.addListener(this);
    //irKnob.setRange(0, processor.irFiles.size() - 1);
    irKnob.setRange(0.0, 1.0);
    irKnob.setValue(0.0);
    irKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    irKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    irKnob.setNumDecimalPlacesToDisplay(1);
    irKnob.setDoubleClickReturnValue(true, 0.0);

    auto irValue = getParameterValue(irName);
    Slider& irSlider = getIrSlider();
    irSlider.setValue(irValue, NotificationType::dontSendNotification);

    irKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getIrSlider().getValue());
        const float irValue = getParameterValue(irName);

        if (!approximatelyEqual(irValue, sliderValue))
        {
            setParameterValue(irName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getIrSlider().getValue());

            if (!oscSender.send(irAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + irAddressPattern);
            }
        }
    };

    //addAndMakeVisible(irSelect);
    irSelect.setColour(juce::Label::textColourId, juce::Colours::black);
    int i = 1;
    for (const auto& jsonFile : processor.irFiles) {
        irSelect.addItem(jsonFile.getFileNameWithoutExtension(), i);
        i += 1;
    }
    irSelect.onChange = [this] {irSelectChanged(); };
    irSelect.setSelectedItemIndex(processor.current_ir_index, juce::NotificationType::dontSendNotification);
    irSelect.setScrollWheelEnabled(true);

    //addAndMakeVisible(loadIR);
    loadIR.setButtonText("Import IR");
    loadIR.setColour(juce::Label::textColourId, juce::Colours::black);
    loadIR.addListener(this);

    // Toggle IR
    //addAndMakeVisible(irButton); // Toggle is for testing purposes
    irButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    irButton.onClick = [this] { updateToggleState(&irButton, "IR");   };

    // Toggle LSTM
    //addAndMakeVisible(lstmButton); // Toggle is for testing purposes
    lstmButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    lstmButton.onClick = [this] { updateToggleState(&lstmButton, "LSTM");   };
  

    addAndMakeVisible(ampGainKnob);
    //ampGainKnob.setLookAndFeel(&blueLookAndFeel);
    ampGainKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampGainKnob.setNumDecimalPlacesToDisplay(1);
    ampGainKnob.addListener(this);
    ampGainKnob.setRange(0.0, 1.0);
    ampGainKnob.setValue(0.5);
    ampGainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampGainKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampGainKnob.setNumDecimalPlacesToDisplay(2);
    ampGainKnob.setDoubleClickReturnValue(true, 0.5);

    auto gainValue = getParameterValue(gainName);
    Slider& gainSlider = getGainSlider();
    gainSlider.setValue(gainValue, NotificationType::dontSendNotification);

    ampGainKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getGainSlider().getValue());
        const float gainValue = getParameterValue(gainName);

        if (!approximatelyEqual(gainValue, sliderValue))
        {
            setParameterValue(gainName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getGainSlider().getValue());

            if (!oscSender.send(gainAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + gainAddressPattern);
            }
        }
    };

    addAndMakeVisible(ampMasterKnob);
    //ampMasterKnob.setLookAndFeel(&blueLookAndFeel);
    ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampMasterKnob.setNumDecimalPlacesToDisplay(1);
    ampMasterKnob.addListener(this);
    ampMasterKnob.setRange(0.0, 1.0);
    ampMasterKnob.setValue(0.5);
    ampMasterKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampMasterKnob.setNumDecimalPlacesToDisplay(2);
    //ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20 );
    //ampMasterKnob.setNumDecimalPlacesToDisplay(1);
    ampMasterKnob.setDoubleClickReturnValue(true, 0.5);

    auto masterValue = getParameterValue(masterName);
    Slider& masterSlider = getMasterSlider();
    masterSlider.setValue(masterValue, NotificationType::dontSendNotification);

    ampMasterKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getMasterSlider().getValue());
        const float masterValue = getParameterValue(masterName);

        if (!approximatelyEqual(masterValue, sliderValue))
        {
            setParameterValue(masterName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getMasterSlider().getValue());

            if (!oscSender.send(masterAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + masterAddressPattern);
            }
        }
    };


    addAndMakeVisible(ampBassKnob);
    //ampBassKnob.setLookAndFeel(&blueLookAndFeel);
    ampBassKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampBassKnob.setNumDecimalPlacesToDisplay(1);
    ampBassKnob.addListener(this);
    ampBassKnob.setRange(0.0, 1.0);
    ampBassKnob.setValue(0.5);
    ampBassKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampBassKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampBassKnob.setNumDecimalPlacesToDisplay(2);
    ampBassKnob.setDoubleClickReturnValue(true, 0.5);

    auto bassValue = getParameterValue(bassName);
    Slider& bassSlider = getBassSlider();
    bassSlider.setValue(bassValue, NotificationType::dontSendNotification);

    ampBassKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getBassSlider().getValue());
        const float bassValue = getParameterValue(bassName);

        if (!approximatelyEqual(bassValue, sliderValue))
        {
            setParameterValue(bassName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getBassSlider().getValue());

            if (!oscSender.send(bassAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + bassAddressPattern);
            }
        }
    };

    addAndMakeVisible(ampMidKnob);
    //ampMidKnob.setLookAndFeel(&blueLookAndFeel);
    ampMidKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampMidKnob.setNumDecimalPlacesToDisplay(1);
    ampMidKnob.addListener(this);
    ampMidKnob.setRange(0.0, 1.0);
    ampMidKnob.setValue(0.5);
    ampMidKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMidKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampMidKnob.setNumDecimalPlacesToDisplay(2);
    ampMidKnob.setDoubleClickReturnValue(true, 0.5);

    auto midValue = getParameterValue(midName);
    Slider& midSlider = getMidSlider();
    midSlider.setValue(midValue, NotificationType::dontSendNotification);

    ampMidKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getMidSlider().getValue());
        const float midValue = getParameterValue(midName);

        if (!approximatelyEqual(midValue, sliderValue))
        {
            setParameterValue(midName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getMidSlider().getValue());

            if (!oscSender.send(midAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + midAddressPattern);
            }
        }
    };

    addAndMakeVisible(ampTrebleKnob);
    //ampTrebleKnob.setLookAndFeel(&blueLookAndFeel);
    ampTrebleKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampTrebleKnob.setNumDecimalPlacesToDisplay(1);
    ampTrebleKnob.addListener(this);
    ampTrebleKnob.setRange(0.0, 1.0);
    ampTrebleKnob.setValue(0.5);
    ampTrebleKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampTrebleKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampTrebleKnob.setNumDecimalPlacesToDisplay(2);
    ampTrebleKnob.setDoubleClickReturnValue(true, 0.5);

    auto trebleValue = getParameterValue(trebleName);
    Slider& trebleSlider = getTrebleSlider();
    trebleSlider.setValue(trebleValue, NotificationType::dontSendNotification);

    ampTrebleKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getTrebleSlider().getValue());
        const float trebleValue = getParameterValue(trebleName);

        if (!approximatelyEqual(trebleValue, sliderValue))
        {
            setParameterValue(trebleName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getTrebleSlider().getValue());

            if (!oscSender.send(trebleAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + trebleAddressPattern);
            }
        }
    };

    addAndMakeVisible(ampPresenceKnob);
    //ampPresenceKnob.setLookAndFeel(&blueLookAndFeel);
    ampPresenceKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampPresenceKnob.setNumDecimalPlacesToDisplay(1);
    ampPresenceKnob.addListener(this);
    ampPresenceKnob.setRange(0.0, 1.0);
    ampPresenceKnob.setValue(0.5);
    ampPresenceKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampPresenceKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampPresenceKnob.setNumDecimalPlacesToDisplay(2);
    ampPresenceKnob.setDoubleClickReturnValue(true, 0.5);

    auto presenceValue = getParameterValue(trebleName);
    Slider& presenceSlider = getPresenceSlider();
    trebleSlider.setValue(presenceValue, NotificationType::dontSendNotification);

    ampPresenceKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getPresenceSlider().getValue());
        const float presenceValue = getParameterValue(presenceName);

        if (!approximatelyEqual(presenceValue, sliderValue))
        {
            setParameterValue(presenceName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getPresenceSlider().getValue());

            if (!oscSender.send(presenceAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + presenceAddressPattern);
            }
        }
    };

    addAndMakeVisible(ampDelayKnob);
    //ampDelayKnob.setLookAndFeel(&blueLookAndFeel);
    ampDelayKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampDelayKnob.setNumDecimalPlacesToDisplay(1);
    ampDelayKnob.addListener(this);
    ampDelayKnob.setRange(0.0, 1.0);
    ampDelayKnob.setValue(0.0);
    ampDelayKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampDelayKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampDelayKnob.setNumDecimalPlacesToDisplay(2);
    ampDelayKnob.setDoubleClickReturnValue(true, 0.0);

    auto delayValue = getParameterValue(delayName);
    Slider& delaySlider = getDelaySlider();
    delaySlider.setValue(delayValue, NotificationType::dontSendNotification);

    ampDelayKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getDelaySlider().getValue());
        const float delayValue = getParameterValue(delayName);

        if (!approximatelyEqual(delayValue, sliderValue))
        {
            setParameterValue(delayName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getDelaySlider().getValue());

            if (!oscSender.send(delayAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + delayAddressPattern);
            }
        }
    };

    addAndMakeVisible(ampReverbKnob);
    //ampReverbKnob.setLookAndFeel(&blueLookAndFeel);
    ampReverbKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampReverbKnob.setNumDecimalPlacesToDisplay(1);
    ampReverbKnob.addListener(this);
    ampReverbKnob.setRange(0.0, 1.0);
    ampReverbKnob.setValue(0.0);
    ampReverbKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampReverbKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampReverbKnob.setNumDecimalPlacesToDisplay(2);
    ampReverbKnob.setDoubleClickReturnValue(true, 0.0);

    auto reverbValue = getParameterValue(reverbName);
    Slider& reverbSlider = getReverbSlider();
    reverbSlider.setValue(reverbValue, NotificationType::dontSendNotification);

    ampReverbKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getReverbSlider().getValue());
        const float reverbValue = getParameterValue(reverbName);

        if (!approximatelyEqual(reverbValue, sliderValue))
        {
            setParameterValue(reverbName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getReverbSlider().getValue());

            if (!oscSender.send(reverbAddressPattern, value))
            {
                updateOutConnectedLabel(false);
            }
            else
            {
                DBG("Sent value " + String(value) + " to AP " + reverbAddressPattern);
            }
        }
    };

    addAndMakeVisible(GainLabel);
    GainLabel.setText("GAIN", juce::NotificationType::dontSendNotification);
    GainLabel.setJustificationType(juce::Justification::centred);
    GainLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));

    addAndMakeVisible(LevelLabel);
    LevelLabel.setText("LEVEL", juce::NotificationType::dontSendNotification);
    LevelLabel.setJustificationType(juce::Justification::centred);
    LevelLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));

    addAndMakeVisible(BassLabel);
    BassLabel.setText("BASS", juce::NotificationType::dontSendNotification);
    BassLabel.setJustificationType(juce::Justification::centred);
    BassLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));

    addAndMakeVisible(MidLabel);
    MidLabel.setText("MID", juce::NotificationType::dontSendNotification);
    MidLabel.setJustificationType(juce::Justification::centred);
    MidLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));

    addAndMakeVisible(TrebleLabel);
    TrebleLabel.setText("TREBLE", juce::NotificationType::dontSendNotification);
    TrebleLabel.setJustificationType(juce::Justification::centred);
    TrebleLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));

    addAndMakeVisible(PresenceLabel);
    PresenceLabel.setText("PRESENCE", juce::NotificationType::dontSendNotification);
    PresenceLabel.setJustificationType(juce::Justification::centred);
    PresenceLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));

    addAndMakeVisible(DelayLabel);
    DelayLabel.setText("DELAY", juce::NotificationType::dontSendNotification);
    DelayLabel.setJustificationType(juce::Justification::centred);
    DelayLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));

    addAndMakeVisible(ReverbLabel);
    ReverbLabel.setText("REVERB", juce::NotificationType::dontSendNotification);
    ReverbLabel.setJustificationType(juce::Justification::centred);
    ReverbLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));

    addAndMakeVisible(toneDropDownLabel);
    toneDropDownLabel.setText("TONE", juce::NotificationType::dontSendNotification);
    toneDropDownLabel.setJustificationType(juce::Justification::centred);
    toneDropDownLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));

    addAndMakeVisible(irDropDownLabel);
    irDropDownLabel.setText("IR", juce::NotificationType::dontSendNotification);
    irDropDownLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(versionLabel);
    versionLabel.setText("v1.3.0", juce::NotificationType::dontSendNotification);
    versionLabel.setJustificationType(juce::Justification::centred);

    juce::Font font("Arial Black", 17.0f, juce::Font::plain); // 0.75);
    GainLabel.setFont(font);
    LevelLabel.setFont(font);
    BassLabel.setFont(font);
    MidLabel.setFont(font);
    TrebleLabel.setFont(font);
    PresenceLabel.setFont(font);
    DelayLabel.setFont(font);
    ReverbLabel.setFont(font);
    toneDropDownLabel.setFont(font);
    irDropDownLabel.setFont(font);
    versionLabel.setFont(font);


    // Name controls:
    addAndMakeVisible(ampNameLabel);
    ampNameField.setEditable(true, true, true);
    addAndMakeVisible(ampNameField);

    //IP controls :
    ipField.setEditable(true, true, true);
    addAndMakeVisible(ipLabel);
    ipLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));
    ipLabel.setFont(font);
    addAndMakeVisible(ipField);

    //Port controls :
    addAndMakeVisible(outPortNumberLabel);
    outPortNumberLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));
    outPortNumberLabel.setFont(font);
    outPortNumberField.setEditable(true, true, true);
    addAndMakeVisible(outPortNumberField);
    addAndMakeVisible(outConnectedLabel);

    addAndMakeVisible(inPortNumberLabel);
    inPortNumberLabel.setColour(juce::Label::textColourId, juce::Colours::grey.withBrightness(0.7));
    inPortNumberLabel.setFont(font);
    inPortNumberField.setEditable(true, true, true);
    addAndMakeVisible(inPortNumberField);
    addAndMakeVisible(inConnectedLabel);

    // Connection light

    addAndMakeVisible(piConnect);
    piConnect.setClickingTogglesState(false);
    //piConnect.setToggleState(oscSender.)



    // OSC messaging

    getInPortNumberField().addListener(this);
    getAmpNameField().addListener(this);
    getOutPortNumberField().addListener(this);
    getIPField().addListener(this);

    oscReceiver.getGainValue().addListener(this);
    oscReceiver.getMasterValue().addListener(this);

    oscReceiver.getBassValue().addListener(this);
    oscReceiver.getMidValue().addListener(this);
    oscReceiver.getTrebleValue().addListener(this);
    oscReceiver.getPresenceValue().addListener(this);

    oscReceiver.getModelValue().addListener(this);
    oscReceiver.getIrValue().addListener(this);

    updateInConnectedLabel();

    connectSender();

    // Size of plugin GUI
    setSize(400, 515);

    // Set gain knob color based on conditioned/snapshot model 
    //setParamKnobColor();
}

NeuralPiAudioProcessorEditor::~NeuralPiAudioProcessorEditor()
{
}

//==============================================================================
void NeuralPiAudioProcessorEditor::paint (Graphics& g)
{
    // Workaround for graphics on Windows builds (clipping code doesn't work correctly on Windows)
    g.fillAll(juce::Colours::black); 

    //Create Bounds rectangle for refrence
    juce::Rectangle<int> border = getLocalBounds();
    juce::Rectangle<int> bounds = border.reduced(10);

    //creating shading for background
    g.setColour(juce::Colours::steelblue.withBrightness(0.3));
    g.fillRoundedRectangle(bounds.getX() - 5, bounds.getY() - 5, bounds.getWidth() + 10, bounds.getHeight() + 10, 10);
    g.setColour(juce::Colours::steelblue.withBrightness(0.4));
    g.fillRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 10);

    //Doppelganger logo
    g.setOpacity(1.0);
    g.drawImageWithin(logo, bounds.getX()+92, bounds.getY()+140, 200, 70, juce::RectanglePlacement::centred, false);

    //Visual Knob bounds
    juce::Rectangle<float> knobsBounds1(bounds.getX() + 10, bounds.getY() + (bounds.getHeight() / 4) - 10, bounds.getWidth() - 20, (bounds.getHeight() / 4) - 5);
    juce::Rectangle<float> knobsBounds2(bounds.getX() + 10, knobsBounds1.getY() + knobsBounds1.getHeight() + 5, bounds.getWidth() - 20, (bounds.getHeight() / 4) - 5);
    juce::Rectangle<float> knobsBounds3(bounds.getX() + 10, knobsBounds2.getY() + knobsBounds2.getHeight() + 5, bounds.getWidth() - 20, (bounds.getHeight() / 4) - 5);

    g.setColour(juce::Colours::steelblue.withBrightness(0.1));
    g.drawRoundedRectangle(knobsBounds1.reduced(10), 15, 2);
    g.drawRoundedRectangle(knobsBounds2.reduced(10), 15, 2);
    g.drawRoundedRectangle(knobsBounds3.reduced(10), 15, 2);

    //Value Display Boxes ------------
    g.setColour(juce::Colours::black);

    //IP
    juce::Rectangle<int> ipScreen(bounds.getX() + (bounds.getWidth() / 4) - 50, bounds.getY() + 55, 75, 15);
    g.fillRect(ipScreen);

    //OSC Out
    juce::Rectangle<int> oscOutScreen(bounds.getX() + (bounds.getWidth() / 2) - 37, modelSelect.getY() + 45, 55, 15);
    g.fillRect(oscOutScreen);

    //OSC In
    juce::Rectangle<int> oscInScreen(bounds.getX() + ((bounds.getWidth() / 4) * 3) - 37, modelSelect.getY() + 45, 55, 15);
    g.fillRect(oscInScreen);

    //Knobs-----------------------------
    juce::Rectangle<int> knobScreens[8];
    juce::Slider* sliderRefs[8];

    //Filling sliderRefs
    sliderRefs[0] = &ampGainKnob;
    sliderRefs[1] = &ampMasterKnob;
    sliderRefs[2] = &ampBassKnob;
    sliderRefs[3] = &ampMidKnob;
    sliderRefs[4] = &ampTrebleKnob;
    sliderRefs[5] = &ampDelayKnob;
    sliderRefs[6] = &ampReverbKnob;
    sliderRefs[7] = &ampPresenceKnob;

    //Screen rectangles
    juce::Rectangle<int> gainScreen;
    knobScreens[0] = gainScreen;
    juce::Rectangle<int> masterScreen;
    knobScreens[1] = masterScreen;
    juce::Rectangle<int> bassScreen;
    knobScreens[2] = bassScreen;
    juce::Rectangle<int> midScreen;
    knobScreens[3] = midScreen;
    juce::Rectangle<int> trebleScreen;
    knobScreens[4] = trebleScreen;
    juce::Rectangle<int> delayScreen;
    knobScreens[5] = delayScreen;
    juce::Rectangle<int> reverbScreen;
    knobScreens[6] = reverbScreen;
    juce::Rectangle<int> presenceScreen;
    knobScreens[7] = presenceScreen;

    //Drawing knob screens
    for (int i = 0; i < 8; i++) {
        knobScreens[i].setBounds(sliderRefs[i]->getX() + 39, sliderRefs[i]->getY() + 104, sliderRefs[i]->getTextBoxWidth() - 2, sliderRefs[i]->getTextBoxHeight() - 2);
        g.fillRect(knobScreens[i]);
    }

    //Drawing Label backings
    juce::Rectangle<int> labelBacks[8];

    //Filling label backing array
    juce::Rectangle<int> gainLabelBack;
    labelBacks[0] = gainLabelBack;
    juce::Rectangle<int> masterLabelBack;
    labelBacks[1] = masterLabelBack;
    juce::Rectangle<int> bassLabelBack;
    labelBacks[2] = bassLabelBack;
    juce::Rectangle<int> midLabelBack;
    labelBacks[3] = midLabelBack;
    juce::Rectangle<int> trebleLabelBack;
    labelBacks[4] = trebleLabelBack;
    juce::Rectangle<int> delayLabelBack;
    labelBacks[5] = delayLabelBack;
    juce::Rectangle<int> reverbLabelBack;
    labelBacks[6] = reverbLabelBack;
    juce::Rectangle<int> presenceLabelBack;
    labelBacks[7] = presenceLabelBack;

    //Drawing backings
    for (int i = 0; i < 8; i++) {
        labelBacks[i].setBounds(sliderRefs[i]->getX() + 27, sliderRefs[i]->getY() + 7, sliderRefs[i]->getTextBoxWidth() + 26, sliderRefs[i]->getTextBoxHeight() - 3);
        g.setColour(juce::Colours::steelblue.withBrightness(0.4));
        g.fillRect(labelBacks[i]);
        g.setColour(juce::Colours::steelblue.withBrightness(0.1));
        g.drawRoundedRectangle(labelBacks[i].toFloat(), 3, 2);
    }
}

void NeuralPiAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    const float margin = 20;

    juce::Rectangle<int> bounds = getLocalBounds().reduced(10);
    juce::Rectangle<int> knobsBounds1(bounds.getX(), bounds.getY() + (bounds.getHeight() / 4) - 15, bounds.getWidth(), (bounds.getHeight() / 4) - 10);
    juce::Rectangle<int> knobsBounds2(bounds.getX(), knobsBounds1.getY() + knobsBounds1.getHeight() + 10, bounds.getWidth(), bounds.getHeight() / 4);
    juce::Rectangle<int> knobsBounds3(bounds.getX(), knobsBounds2.getY() + knobsBounds2.getHeight(), bounds.getWidth(), bounds.getHeight() / 4);
    juce::Rectangle<int> comboBoxBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight() / 4);

    // Connection light
    piConnect.setBounds(bounds.getX() + 325, bounds.getY() + 60, 20, 20);

    //Model Select Widgets
    toneDropDownLabel.setBounds(bounds.getX() - 5, bounds.getY() + 10, 60, 20);
    modelSelect.setBounds(bounds.getX() + 50, toneDropDownLabel.getY(), bounds.getWidth() - 120, 25);
    loadButton.setBounds(modelSelect.getX() + modelSelect.getWidth() + 5, modelSelect.getY(), 60, 25);
    modelKnob.setBounds(140, 40, 75, 95);

    //irDropDownLabel.setBounds(bounds.getX() + 10, modelSelect.getY() + 30, 80, 20);
    //irSelect.setBounds(bounds.getX() + 5, irDropDownLabel.getY() + 25, bounds.getWidth() / 4, 25);
    //irButton.setBounds(248, 42, 257, 25);
    lstmButton.setBounds(248, 10, 257, 25);

    // Amp Widgets
    //=====================

    //Gain and Volume
    ampGainKnob.setBounds(knobsBounds2.getX() + 20, knobsBounds1.getY(), knobsBounds2.getWidth() / 3, knobsBounds2.getHeight());
    ampMasterKnob.setBounds(knobsBounds2.getX() + ((knobsBounds1.getWidth() / 3) * 2) - 20, knobsBounds1.getY(), knobsBounds2.getWidth() / 3, knobsBounds2.getHeight());

    //EQ
    ampBassKnob.setBounds(knobsBounds2.getX() + 20, knobsBounds2.getY(), knobsBounds2.getWidth() / 3, knobsBounds2.getHeight());
    ampMidKnob.setBounds(knobsBounds2.getX() + (knobsBounds2.getWidth() / 3), knobsBounds2.getY(), knobsBounds1.getWidth() / 3, knobsBounds2.getHeight());
    ampTrebleKnob.setBounds(knobsBounds2.getX() + ((knobsBounds1.getWidth() / 3) * 2) - 20, knobsBounds2.getY(), knobsBounds1.getWidth() / 3, knobsBounds2.getHeight());

    //Spacial effects
    ampDelayKnob.setBounds(knobsBounds3.getX() + 20, knobsBounds3.getY(), knobsBounds2.getWidth() / 3, knobsBounds2.getHeight());
    ampReverbKnob.setBounds(knobsBounds3.getX() + (knobsBounds2.getWidth() / 3), knobsBounds3.getY(), knobsBounds1.getWidth() / 3, knobsBounds2.getHeight());
    ampPresenceKnob.setBounds(knobsBounds3.getX() + ((knobsBounds1.getWidth() / 3) * 2) - 20, knobsBounds3.getY(), knobsBounds1.getWidth() / 3, knobsBounds2.getHeight());

    //Labels
    //======================

    //Gain and Volume
    GainLabel.setBounds(ampGainKnob.getX() + 25, knobsBounds1.getY() + 10, 80, 10);
    LevelLabel.setBounds(ampMasterKnob.getX() + 25, knobsBounds1.getY() + 10, 80, 10);

    //EQ
    BassLabel.setBounds(ampBassKnob.getX() + 25, knobsBounds2.getY() + 10, 80, 10);
    MidLabel.setBounds(ampMidKnob.getX() + 25, knobsBounds2.getY() + 10, 80, 10);
    TrebleLabel.setBounds(ampTrebleKnob.getX() + 25, knobsBounds2.getY() + 10, 80, 10);

    //Spacial Effects
    DelayLabel.setBounds(ampDelayKnob.getX() + 25, knobsBounds3.getY() + 10, 80, 10);
    ReverbLabel.setBounds(ampReverbKnob.getX() + 25, knobsBounds3.getY() + 10, 80, 10);
    PresenceLabel.setBounds(ampPresenceKnob.getX() + 25, knobsBounds3.getY() + 10, 80, 10);

    addAndMakeVisible(ampNameLabel);
    ampNameField.setEditable(true, true, true);
    addAndMakeVisible(ampNameField);

    // IP controls:
    ipField.setBounds(bounds.getX() + (bounds.getWidth() / 4) - 50, modelSelect.getY() + 45, 75, 15);
    ipLabel.setBounds(bounds.getX() + (bounds.getWidth() / 4) - 25, ipField.getY() + 25, 30, 10);

    // Port controls:
    outPortNumberField.setBounds(bounds.getX() + (bounds.getWidth() / 2) - 37, modelSelect.getY() + 45, 55, 15);
    outPortNumberLabel.setBounds(bounds.getX() + (bounds.getWidth() / 2) - 45, ipLabel.getY(), 150, 10);
    inPortNumberField.setBounds(bounds.getX() + ((bounds.getWidth() / 4) * 3) - 37, modelSelect.getY() + 45, 55, 15);
    inPortNumberLabel.setBounds(bounds.getX() + ((bounds.getWidth() / 4) * 3) - 40, ipLabel.getY(), 150, 10);
}

void NeuralPiAudioProcessorEditor::modelSelectChanged()
{
    const int selectedFileIndex = modelSelect.getSelectedItemIndex();
    if (selectedFileIndex >= 0 && selectedFileIndex < processor.jsonFiles.size()) {
        File selectedFile = processor.userAppDataDirectory_tones.getFullPathName() + "/" + modelSelect.getText() + ".json";
        //processor.loadConfig(processor.jsonFiles[selectedFileIndex]);
        processor.loadConfig(selectedFile);
        processor.current_model_index = selectedFileIndex;
    }
    auto newValue = static_cast<float>(processor.current_model_index / (processor.num_models - 1.0));
    modelKnob.setValue(newValue);
    setParamKnobColor();
}

void NeuralPiAudioProcessorEditor::irSelectChanged()
{
    const int selectedFileIndex = irSelect.getSelectedItemIndex();
    if (selectedFileIndex >= 0 && selectedFileIndex < processor.irFiles.size()) {
        File selectedFile = processor.userAppDataDirectory_irs.getFullPathName() + "/" + irSelect.getText() + ".wav";
        //processor.loadIR(processor.irFiles[selectedFileIndex]);
        processor.loadIR(selectedFile);
        processor.current_ir_index = selectedFileIndex;
    }
    auto newValue = static_cast<float>(processor.current_ir_index / (processor.num_irs - 1.0));
    irKnob.setValue(newValue);
}

void NeuralPiAudioProcessorEditor::updateToggleState(juce::Button* button, juce::String name)
{
    if (name == "IR")
        processor.ir_state = button->getToggleState();
    else
        processor.lstm_state = button->getToggleState();
}

void NeuralPiAudioProcessorEditor::loadButtonClicked()
{  
    myChooser = std::make_unique<FileChooser> ("Select one or more .json tone files to import",
                                               File::getSpecialLocation (File::userDesktopDirectory),
                                               "*.json");

    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems;

    myChooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)
    {
        Array<File> files = chooser.getResults();
        for (auto file : files) {
            File fullpath = processor.userAppDataDirectory_tones.getFullPathName() + "/" + file.getFileName();
            bool b = fullpath.existsAsFile();
            if (b == false) {

                processor.loadConfig(file);
                fname = file.getFileName();
                processor.loaded_tone = file;
                processor.loaded_tone_name = fname;
                processor.custom_tone = 1;

                // Copy selected file to model directory and load into dropdown menu
                bool a = file.copyFileTo(fullpath);
                if (a == true) {
                    modelSelect.addItem(file.getFileNameWithoutExtension(), processor.jsonFiles.size() + 1);
                    modelSelect.setSelectedItemIndex(processor.jsonFiles.size(), juce::NotificationType::dontSendNotification);
                    processor.jsonFiles.push_back(file);
                    processor.num_models += 1;
                }
                // Sort jsonFiles alphabetically
                std::sort(processor.jsonFiles.begin(), processor.jsonFiles.end());
            }
        }
    });
    setParamKnobColor();
}

void NeuralPiAudioProcessorEditor::loadIRClicked()
{
    myChooser = std::make_unique<FileChooser> ("Select one or more .wav IR files to import",
                                               File::getSpecialLocation (File::userDesktopDirectory),
                                               "*.wav");

    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems;

    myChooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)   
    {
        Array<File> files = chooser.getResults();
        for (auto file : files) {
            File fullpath = processor.userAppDataDirectory_irs.getFullPathName() + "/" + file.getFileName();
            bool b = fullpath.existsAsFile();
            if (b == false) {

                processor.loadIR(file);
                fname = file.getFileName();
                processor.loaded_ir = file;
                processor.loaded_ir_name = fname;
                processor.custom_ir = 1;

                // Copy selected file to model directory and load into dropdown menu
                bool a = file.copyFileTo(fullpath);
                if (a == true) {
                    irSelect.addItem(file.getFileNameWithoutExtension(), processor.irFiles.size() + 1);
                    irSelect.setSelectedItemIndex(processor.irFiles.size(), juce::NotificationType::dontSendNotification);
                    processor.irFiles.push_back(file);
                    processor.num_irs += 1;
                }
                // Sort jsonFiles alphabetically
                std::sort(processor.irFiles.begin(), processor.irFiles.end());
            }
        }
    });
}


void NeuralPiAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton) {
        loadButtonClicked();
    }
    else
    {
        loadIRClicked();
    }
}


void NeuralPiAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &modelKnob) {
        if (slider->getValue() >= 0 && slider->getValue() < processor.jsonFiles.size()) {
            modelSelect.setSelectedItemIndex(processor.getModelIndex(slider->getValue()), juce::NotificationType::dontSendNotification);
        }
    } else if (slider == &irKnob) {
        if (slider->getValue() >= 0 && slider->getValue() < processor.irFiles.size()) {
            irSelect.setSelectedItemIndex(processor.getIrIndex(slider->getValue()), juce::NotificationType::dontSendNotification);
        }
    }
}

// OSC Messages
Slider& NeuralPiAudioProcessorEditor::getGainSlider()
{
    return ampGainKnob;
}

Slider& NeuralPiAudioProcessorEditor::getMasterSlider()
{
    return ampMasterKnob;
}

Slider& NeuralPiAudioProcessorEditor::getBassSlider()
{
    return ampBassKnob;
}

Slider& NeuralPiAudioProcessorEditor::getMidSlider()
{
    return ampMidKnob;
}

Slider& NeuralPiAudioProcessorEditor::getTrebleSlider()
{
    return ampTrebleKnob;
}

Slider& NeuralPiAudioProcessorEditor::getPresenceSlider()
{
    return ampPresenceKnob;
}

Slider& NeuralPiAudioProcessorEditor::getDelaySlider()
{
    return ampDelayKnob;
}

Slider& NeuralPiAudioProcessorEditor::getReverbSlider()
{
    return ampReverbKnob;
}

Slider& NeuralPiAudioProcessorEditor::getModelSlider()
{
    return modelKnob;
}

Slider& NeuralPiAudioProcessorEditor::getIrSlider()
{
    return irKnob;
}


Label& NeuralPiAudioProcessorEditor::getOutPortNumberField()
{
    return outPortNumberField;
}

Label& NeuralPiAudioProcessorEditor::getInPortNumberField()
{
    return inPortNumberField;
}

Label& NeuralPiAudioProcessorEditor::getIPField()
{
    return ipField;
}

Label& NeuralPiAudioProcessorEditor::getAmpNameField()
{
    return ampNameField;
}

Label& NeuralPiAudioProcessorEditor::getOutConnectedLabel()
{
    return outConnectedLabel;
}

Label& NeuralPiAudioProcessorEditor::getInConnectedLabel()
{
    return inConnectedLabel;
}

void NeuralPiAudioProcessorEditor::buildAddressPatterns()
{
    gainAddressPattern = "/parameter/" + ampName + "/Gain";
    masterAddressPattern = "/parameter/" + ampName + "/Master";
    bassAddressPattern = "/parameter/" + ampName + "/Bass";
    midAddressPattern = "/parameter/" + ampName + "/Mid";
    trebleAddressPattern = "/parameter/" + ampName + "/Treble";
    presenceAddressPattern = "/parameter/" + ampName + "/Presence";
    delayAddressPattern = "/parameter/" + ampName + "/Delay";
    reverbAddressPattern = "/parameter/" + ampName + "/Reverb";
    modelAddressPattern = "/parameter/" + ampName + "/Model";
    irAddressPattern = "/parameter/" + ampName + "/Ir";
}

void NeuralPiAudioProcessorEditor::connectSender()
{
    // specify here where to send OSC messages to: host URL and UDP port number
    if (!oscSender.connect(outgoingIP, outgoingPort))
    {
        piConnect.setToggleState(true, dontSendNotification);
    }
    else
    {
        piConnect.setToggleState(false, dontSendNotification);
    }
}

void NeuralPiAudioProcessorEditor::updateOutgoingIP(String ip)
{
    outgoingIP = std::move(ip);
    connectSender();
}

void NeuralPiAudioProcessorEditor::updateOutgoingPort(int port)
{
    outgoingPort = port;
    connectSender();
}

void NeuralPiAudioProcessorEditor::labelTextChanged(Label* labelThatHasChanged)
{
    if (labelThatHasChanged == &getInPortNumberField())
    {
        const int newPort = getInPortNumberField().getTextValue().toString().getIntValue();
        oscReceiver.changePort(newPort);
        updateInConnectedLabel();
    }
    else if (labelThatHasChanged == &getOutPortNumberField())
    {
        const int newPort = getOutPortNumberField().getTextValue().toString().getIntValue();
        updateOutgoingPort(newPort);
    }
    else if (labelThatHasChanged == &getIPField())
    {
        const String newIP = getIPField().getTextValue().toString();
        updateOutgoingIP(newIP);
    }
    /*
    else if (labelThatHasChanged == getAmpNameField())
    {
        ampName = getAmpNameField().getTextValue().toString();
        buildAddressPatterns();
        oscReceiver.updateAmpName(getAmpNameField().getTextValue().toString());
    }
    */
}

void NeuralPiAudioProcessorEditor::updateInConnectedLabel()
{
    const bool connected = oscReceiver.isConnected();
    if (connected)
    {
        getInConnectedLabel().setText("(Connected)", dontSendNotification);
    }
    else
    {
        getInConnectedLabel().setText("(Disconnected!)", dontSendNotification);
    }
}

void NeuralPiAudioProcessorEditor::updateOutConnectedLabel(bool connected)
{
    if (connected)
    {
        getOutConnectedLabel().setText("(Connected)", dontSendNotification);
    }
    else
    {
        getOutConnectedLabel().setText("(Disconnected!)", dontSendNotification);
    }
}

// This callback is invoked if an OSC message has been received setting either value.
void NeuralPiAudioProcessorEditor::valueChanged(Value& value)
{
    if (value.refersToSameSourceAs(oscReceiver.getGainValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getGainSlider().getValue()))
        {
            getGainSlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
    else if (value.refersToSameSourceAs(oscReceiver.getMasterValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getMasterSlider().getValue()))
        {
            getMasterSlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
    if (value.refersToSameSourceAs(oscReceiver.getBassValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getBassSlider().getValue()))
        {
            getBassSlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
    else if (value.refersToSameSourceAs(oscReceiver.getMidValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getMidSlider().getValue()))
        {
            getMidSlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
    if (value.refersToSameSourceAs(oscReceiver.getTrebleValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getTrebleSlider().getValue()))
        {
            getTrebleSlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
    else if (value.refersToSameSourceAs(oscReceiver.getPresenceValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getPresenceSlider().getValue()))
        {
            getPresenceSlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
    if (value.refersToSameSourceAs(oscReceiver.getDelayValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getDelaySlider().getValue()))
        {
            getDelaySlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
    else if (value.refersToSameSourceAs(oscReceiver.getReverbValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getReverbSlider().getValue()))
        {
            getReverbSlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
    else if (value.refersToSameSourceAs(oscReceiver.getModelValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getModelSlider().getValue()))
        {
            getModelSlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
    else if (value.refersToSameSourceAs(oscReceiver.getIrValue()))
    {
        if (!approximatelyEqual(static_cast<double> (value.getValue()), getIrSlider().getValue()))
        {
            getIrSlider().setValue(static_cast<double> (value.getValue()),
                NotificationType::sendNotification);
        }
    }
}

void NeuralPiAudioProcessorEditor::timerCallback()
{
    getGainSlider().setValue(getParameterValue(gainName), NotificationType::dontSendNotification);
    getMasterSlider().setValue(getParameterValue(masterName), NotificationType::dontSendNotification);
    getBassSlider().setValue(getParameterValue(bassName), NotificationType::dontSendNotification);
    getMidSlider().setValue(getParameterValue(midName), NotificationType::dontSendNotification);
    getTrebleSlider().setValue(getParameterValue(trebleName), NotificationType::dontSendNotification);
    getPresenceSlider().setValue(getParameterValue(presenceName), NotificationType::dontSendNotification);
    getDelaySlider().setValue(getParameterValue(delayName), NotificationType::dontSendNotification);
    getReverbSlider().setValue(getParameterValue(reverbName), NotificationType::dontSendNotification);
    getModelSlider().setValue(getParameterValue(modelName), NotificationType::dontSendNotification);
    getIrSlider().setValue(getParameterValue(irName), NotificationType::dontSendNotification);
}

AudioProcessorParameter* NeuralPiAudioProcessorEditor::getParameter(const String& paramId)
{
    if (auto* proc = getAudioProcessor())
    {
        auto& params = proc->getParameters();

        for (auto p : params)
        {
            if (auto* param = dynamic_cast<AudioProcessorParameterWithID*> (p))
            {
                if (param->paramID == paramId)
                    return param;
            }
        }
    }

    return nullptr;
}

float NeuralPiAudioProcessorEditor::getParameterValue(const String& paramId)
{
    if (auto* param = getParameter(paramId))
        return param->getValue();

    return 0.0f;
}

void NeuralPiAudioProcessorEditor::setParameterValue(const String& paramId, float value)
{
    if (auto* param = getParameter(paramId))
        param->setValueNotifyingHost(value);
}


void NeuralPiAudioProcessorEditor::setParamKnobColor()
{
    // If the knob is used for a parameter, change it to red
    if (processor.params == 0) {
        //ampGainKnob.setLookAndFeel(&blueLookAndFeel);
        //ampMasterKnob.setLookAndFeel(&blueLookAndFeel);
    }
    else if (processor.params == 1) {
        //ampGainKnob.setLookAndFeel(&redLookAndFeel);
        //ampMasterKnob.setLookAndFeel(&blueLookAndFeel);
    }
    else if (processor.params == 2) {
        //ampGainKnob.setLookAndFeel(&redLookAndFeel);
        //ampMasterKnob.setLookAndFeel(&redLookAndFeel);
    }

}
