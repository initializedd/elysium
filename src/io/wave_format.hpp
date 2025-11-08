#pragma once

#include <fstream>
#include <cstdint>
#include <optional>
#include <vector>

struct wave_format {
    struct hdr {
        std::uint32_t id;
        std::uint32_t size;
        std::uint32_t fmt;
    } hdr;

    struct fmt {
        std::uint32_t id;
        std::uint32_t size;
        std::uint16_t fmt;
        std::uint16_t channels;
        std::uint32_t sample_rate;
        std::uint32_t byte_rate;
        std::uint16_t block_align;
        std::uint16_t bits_per_sample;
    } fmt;

    struct data {
        std::uint32_t id;
        std::uint32_t size;
        std::vector<std::byte> data; // TODO: investigate whether type param needs to change in some scenarios
    } data;
};

inline std::optional<wave_format> load_wave_file(const std::string_view& path) {
    std::ifstream ifs(path.data(), std::ios::binary);
    if (!ifs.is_open()) {
        return std::nullopt;
    }

    wave_format result;
    ifs.read(reinterpret_cast<char*>(&result.hdr.id), sizeof(result.hdr.id));
    ifs.read(reinterpret_cast<char*>(&result.hdr.size), sizeof(result.hdr.size));
    ifs.read(reinterpret_cast<char*>(&result.hdr.fmt), sizeof(result.hdr.fmt));
    ifs.read(reinterpret_cast<char*>(&result.fmt.id), sizeof(result.fmt.id));
    ifs.read(reinterpret_cast<char*>(&result.fmt.size), sizeof(result.fmt.size));
    ifs.read(reinterpret_cast<char*>(&result.fmt.fmt), sizeof(result.fmt.fmt));
    ifs.read(reinterpret_cast<char*>(&result.fmt.channels), sizeof(result.fmt.channels));
    ifs.read(reinterpret_cast<char*>(&result.fmt.sample_rate), sizeof(result.fmt.sample_rate));
    ifs.read(reinterpret_cast<char*>(&result.fmt.byte_rate), sizeof(result.fmt.byte_rate));
    ifs.read(reinterpret_cast<char*>(&result.fmt.block_align), sizeof(result.fmt.block_align));
    ifs.read(reinterpret_cast<char*>(&result.fmt.bits_per_sample), sizeof(result.fmt.bits_per_sample));
    ifs.read(reinterpret_cast<char*>(&result.data.id), sizeof(result.data.id));
    ifs.read(reinterpret_cast<char*>(&result.data.size), sizeof(result.data.size));
    result.data.data.resize(result.data.size);
    ifs.read(reinterpret_cast<char*>(result.data.data.data()), result.data.size);

    return result;
}

