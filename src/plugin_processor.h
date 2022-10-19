#ifndef __PLUGIN_PROCESSOR_H__
#define __PLUGIN_PROCESSOR_H__

#include <string>
#include <array>
#include <mutex>
#include "compressor.h"
#include "level_meter.h"
#include "pluginterfaces/vst2.x/audioeffectx.h"

class plugin_processor : public AudioEffectX
{
public:
    enum
    {
        PARAMETER_IDX_THRESH = 0,
        PARAMETER_IDX_RATIO,
        PARAMETER_IDX_KNEE,
        PARAMETER_IDX_GAIN,
        PARAMETER_IDX_ATTACK,
        PARAMETER_IDX_RELEASE,
        PARAMETER_IDX_RMS,
        PARAMETER_IDX_PRE,
        PARAMETER_NUM,
    };
    
    struct parameter
    {
        float get_value()
        {
            return value / scale;
        }
        
        void set_value(float v)
        {
            value = v * scale;
        }
        
        std::string get_display()
        {
            char buf[32];
            sprintf(buf, "%.2f", value);
            return buf;
        }
        
        
        std::string name;
        std::string label;
        float scale;
        float min;
        float max;
        float value;
    };
    
    struct lv_meter
    {
        float in_peek_db;
        float in_rms_db;
        float out_peek_db;
        float out_rms_db;
        float gr_db;
    };
    
public:
    plugin_processor(audioMasterCallback audio_master);
    ~plugin_processor();

public:
    
    // Processing
    virtual void processReplacing(float** inputs, float** outputs, VstInt32 sample_frames);
    virtual void processDoubleReplacing(double** inputs, double** outputs, VstInt32 sample_frames);

    virtual void setSampleRate(float sample_rate);
    
    // Program
    virtual void setProgramName(char* name);
    virtual void getProgramName(char* name);

    // Parameters
    virtual void setParameter(VstInt32 index, float value);
    virtual float getParameter(VstInt32 index);
    virtual void getParameterLabel(VstInt32 index, char* label);
    virtual void getParameterDisplay(VstInt32 index, char* text);
    virtual void getParameterName(VstInt32 index, char* text);

    virtual bool getEffectName(char* name);
    virtual bool getVendorString(char* text);
    virtual bool getProductString(char* text);
    virtual VstInt32 getVendorVersion();

public:
    void set_patameter(std::int32_t idx, float value);
    float get_patameter(std::int32_t idx);
    void get_patameter(std::int32_t idx, parameter& param);
    
    std::int32_t get_ratio_map(float *map_in, float *map_out, float min, float max, float setup, std::int32_t buf_size);
    void get_lv_meter_info(lv_meter info[2]);
    
private:
    void _set_patameter(std::int32_t idx, float value);
    
private:
    std::array<parameter, PARAMETER_NUM> _parameters{
        /* name label scale min max def */
        parameter{"thresh", "dB", -200, -60, 0, 0},
        parameter{"ratio", "", 200, 1, 100, 4},
        parameter{"knee", "dB", 100, 0, 24, 3},
        parameter{"gain", "dB", 100, 0, 30, 0},
        parameter{"attack", "ms", 5000, 0, 500, 5},
        parameter{"release", "ms", 10000, 0, 5000, 100},
        parameter{"rms", "ms", 10000, 0, 1000, 5},
        parameter{"pre", "ms", 10000, 0, 1000, 0},
    };
    
    char _program_name[kVstMaxProgNameLen + 1];
    
    std::mutex _mtx;
    compressor _comp[2];
    level_meter _in_meter[2];
    level_meter _out_meter[2];
};

#endif
