#include "CTAG_Audio.h"

/**
 * @file CTAG_Audio.cpp
 * @brief Implementation file for the CTAG audio library for ESP32.
 * @note This implementation assumes that the I2S pin macros
 * (e.g., PIN_I2S_MCLK, PIN_I2S_BCLK) are defined in the user's
 * project scope, typically via a board-specific "pins_arduino.h".
 */

// --- Constants ---
#define SAMPLE_RATE     (44100)
#define BITS_PER_SAMPLE (I2S_BITS_PER_SAMPLE_16BIT)


// =========================================================================
// === Layer 1: CTAG_AudioCodec Implementation                           ===
// =========================================================================

/**
 * @class CTAG_AudioCodec
 * @brief Low-level driver to control the TLV320AIC3254 audio codec via I2C.
 */

CTAG_AudioCodec::CTAG_AudioCodec(uint8_t i2c_addr) : _i2c_addr(i2c_addr) {}

void CTAG_AudioCodec::_write_register(uint8_t page, uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_i2c_addr);
    Wire.write(0x00); // Address of the Page Select Register
    Wire.write(page); // Select the desired page
    Wire.endTransmission();
    
    Wire.beginTransmission(_i2c_addr);
    Wire.write(reg);   // The register address on the selected page
    Wire.write(value); // The value to be written
    Wire.endTransmission();
}

void CTAG_AudioCodec::_configure_tlv320aic3254() {
    // Default initialization sequence for the codec.
    _write_register(0, 1, 0x01); delay(10);
    _write_register(1, 1, 0x08); _write_register(1, 2, 0x01); _write_register(1, 10, 0x08);
    _write_register(0, 27, 0x10); _write_register(0, 28, 0x00); _write_register(0, 4, 0x00);
    _write_register(0, 5, 0x00); _write_register(0, 13, 0x00); _write_register(0, 14, 0x80);
    _write_register(0, 20, 0x80); _write_register(0, 11, 0x81); _write_register(0, 12, 0x82);
    _write_register(0, 18, 0x81); _write_register(0, 19, 0x82); _write_register(1, 14, 0x08);
    _write_register(1, 15, 0x08); _write_register(1, 12, 0x08); _write_register(1, 13, 0x08);
    _write_register(0, 64, 0x00); _write_register(0, 65, 0x00); _write_register(0, 66, 0x00);
    _write_register(0, 63, 0xD4); _write_register(1, 9, 0x3C); _write_register(1, 16, 0x00);
    _write_register(1, 17, 0x00); _write_register(1, 18, 0x06); _write_register(1, 19, 0x06);
    _write_register(1, 52, 0x40); _write_register(1, 55, 0x40); _write_register(1, 54, 0x40);
    _write_register(1, 57, 0x40); _write_register(1, 59, 0x00); _write_register(1, 60, 0x00);
    _write_register(0, 81, 0xC0); _write_register(0, 82, 0x00);
    delay(10);
}

bool CTAG_AudioCodec::begin(int sdaPin, int sclPin) {
    bool success = Wire.begin(sdaPin, sclPin);
    if (success) {
        _configure_tlv320aic3254();
    }
    return success;
}

void CTAG_AudioCodec::setHeadphoneVolume(uint8_t volume) {
    volume = constrain(volume, 0, 100);
    int8_t reg_val = map(volume, 0, 100, 0x3B, 0x14);
    _write_register(1, 16, (uint8_t)reg_val);
    _write_register(1, 17, (uint8_t)reg_val);
}

void CTAG_AudioCodec::setLineOutVolume(uint8_t volume) {
    volume = constrain(volume, 0, 100);
    int8_t reg_val = map(volume, 0, 100, 0x3A, 0x1D);
    _write_register(1, 18, (uint8_t)reg_val);
    _write_register(1, 19, (uint8_t)reg_val);
}


// =========================================================================
// === Layer 2: Audio Engine Implementation                              ===
// =========================================================================
namespace CTAG_AudioEngine {
    static CTAG_AudioSource* currentSource = nullptr;
    static i2s_port_t _i2s_port;


    static void audio_task(void* /*params*/) {
        const int buffer_samples = 256;
        int16_t i2s_buffer[buffer_samples * 2];
        size_t bytes_written = 0;
        while (true) {
            for (int i = 0; i < buffer_samples * 2; i += 2) {
                int16_t s = currentSource ? currentSource->getNextSample() : 0;
                i2s_buffer[i]     = s;
                i2s_buffer[i + 1] = s;
            }
            i2s_write(_i2s_port,
                      i2s_buffer,
                      sizeof(i2s_buffer),
                      &bytes_written,
                      portMAX_DELAY);
        }
    }

    void init(i2s_port_t i2s_port) {
        _i2s_port = i2s_port;

        // zero-init + configure
        i2s_config_t cfg{};  
        cfg.mode               = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        cfg.sample_rate        = SAMPLE_RATE;
        cfg.bits_per_sample    = BITS_PER_SAMPLE;
        cfg.channel_format     = I2S_CHANNEL_FMT_RIGHT_LEFT;
        cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
        cfg.intr_alloc_flags   = ESP_INTR_FLAG_LEVEL1;
        cfg.dma_buf_count      = 8;
        cfg.dma_buf_len        = 256;
        cfg.use_apll           = true;
        cfg.tx_desc_auto_clear = true;
        i2s_driver_install(_i2s_port, &cfg, 0, nullptr);

        i2s_pin_config_t pin_cfg{};  
        pin_cfg.mck_io_num   = PIN_I2S_MCLK;
        pin_cfg.bck_io_num   = PIN_I2S_BCLK;
        pin_cfg.ws_io_num    = PIN_I2S_WS;
        pin_cfg.data_out_num = PIN_I2S_SDOUT;
        pin_cfg.data_in_num  = I2S_PIN_NO_CHANGE;
        i2s_set_pin(_i2s_port, &pin_cfg);

        i2s_set_clk(_i2s_port, SAMPLE_RATE, BITS_PER_SAMPLE, I2S_CHANNEL_STEREO);
    }

    void setSource(CTAG_AudioSource* source) {
        currentSource = source;
    }

    void begin() {
        // now _block_ in whichever task called us:
        audio_task(nullptr);
    }

    void renderBlock() {
        const int buffer_samples = 256;
        int16_t buf[buffer_samples * 2];
        size_t  written;
        for (int i = 0; i < buffer_samples; ++i) {
            int16_t s = currentSource ? currentSource->getNextSample() : 0;
            buf[2*i  ] = s;
            buf[2*i+1] = s;
        }
        i2s_write(_i2s_port, buf, sizeof(buf), &written, portMAX_DELAY);
    }

    void audioLoop() {
        while (true) renderBlock();
    }
}

// =========================================================================
// === Layer 3: CTAG_VCO_Sine Implementation                             ===
// =========================================================================


// --- CTAG_VCO_Sine with Vibrato LFO ---
CTAG_VCO_Sine::CTAG_VCO_Sine(float sampleRate)
    : _sampleRate(sampleRate)
    , _frequency(440.0f)
    , _amplitude(0.5f)
    , _phase(0.0f)
    , _phaseIncrement((2.0f * M_PI * _frequency) / _sampleRate)
    , _lfoRate(5.0f)
    , _lfoDepth(0.0f)
    , _lfoPhase(0.0f)
    , _lfoIncrement((2.0f * M_PI * _lfoRate) / _sampleRate)
{
    setFrequency(_frequency);
    setLfoRate(_lfoRate);
    setLfoDepth(_lfoDepth);
}
void CTAG_VCO_Sine::setFrequency(float freq) {
    _frequency = freq;
    _phaseIncrement = (2.0f * M_PI * _frequency) / _sampleRate;
}

void CTAG_VCO_Sine::setAmplitude(float amp) {
    _amplitude = constrain(amp, 0.0f, 1.0f);
}

void CTAG_VCO_Sine::setLfoRate(float rate) {
    _lfoRate = rate;
    _lfoIncrement = (2.0f * M_PI * _lfoRate) / _sampleRate;
}

void CTAG_VCO_Sine::setLfoDepth(float depth) {
    _lfoDepth = depth;
}

int16_t CTAG_VCO_Sine::getNextSample() {
    // Advance LFO
    _lfoPhase += _lfoIncrement;
    if (_lfoPhase >= 2.0f * M_PI) _lfoPhase -= 2.0f * M_PI;
    float vibrato = sin(_lfoPhase) * _lfoDepth;

    // Instantaneous phase increment with vibrato
    float instIncrement = (2.0f * M_PI * (_frequency + vibrato)) / _sampleRate;
    _phase += instIncrement;
    if (_phase >= 2.0f * M_PI) _phase -= 2.0f * M_PI;

    float out = sin(_phase) * _amplitude;
    return (int16_t)(out * 32767.0f);
}

// --- CTAG_VCO_Square with Pulse-Width Control ---
CTAG_VCO_Square::CTAG_VCO_Square(float sampleRate)
    : _sampleRate(sampleRate)
    , _frequency(440.0f)
    , _amplitude(0.5f)
    , _phase(0.0f)
    , _phaseIncrement((2.0f * M_PI * _frequency) / _sampleRate)
    , _dutyCycle(0.5f)
{
    // Ensure internal state matches defaults
    setFrequency(_frequency);
    setAmplitude(_amplitude);
    setDutyCycle(_dutyCycle);
}

void CTAG_VCO_Square::setFrequency(float freq) {
    _frequency = freq;
    _phaseIncrement = (2.0f * M_PI * _frequency) / _sampleRate;
}

void CTAG_VCO_Square::setAmplitude(float amp) {
    _amplitude = constrain(amp, 0.0f, 1.0f);
}

void CTAG_VCO_Square::setDutyCycle(float duty) {
    // Clamp duty between 5% and 95% to avoid extreme pulse widths
    _dutyCycle = constrain(duty, 0.05f, 0.95f);
}

int16_t CTAG_VCO_Square::getNextSample() {
    // Advance phase
    _phase += _phaseIncrement;
    if (_phase >= 2.0f * M_PI) {
        _phase -= 2.0f * M_PI;
    }

    // Output high for the first portion of the cycle,
    // then low for the remainder, scaled by amplitude.
    float out = (_phase < (2.0f * M_PI * _dutyCycle) ? 1.0f : -1.0f)
                * _amplitude;

    return (int16_t)(out * 32767.0f);
}

// --- CTAG_VCO_Saw with Skew Control ---
CTAG_VCO_Saw::CTAG_VCO_Saw(float sampleRate)
    : _sampleRate(sampleRate)
    , _frequency(440.0f)
    , _amplitude(0.5f)
    , _phase(0.0f)
    , _phaseIncrement((2.0f * M_PI * _frequency) / _sampleRate)
    , _skew(0.5f)
{
    setFrequency(_frequency);
    setAmplitude(_amplitude);
    setSkew(_skew);
}

void CTAG_VCO_Saw::setFrequency(float freq) {
    _frequency = freq;
    _phaseIncrement = (2.0f * M_PI * _frequency) / _sampleRate;
}

void CTAG_VCO_Saw::setAmplitude(float amp) {
    _amplitude = constrain(amp, 0.0f, 1.0f);
}

void CTAG_VCO_Saw::setSkew(float skew) {
    // avoid division by zero or full-cycle peaks
    _skew = constrain(skew, 0.01f, 0.99f);
}

int16_t CTAG_VCO_Saw::getNextSample() {
    // Advance phase
    _phase += _phaseIncrement;
    if (_phase >= 2.0f * M_PI) {
        _phase -= 2.0f * M_PI;
    }

    // Normalize phase into [0…1)
    float norm = _phase / (2.0f * M_PI);
    float out;

    // Rising ramp until skew, then falling ramp
    if (norm < _skew) {
        out = -1.0f + (norm / _skew) * 2.0f;
    } else {
        out =  1.0f - ((norm - _skew) / (1.0f - _skew)) * 2.0f;
    }

    // Apply amplitude
    out *= _amplitude;

    // Convert to 16-bit
    return (int16_t)(out * 32767.0f);
}




// --- CTAG_FMSynth ---
CTAG_FMSynth::CTAG_FMSynth(float sampleRate)
    : _sampleRate(sampleRate),
      _carrierFreq(440.0f), _amplitude(0.5f),
       _modFreq(220.0f), _modIndex(0.0f), 
      _carrierPhase(0.0f), _modPhase(0.0f)
{
    setCarrierFreq(_carrierFreq);
    setAmplitude(_amplitude);
    setModFreq(_modFreq);
    setModIndex(_modIndex);
}

void CTAG_FMSynth::setCarrierFreq(float freq) {
    _carrierFreq = freq;
    _carrierInc  = (2.0f * M_PI * _carrierFreq) / _sampleRate;
}

void CTAG_FMSynth::setModFreq(float freq) {
    _modFreq = freq;
    _modInc  = (2.0f * M_PI * _modFreq) / _sampleRate;
}

void CTAG_FMSynth::setModIndex(float index) {
    _modIndex = index;
}

void CTAG_FMSynth::setAmplitude(float amp) {
    _amplitude = constrain(amp, 0.0f, 1.0f);
}

int16_t CTAG_FMSynth::getNextSample() {
    // advance modulator
    _modPhase += _modInc;
    if (_modPhase >= 2.0f * M_PI) _modPhase -= 2.0f * M_PI;
    float mod = sin(_modPhase) * _modIndex;

    // advance carrier, including FM
    _carrierPhase += _carrierInc + mod;
    if (_carrierPhase >= 2.0f * M_PI) _carrierPhase -= 2.0f * M_PI;

    float out = sin(_carrierPhase) * _amplitude;
    return (int16_t)(out * 32767.0f);
}