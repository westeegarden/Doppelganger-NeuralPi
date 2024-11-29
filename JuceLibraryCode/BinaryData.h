/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   BluesJr_json;
    const int            BluesJr_jsonSize = 42659;

    extern const char*   BluesJR_FullD_json;
    const int            BluesJR_FullD_jsonSize = 40529;

    extern const char*   doppelgangerLogo4_PNG;
    const int            doppelgangerLogo4_PNGSize = 697139;

    extern const char*   guiUpdate_JPG;
    const int            guiUpdate_JPGSize = 51927;

    extern const char*   HT40_Overdrive_json;
    const int            HT40_Overdrive_jsonSize = 42499;

    extern const char*   npi_background_jpg;
    const int            npi_background_jpgSize = 47900;

    extern const char*   TS9_json;
    const int            TS9_jsonSize = 42593;

    extern const char*   TS9_FullD_json;
    const int            TS9_FullD_jsonSize = 40673;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 8;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
