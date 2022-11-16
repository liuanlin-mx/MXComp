#include "plugin_processor.h"
#include "plugin_editor.h"
#include "net_log.h"

AudioEffect* createEffectInstance(audioMasterCallback audio_master)
{
    plugin_processor *effect = new plugin_processor(audio_master);
    AEffEditor *editor = new plugin_editor(effect);
    effect->setEditor(editor);
    return effect;
}

plugin_processor::plugin_processor(audioMasterCallback audio_master)
    : AudioEffectX(audio_master, 1, PARAMETER_NUM)	// 1 program, 1 parameter only
    , _detector(0)
{
    setNumInputs(2);        // stereo in
    setNumOutputs(2);       // stereo out
    setUniqueID('Comp');    // identify
    canProcessReplacing();  // supports replacing output
    //canDoubleReplacing();   // supports double precision processing

    vst_strncpy(_program_name, "Default", kVstMaxProgNameLen);  // default program name
    
    _svf_filter_lp[0].updateCoefficients(_max_freq, _filter_q, SvfLinearTrapOptimised2::LOW_PASS_FILTER, _sample_rate);
    _svf_filter_lp[1].updateCoefficients(_max_freq, _filter_q, SvfLinearTrapOptimised2::LOW_PASS_FILTER, _sample_rate);
    
    _svf_filter_hp[0].updateCoefficients(_min_freq, _filter_q, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, _sample_rate);
    _svf_filter_hp[1].updateCoefficients(_min_freq, _filter_q, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, _sample_rate);
    
    for (std::int32_t i = 0; i < PARAMETER_NUM; i++)
    {
        _set_patameter(i, _parameters[i].value);
    }
    
}

plugin_processor::~plugin_processor()
{
    // nothing to do here
}


void plugin_processor::processReplacing(float** inputs, float** outputs, VstInt32 sample_frames)
{
    float* in1  =  inputs[0];
    float* in2  =  inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];

    std::lock_guard<std::mutex> l(_mtx);
    for (std::int32_t i = 0; i < sample_frames; i++)
    {
        float left = in1[i];
        float right = in2[i];
        
        _wave_view_in[0].put_sample(left);
        _wave_view_in[1].put_sample(right);
        
        if (_min_freq > 20)
        {
            left = _svf_filter_hp[0].tick(left);
            right = _svf_filter_hp[1].tick(right);
        }
        
        if (_max_freq < 20000)
        {
            left = _svf_filter_lp[0].tick(left);
            right = _svf_filter_lp[1].tick(right);
        }
        
        

        _in_meter[0].put(left);
        _in_meter[1].put(right);
        switch (_detector)
        {
            case DETECTOR_LR:
            {
                out1[i] = _comp[0].process(in1[i], left);
                out2[i] = _comp[1].process(in2[i], right);
                break;
            }
            case DETECTOR_MAX_LR:
            {
                float in = std::max(left, right);
                out1[i] = _comp[0].process(in1[i], in);
                out2[i] = _comp[1].process(in2[i], in);
                break;
            }
            case DETECTOR_AVG_LR:
            {
                float in = (left + right) * 0.5;
                out1[i] = _comp[0].process(in1[i], in);
                out2[i] = _comp[1].process(in2[i], in);
                break;
            }
            case DETECTOR_L:
            {
                out1[i] = _comp[0].process(in1[i], left);
                out2[i] = _comp[1].process(in2[i], left);
                break;
            }
            case DETECTOR_R:
            {
                out1[i] = _comp[0].process(in1[i], right);
                out2[i] = _comp[1].process(in2[i], right);
                break;
            }
            case DETECTOR_SIDE:
            {
                break;
            }
            default:
            {
                float in = std::max(left, right);
                out1[i] = _comp[0].process(in1[i], in);
                out2[i] = _comp[1].process(in2[i], in);
                break;
            }
        }
        
        _wave_view_out[0].put_sample(out1[i]);
        _wave_view_out[1].put_sample(out2[i]);
        _gr_view[0].put_sample(_comp[0].get_gr_db());
        _gr_view[1].put_sample(_comp[1].get_gr_db());
        _out_meter[0].put(out1[i]);
        _out_meter[1].put(out2[i]);
        
        
    }
}


void plugin_processor::processDoubleReplacing(double** inputs, double** outputs, VstInt32 sample_frames)
{
}


void plugin_processor::setSampleRate(float sample_rate)
{
    std::lock_guard<std::mutex> l(_mtx);
    _sample_rate = sample_rate;
    AudioEffect::setSampleRate(sample_rate);
    _comp[0].set_sample_rate(sample_rate);
    _comp[1].set_sample_rate(sample_rate);
    
    _wave_view_in[0].set_sample_rate(sample_rate);
    _wave_view_in[1].set_sample_rate(sample_rate);
    _wave_view_out[0].set_sample_rate(sample_rate);
    _wave_view_out[1].set_sample_rate(sample_rate);
    _gr_view[0].set_sample_rate(sample_rate);
    _gr_view[1].set_sample_rate(sample_rate);
        
    _svf_filter_lp[0].resetState();
    _svf_filter_lp[0].updateCoefficients(_max_freq, _filter_q, SvfLinearTrapOptimised2::LOW_PASS_FILTER, _sample_rate);
    _svf_filter_lp[1].resetState();
    _svf_filter_lp[1].updateCoefficients(_max_freq, _filter_q, SvfLinearTrapOptimised2::LOW_PASS_FILTER, _sample_rate);
    
    _svf_filter_hp[0].resetState();
    _svf_filter_hp[0].updateCoefficients(_min_freq, _filter_q, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, _sample_rate);
    _svf_filter_hp[1].resetState();
    _svf_filter_hp[1].updateCoefficients(_min_freq, _filter_q, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, _sample_rate);
}
    
void plugin_processor::setProgramName(char* name)
{
    std::lock_guard<std::mutex> l(_mtx);
    vst_strncpy(_program_name, name, kVstMaxProgNameLen);
}

void plugin_processor::getProgramName(char* name)
{
    std::lock_guard<std::mutex> l(_mtx);
    vst_strncpy(name, _program_name, kVstMaxProgNameLen);
}

void plugin_processor::setParameter(VstInt32 index, float value)
{
    std::lock_guard<std::mutex> l(_mtx);
    _parameters[index].set_value(value);
    _set_patameter(index, _parameters[index].value);
}


float plugin_processor::getParameter(VstInt32 index)
{
    std::lock_guard<std::mutex> l(_mtx);
    return _parameters[index].get_value();
}


void plugin_processor::getParameterName(VstInt32 index, char* label)
{
    std::lock_guard<std::mutex> l(_mtx);
    vst_strncpy(label, _parameters[index].name.c_str(), kVstMaxParamStrLen);
}


void plugin_processor::getParameterDisplay(VstInt32 index, char* text)
{
    std::lock_guard<std::mutex> l(_mtx);
    vst_strncpy(text, _parameters[index].get_display().c_str(), kVstMaxParamStrLen);
}


void plugin_processor::getParameterLabel(VstInt32 index, char* label)
{
    std::lock_guard<std::mutex> l(_mtx);
    vst_strncpy(label, _parameters[index].label.c_str(), kVstMaxParamStrLen);
}


bool plugin_processor::getEffectName(char* name)
{
    std::lock_guard<std::mutex> l(_mtx);
    vst_strncpy(name, "MXComp", kVstMaxEffectNameLen);
    return true;
}


bool plugin_processor::getProductString(char* text)
{
    std::lock_guard<std::mutex> l(_mtx);
    vst_strncpy(text, "MXComp", kVstMaxProductStrLen);
    return true;
}


bool plugin_processor::getVendorString(char* text)
{
    std::lock_guard<std::mutex> l(_mtx);
    vst_strncpy(text, "liuanlin-mx", kVstMaxVendorStrLen);
    return true;
}


VstInt32 plugin_processor::getVendorVersion()
{
    std::lock_guard<std::mutex> l(_mtx);
    return 1000;
}


void plugin_processor::set_patameter(std::int32_t idx, float value)
{
    std::lock_guard<std::mutex> l(_mtx);
    _parameters[idx].value = value;
    _set_patameter(idx, value);
}


float plugin_processor::get_patameter(std::int32_t idx)
{
    std::lock_guard<std::mutex> l(_mtx);
    return _parameters[idx].value;
}

void plugin_processor::get_patameter(std::int32_t idx, plugin_processor::parameter& param)
{
    std::lock_guard<std::mutex> l(_mtx);
    param = _parameters[idx];
}

std::int32_t plugin_processor::get_ratio_map(float *map_in, float *map_out, float min, float max, float setup, std::int32_t buf_size)
{
    std::lock_guard<std::mutex> l(_mtx);
    return _comp[0].get_ratio_map(map_in, map_out, min, max, setup, buf_size);
}

void plugin_processor::get_lv_meter_info(plugin_processor::lv_meter info[2])
{
    std::lock_guard<std::mutex> l(_mtx);
    info[0].in_peek_db = _in_meter[0].get_peek_db();
    info[0].in_rms_db = _in_meter[0].get_rms_db();
    info[0].out_peek_db = _out_meter[0].get_peek_db();
    info[0].out_rms_db = _out_meter[0].get_rms_db();
    info[0].gr_db = _comp[0].get_gr_db();
    
    info[1].in_peek_db = _in_meter[1].get_peek_db();
    info[1].in_rms_db = _in_meter[1].get_rms_db();
    info[1].out_peek_db = _out_meter[1].get_peek_db();
    info[1].out_rms_db = _out_meter[1].get_rms_db();
    info[1].gr_db = _comp[1].get_gr_db();
}

std::uint32_t plugin_processor::read_in_wave(std::uint32_t ch, float *buf, std::uint32_t max_cnt)
{
    if (ch < 2)
    {
        return _wave_view_in[ch].read_wave(buf, max_cnt);
    }
    return 0;
}

std::uint32_t plugin_processor::read_out_wave(std::uint32_t ch, float *buf, std::uint32_t max_cnt)
{
    if (ch < 2)
    {
        return _wave_view_out[ch].read_wave(buf, max_cnt);
    }
    return 0;
}

std::uint32_t plugin_processor::read_gr_wave(std::uint32_t ch, float *buf, std::uint32_t max_cnt)
{
    if (ch < 2)
    {
        return _gr_view[ch].read_wave(buf, max_cnt);
    }
    return 0;
}

void plugin_processor::_set_patameter(std::int32_t idx, float value)
{
    switch (idx)
    {
        case PARAMETER_IDX_THRESH:
        {
            _comp[0].set_thresh(value);
            _comp[1].set_thresh(value);
            break;
        }
        case PARAMETER_IDX_RATIO:
        {
            _comp[0].set_ratio(value);
            _comp[1].set_ratio(value);
            break;
        }
        case PARAMETER_IDX_KNEE:
        {
            _comp[0].set_knee(value);
            _comp[1].set_knee(value);
            break;
        }
        case PARAMETER_IDX_GAIN:
        {
            _comp[0].set_gain(value);
            _comp[1].set_gain(value);
            _wave_view_in[0].set_gain(value);
            _wave_view_in[1].set_gain(value);
            break;
        }
        case PARAMETER_IDX_ATTACK:
        {
            _comp[0].set_attack(value);
            _comp[1].set_attack(value);
            break;
        }
        case PARAMETER_IDX_RELEASE:
        {
            _comp[0].set_release(value);
            _comp[1].set_release(value);
            break;
        }
        case PARAMETER_IDX_RMS:
        {
            _comp[0].set_rms(value);
            _comp[1].set_rms(value);
            break;
        }
        case PARAMETER_IDX_PRE:
        {
            _comp[0].set_pre(value);
            _comp[1].set_pre(value);
            
            setInitialDelay(_comp[0].get_latency());
            break;
        }
        case PARAMETER_IDX_DETECTOR:
        {
            _detector = value;
            break;
        }
        case PARAMETER_IDX_FILTER_HP_FREQ:
        {
            _min_freq = value;
            _svf_filter_hp[0].resetState();
            _svf_filter_hp[0].updateCoefficients(_min_freq, _filter_q, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, _sample_rate);
            _svf_filter_hp[1].resetState();
            _svf_filter_hp[1].updateCoefficients(_min_freq, _filter_q, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, _sample_rate);
            break;
        }
        case PARAMETER_IDX_FILTER_LP_FREQ:
        {
            _max_freq = value;
            _svf_filter_lp[0].resetState();
            _svf_filter_lp[0].updateCoefficients(_max_freq, _filter_q, SvfLinearTrapOptimised2::LOW_PASS_FILTER, _sample_rate);
            _svf_filter_lp[1].resetState();
            _svf_filter_lp[1].updateCoefficients(_max_freq, _filter_q, SvfLinearTrapOptimised2::LOW_PASS_FILTER, _sample_rate);
            break;
        }
        case PARAMETER_IDX_FILTER_Q:
        {
            _filter_q = value;
            _svf_filter_hp[0].resetState();
            _svf_filter_hp[0].updateCoefficients(_min_freq, _filter_q, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, _sample_rate);
            _svf_filter_hp[1].resetState();
            _svf_filter_hp[1].updateCoefficients(_min_freq, _filter_q, SvfLinearTrapOptimised2::HIGH_PASS_FILTER, _sample_rate);
            
            _svf_filter_lp[0].resetState();
            _svf_filter_lp[0].updateCoefficients(_max_freq, _filter_q, SvfLinearTrapOptimised2::LOW_PASS_FILTER, _sample_rate);
            _svf_filter_lp[1].resetState();
            _svf_filter_lp[1].updateCoefficients(_max_freq, _filter_q, SvfLinearTrapOptimised2::LOW_PASS_FILTER, _sample_rate);
            break;
        }
        case PARAMETER_IDX_WAVE_VIEW_DURATION:
        {
            _wave_view_in[0].set_duration(value);
            _wave_view_in[1].set_duration(value);
            _wave_view_out[0].set_duration(value);
            _wave_view_out[1].set_duration(value);
            _gr_view[0].set_duration(value);
            _gr_view[1].set_duration(value);
            break;
        }
    }
}