#pragma once

#include <stdexcept>
#include <iostream>
#include <queue>
#include <vector>
#include <mutex>
#include <stop_token>
#include <unordered_map>
#include <AL/alc.h>
#include <AL/al.h>

#include "../io/wave_format.hpp"

namespace elysium {

enum class system_t {
    sound
};

template <system_t t> requires (t == system_t::sound)
class system {
private:
    std::mutex mtx;
    std::vector<ALuint> buffers;
    std::vector<ALuint> sources;
    std::unordered_map<std::string_view, int> name_to_source_idx;
    std::queue<ALuint> commands;

public:
    system_t type = t;

    system() {
        ALCdevice* device = alcOpenDevice(nullptr);
        if (!device) {
            throw std::runtime_error("Failed to open the default audio device");
        }

        ALCcontext* context = alcCreateContext(device, nullptr);
        if (!context) {
            throw std::runtime_error("Failed to create an audio context using the default audio device");
        }
        if (alcGetError(device) != ALC_NO_ERROR) {
            throw std::runtime_error("Audio context already exists for device or device is invalid");
        }

        if (!alcMakeContextCurrent(context)) {
            throw std::runtime_error("Failed to set the current audio context");
        }
    }

    ~system() {
        {
            std::scoped_lock lock(mtx); // TODO: consider alternatives
            name_to_source_idx.clear();
            alDeleteSources(sources.size(), sources.data());
            if (alGetError() != AL_NO_ERROR) {
                std::cout << "Failed to delete sources\n";
            }
            sources.clear();

            alDeleteBuffers(buffers.size(), buffers.data());
            if (alGetError() != AL_NO_ERROR) {
                std::cout << "Failed to delete buffers\n";
            }
            buffers.clear();
        }

        ALCcontext* context = alcGetCurrentContext();
        if (!context) {
            std::cout << "Failed to get the current audio context\n";
        }

        ALCdevice* device = alcGetContextsDevice(context);
        if (!device) {
            std::cout << "Failed to get the audio device used in the current context\n";
        }
        if (alcGetError(device) != ALC_NO_ERROR) {
            std::cout << "The current context is invalid\n";
        }

        if (!alcMakeContextCurrent(nullptr)) {
            std::cout << "Failed to set the current audio context\n";
        }
        if (alGetError() != AL_NO_ERROR) {
            std::cout << "The context specified is invalid\n";
        }

        alcDestroyContext(context);
        if (alcGetError(device) != AL_NO_ERROR) {
            std::cout << "The context specified is invalid\n";
        }

        if (!alcCloseDevice(device)) {
            std::cout << "Failed to close the audio device that was used with the previous audio context\n";
        }
        if (alcGetError(device) != AL_NO_ERROR) {
            std::cout << "The device specified is invalid\n";
        }
    }

    void event_loop(std::stop_token token) {
        while (!token.stop_requested()) {
            std::scoped_lock lock{mtx};
            while (!commands.empty()) {
                int idx = commands.front();
                commands.pop();

                alSourcePlay(sources[idx]);
                if (alGetError() != AL_NO_ERROR) {
                    std::cout << "OpenAL failed to play source\n";
                }
            }
        }
    }

    void play_sound(const std::string_view& name) {
        std::scoped_lock lock(mtx);
        int idx = name_to_source_idx[name];
        commands.push(idx);
    }

    void adjust_sound_volume(const std::string_view& name, const float volume) {
        std::scoped_lock lock{mtx};
        int idx = name_to_source_idx[name];
        alSourcef(sources[idx], AL_GAIN, volume);
    }

    void load_sound(const std::string_view& name, const std::string_view& path) {
        std::optional<wave_format> wave_file = load_wave_file(path);
        if (!wave_file.has_value()) {
            std::cout << "Failed to load sound: " << path << '\n';
            return;
        }

        ALuint buffer_id;
        alGenBuffers(1, &buffer_id);
        if (alGetError() != AL_NO_ERROR) {
          std::cout << "OpenAL failed to generate buffers\n";
        }

        alBufferData(buffer_id, AL_FORMAT_MONO8, wave_file->data.data.data(), wave_file->data.size, wave_file->fmt.sample_rate);
        if (alGetError() != AL_NO_ERROR) {
          std::cout << "OpenAL failed to buffer data\n";
        }

        ALuint source_id;
        alGenSources(1, &source_id);
        if (alGetError() != AL_NO_ERROR) {
          std::cout << "OpenAL failed to generate sources\n";
        }

        alSourcei(source_id, AL_BUFFER, buffer_id);
        if (alGetError() != AL_NO_ERROR) {
          std::cout << "OpenAL failed to attach buffer to source\n";
        }

        std::scoped_lock lock(mtx);
        buffers.push_back(buffer_id);
        sources.push_back(source_id);
        name_to_source_idx[name] = sources.size() - 1; // TODO: investigate other approaches
    }
};
} // namespace elysium

