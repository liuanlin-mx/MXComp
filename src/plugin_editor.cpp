#include "plugin_editor.h"
#include "imgui-knobs.h"
#include "net_log.h"

plugin_editor::plugin_editor(plugin_processor* effect)
    : imgui_vst_editor(effect, 800, 600)
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
}

plugin_editor::~plugin_editor()
{
}


bool plugin_editor::open(void* ptr)
{
    imgui_vst_editor::open(ptr);
    
    for (std::int32_t i = 0; i < plugin_processor::PARAMETER_NUM; i++)
    {
        _effect->get_patameter(i, _parameter[i]);
    }
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
    
    _draw_comp();
    
    ImGui::SameLine();
    _draw_filter();
    
    ImGui::SameLine();
    _draw_meter();
    
    _draw_wave();
    
    ImGui::End();
    
    ImPlot::SetCurrentContext(NULL);
}

void plugin_editor::_draw_filter()
{
    ImGui::BeginGroup();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_FILTER_HP_FREQ;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 0.1f, "%.0f", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::SameLine();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_FILTER_LP_FREQ;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 1.f, "%.0fHz", ImGuiKnobVariant_WiperOnly))
        //if (ImGui::SliderFloat(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, "%.0fHz"))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::NewLine();
    {
        std::int32_t idx = plugin_processor::PARAMETER_IDX_FILTER_Q;
        if (ImGuiKnobs::Knob(_parameter[idx].name.c_str(), &_parameter[idx].value, _parameter[idx].min, _parameter[idx].max, 0.1f, "%.1f", ImGuiKnobVariant_WiperOnly))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
    }
    ImGui::EndGroup();
}

void plugin_editor::_draw_comp()
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
    
    
    {
        ImGui::PushItemWidth(220);
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
        ImGui::PushItemWidth(220);
        std::int32_t idx = plugin_processor::PARAMETER_IDX_WAVE_VIEW_DURATION;
        if (ImGui::SliderFloat("##duration", &_parameter[idx].value, 1, 60, "%.0f"))
        {
            _effect->set_patameter(idx, _parameter[idx].value);
        }
        ImGui::PopItemWidth();
    }
    ImGui::EndGroup();
}


void plugin_editor::_draw_meter()
{
    ImGui::BeginGroup();
    
    plugin_processor::lv_meter info[2];
    _effect->get_lv_meter_info(info);
    
    if (ImGui::GetTime() - _update_time > 0.1)
    {
        _update_time = ImGui::GetTime();
        _ratio_map_count = _effect->get_ratio_map(_ratio_map_in, _ratio_map_out, -60, 0, 0.1, 1024);
    }
    
    if (ImPlot::BeginPlot("##ratio", ImVec2(-1, 250)/*, ImPlotFlags_Equal*/))
    {
        ImPlotAxisFlags axis_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight;
        ImPlot::SetupAxes(0, 0, axis_flags, axis_flags | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxesLimits(-60, 6, -60, 6);
        
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
            
            for (; idx < _ratio_map_count; idx++)
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
        
        ImPlot::EndPlot();
    }
    
    ImGui::EndGroup();
}

void plugin_editor::_draw_wave()
{
    ImGui::BeginGroup();
    
    if (ImPlot::BeginPlot("##wave left", ImVec2(-1, 150)/*, ImPlotFlags_Equal*/))
    {
        ImPlotAxisFlags axis_flags =  ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoDecorations;
        ImPlot::SetupAxes(0, 0, axis_flags, axis_flags | ImPlotAxisFlags_Lock | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxesLimits(0, 1024, -1.0, 1.0);
        
        ImPlot::SetupAxis(ImAxis_Y2, NULL, ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_Lock
                                            | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks);
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
            ImPlot::PlotLine("##gr left", _wave, cnt);
        }
        
        
        ImPlot::EndPlot();
    }
    
    if (ImPlot::BeginPlot("##wave right", ImVec2(-1, 150)))
    {
        ImPlotAxisFlags axis_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoDecorations;
        ImPlot::SetupAxes(0, 0, axis_flags, axis_flags | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxesLimits(0, 1024, -1.0, 1.0);
        
        ImPlot::SetupAxis(ImAxis_Y2, NULL, ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_Lock
                                            | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks);
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
            ImPlot::PlotLine("##gr right", _wave, cnt);
        }
        
        ImPlot::EndPlot();
    }
    ImGui::EndGroup();
}