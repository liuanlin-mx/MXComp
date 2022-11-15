#ifndef __PLUGIN_EDITOR_H__
#define __PLUGIN_EDITOR_H__
#include <math.h>
#include "imgui_vst_backend/imgui_vst_editor.h"
#include "plugin_processor.h"
#include "implot.h"


class plugin_processor;
class plugin_editor: public imgui_vst_editor
{
public:
    explicit plugin_editor(plugin_processor* effect);
    ~plugin_editor();
    
private:
    virtual bool open(void* ptr);
    virtual void draw(std::int32_t w, std::int32_t h);
    
private:
    inline float _to_db(float v)
    {
        return 20. * log10(v);
    }
    
    void _draw_comp();
    void _draw_filter();
    void _draw_meter();
    void _draw_wave();
    
private:
    plugin_processor *_effect;
    plugin_processor::parameter _parameter[plugin_processor::PARAMETER_NUM];
    ImPlotContext *_implot_ctx;
    
    float _ratio_map_in[1024];
    float _ratio_map_out[1024];
    std::uint32_t _ratio_map_count;
    float _update_time;
    float _wave[1024];
    float _duration = 10;
};
#endif