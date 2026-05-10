#include "debug_ui.h"
#include "display.h"          /* render_method, cull_method enums */

#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>



nk_bool dbg_show_grid          = 0;
nk_bool dbg_wireframe_overlay  = 0;

dbg_model_entry_t dbg_models[DBG_MAX_MODELS];
int               dbg_model_count    = 0;
int               dbg_selected_model = -1;   /* -1 = nothing yet */



/* Check if a file exists (good enough for our use) */
static bool file_exists(const char *path) {
    FILE *f = fopen(path, "r");
    if (f) { fclose(f); return true; }
    return false;
}

/* Strip extension from a filename into buf (max buflen chars). */
static void strip_ext(const char *filename, char *buf, int buflen) {
    strncpy(buf, filename, buflen - 1);
    buf[buflen - 1] = '\0';
    char *dot = strrchr(buf, '.');
    if (dot) *dot = '\0';
}



void debug_ui_init(void) {
    dbg_model_count = 0;
    memset(dbg_models, 0, sizeof(dbg_models));

    DIR *dir = opendir("./models");
    if (!dir) {
        fprintf(stderr, "[debug_ui] could not open ./models/\n");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && dbg_model_count < DBG_MAX_MODELS) {
        /* only care about .obj files */
        const char *dot = strrchr(entry->d_name, '.');
        if (!dot || strcmp(dot, ".obj") != 0) continue;

        dbg_model_entry_t *m = &dbg_models[dbg_model_count];

        /* build obj path */
        snprintf(m->obj_path, DBG_MAX_PATH, "./models/%s", entry->d_name);

        /* display name = filename without extension */
        strip_ext(entry->d_name, m->display_name, sizeof(m->display_name));

        /* look for a matching .png texture */
        snprintf(m->tex_path, DBG_MAX_PATH, "./models/%s.png", m->display_name);
        m->has_texture = file_exists(m->tex_path);

        dbg_model_count++;
    }
    closedir(dir);

    fprintf(stdout, "[debug_ui] found %d model(s) in ./models/\n", dbg_model_count);
}


void debug_ui_set_aero_theme(struct nk_context *ctx) {
    struct nk_color table[NK_COLOR_COUNT];


    table[NK_COLOR_TEXT]                  = nk_rgba(240, 245, 255, 255);
    table[NK_COLOR_WINDOW]                = nk_rgba( 20,  40,  75, 210);
    table[NK_COLOR_HEADER]                = nk_rgba( 40,  80, 140, 230);
    table[NK_COLOR_BORDER]                = nk_rgba(100, 160, 220, 180);
    table[NK_COLOR_BUTTON]                = nk_rgba( 55, 105, 175, 240);
    table[NK_COLOR_BUTTON_HOVER]          = nk_rgba( 75, 140, 210, 255);
    table[NK_COLOR_BUTTON_ACTIVE]         = nk_rgba( 40,  85, 150, 255);
    table[NK_COLOR_TOGGLE]                = nk_rgba( 30,  55, 100, 200);
    table[NK_COLOR_TOGGLE_HOVER]          = nk_rgba( 50,  90, 150, 220);
    table[NK_COLOR_TOGGLE_CURSOR]         = nk_rgba(120, 190, 255, 255);
    table[NK_COLOR_SELECT]                = nk_rgba( 30,  55, 100, 200);
    table[NK_COLOR_SELECT_ACTIVE]         = nk_rgba( 55, 105, 175, 240);
    table[NK_COLOR_SLIDER]                = nk_rgba( 30,  55, 100, 200);
    table[NK_COLOR_SLIDER_CURSOR]         = nk_rgba(100, 180, 255, 255);
    table[NK_COLOR_SLIDER_CURSOR_HOVER]   = nk_rgba(140, 200, 255, 255);
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE]  = nk_rgba( 80, 150, 230, 255);
    table[NK_COLOR_PROPERTY]              = nk_rgba( 30,  55, 100, 200);
    table[NK_COLOR_EDIT]                  = nk_rgba( 25,  45,  85, 220);
    table[NK_COLOR_EDIT_CURSOR]           = nk_rgba(240, 245, 255, 255);
    table[NK_COLOR_COMBO]                 = nk_rgba( 30,  55, 100, 200);
    table[NK_COLOR_CHART]                 = nk_rgba( 25,  45,  85, 220);
    table[NK_COLOR_CHART_COLOR]           = nk_rgba(100, 180, 255, 255);
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(180, 220, 255, 255);
    table[NK_COLOR_SCROLLBAR]             = nk_rgba( 20,  40,  75, 180);
    table[NK_COLOR_SCROLLBAR_CURSOR]      = nk_rgba( 70, 130, 200, 200);
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER]  = nk_rgba( 90, 155, 225, 230);
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba( 55, 105, 175, 255);
    table[NK_COLOR_TAB_HEADER]            = nk_rgba( 40,  80, 140, 230);

    nk_style_from_table(ctx, table);


    ctx->style.window.rounding  = 6.0f;
    ctx->style.button.rounding  = 4.0f;
    ctx->style.window.header.label_padding = nk_vec2(4, 2);


    ctx->style.window.border  = 1.5f;
    ctx->style.button.border  = 1.0f;
}


bool debug_ui_render(struct nk_context *ctx,
                     float delta_time,
                     int   num_tris_rendered,
                     int   total_faces) {

    bool model_swap_requested = false;

    if (nk_begin(ctx, "Debug",
                 nk_rect(10, 10, 280, 520),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE |
                 NK_WINDOW_MOVABLE | NK_WINDOW_MINIMIZABLE)) {


        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Render Mode", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 25, 2);
        if (nk_button_label(ctx, "1 Wire+Dots"))
            render_method = RENDER_WIRE_VERTEX;
        if (nk_button_label(ctx, "2 Wireframe"))
            render_method = RENDER_WIRE;

        nk_layout_row_dynamic(ctx, 25, 2);
        if (nk_button_label(ctx, "3 Filled"))
            render_method = RENDER_FILL;
        if (nk_button_label(ctx, "4 Fill+Wire"))
            render_method = RENDER_FILL_WIRE;

        nk_layout_row_dynamic(ctx, 25, 2);
        if (nk_button_label(ctx, "5 Textured"))
            render_method = RENDER_TEXTURED;
        if (nk_button_label(ctx, "6 Tex+Wire"))
            render_method = RENDER_TEXTURE_WIRE;


        nk_layout_row_dynamic(ctx, 8, 1);
        nk_spacing(ctx, 1);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Back-face Culling", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 25, 2);
        if (nk_button_label(ctx, "C Enable"))
            cull_method = CULL_BACKFACE;
        if (nk_button_label(ctx, "D Disable"))
            cull_method = CULL_NONE;

        nk_layout_row_dynamic(ctx, 8, 1);
        nk_spacing(ctx, 1);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Performance", NK_TEXT_LEFT);

        char buf[128];

        nk_layout_row_dynamic(ctx, 18, 1);
        float fps = (delta_time > 0.0f) ? 1.0f / delta_time : 0.0f;
        snprintf(buf, sizeof(buf), "FPS: %.1f", fps);
        nk_label(ctx, buf, NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 18, 1);
        snprintf(buf, sizeof(buf), "Frame: %.2f ms", delta_time * 1000.0f);
        nk_label(ctx, buf, NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 18, 1);
        snprintf(buf, sizeof(buf), "Triangles: %d / %d", num_tris_rendered, total_faces);
        nk_label(ctx, buf, NK_TEXT_LEFT);


        nk_layout_row_dynamic(ctx, 8, 1);
        nk_spacing(ctx, 1);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Debug Visualization", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_checkbox_label(ctx, "Show Grid", &dbg_show_grid);

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_checkbox_label(ctx, "Wireframe Overlay", &dbg_wireframe_overlay);

        nk_layout_row_dynamic(ctx, 8, 1);
        nk_spacing(ctx, 1);

        nk_layout_row_dynamic(ctx, 20, 1);
        snprintf(buf, sizeof(buf), "Models (%d found)", dbg_model_count);
        nk_label(ctx, buf, NK_TEXT_LEFT);

        for (int i = 0; i < dbg_model_count; i++) {
            nk_layout_row_dynamic(ctx, 25, 1);


            if (i == dbg_selected_model) {
                snprintf(buf, sizeof(buf), "> %s %s",
                         dbg_models[i].display_name,
                         dbg_models[i].has_texture ? "(tex)" : "(no tex)");
            } else {
                snprintf(buf, sizeof(buf), "  %s %s",
                         dbg_models[i].display_name,
                         dbg_models[i].has_texture ? "(tex)" : "(no tex)");
            }

            if (nk_button_label(ctx, buf)) {
                if (i != dbg_selected_model) {
                    dbg_selected_model = i;
                    model_swap_requested = true;
                }
            }
        }
    }
    nk_end(ctx);

    return model_swap_requested;
}
