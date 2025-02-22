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
    This class contains some helpful static methods for dealing with decibel values.

    @tags{Audio}
*/
class Decibels
{
public:
    //==============================================================================
    /** Converts a dBFS value to its equivalent gain level.

        A gain of 1.0 = 0 dB, and lower gains map onto negative decibel values. Any
        decibel value lower than minusInfinityDb will return a gain of 0.
    */
    template <typename Type>
    static Type decibelsToGain (Type decibels,
                                Type minusInfinityDb = Type (defaultMinusInfinitydB))
    {
        return decibels > minusInfinityDb ? std::pow (Type (10.0), decibels * Type (0.05))
                                          : Type();
    }

    /** Converts a gain level into a dBFS value.

        A gain of 1.0 = 0 dB, and lower gains map onto negative decibel values.
        If the gain is 0 (or negative), then the method will return the value
        provided as minusInfinityDb.
    */
    template <typename Type>
    static Type gainToDecibels (Type gain,
                                Type minusInfinityDb = Type (defaultMinusInfinitydB))
    {
        return gain > Type() ? jmax (minusInfinityDb, static_cast<Type> (std::log10 (gain)) * Type (20.0))
                             : minusInfinityDb;
    }

    /** Restricts a gain value based on a lower bound specified in dBFS.

        This is useful if you want to make sure a gain value never reaches zero.
    */
    template <typename Type>
    static Type gainWithLowerBound (Type gain, Type lowerBoundDb)
    {
        // You probably want to use a negative decibel value or the gain will
        // be restricted to boosting only!
        jassert (lowerBoundDb < (Type) 0.0);

        return jmax ((Type) gain, Decibels::decibelsToGain (lowerBoundDb, lowerBoundDb - (Type) 1.0));
    }

    //==============================================================================
    /** Converts a decibel reading to a string.

        By default the returned string will have the 'dB' suffix added, but this can be removed by
        setting the shouldIncludeSuffix argument to false. If a customMinusInfinityString argument
        is provided this will be returned if the value is lower than minusInfinityDb, otherwise
        the return value will be "-INF".
    */
    template <typename Type>
    static String toString (Type decibels,
                            int decimalPlaces = 2,
                            Type minusInfinityDb = Type (defaultMinusInfinitydB),
                            bool shouldIncludeSuffix = true,
                            StringRef customMinusInfinityString = {})
    {
        String s;
        s.preallocateBytes (20);

        if (decibels <= minusInfinityDb)
        {
            if (customMinusInfinityString.isEmpty())
                s << "-INF";
            else
                s << customMinusInfinityString;
        }
        else
        {
            if (decibels >= Type())
                s << '+';

            if (decimalPlaces <= 0)
                s << roundToInt (decibels);
            else
                s << String (decibels, decimalPlaces);
        }

        if (shouldIncludeSuffix)
            s << " dB";

        return s;
    }

private:
    //==============================================================================
    enum { defaultMinusInfinitydB = -100 };

    Decibels() = delete; // This class can't be instantiated, it's just a holder for static methods..
};

} // namespace juce
