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

static bool exeIsAvailable (String executable)
{
    ChildProcess child;

    if (child.start ("which " + executable))
    {
        child.waitForProcessToFinish (60 * 1000);
        return (child.getExitCode() == 0);
    }

    return false;
}

static bool isSet (int flags, int toCheck)
{
    return (flags & toCheck) != 0;
}

class FileChooser::Native final : public FileChooser::Pimpl,
                                  private Timer
{
public:
    Native (FileChooser& fileChooser, int flags)
        : owner (fileChooser),
          // kdialog/zenity only support opening either files or directories.
          // Files should take precedence, if requested.
          isDirectory         (isSet (flags, FileBrowserComponent::canSelectDirectories) && ! isSet (flags, FileBrowserComponent::canSelectFiles)),
          isSave              (isSet (flags, FileBrowserComponent::saveMode)),
          selectMultipleFiles (isSet (flags, FileBrowserComponent::canSelectMultipleItems)),
          warnAboutOverwrite  (isSet (flags, FileBrowserComponent::warnAboutOverwriting))
    {
        const File previousWorkingDirectory (File::getCurrentWorkingDirectory());

        // use kdialog for KDE sessions or if zenity is missing
        if (exeIsAvailable ("kdialog") && (isKdeFullSession() || ! exeIsAvailable ("zenity")))
            addKDialogArgs();
        else
            addZenityArgs();
    }

    ~Native() override
    {
        finish (true);
    }

    void runModally() override
    {
       #if JUCE_MODAL_LOOPS_PERMITTED
        child.start (args, ChildProcess::wantStdOut);

        while (child.isRunning())
            if (! MessageManager::getInstance()->runDispatchLoopUntil (20))
                break;

        finish (false);
       #else
        jassertfalse;
       #endif
    }

    void launch() override
    {
        child.start (args, ChildProcess::wantStdOut);
        startTimer (100);
    }

private:
    FileChooser& owner;
    bool isDirectory, isSave, selectMultipleFiles, warnAboutOverwrite;

    ChildProcess child;
    StringArray args;
    String separator;

    void timerCallback() override
    {
        if (! child.isRunning())
        {
            stopTimer();
            finish (false);
        }
    }

    void finish (bool shouldKill)
    {
        String result;
        Array<URL> selection;

        if (shouldKill)
            child.kill();
        else
            result = child.readAllProcessOutput().trim();

        if (result.isNotEmpty())
        {
            StringArray tokens;

            if (selectMultipleFiles)
                tokens.addTokens (result, separator, "\"");
            else
                tokens.add (result);

            for (auto& token : tokens)
                selection.add (URL (File::getCurrentWorkingDirectory().getChildFile (token)));
        }

        if (! shouldKill)
        {
            child.waitForProcessToFinish (60 * 1000);
            owner.finished (selection);
        }
    }

    static uint64 getTopWindowID() noexcept
    {
        if (TopLevelWindow* top = TopLevelWindow::getActiveTopLevelWindow())
            return (uint64) (pointer_sized_uint) top->getWindowHandle();

        return 0;
    }

    static bool isKdeFullSession()
    {
        return SystemStats::getEnvironmentVariable ("KDE_FULL_SESSION", String())
                     .equalsIgnoreCase ("true");
    }

    void addKDialogArgs()
    {
        args.add ("kdialog");

        if (owner.title.isNotEmpty())
            args.add ("--title=" + owner.title);

        if (uint64 topWindowID = getTopWindowID())
        {
            args.add ("--attach");
            args.add (String (topWindowID));
        }

        if (selectMultipleFiles)
        {
            separator = "\n";
            args.add ("--multiple");
            args.add ("--separate-output");
            args.add ("--getopenfilename");
        }
        else
        {
            if (isSave)             args.add ("--getsavefilename");
            else if (isDirectory)   args.add ("--getexistingdirectory");
            else                    args.add ("--getopenfilename");
        }

        File startPath;

        if (owner.startingFile.exists())
        {
            startPath = owner.startingFile;
        }
        else if (owner.startingFile.getParentDirectory().exists())
        {
            startPath = owner.startingFile.getParentDirectory();
        }
        else
        {
            startPath = File::getSpecialLocation (File::userHomeDirectory);

            if (isSave)
                startPath = startPath.getChildFile (owner.startingFile.getFileName());
        }

        args.add (startPath.getFullPathName());
        args.add ("(" + owner.filters.replaceCharacter (';', ' ') + ")");
    }

    void addZenityArgs()
    {
        args.add ("zenity");
        args.add ("--file-selection");

        const auto getUnderstandsConfirmOverwrite = []
        {
            // --confirm-overwrite is deprecated in zenity 3.91 and higher
            ChildProcess process;
            process.start ("zenity --version");
            process.waitForProcessToFinish (1000);
            const auto versionString = process.readAllProcessOutput();
            const auto version = StringArray::fromTokens (versionString.trim(), ".", "");
            return version.size() >= 2
                   && (version[0].getIntValue() < 3
                       || (version[0].getIntValue() == 3 && version[1].getIntValue() < 91));
        };

        if (warnAboutOverwrite && getUnderstandsConfirmOverwrite())
            args.add ("--confirm-overwrite");

        if (owner.title.isNotEmpty())
            args.add ("--title=" + owner.title);

        if (selectMultipleFiles)
        {
            separator = ":";
            args.add ("--multiple");
            args.add ("--separator=" + separator);
        }
        else
        {
            if (isSave)
                args.add ("--save");
        }

        if (isDirectory)
            args.add ("--directory");

        if (owner.filters.isNotEmpty() && owner.filters != "*" && owner.filters != "*.*")
        {
            StringArray tokens;
            tokens.addTokens (owner.filters, ";,|", "\"");

            args.add ("--file-filter=" + tokens.joinIntoString (" "));
        }

        if (owner.startingFile.isDirectory())
            owner.startingFile.setAsCurrentWorkingDirectory();
        else if (owner.startingFile.getParentDirectory().exists())
            owner.startingFile.getParentDirectory().setAsCurrentWorkingDirectory();
        else
            File::getSpecialLocation (File::userHomeDirectory).setAsCurrentWorkingDirectory();

        auto filename = owner.startingFile.getFileName();

        if (! filename.isEmpty())
            args.add ("--filename=" + filename);

        // supplying the window ID of the topmost window makes sure that Zenity pops up..
        if (uint64 topWindowID = getTopWindowID())
            setenv ("WINDOWID", String (topWindowID).toRawUTF8(), true);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Native)
};

bool FileChooser::isPlatformDialogAvailable()
{
   #if JUCE_DISABLE_NATIVE_FILECHOOSERS
    return false;
   #else
    static bool canUseNativeBox = exeIsAvailable ("zenity") || exeIsAvailable ("kdialog");
    return canUseNativeBox;
   #endif
}

std::shared_ptr<FileChooser::Pimpl> FileChooser::showPlatformDialog (FileChooser& owner, int flags, FilePreviewComponent*)
{
    return std::make_shared<Native> (owner, flags);
}

} // namespace juce
