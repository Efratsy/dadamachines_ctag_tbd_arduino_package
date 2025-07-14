/**
 * @file CTAG_Audio.h
 * @brief Main header file for the CTAG audio library for ESP32.
 *
 * @defgroup Libraries_Audio CTAG_Audio
 * @ingroup Libraries
 * 
 * This file defines the public interface for the three main components of the library:
 * 1. CTAG_AudioCodec: A low-level driver for the audio codec hardware.
 * 2. CTAG_AudioSource: An abstract base class for creating audio-generating "plugins".
 * 3. CTAG_AudioEngine: The main engine that handles I2S streaming and processing.
 */
#pragma once
#ifndef CTAG_AUDIO_H
#define CTAG_AUDIO_H

#include <Arduino.h>
#include <Wire.h>
#include "driver/i2s.h"
#include <math.h>
#include <vector>
    

// =========================================================================
// === Layer 1: The Codec Driver                                         ===
// =========================================================================

/**
 * @class CTAG_AudioCodec
 * @brief Low-level driver to control the audio codec chip via I2C.
 */
class CTAG_AudioCodec {
public:
    CTAG_AudioCodec(uint8_t i2c_addr = 0x18);

    /**
     * @brief Initializes the I2C communication and configures the codec.
     * @param sdaPin The I2C SDA pin.
     * @param sclPin The I2C SCL pin.
     * @return True on success.
     */
    bool begin(int sdaPin, int sclPin);
    
    /**
     * @brief Sets the volume for the headphone output.
     * @param volume 0 (mute) to 100 (maximum volume).
     */
    void setHeadphoneVolume(uint8_t volume);

    /**
     * @brief Sets the volume for the line-out.
     * @param volume 0 (mute) to 100 (maximum volume).
     */
    void setLineOutVolume(uint8_t volume);

private:
    void _write_register(uint8_t page, uint8_t reg, uint8_t value);
    void _configure_tlv320aic3254();
    uint8_t _i2c_addr;
};


// =========================================================================
// === Layer 2 & 3: Audio Engine & Audio Sources                         ===
// =========================================================================

/**
 * @class CTAG_AudioSource
 * @brief Abstract base class for all audio sources (oscillators, samplers, etc.).
 * @note Any audio-generating "plugin" must inherit from this class
 * and implement the getNextSample() method.
 */
class CTAG_AudioSource {
public:
    virtual ~CTAG_AudioSource() {}

    /**
     * @brief Must be implemented by the derived class to generate a single audio sample.
     * @return A 16-bit signed audio sample (-32768 to 32767).
     */
    virtual int16_t getNextSample() = 0;
};


/**
 * @namespace CTAG_AudioEngine
 * @brief The main audio engine, implemented as a static namespace.
 */
namespace CTAG_AudioEngine {
    /**
     * @brief Configure the codec + I²S peripheral.
     * Must be called once before starting the audio loop.
     */
    void init(i2s_port_t i2s_port = I2S_NUM_0);

    /**
     * @brief Pick which CTAG_AudioSource to pull samples from.
     */
    void setSource(CTAG_AudioSource* source);

    /**
     * @brief Blocking call that never returns:
     *        streams audio forever on the calling task/thread.
     * @note This replaces the old begin() that spun up its own task.
     */
    void begin();


    void renderBlock();


    void audioLoop() __attribute__((deprecated));
}


// =========================================================================
// === Example Implementation of a "Plugin"                              ===
// =========================================================================

/**
 * @class CTAG_VCO_Sine
 * @brief A simple sine wave oscillator (VCO).
 */
/**
 * @class CTAG_VCO_Sine
 * @brief A simple sine wave oscillator (VCO) with optional vibrato LFO.
 *
 * Knob assignments:
 * 1) Frequency (Hz)
 * 2) Amplitude (0.0 - 1.0)
 * 3) Vibrato LFO Rate (Hz)
 * 4) Vibrato LFO Depth (Hz)
 */
class CTAG_VCO_Sine : public CTAG_AudioSource {
public:
    /**
     * @brief Constructs a new sine wave oscillator.
     * @param sampleRate The sample rate of the audio engine (e.g., 44100.0f).
     */
    CTAG_VCO_Sine(float sampleRate = 44100.0f);

    /**
     * @brief Sets the frequency of the oscillator.
     * @param freq The desired frequency in Hz.
     */
    void setFrequency(float freq);

    /**
     * @brief Sets the amplitude (volume) of the oscillator.
     * @param amp Amplitude from 0.0 (silence) to 1.0 (max).
     */
    void setAmplitude(float amp);
    
    /**
     * @brief Sets the vibrato LFO rate.
     * @param rate Vibrato rate in Hz.
     */
    void setLfoRate(float rate);

    /**
     * @brief Sets the vibrato LFO depth.
     * @param depth Vibrato depth in Hz.
     */
    void setLfoDepth(float depth);
    
    /**
     * @brief Generates the next sample of the sine wave with vibrato.
     * @return A 16-bit signed audio sample.
     */
    int16_t getNextSample() override;

private:
    float _sampleRate;
    float _frequency;
    float _amplitude;
    float _phase;
    float _phaseIncrement;

    // Vibrato LFO parameters
    float _lfoRate     = 5.0f;
    float _lfoDepth    = 0.0f;
    float _lfoPhase    = 0.0f;
    float _lfoIncrement= 0.0f;
};


/**
 * @class CTAG_VCO_Square
 * @brief A simple square-wave oscillator (VCO) with pulse-width control.
 *
 * Knob assignments:
 *   1) Frequency (Hz)
 *   2) Amplitude (0.0 – 1.0)
 *   3) Duty Cycle (0.05 – 0.95)
 *   4) (unused / reserved for future)
 */
class CTAG_VCO_Square : public CTAG_AudioSource {
public:
    /**
     * @brief Constructs a new square wave oscillator.
     * @param sampleRate The sample rate of the audio engine (e.g., 44100.0f).
     */
    CTAG_VCO_Square(float sampleRate = 44100.0f);

    /**
     * @brief Sets the frequency of the oscillator.
     * @param freq The desired frequency in Hz.
     */
    void setFrequency(float freq);

    /**
     * @brief Sets the amplitude (volume) of the oscillator.
     * @param amp Amplitude from 0.0 (silence) to 1.0 (max).
     */
    void setAmplitude(float amp);

    /**
     * @brief Sets the pulse-width (duty-cycle) of the square wave.
     * @param duty Duty-cycle from 0.05 (5%) to 0.95 (95%).
     */
    void setDutyCycle(float duty);

    /**
     * @brief Generates the next sample of the square wave.
     * @return A 16-bit signed audio sample.
     */
    int16_t getNextSample() override;

private:
    float _sampleRate;
    float _frequency;
    float _amplitude;
    float _phase;
    float _phaseIncrement;

    // Pulse-width (duty-cycle) between 0.0 and 1.0
    float _dutyCycle = 0.5f;
};


/**
 * @class CTAG_VCO_Saw
 * @brief A simple sawtooth-wave oscillator (VCO) with skew control.
 *
 * Knob assignments:
 *   1) Frequency (Hz)
 *   2) Amplitude (0.0 – 1.0)
 *   3) Skew (0.01 – 0.99) : position of the waveform’s peak within each cycle
 *   4) (unused / reserved for future)
 */
class CTAG_VCO_Saw : public CTAG_AudioSource {
public:
    /**
     * @brief Constructs a new saw wave oscillator.
     * @param sampleRate The sample rate of the audio engine (e.g., 44100.0f).
     */
    CTAG_VCO_Saw(float sampleRate = 44100.0f);

    /**
     * @brief Sets the frequency of the oscillator.
     * @param freq The desired frequency in Hz.
     */
    void setFrequency(float freq);

    /**
     * @brief Sets the amplitude (volume) of the oscillator.
     * @param amp Amplitude from 0.0 (silence) to 1.0 (max).
     */
    void setAmplitude(float amp);

    /**
     * @brief Sets the skew (waveform symmetry) of the oscillator.
     * @param skew Fraction from 0.01 (fast rise, long fall) to 0.99 (long rise, fast fall).
     */
    void setSkew(float skew);

    /**
     * @brief Generates the next sample of the saw wave.
     * @return A 16-bit signed audio sample.
     */
    int16_t getNextSample() override;

private:
    float _sampleRate;
    float _frequency;
    float _amplitude;
    float _phase;
    float _phaseIncrement;

    /// Position of the peak in the cycle [0.01…0.99]
    float _skew;
};


/**
 * @class CTAG_FMSynth
 * @brief A basic 2-operator FM synthesizer voice.
 *
 * Knob assignments:
 *   1) Carrier Frequency (Hz)
 *   2) Amplitude (0.0 – 1.0)
 *   3) Modulator Frequency (Hz)
 *   4) Modulation Index (depth)
 */
class CTAG_FMSynth : public CTAG_AudioSource {
public:
    /**
     * @brief Constructs a new FM synth voice.
     * @param sampleRate The audio engine’s sample rate (e.g. 44100.0f).
     */
    CTAG_FMSynth(float sampleRate = 44100.0f);

    /**
     * @brief Sets the carrier oscillator’s frequency.
     * @param freq Frequency in Hz.
     */
    void setCarrierFreq(float freq);

    /**
     * @brief Sets the overall output amplitude.
     * @param amp Amplitude from 0.0 (silence) to 1.0 (max).
     */
    void setAmplitude(float amp);

    /**
     * @brief Sets the modulator oscillator’s frequency.
     * @param freq Frequency in Hz.
     */
    void setModFreq(float freq);

    /**
     * @brief Sets the modulation index (depth of frequency modulation).
     * @param index Modulation index (unitless).
     */
    void setModIndex(float index);

    /**
     * @brief Generates the next audio sample.
     * @return A 16-bit signed audio sample.
     */
    int16_t getNextSample() override;

private:
    float _sampleRate;

    float _carrierFreq;
    float _modFreq;
    float _modIndex;
    float _amplitude;

    float _carrierPhase;
    float _modPhase;

    float _carrierInc;
    float _modInc;
};



#endif // CTAG_AUDIO_H