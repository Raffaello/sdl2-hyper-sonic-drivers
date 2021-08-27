#pragma once

namespace audio
{
    namespace scummvm
    {
        class Channel {
        public:
            Channel(Mixer* mixer, Mixer::SoundType type, AudioStream* stream, DisposeAfterUse::Flag autofreeStream, bool reverseStereo, int id, bool permanent);
            ~Channel();

            /**
             * Mixes the channel's samples into the given buffer.
             *
             * @param data buffer where to mix the data
             * @param len  number of sample *pairs*. So a value of
             *             10 means that the buffer contains twice 10 sample, each
             *             16 bits, for a total of 40 bytes.
             * @return number of sample pairs processed (which can still be silence!)
             */
            int mix(int16* data, uint len);

            /**
             * Queries whether the channel is still playing or not.
             */
            bool isFinished() const { return _stream->endOfStream(); }

            /**
             * Queries whether the channel is a permanent channel.
             * A permanent channel is not affected by a Mixer::stopAll
             * call.
             */
            bool isPermanent() const { return _permanent; }

            /**
             * Returns the id of the channel.
             */
            int getId() const { return _id; }

            /**
             * Pauses or unpaused the channel in a recursive fashion.
             *
             * @param paused true, when the channel should be paused.
             *               false when it should be unpaused.
             */
            void pause(bool paused);

            /**
             * Queries whether the channel is currently paused.
             */
            bool isPaused() const { return (_pauseLevel != 0); }

            /**
             * Sets the channel's own volume.
             *
             * @param volume new volume
             */
            void setVolume(const byte volume);

            /**
             * Gets the channel's own volume.
             *
             * @return volume
             */
            byte getVolume();

            /**
             * Sets the channel's balance setting.
             *
             * @param balance new balance
             */
            void setBalance(const int8 balance);

            /**
             * Gets the channel's balance setting.
             *
             * @return balance
             */
            int8 getBalance();

            /**
             * Notifies the channel that the global sound type
             * volume settings changed.
             */
            void notifyGlobalVolChange() { updateChannelVolumes(); }

            /**
             * Queries how long the channel has been playing.
             */
            Timestamp getElapsedTime();

            /**
             * Queries the channel's sound type.
             */
            Mixer::SoundType getType() const { return _type; }

            /**
             * Sets the channel's sound handle.
             *
             * @param handle new handle
             */
            void setHandle(const SoundHandle handle) { _handle = handle; }

            /**
             * Queries the channel's sound handle.
             */
            SoundHandle getHandle() const { return _handle; }

        private:
            const Mixer::SoundType _type;
            SoundHandle _handle;
            bool _permanent;
            int _pauseLevel;
            int _id;

            byte _volume;
            int8 _balance;

            void updateChannelVolumes();
            st_volume_t _volL, _volR;

            Mixer* _mixer;

            uint32 _samplesConsumed;
            uint32 _samplesDecoded;
            uint32 _mixerTimeStamp;
            uint32 _pauseStartTime;
            uint32 _pauseTime;

            RateConverter* _converter;
            Common::DisposablePtr<AudioStream> _stream;
        };
    }
}
