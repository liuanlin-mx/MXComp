#include "plugin_editor.h"
#include "imgui-knobs.h"
#include "net_log.h"

plugin_editor::plugin_editor(plugin_processor* effect)
    : imgui_vst_editor(effect, 640, 540)
    , _effect(effect)
    , _implot_ctx(NULL)
    , _update_time(0)
{
    
    for (std::uint32_t i = 0; i < sizeof(_ratio_map_in) / sizeof(_ratio_map_in[0]); i++)
    {
        _ratio_map_in[i] = 0;
        _ratio_map_out[i] = 0;
    }
    _ratio_map_count = 0;
    _eq_count = 0;
}

plugin_editor::~plugin_editor()
{
}


void plugin_editor::draw_init()
{
    for (std::int32_t i = 0; i < plugin_processor::PARAMETER_NUM; i++)
    {
        _effect->get_patameter(i, _parameter[i]);
    }
    _texture[0].init();
    _texture[1].init();
}


void plugin_editor::draw_uninit()
{
    _texture[0].uninit();
    _texture[1].uninit();
}


void plugin_editor::draw(std::int32_t w, std::int32_t h)
{
    if (_implot_ctx == NULL)
    {
        _implot_ctx = ImPlot::CreateContext();
    }
    ImPlot::SetCurrentContext(_implot_ctx);
    
    ImGui::Begin("MXComp", NULL,
                    ImGuiWindowFlags_NoTitleBar 
                    | ImGuiWindowFlags_NoResize
                    | ImGuiWindowFlags_NoMove);
            
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(w, h), ImGuiCond_Always);
    ImPlot::CreateContext();
    
    _draw_knob();
    
    ImGui::SameLine();
    _draw_meter();
    
    std::int32_t idx = plugin_processor::PARAMETER_IDX_SHOW_FFT;
    if (_parameter[idx].value > 0.5)
    {
        _draw_spectrum();
    }
    else
    {
        _draw_wave();
    }
    ImGui::End();
    
    ImPlot::SetCurrentContext(NULL);
}

void plugin_editor::_draw_knob()
{
    ImGui::BeginGroup();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_THRESH;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 0.1f, "%.1fdB", ImGuiKnobVariant_WiperOnly, 0, ImGuiKnobFlags_ValueTooltip))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::SameLine();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_RATIO;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 0.1f, "%.1f", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::SameLine();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_KNEE;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 0.1f, "%.1fdB", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::SameLine();
    
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_GAIN;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 0.1f, "%.1fdB", ImGuiKnobVariant_WiperOnly, 0, ImGuiKnobFlags_DragHorizontal))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::SameLine();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_FILTER_HP_FREQ;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 1, "%.0f", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::NewLine();
    
    
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_ATTACK;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 0.1f, "%.1fms", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::SameLine();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_RELEASE;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 1.f, "%.0fms", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::SameLine();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_RMS;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 0.1f, "%.1fms", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::SameLine();
    
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_PRE;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 1.f, "%.0fms", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    
    ImGui::SameLine();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_FILTER_LP_FREQ;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 10, "%.0f", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    
    {
        ImGui::PushItemWidth(290);
        std::int32_t idx = plugin_processor::PARAMETER_IDX_DETECTOR;
        int contents_type = _parameter[idx].value;
        if (ImGui::Combo("##Detector", &contents_type, "(L,R)\0Max(L,R)\0Avg(L,R)\0L\0R\0Side\0"))
        {
            _parameter[idx].value = contents_type;
            _effect->set_patameter(idx, _parameter[idx].value);
        }
        ImGui::PopItemWidth();
    }
    
    {
        ImGui::PushItemWidth(200);
        std::int32_t idx = plugin_processor::PARAMETER_IDX_WAVE_VIEW_DURATION;
        if (ImGui::SliderFloat("##duration", &_parameter[idx].value, 1, 60, "%.0f"))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
        ImGui::PopItemWidth();
    }
    ImGui::SameLine();
    {
        ImGui::PushItemWidth(90);
        std::int32_t idx = plugin_processor::PARAMETER_IDX_SHOW_FFT;
        bool v = _parameter[idx].value > 0.5;
        if (ImGui::Checkbox(_parameter[idx].name.c_str(), &v))
        {
            _parameter[idx].value = v? 1: 0;
            _effect->set_patameter(idx, _parameter[idx].value);
        }
        ImGui::PopItemWidth();
    }
    ImGui::EndGroup();
}

static inline float _hz_to_mel(float hz)
{
    return 1127 * log(1 + hz / 700);
}

static inline float _mel_to_hz(float mel)
{
    return (exp(mel / 1127) - 1) * 700;
}

static inline double transform_forward_mel(double v, void*)
{
    return _hz_to_mel(v);
}

static inline double transform_inverse_hz(double v, void*)
{
    return _mel_to_hz(v);
}

void plugin_editor::_draw_meter()
{
    ImGui::BeginGroup();
    
    plugin_processor::lv_meter info[2];
    _effect->get_lv_meter_info(info);
    
    if (ImGui::GetTime() - _update_time > 0.5)
    {
        _update_time = ImGui::GetTime();
        _ratio_map_count = _effect->get_ratio_map(_ratio_map_in, _ratio_map_out, -60, 0, 0.1, 1024);
        //_eq_count = _effect->get_eq_curve(_eq_curve, _eq_freq_scale, sizeof(_eq_curve) / sizeof(_eq_curve[0]));
    }
    
    if (ImPlot::BeginPlot("##ratio", ImVec2(-1, 250)/*, ImPlotFlags_Equal*/))
    {
        ImPlotAxisFlags axis_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight;
        ImPlot::SetupAxes(0, 0, axis_flags, axis_flags | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxesLimits(-60, 6, -60, 6);
        
    #if 0
        ImPlot::SetupAxis(ImAxis_X2, NULL, ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_Lock
                                            | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxis(ImAxis_Y2, NULL, ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_Lock
                                            | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks);
        ImPlot::SetupAxisLimits(ImAxis_X2, 0, 24000);
        ImPlot::SetupAxisLimits(ImAxis_Y2, -8, 6);
        
        ImPlot::SetupAxisScale(ImAxis_X2, transform_forward_mel, transform_inverse_hz);
    
        double xtick[] = {200, 500, 1000, 2000, 3000, 5000, 10000, 20000};
        const char *xlabels[] = {"200", "500", "1k", "2k", "3k", "5k", "10k", "20k"};
        ImPlot::SetupAxisTicks(ImAxis_X2, xtick, sizeof(xtick) / sizeof(xtick[0]), xlabels);
    #endif
        
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2);
            ImPlot::PlotLine("##ratio", _ratio_map_in, _ratio_map_out, _ratio_map_count);
            ImPlot::PopStyleVar();
        }
        
       
        {
            ImPlotBarGroupsFlags flags = ImPlotBarGroupsFlags_Stacked;
            static const char* labels[] = {"##gr","##range"};
            float gr[8] = {0, 0, -info[0].gr_db, -info[1].gr_db,
                            0, 0, -60, -60};
                            
            ImPlot::PlotBarGroups(labels, gr, 2, 4, 0.67, 0, flags);
        }
        
        {
            std::int32_t idx = 0;
            float max = std::max(info[0].in_rms_db, info[1].in_rms_db);
            
            for (; idx < (std::int32_t)_ratio_map_count; idx++)
            {
                if (_ratio_map_in[idx] >= max)
                {
                    break;
                }
            }
            if (idx > 0)
            {
                ImPlot::PlotShaded("##left shaded", _ratio_map_in, _ratio_map_out, idx, -INFINITY);
            }
        }
        
        {
            float left_vx[2] = {info[0].in_peek_db, info[0].in_rms_db};
            float left_vy[2] = {info[0].out_peek_db, info[0].out_rms_db};
            float right_vx[2] = {info[1].in_peek_db, info[1].in_rms_db};
            float right_vy[2] = {info[1].out_peek_db, info[1].out_rms_db};
            ImPlot::PlotScatter("##left scatter", left_vx, left_vy, 2);
            ImPlot::PlotScatter("##right scatter", right_vx, right_vy, 2);
        }
        
        {
            std::int32_t idx = plugin_processor::PARAMETER_IDX_THRESH;
            double x = _parameter[idx].value;
            ImPlot::TagX(x, ImVec4(1, 0, 0, 0.5), "%0.1f", x);
            if (ImPlot::DragLineX(0, &x, ImVec4(1, 1, 1, 1), 1))
            {
                if (x > 0.)
                {
                    x = 0.;
                }
                if (x < -60)
                {
                    x = -60;
                }
                
                _parameter[idx].value = x;
                _effect->set_patameter(idx, _parameter[idx].value);
            }
        }
        
    #if 0
        {
            ImPlot::SetAxes(ImAxis_X2, ImAxis_Y2);
            ImPlot::PlotLine("##eq curve", _eq_freq_scale, _eq_curve, _eq_count);
            
            {
                std::int32_t idx = plugin_processor::PARAMETER_IDX_FILTER_LP_FREQ;
                double freq = _parameter[idx].value;
                if (freq < 1000)
                {
                    ImPlot::TagX(freq, ImVec4(0, 0, 1, 0.5), "%0.0f", freq);
                }
                else
                {
                    ImPlot::TagX(freq, ImVec4(0, 0, 1, 0.5), "%0.1fk", freq / 1000.);
                }
                
                if (ImPlot::DragLineX(1, &freq, ImVec4(1, 1, 1, 0.3), 1))
                {
                    if (freq > 20000)
                    {
                        freq = 20000;
                    }
                    if (freq < 0)
                    {
                        freq = 0;
                    }
                    _parameter[idx].value = freq;
                    _effect->set_patameter(idx, _parameter[idx].value);
                }
            }
            
            
            {
                std::int32_t idx = plugin_processor::PARAMETER_IDX_FILTER_HP_FREQ;
                double freq = _parameter[idx].value;
                if (freq < 1000)
                {
                    ImPlot::TagX(freq, ImVec4(0, 0, 1, 0.5), "%0.0f", freq);
                }
                else
                {
                    ImPlot::TagX(freq, ImVec4(0, 0, 1, 0.5), "%0.1fk", freq / 1000.);
                }
                
                if (ImPlot::DragLineX(2, &freq, ImVec4(1, 1, 1, 0.3), 1))
                {
                    if (freq > 20000)
                    {
                        freq = 20000;
                    }
                    if (freq < 0)
                    {
                        freq = 0;
                    }
                    _parameter[idx].value = freq;
                    _effect->set_patameter(idx, _parameter[idx].value);
                }
            }
        }
    #endif
    
        ImPlot::EndPlot();
    }
    
    ImGui::EndGroup();
}

void plugin_editor::_draw_wave()
{
    ImGui::BeginGroup();
    
    if (ImPlot::BeginPlot("##wave left", ImVec2(-1, 130)/*, ImPlotFlags_Equal*/))
    {
        ImPlotAxisFlags axis_flags =  ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoDecorations;
        ImPlot::SetupAxes(0, 0, axis_flags, axis_flags | ImPlotAxisFlags_Lock | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxesLimits(0, 1024, -1.0, 1.0);
        
        ImPlot::SetupAxis(ImAxis_Y2, NULL, ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_Lock
                                            | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxisLimits(ImAxis_Y2, -18, 0);
        
        {
            std::uint32_t cnt = _effect->read_in_wave(0, _wave, 1024);
            ImPlot::PlotLine("##wave left in", _wave, cnt);
            
            cnt = _effect->read_out_wave(0, _wave, 1024);
            ImPlot::PlotLine("##wave left out", _wave, cnt);
        }
        
        {
            ImPlot::SetAxis(ImAxis_Y2);
            std::uint32_t cnt = _effect->read_gr_wave(0, _wave, 1024);
            for (std::uint32_t i = 0; i < cnt; i++)
            {
                _wave[i] = -_wave[i];
            }
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2);
            ImPlot::PlotLine("##gr left", _wave, cnt);
            ImPlot::PopStyleVar();
        }
        
        
        ImPlot::EndPlot();
    }
    
    if (ImPlot::BeginPlot("##wave right", ImVec2(-1, 130)))
    {
        ImPlotAxisFlags axis_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoDecorations;
        ImPlot::SetupAxes(0, 0, axis_flags, axis_flags | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxesLimits(0, 1024, -1.0, 1.0);
        
        ImPlot::SetupAxis(ImAxis_Y2, NULL, ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_Lock
                                            | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxisLimits(ImAxis_Y2, -18, 0);
        
        {
            std::uint32_t cnt = _effect->read_in_wave(1, _wave, 1024);
            ImPlot::PlotLine("##wave right in", _wave, cnt);
            
            cnt = _effect->read_out_wave(1, _wave, 1024);
            ImPlot::PlotLine("##wave right out", _wave, cnt);
            
        }
        
        {
            ImPlot::SetAxis(ImAxis_Y2);
            std::uint32_t cnt = _effect->read_gr_wave(1, _wave, 1024);
            for (std::uint32_t i = 0; i < cnt; i++)
            {
                _wave[i] = -_wave[i];
            }
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2);
            ImPlot::PlotLine("##gr right", _wave, cnt);
            ImPlot::PopStyleVar();
        }
        
        ImPlot::EndPlot();
    }
    ImGui::EndGroup();
}


void plugin_editor::_draw_spectrum()
{
    ImGui::BeginGroup();
    
    if (ImPlot::BeginPlot("##spectrum left", ImVec2(-1, 130)/*, ImPlotFlags_Equal*/))
    {
        ImPlotAxisFlags axis_flags =  ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoDecorations;
        ImPlot::SetupAxes(0, 0, axis_flags, axis_flags | ImPlotAxisFlags_Lock | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxesLimits(0, 1, 0, 1.0);
        
        ImPlot::SetupAxis(ImAxis_Y2, NULL, ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_Lock
                                            | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxisLimits(ImAxis_Y2, -18, 0);
        
        {
            _effect->read_fft_image(0, _rgb);
            ImTextureID texture = (ImTextureID)_texture[0].load(_rgb, _effect->get_fft_image_width(0), _effect->get_fft_image_height(0));
            static ImVec2 bmin(0, 0);
            static ImVec2 bmax(1, 1);
            ImPlot::PlotImage("##fft left", texture, bmin, bmax);
        }
        
        {
            ImPlot::SetAxis(ImAxis_Y2);
            std::uint32_t cnt = _effect->read_gr_wave(0, _wave, 1024);
            for (std::uint32_t i = 0; i < cnt; i++)
            {
                _wave[i] = -_wave[i];
            }
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2);
            ImPlot::PlotLine("##gr left", _wave, cnt, 1. / cnt);
            ImPlot::PopStyleVar();
        }
        
        
        ImPlot::EndPlot();
    }
    
    if (ImPlot::BeginPlot("##spectrum right", ImVec2(-1, 130)))
    {
        ImPlotAxisFlags axis_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoDecorations;
        ImPlot::SetupAxes(0, 0, axis_flags, axis_flags | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxesLimits(0, 1.0, 0, 1.0);
        
        ImPlot::SetupAxis(ImAxis_Y2, NULL, ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_Lock
                                            | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxisLimits(ImAxis_Y2, -18, 0);
        
        
        {
            _effect->read_fft_image(1, _rgb);
            ImTextureID texture = (ImTextureID)_texture[1].load(_rgb, _effect->get_fft_image_width(1), _effect->get_fft_image_height(1));
            static ImVec2 bmin(0, 0);
            static ImVec2 bmax(1, 1);
            ImPlot::PlotImage("##fft right", texture, bmin, bmax);
        }
        
        {
            ImPlot::SetAxis(ImAxis_Y2);
            std::uint32_t cnt = _effect->read_gr_wave(1, _wave, 1024);
            for (std::uint32_t i = 0; i < cnt; i++)
            {
                _wave[i] = -_wave[i];
            }
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2);
            ImPlot::PlotLine("##gr right", _wave, cnt, 1. / cnt);
            ImPlot::PopStyleVar();
        }
        
        ImPlot::EndPlot();
    }
    ImGui::EndGroup();
}