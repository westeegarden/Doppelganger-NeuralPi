/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce::dsp
{

/**
    Acts as a polymorphic base class for processors.
    This exposes the same set of methods that a processor must implement as virtual
    methods, so that you can use the ProcessorWrapper class to wrap an instance of
    a subclass, and then pass that around using ProcessorBase as a base class.
    @see ProcessorWrapper

    @tags{DSP}
*/
struct ProcessorBase
{
    ProcessorBase() = default;
    virtual ~ProcessorBase() = default;

    virtual void prepare (const ProcessSpec&)  = 0;
    virtual void process (const ProcessContextReplacing<float>&) = 0;
    virtual void reset() = 0;
};


//==============================================================================
/**
    Wraps an instance of a given processor class, and exposes it through the
    ProcessorBase interface.
    @see ProcessorBase

    @tags{DSP}
*/
template <typename ProcessorType>
struct ProcessorWrapper  : public ProcessorBase
{
    void prepare (const ProcessSpec& spec) override
    {
        processor.prepare (spec);
    }

    void process (const ProcessContextReplacing<float>& context) override
    {
        processor.process (context);
    }

    void reset() override
    {
        processor.reset();
    }

    ProcessorType processor;
};

} // namespace juce::dsp