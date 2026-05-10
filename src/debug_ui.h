#ifndef DEBUG_UI_H
#define DEBUG_UI_H

#include "nuklear.h"
#include <stdbool.h>



extern nk_bool dbg_show_grid;
extern nk_bool dbg_wireframe_overlay;


#define DBG_MAX_MODELS 32
#define DBG_MAX_PATH 256

typedef struct {
  char obj_path[DBG_MAX_PATH]; /* e.g. "./models/efa.obj"  */
  char tex_path[DBG_MAX_PATH]; /* e.g. "./models/efa.png"  */
  char display_name[64];       /* e.g. "efa"               */
  bool has_texture;
} dbg_model_entry_t;

extern dbg_model_entry_t dbg_models[DBG_MAX_MODELS];
extern int dbg_model_count;
extern int dbg_selected_model;


void debug_ui_init(void);

void debug_ui_set_aero_theme(struct nk_context *ctx);

bool debug_ui_render(struct nk_context *nk_ctx, float delta_time,
                     int num_tris_rendered, int total_faces);

#endif
