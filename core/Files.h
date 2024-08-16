#ifndef SORBET_AST_FILES_H
#define SORBET_AST_FILES_H

#include "core/FileRef.h"
#include "core/LocOffsets.h"
#include "core/Names.h"
#include "core/PackagedLevel.h"
#include "core/StrictLevel.h"
#include <string>
#include <string_view>

namespace sorbet::core {
class GlobalState;
struct LocalSymbolTableHashes;
struct FileHash;
namespace serialize {
class SerializerImpl;
}

class File final {
public:
    enum class Type : uint8_t {
        NotYetRead,
        PayloadGeneration, // Files marked during --store-state
        Payload,           // Files loaded from the binary payload
        Normal,
        TombStone,
    };

    // Epoch is _only_ used in LSP mode. Do not depend on it elsewhere.
    // TODO(jvilk): Delurk epoch usage and use something like pointer equality to check if a file has changed.
    const uint32_t epoch;

    friend class GlobalState;
    friend class ::sorbet::core::serialize::SerializerImpl;

    static StrictLevel fileStrictSigil(std::string_view source);
    static PackagedLevel filePackagedSigil(std::string_view source);

    std::string_view path() const;
    std::string_view source() const;
    Type sourceType;

    bool isPayload() const;
    bool isRBI() const;
    bool isStdlib() const;
    bool isPackageRBI() const;

    bool permitOverloadDefinitions() const;

    static bool isRBIPath(std::string_view path);
    static bool isPackagePath(std::string_view path);

    bool isPackage() const;

    // Whether the file is open in the LSP client. (Always false if not running under LSP.)
    bool isOpenInClient() const;
    // Set whether the file is open in the LSP client. Should only be used by LSPPreprocessor when
    // creating files using the set of openFiles.
    void setIsOpenInClient(bool isOpenInClient);

    // flag accessors
    bool isPackagedTest() const;

    bool hasParseErrors() const;
    void setHasParseErrors(bool value);

    bool cached() const;
    void setCached(bool value);

    // Returns whether or not this file is considered to be packaged.
    bool isPackaged() const;

    File(std::string &&path_, std::string &&source_, Type sourceType, uint32_t epoch = 0);
    File(File &&other) = delete;
    File(const File &other) = delete;
    File() = delete;
    std::unique_ptr<File> deepCopy(GlobalState &) const;
    std::vector<int> &lineBreaks() const;
    int lineCount() const;
    StrictLevel minErrorLevel() const;

    /** Given a 1-based line number, returns a string view of the line. */
    std::string_view getLine(int i) const;

    void setFileHash(std::unique_ptr<const FileHash> hash);
    const std::shared_ptr<const FileHash> &getFileHash() const;

    static constexpr std::string_view URL_PREFIX = "https://github.com/sorbet/sorbet/tree/master/";
    static std::string censorFilePathForSnapshotTests(std::string_view orig);

private:
    struct Flags {
        // if 'true' file is completely cached in kvstore
        bool cached : 1;
        // some reasonable invariants don't hold for invalid files
        bool hasParseErrors : 1;
        // only relevant in --stripe-packages mode: is the file a `.test.rb` file?
        bool isPackagedTest : 1;
        // if true, the file is an rbi generated by --package-gen-output
        bool isPackageRBI : 1;
        // if true, the file is a stripe package file
        bool isPackage : 1;
        // Documented on public accessors
        bool isOpenInClient : 1;

        Flags(std::string_view path);
    };
    CheckSize(Flags, 1, 1);

    Flags flags;

private:
    const PackagedLevel packagedLevel;

public:
    const std::string path_;
    const std::string source_;
    mutable std::shared_ptr<std::vector<int>> lineBreaks_;

    mutable StrictLevel minErrorLevel_ = StrictLevel::Max;

public:
    const StrictLevel originalSigil;
    StrictLevel strictLevel;

private:
    std::shared_ptr<const FileHash> hash_;
};

CheckSize(File, 96, 8);

template <typename H> H AbslHashValue(H h, const FileRef &m) {
    return H::combine(std::move(h), m.id());
}
} // namespace sorbet::core
#endif // SORBET_AST_FILES_H
