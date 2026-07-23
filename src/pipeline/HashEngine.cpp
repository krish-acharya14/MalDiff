#include "pipeline/HashEngine.hpp"

#include <iomanip>
#include <memory>
#include <sstream>
#include <openssl/evp.h>

namespace mede::pipeline {
    namespace {
        struct EvpMdCtxDeleter {
            void operator()(EVP_MD_CTX* ctx) const noexcept { EVP_MD_CTX_free(ctx); }
        };
        using EvpMdCtxPtr = std::unique_ptr<EVP_MD_CTX, EvpMdCtxDeleter>;

        std::string computeDigest(const EVP_MD* algorithm, std::span<const uint8_t>data) {
            EvpMdCtxPtr ctx(EVP_MD_CTX_new());
            if (!ctx) return {};
            if (EVP_DigestInit_ex(ctx.get(), algorithm, nullptr) != 1) return {};
            if (data.size() > 0 && EVP_DigestUpdate(ctx.get(), data.data(), data.size()) != 1) return {};

            unsigned char digest[EVP_MAX_MD_SIZE];
            unsigned int digestLen = 0;
            if (EVP_DigestFinal_ex(ctx.get(), digest, &digestLen) != 1) return {};

            return HashEngine::toHexString(digest, digestLen);
        }
    }

    common::Result<models::HashInfo> HashEngine::computeAll(std::span<const uint8_t> data) {
        models::HashInfo hashes;
        hashes.md5 = computeDigest(EVP_md5(), data);
        hashes.sha1 = computeDigest(EVP_sha1(), data);
        hashes.sha256 = computeDigest(EVP_sha256(), data);

        if (hashes.md5.empty() || hashes.sha1.empty() || hashes.sha256.empty())
            return common::Result<models::HashInfo>::failure("Hash computation failed (OpenSSL EVP error)");
        
        return common::Result<models::HashInfo>::success(std::move(hashes));
    }

    std::string HashEngine::toHexString(const unsigned char* bytes, std::size_t len) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (std::size_t i = 0; i < len; ++i) 
            oss << std::setw(2) << static_cast<int>(bytes[i]);
        
        return oss.str();
    }
}
