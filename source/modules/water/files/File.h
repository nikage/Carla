/*
  ==============================================================================

   This file is part of the Water library.
   Copyright (c) 2016 ROLI Ltd.
   Copyright (C) 2017-2022 Filipe Coelho <falktx@falktx.com>

   Permission is granted to use this software under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license/

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
   FITNESS. IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT,
   OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
   USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
   TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
   OF THIS SOFTWARE.

  ==============================================================================
*/

#ifndef WATER_FILE_H_INCLUDED
#define WATER_FILE_H_INCLUDED

#include "../containers/Array.h"
#include "../misc/Result.h"
#include "../text/String.h"

#include <vector>

namespace water {

//==============================================================================
/**
    Represents a local file or directory.

    This class encapsulates the absolute pathname of a file or directory, and
    has methods for finding out about the file and changing its properties.

    To read or write to the file, there are methods for returning an input or
    output stream.

    @see FileInputStream, FileOutputStream
*/
class File
{
public:
    //==============================================================================
    /** Creates an (invalid) file object.

        The file is initially set to an empty path, so getFullPathName() will return
        an empty string.

        You can use its operator= method to point it at a proper file.
    */
    File() noexcept;

    /** Creates a file from an absolute path.

        If the path supplied is a relative path, it is taken to be relative
        to the current working directory (see File::getCurrentWorkingDirectory()),
        but this isn't a recommended way of creating a file, because you
        never know what the CWD is going to be.

        On the Mac/Linux, the path can include "~" notation for referring to
        user home directories.
    */
    File (const String& absolutePath);

    /** Creates a copy of another file object. */
    File (const File&);

    /** Destructor. */
    ~File() noexcept  {}

    /** Sets the file based on an absolute pathname.

        If the path supplied is a relative path, it is taken to be relative
        to the current working directory (see File::getCurrentWorkingDirectory()),
        but this isn't a recommended way of creating a file, because you
        never know what the CWD is going to be.

        On the Mac/Linux, the path can include "~" notation for referring to
        user home directories.
    */
    File& operator= (const String& newAbsolutePath);

    /** Copies from another file object. */
    File& operator= (const File& otherFile);

    //==============================================================================
    /** Checks whether the file actually exists.

        @returns    true if the file exists, either as a file or a directory.
        @see existsAsFile, isDirectory
    */
    bool exists() const;

    /** Checks whether the file exists and is a file rather than a directory.

        @returns    true only if this is a real file, false if it's a directory
                    or doesn't exist
        @see exists, isDirectory
    */
    bool existsAsFile() const;

    /** Checks whether the file is a directory that exists.

        @returns    true only if the file is a directory which actually exists, so
                    false if it's a file or doesn't exist at all
        @see exists, existsAsFile
    */
    bool isDirectory() const;

    /** Checks whether the file is invalid (empty path).
    */
    bool isNull() const;

    /** Checks whether the file is valid (non-empty path).
    */
    bool isNotNull() const;

    /** Returns the size of the file in bytes.

        @returns    the number of bytes in the file, or 0 if it doesn't exist.
    */
    int64 getSize() const;

    /** Utility function to convert a file size in bytes to a neat string description.

        So for example 100 would return "100 bytes", 2000 would return "2 KB",
        2000000 would produce "2 MB", etc.
    */
    static String descriptionOfSizeInBytes (int64 bytes);

    //==============================================================================
    /** Returns the complete, absolute path of this file.

        This includes the filename and all its parent folders. On Windows it'll
        also include the drive letter prefix; on Mac or Linux it'll be a complete
        path starting from the root folder.

        If you just want the file's name, you should use getFileName() or
        getFileNameWithoutExtension().

        @see getFileName, getRelativePathFrom
    */
    const String& getFullPathName() const noexcept          { return fullPath; }

    /** Returns the last section of the pathname.

        Returns just the final part of the path - e.g. if the whole path
        is "/moose/fish/foo.txt" this will return "foo.txt".

        For a directory, it returns the final part of the path - e.g. for the
        directory "/moose/fish" it'll return "fish".

        If the filename begins with a dot, it'll return the whole filename, e.g. for
        "/moose/.fish", it'll return ".fish"

        @see getFullPathName, getFileNameWithoutExtension
    */
    String getFileName() const;

    /** Creates a relative path that refers to a file relatively to a given directory.

        e.g. File ("/moose/foo.txt").getRelativePathFrom (File ("/moose/fish/haddock"))
             would return "../../foo.txt".

        If it's not possible to navigate from one file to the other, an absolute
        path is returned. If the paths are invalid, an empty string may also be
        returned.

        @param directoryToBeRelativeTo  the directory which the resultant string will
                                        be relative to. If this is actually a file rather than
                                        a directory, its parent directory will be used instead.
                                        If it doesn't exist, it's assumed to be a directory.
        @see getChildFile, isAbsolutePath
    */
    String getRelativePathFrom (const File& directoryToBeRelativeTo) const;

    //==============================================================================
    /** Returns the file's extension.

        Returns the file extension of this file, also including the dot.

        e.g. "/moose/fish/foo.txt" would return ".txt"

        @see hasFileExtension, withFileExtension, getFileNameWithoutExtension
    */
    String getFileExtension() const;

    /** Checks whether the file has a given extension.

        @param extensionToTest  the extension to look for - it doesn't matter whether or
                                not this string has a dot at the start, so ".wav" and "wav"
                                will have the same effect. To compare with multiple extensions, this
                                parameter can contain multiple strings, separated by semi-colons -
                                so, for example: hasFileExtension (".jpeg;png;gif") would return
                                true if the file has any of those three extensions.

        @see getFileExtension, withFileExtension, getFileNameWithoutExtension
    */
    bool hasFileExtension (StringRef extensionToTest) const;

    /** Returns a version of this file with a different file extension.

        e.g. File ("/moose/fish/foo.txt").withFileExtension ("html") returns "/moose/fish/foo.html"

        @param newExtension     the new extension, either with or without a dot at the start (this
                                doesn't make any difference). To get remove a file's extension altogether,
                                pass an empty string into this function.

        @see getFileName, getFileExtension, hasFileExtension, getFileNameWithoutExtension
    */
    File withFileExtension (StringRef newExtension) const;

    /** Returns the last part of the filename, without its file extension.

        e.g. for "/moose/fish/foo.txt" this will return "foo".

        @see getFileName, getFileExtension, hasFileExtension, withFileExtension
    */
    String getFileNameWithoutExtension() const;

    //==============================================================================
    /** Returns a file that represents a relative (or absolute) sub-path of the current one.

        This will find a child file or directory of the current object.

        e.g.
            File ("/moose/fish").getChildFile ("foo.txt") will produce "/moose/fish/foo.txt".
            File ("/moose/fish").getChildFile ("haddock/foo.txt") will produce "/moose/fish/haddock/foo.txt".
            File ("/moose/fish").getChildFile ("../foo.txt") will produce "/moose/foo.txt".

        If the string is actually an absolute path, it will be treated as such, e.g.
            File ("/moose/fish").getChildFile ("/foo.txt") will produce "/foo.txt"

        @see getSiblingFile, getParentDirectory, getRelativePathFrom, isAChildOf
    */
    File getChildFile (StringRef relativeOrAbsolutePath) const;

    /** Returns a file which is in the same directory as this one.

        This is equivalent to getParentDirectory().getChildFile (name).

        @see getChildFile, getParentDirectory
    */
    File getSiblingFile (StringRef siblingFileName) const;

    //==============================================================================
    /** Returns the directory that contains this file or directory.

        e.g. for "/moose/fish/foo.txt" this will return "/moose/fish".
    */
    File getParentDirectory() const;

    /** Checks whether a file is somewhere inside a directory.

        Returns true if this file is somewhere inside a subdirectory of the directory
        that is passed in. Neither file actually has to exist, because the function
        just checks the paths for similarities.

        e.g. File ("/moose/fish/foo.txt").isAChildOf ("/moose") is true.
             File ("/moose/fish/foo.txt").isAChildOf ("/moose/fish") is also true.
    */
    bool isAChildOf (const File& potentialParentDirectory) const;

    //==============================================================================
    /** Chooses a filename relative to this one that doesn't already exist.

        If this file is a directory, this will return a child file of this
        directory that doesn't exist, by adding numbers to a prefix and suffix until
        it finds one that isn't already there.

        If the prefix + the suffix doesn't exist, it won't bother adding a number.

        e.g. File ("/moose/fish").getNonexistentChildFile ("foo", ".txt", true) might
             return "/moose/fish/foo(2).txt" if there's already a file called "foo.txt".

        @param prefix                   the string to use for the filename before the number
        @param suffix                   the string to add to the filename after the number
        @param putNumbersInBrackets     if true, this will create filenames in the
                                        format "prefix(number)suffix", if false, it will leave the
                                        brackets out.
    */
    File getNonexistentChildFile (const String& prefix,
                                  const String& suffix,
                                  bool putNumbersInBrackets = true) const;

    /** Chooses a filename for a sibling file to this one that doesn't already exist.

        If this file doesn't exist, this will just return itself, otherwise it
        will return an appropriate sibling that doesn't exist, e.g. if a file
        "/moose/fish/foo.txt" exists, this might return "/moose/fish/foo(2).txt".

        @param putNumbersInBrackets     whether to add brackets around the numbers that
                                        get appended to the new filename.
    */
    File getNonexistentSibling (bool putNumbersInBrackets = true) const;

    //==============================================================================
    /** Compares the pathnames for two files. */
    bool operator== (const File&) const;
    /** Compares the pathnames for two files. */
    bool operator!= (const File&) const;
    /** Compares the pathnames for two files. */
    bool operator< (const File&) const;
    /** Compares the pathnames for two files. */
    bool operator> (const File&) const;

    //==============================================================================
    /** Checks whether a file can be created or written to.

        @returns    true if it's possible to create and write to this file. If the file
                    doesn't already exist, this will check its parent directory to
                    see if writing is allowed.
        @see setReadOnly
    */
    bool hasWriteAccess() const;

    /** Changes the write-permission of a file or directory.

        @param shouldBeReadOnly     whether to add or remove write-permission
        @param applyRecursively     if the file is a directory and this is true, it will
                                    recurse through all the subfolders changing the permissions
                                    of all files
        @returns    true if it manages to change the file's permissions.
        @see hasWriteAccess
    */
    bool setReadOnly (bool shouldBeReadOnly,
                      bool applyRecursively = false) const;

    /** Changes the execute-permissions of a file.

        @param shouldBeExecutable   whether to add or remove execute-permission
        @returns    true if it manages to change the file's permissions.
    */
    bool setExecutePermission (bool shouldBeExecutable) const;

    /** Returns true if this file is a hidden or system file.
        The criteria for deciding whether a file is hidden are platform-dependent.
    */
    bool isHidden() const;

    /** Returns a unique identifier for the file, if one is available.

        Depending on the OS and file-system, this may be a unix inode number or
        a win32 file identifier, or 0 if it fails to find one. The number will
        be unique on the filesystem, but not globally.
    */
    uint64 getFileIdentifier() const;

    /** If possible, this will try to create a version string for the given file.

        The OS may be able to look at the file and give a version for it - e.g. with
        executables, bundles, dlls, etc. If no version is available, this will
        return an empty string.
    */
    String getVersion() const;

    //==============================================================================
    /** Creates an empty file if it doesn't already exist.

        If the file that this object refers to doesn't exist, this will create a file
        of zero size.

        If it already exists or is a directory, this method will do nothing.

        If the parent directories of the File do not exist then this method will
        recursively create the parent directories.

        @returns    a result to indicate whether the file was created successfully,
                    or an error message if it failed.
        @see createDirectory
    */
    Result create() const;

    /** Creates a new directory for this filename.

        This will try to create the file as a directory, and will also create
        any parent directories it needs in order to complete the operation.

        @returns    a result to indicate whether the directory was created successfully, or
                    an error message if it failed.
        @see create
    */
    Result createDirectory() const;

    /** Deletes a file.

        If this file is actually a directory, it may not be deleted correctly if it
        contains files. See deleteRecursively() as a better way of deleting directories.

        @returns    true if the file has been successfully deleted (or if it didn't exist to
                    begin with).
        @see deleteRecursively
    */
    bool deleteFile() const;

    /** Deletes a file or directory and all its subdirectories.

        If this file is a directory, this will try to delete it and all its subfolders. If
        it's just a file, it will just try to delete the file.

        @returns    true if the file and all its subfolders have been successfully deleted
                    (or if it didn't exist to begin with).
        @see deleteFile
    */
    bool deleteRecursively() const;

    /** Moves or renames a file.

        Tries to move a file to a different location.
        If the target file already exists, this will attempt to delete it first, and
        will fail if this can't be done.

        Note that the destination file isn't the directory to put it in, it's the actual
        filename that you want the new file to have.

        Also note that on some OSes (e.g. Windows), moving files between different
        volumes may not be possible.

        @returns    true if the operation succeeds
    */
    bool moveFileTo (const File& targetLocation) const;

    /** Copies a file.

        Tries to copy a file to a different location.
        If the target file already exists, this will attempt to delete it first, and
        will fail if this can't be done.

        @returns    true if the operation succeeds
    */
    bool copyFileTo (const File& targetLocation) const;

    /** Replaces a file.

        Replace the file in the given location, assuming the replaced files identity.
        Depending on the file system this will preserve file attributes such as
        creation date, short file name, etc.

        If replacement succeeds the original file is deleted.

        @returns    true if the operation succeeds
    */
    bool replaceFileIn (const File& targetLocation) const;

    /** Copies a directory.

        Tries to copy an entire directory, recursively.

        If this file isn't a directory or if any target files can't be created, this
        will return false.

        @param newDirectory    the directory that this one should be copied to. Note that this
                               is the name of the actual directory to create, not the directory
                               into which the new one should be placed, so there must be enough
                               write privileges to create it if it doesn't exist. Any files inside
                               it will be overwritten by similarly named ones that are copied.
    */
    bool copyDirectoryTo (const File& newDirectory) const;

    //==============================================================================
    /** Used in file searching, to specify whether to return files, directories, or both.
    */
    enum TypesOfFileToFind
    {
        findDirectories             = 1,    /**< Use this flag to indicate that you want to find directories. */
        findFiles                   = 2,    /**< Use this flag to indicate that you want to find files. */
        findFilesAndDirectories     = 3,    /**< Use this flag to indicate that you want to find both files and directories. */
        ignoreHiddenFiles           = 4     /**< Add this flag to avoid returning any hidden files in the results. */
    };

    /** Searches inside a directory for files matching a wildcard pattern.

        Assuming that this file is a directory, this method will search it
        for either files or subdirectories whose names match a filename pattern.

        @param results                  an array to which File objects will be added for the
                                        files that the search comes up with
        @param whatToLookFor            a value from the TypesOfFileToFind enum, specifying whether to
                                        return files, directories, or both. If the ignoreHiddenFiles flag
                                        is also added to this value, hidden files won't be returned
        @param searchRecursively        if true, all subdirectories will be recursed into to do
                                        an exhaustive search
        @param wildCardPattern          the filename pattern to search for, e.g. "*.txt"
        @returns                        the number of results that have been found

        @see getNumberOfChildFiles, DirectoryIterator
    */
    int findChildFiles (std::vector<File>& results,
                        int whatToLookFor,
                        bool searchRecursively,
                        const String& wildCardPattern = "*") const;

    /** Searches inside a directory and counts how many files match a wildcard pattern.

        Assuming that this file is a directory, this method will search it
        for either files or subdirectories whose names match a filename pattern,
        and will return the number of matches found.

        This isn't a recursive call, and will only search this directory, not
        its children.

        @param whatToLookFor    a value from the TypesOfFileToFind enum, specifying whether to
                                count files, directories, or both. If the ignoreHiddenFiles flag
                                is also added to this value, hidden files won't be counted
        @param wildCardPattern  the filename pattern to search for, e.g. "*.txt"
        @returns                the number of matches found
        @see findChildFiles, DirectoryIterator
    */
    int getNumberOfChildFiles (int whatToLookFor,
                               const String& wildCardPattern = "*") const;

    /** Returns true if this file is a directory that contains one or more subdirectories.
        @see isDirectory, findChildFiles
    */
    bool containsSubDirectories() const;

    //==============================================================================
    /** Creates a stream to read from this file.

        @returns    a stream that will read from this file (initially positioned at the
                    start of the file), or nullptr if the file can't be opened for some reason
        @see createOutputStream, loadFileAsData
    */
    FileInputStream* createInputStream() const;

    /** Creates a stream to write to this file.

        If the file exists, the stream that is returned will be positioned ready for
        writing at the end of the file, so you might want to use deleteFile() first
        to write to an empty file.

        @returns    a stream that will write to this file (initially positioned at the
                    end of the file), or nullptr if the file can't be opened for some reason
        @see createInputStream, appendData, appendText
    */
    FileOutputStream* createOutputStream (size_t bufferSize = 0x8000) const;

    //==============================================================================
    /** Loads a file's contents into memory as a block of binary data.

        Of course, trying to load a very large file into memory will blow up, so
        it's better to check first.

        @param result   the data block to which the file's contents should be appended - note
                        that if the memory block might already contain some data, you
                        might want to clear it first
        @returns        true if the file could all be read into memory
    */
    bool loadFileAsData (MemoryBlock& result) const;

    /** Reads a file into memory as a string.

        Attempts to load the entire file as a zero-terminated string.

        This makes use of InputStream::readEntireStreamAsString, which can
        read either UTF-16 or UTF-8 file formats.
    */
    String loadFileAsString() const;

    /** Reads the contents of this file as text and splits it into lines, which are
        appended to the given StringArray.
    */
    void readLines (StringArray& destLines) const;

    //==============================================================================
    /** Appends a block of binary data to the end of the file.

        This will try to write the given buffer to the end of the file.

        @returns false if it can't write to the file for some reason
    */
    bool appendData (const void* dataToAppend,
                     size_t numberOfBytes) const;

    /** Replaces this file's contents with a given block of data.

        This will delete the file and replace it with the given data.

        A nice feature of this method is that it's safe - instead of deleting
        the file first and then re-writing it, it creates a new temporary file,
        writes the data to that, and then moves the new file to replace the existing
        file. This means that if the power gets pulled out or something crashes,
        you're a lot less likely to end up with a corrupted or unfinished file..

        Returns true if the operation succeeds, or false if it fails.

        @see appendText
    */
    bool replaceWithData (const void* dataToWrite,
                          size_t numberOfBytes) const;

    /** Appends a string to the end of the file.

        This will try to append a text string to the file, as either 16-bit unicode
        or 8-bit characters in the default system encoding.

        It can also write the 'ff fe' unicode header bytes before the text to indicate
        the endianness of the file.

        Any single \\n characters in the string are replaced with \\r\\n before it is written.

        @see replaceWithText
    */
    bool appendText (const String& textToAppend,
                     bool asUnicode = false,
                     bool writeUnicodeHeaderBytes = false) const;

    /** Replaces this file's contents with a given text string.

        This will delete the file and replace it with the given text.

        A nice feature of this method is that it's safe - instead of deleting
        the file first and then re-writing it, it creates a new temporary file,
        writes the text to that, and then moves the new file to replace the existing
        file. This means that if the power gets pulled out or something crashes,
        you're a lot less likely to end up with an empty file..

        For an explanation of the parameters here, see the appendText() method.

        Returns true if the operation succeeds, or false if it fails.

        @see appendText
    */
    bool replaceWithText (const String& textToWrite,
                          bool asUnicode = false,
                          bool writeUnicodeHeaderBytes = false) const;

    /** Attempts to scan the contents of this file and compare it to another file, returning
        true if this is possible and they match byte-for-byte.
    */
    bool hasIdenticalContentTo (const File& other) const;

    //==============================================================================
    /** A set of types of location that can be passed to the getSpecialLocation() method.
    */
    enum SpecialLocationType
    {
        /** The user's home folder. This is the same as using File ("~"). */
        userHomeDirectory,

        /** The folder that should be used for temporary files.
            Always delete them when you're finished, to keep the user's computer tidy!
        */
        tempDirectory,

        /** Returns this application's executable file.

            If running as a plug-in or DLL, this will (where possible) be the DLL rather than the
            host app.

            On the mac this will return the unix binary, not the package folder.
        */
        currentExecutableFile,

        /** In a plugin, this will return the path of the host executable. */
        hostApplicationPath
    };

    /** Finds the location of a special type of file or directory, such as a home folder or
        documents folder.

        @see SpecialLocationType
    */
    static File  getSpecialLocation (const SpecialLocationType type);

    //==============================================================================
    /** Returns a temporary file in the system's temp directory.
        This will try to return the name of a non-existent temp file.
        To get the temp folder, you can use getSpecialLocation (File::tempDirectory).
    */
    static File createTempFile (StringRef fileNameEnding);

    //==============================================================================
    /** Returns the current working directory.
        @see setAsCurrentWorkingDirectory
    */
    static File getCurrentWorkingDirectory();

    /** Sets the current working directory to be this file.

        For this to work the file must point to a valid directory.

        @returns true if the current directory has been changed.
        @see getCurrentWorkingDirectory
    */
    bool setAsCurrentWorkingDirectory() const;

    //==============================================================================
    /** The system-specific file separator character.
        On Windows, this will be '\', on Mac/Linux, it'll be '/'
    */
    static const water_uchar separator;

    /** The system-specific file separator character, as a string.
        On Windows, this will be '\', on Mac/Linux, it'll be '/'
    */
    static const String separatorString;

    //==============================================================================
    /** Returns a version of a filename with any illegal characters removed.

        This will return a copy of the given string after removing characters
        that are not allowed in a legal filename, and possibly shortening the
        string if it's too long.

        Because this will remove slashes, don't use it on an absolute pathname - use
        createLegalPathName() for that.

        @see createLegalPathName
    */
    static String createLegalFileName (const String& fileNameToFix);

    /** Returns a version of a path with any illegal characters removed.

        Similar to createLegalFileName(), but this won't remove slashes, so can
        be used on a complete pathname.

        @see createLegalFileName
    */
    static String createLegalPathName (const String& pathNameToFix);

    /** Indicates whether filenames are case-sensitive on the current operating system. */
    static bool areFileNamesCaseSensitive();

    /** Returns true if the string seems to be a fully-specified absolute path. */
    static bool isAbsolutePath (StringRef path);

    /** Creates a file that simply contains this string, without doing the sanity-checking
        that the normal constructors do.

        Best to avoid this unless you really know what you're doing.
    */
    static File createFileWithoutCheckingPath (const String& absolutePath) noexcept;

    /** Adds a separator character to the end of a path if it doesn't already have one. */
    static String addTrailingSeparator (const String& path);

    //==============================================================================
    /** Tries to create a symbolic link and returns a boolean to indicate success */
    bool createSymbolicLink (const File& linkFileToCreate, bool overwriteExisting) const;

    /** Returns true if this file is a link or alias that can be followed using getLinkedTarget(). */
    bool isSymbolicLink() const;

    /** If this file is a link or alias, this returns the file that it points to.
        If the file isn't actually link, it'll just return itself.
    */
    File getLinkedTarget() const;

    //==============================================================================
    struct NaturalFileComparator
    {
        NaturalFileComparator (bool shouldPutFoldersFirst) noexcept : foldersFirst (shouldPutFoldersFirst) {}

        int compareElements (const File& firstFile, const File& secondFile) const
        {
            if (foldersFirst && (firstFile.isDirectory() != secondFile.isDirectory()))
                return firstFile.isDirectory() ? -1 : 1;

            return firstFile.getFullPathName().compareNatural (secondFile.getFullPathName(), File::areFileNamesCaseSensitive());
        }

        bool foldersFirst;
    };

private:
    //==============================================================================
    String fullPath;

    static String parseAbsolutePath (const String&);
    String getPathUpToLastSlash() const;

    Result createDirectoryInternal (const String&) const;
    bool copyInternal (const File&) const;
    bool moveInternal (const File&) const;
    bool replaceInternal (const File&) const;
    bool setFileTimesInternal (int64 m, int64 a, int64 c) const;
    void getFileTimesInternal (int64& m, int64& a, int64& c) const;
    bool setFileReadOnlyInternal (bool) const;
    bool setFileExecutableInternal (bool) const;
};

}

#endif // WATER_FILE_H_INCLUDED
