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

namespace juce
{

//==============================================================================
/**
    A type of timer class that can run multiple timers with different frequencies,
    all of which share a single callback.

    This class is very similar to the Timer class, but allows you run multiple
    separate timers, where each one has a unique ID number. The methods in this
    class are exactly equivalent to those in Timer, but with the addition of
    this ID number.

    To use it, you need to create a subclass of MultiTimer, implementing the
    timerCallback() method. Then you can start timers with startTimer(), and
    each time the callback is triggered, it passes in the ID of the timer that
    caused it.

    @see Timer

    @tags{Events}
*/
class JUCE_API  MultiTimer
{
protected:
    //==============================================================================
    /** Creates a MultiTimer.

        When created, no timers are running, so use startTimer() to start things off.
    */
    MultiTimer() noexcept;

    /** Creates a copy of another timer.

        Note that this timer will not contain any running timers, even if the one you're
        copying from was running.
    */
    MultiTimer (const MultiTimer&) noexcept;

public:
    //==============================================================================
    /** Destructor. */
    virtual ~MultiTimer();

    //==============================================================================
    /** The user-defined callback routine that actually gets called by each of the
        timers that are running.

        It's perfectly ok to call startTimer() or stopTimer() from within this
        callback to change the subsequent intervals.
    */
    virtual void timerCallback (int timerID) = 0;

    //==============================================================================
    /** Starts a timer and sets the length of interval required.

        If the timer is already started, this will reset it, so the
        time between calling this method and the next timer callback
        will not be less than the interval length passed in.

        @param timerID                  a unique Id number that identifies the timer to
                                        start. This is the id that will be passed back
                                        to the timerCallback() method when this timer is
                                        triggered
        @param  intervalInMilliseconds  the interval to use (any values less than 1 will be
                                        rounded up to 1)
    */
    void startTimer (int timerID, int intervalInMilliseconds) noexcept;

    /** Stops a timer.

        If a timer has been started with the given ID number, it will be cancelled.
        No more callbacks will be made for the specified timer after this method returns.

        If this is called from a different thread, any callbacks that may
        be currently executing may be allowed to finish before the method
        returns.
    */
    void stopTimer (int timerID) noexcept;

    //==============================================================================
    /** Checks whether a timer has been started for a specified ID.
        @returns true if a timer with the given ID is running.
    */
    bool isTimerRunning (int timerID) const noexcept;

    /** Returns the interval for a specified timer ID.
        @returns    the timer's interval in milliseconds if it's running, or 0 if no
                    timer was running for the ID number specified.
    */
    int getTimerInterval (int timerID) const noexcept;


    //==============================================================================
private:
    SpinLock timerListLock;
    OwnedArray<Timer> timers;

    Timer* getCallback (int) const noexcept;
    MultiTimer& operator= (const MultiTimer&);
};

} // namespace juce
