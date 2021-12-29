#include <audio/scummvm/Timestamp.hpp>
#include <cassert>
#include <numeric>

namespace audio
{
    namespace scummvm
    {
            Timestamp::Timestamp(unsigned int ms, unsigned int fr) {
                assert(fr > 0);

                _secs = ms / 1000;
                _framerateFactor = 1000 / std::gcd(1000U, fr);
                _framerate = fr * _framerateFactor;

                // Note that _framerate is always divisible by 1000.
                _numFrames = (ms % 1000) * (_framerate / 1000);
            }

            Timestamp::Timestamp(unsigned int s, unsigned int frames, unsigned int fr) {
                assert(fr > 0);

                _secs = s + (frames / fr);
                _framerateFactor = 1000 / std::gcd(1000U, fr);
                _framerate = fr * _framerateFactor;
                _numFrames = (frames % fr) * _framerateFactor;
            }

            Timestamp Timestamp::convertToFramerate(unsigned int newFramerate) const {
                Timestamp ts(*this);

                if (ts.framerate() != newFramerate) {
                    ts._framerateFactor = 1000 / std::gcd<unsigned int>(1000U, newFramerate);
                    ts._framerate = newFramerate * ts._framerateFactor;

                    const unsigned int g = std::gcd(_framerate, ts._framerate);
                    const unsigned int p = _framerate / g;
                    const unsigned int q = ts._framerate / g;

                    // Convert the frame offset to the new framerate.
                    // We round to the nearest (as opposed to always
                    // rounding down), to minimize rounding errors during
                    // round trip conversions.
                    ts._numFrames = (ts._numFrames * q + p / 2) / p;

                    ts.normalize();
                }

                return ts;
            }

            void Timestamp::normalize() {
                // Convert negative _numFrames values to positive ones by adjusting _secs
                if (_numFrames < 0) {
                    int secsub = 1 + (-_numFrames / _framerate);

                    _numFrames += _framerate * secsub;
                    _secs -= secsub;
                }

                // Wrap around if necessary
                _secs += (_numFrames / _framerate);
                _numFrames %= _framerate;
            }

            bool Timestamp::operator==(const Timestamp& ts) const {
                return cmp(ts) == 0;
            }

            bool Timestamp::operator!=(const Timestamp& ts) const {
                return cmp(ts) != 0;
            }

            bool Timestamp::operator<(const Timestamp& ts) const {
                return cmp(ts) < 0;
            }

            bool Timestamp::operator<=(const Timestamp& ts) const {
                return cmp(ts) <= 0;
            }

            bool Timestamp::operator>(const Timestamp& ts) const {
                return cmp(ts) > 0;
            }

            bool Timestamp::operator>=(const Timestamp& ts) const {
                return cmp(ts) >= 0;
            }

            int Timestamp::cmp(const Timestamp& ts) const {
                int delta = _secs - ts._secs;
                if (!delta) {
                    const unsigned int g = std::gcd(_framerate, ts._framerate);
                    const unsigned int p = _framerate / g;
                    const unsigned int q = ts._framerate / g;

                    delta = (_numFrames * q - ts._numFrames * p);
                }

                return delta;
            }


            Timestamp Timestamp::addFrames(int frames) const {
                Timestamp ts(*this);

                // The frames are given in the original framerate, so we have to
                // adjust by _framerateFactor accordingly.
                ts._numFrames += frames * _framerateFactor;
                ts.normalize();

                return ts;
            }

            Timestamp Timestamp::addMsecs(int ms) const {
                Timestamp ts(*this);
                ts._secs += ms / 1000;
                // Add the remaining frames. Note that _framerate is always divisible by 1000.
                ts._numFrames += (ms % 1000) * (ts._framerate / 1000);

                ts.normalize();

                return ts;
            }

            void Timestamp::addIntern(const Timestamp& ts) {
                assert(_framerate == ts._framerate);
                _secs += ts._secs;
                _numFrames += ts._numFrames;

                normalize();
            }

            Timestamp Timestamp::operator-() const {
                Timestamp result(*this);
                result._secs = -_secs;
                result._numFrames = -_numFrames;
                result.normalize();
                return result;
            }

            Timestamp Timestamp::operator+(const Timestamp& ts) const {
                Timestamp result(*this);
                result.addIntern(ts);
                return result;
            }

            Timestamp Timestamp::operator-(const Timestamp& ts) const {
                Timestamp result(*this);
                result.addIntern(-ts);
                return result;
            }

            int Timestamp::frameDiff(const Timestamp& ts) const {

                int delta = 0;
                if (_secs != ts._secs)
                    delta = (_secs - ts._secs) * _framerate;

                delta += _numFrames;

                if (_framerate == ts._framerate) {
                    delta -= ts._numFrames;
                }
                else {
                    // We need to multiply by the quotient of the two framerates.
                    // We cancel the GCD in this fraction to reduce the risk of
                    // overflows.
                    const unsigned int g = std::gcd(_framerate, ts._framerate);
                    const unsigned int p = _framerate / g;
                    const unsigned int q = ts._framerate / g;

                    delta -= ((long)ts._numFrames * p + q / 2) / (long)q;
                }

                return delta / (int)_framerateFactor;
            }

            int Timestamp::msecsDiff(const Timestamp& ts) const {
                return msecs() - ts.msecs();
            }

            int Timestamp::msecs() const {
                // Note that _framerate is always divisible by 1000.
                return _secs * 1000 + _numFrames / (_framerate / 1000);
            }
    }
}
