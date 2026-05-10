/*  Nuklear config */
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_MEMSET memset
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define NK_SDL_RENDERER_SDL_H <SDL2/SDL.h>
#include "nuklear_sdl_renderer.h"

// renderer
#include "array.h"
#include "camera.h"
#include "clipping.h"
#include "debug_ui.h"
#include "display.h"
#include "light.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_TRIANGLE_PER_MESH 100000


triangle_t triangles_to_render[MAX_TRIANGLE_PER_MESH];
int num_triangles_to_render = 0;
float delta_time = 0;

mat4_t projection_matrix; // perspective projection matrix

bool is_running = false; // check init window
int previous_frame_time = 0;

struct nk_context *nk_ctx = NULL;

// SETUP
//  ok so here we are doing the color buffers , what we need to understand is
//  that we are allocating color buffers for each pixel that is set according to
//  the window width and hieght that we initalized at the top and then set a
//  texture , the texture accepts 5 parameters , first one is where we want it
//  which is the renderer then the pixel format , acces and then the height and
//  width of the texture

void setup(void) {

  render_method = RENDER_WIRE;
  cull_method = CULL_BACKFACE;


  // init perspective projection matrix
  float aspectx = (float)get_window_width() / (float)get_window_height();
  float aspecty = (float)get_window_height() / (float)get_window_width( );
  float fovy = M_PI / 3.0;
  float fovx = atan(tan(fovy / 2) * aspectx) * 2.0;
  float znear = 1.0;
  float zfar = 20.0;
  projection_matrix = mat4_perspective(fovy, aspecty, znear, zfar);

  // initialize frustum plane with a point and a normal
  init_frustum_planes(fovx, fovy, znear, zfar);



  load_obj_file("./models/efa.obj");

  load_png_texture("./models/efa.png");

  // nuklear init happens after the renderer is created
  nk_ctx = nk_sdl_init(window, renderer);

  struct nk_font_atlas *atlas;
  nk_sdl_font_stash_begin(&atlas);
  nk_sdl_font_stash_end();

  // nk_sdl_init` hands existing `window` and `renderer` so
  //  no new SDL window needed. The font stash calls bake the default
  //  built-in font into a texture the renderer can use.

  debug_ui_init();
  debug_ui_set_aero_theme(nk_ctx);


}

void process_input(void) {
  SDL_Event event;

  nk_input_begin(nk_ctx);

  while (SDL_PollEvent(&event)) {

    // here we are passing in the address of the event declared above
    // we use a swtich statement to check the conditions that make is_running
    // false so we can stop the game loop here the first case is SDL_QUIT which
    // is checking if we need to quit and then updates it down below the
    // is_running to false so the game loop ends. SDL_QUIT isnt necessarily the
    // "escape" key but its more like the closing window button yeah the closing
    // red button window , THAT. and if you can read the second case its just
    // pressed down key and if the presseed down key is infact escape you should
    // also stop the loop whihc is is_running false
    nk_sdl_handle_event(&event);

    switch (event.type) {
    case SDL_QUIT:
      is_running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
        is_running = false;
      if (event.key.keysym.sym == SDLK_1)
        render_method = RENDER_WIRE_VERTEX;
      if (event.key.keysym.sym == SDLK_2)
        render_method = RENDER_WIRE;
      if (event.key.keysym.sym == SDLK_3)
        render_method = RENDER_FILL;
      if (event.key.keysym.sym == SDLK_4)
        render_method = RENDER_FILL_WIRE;
      if (event.key.keysym.sym == SDLK_5)
        render_method = RENDER_TEXTURED;
      if (event.key.keysym.sym == SDLK_6)
        render_method = RENDER_TEXTURE_WIRE;
      if (event.key.keysym.sym == SDLK_c)
        cull_method = CULL_BACKFACE;
      if (event.key.keysym.sym == SDLK_q)
        cull_method = CULL_NONE;
      if (event.key.keysym.sym == SDLK_a)
        camera.yaw -= 1.0 * delta_time; // left
      if (event.key.keysym.sym == SDLK_d)
        camera.yaw += 1.0 * delta_time; // right
      if (event.key.keysym.sym == SDLK_UP)
        camera.position.y += 2.0 * delta_time; // up
      if (event.key.keysym.sym == SDLK_DOWN)
        camera.position.y -= 2.0 * delta_time; // down

      if (event.key.keysym.sym == SDLK_w) {
        camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
        camera.position = vec3_add(camera.position, camera.forward_velocity);
      }
      if (event.key.keysym.sym == SDLK_s) {
        camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
        camera.position = vec3_sub(camera.position, camera.forward_velocity);
      } // backward
      break;
    }
  }
  nk_input_end(nk_ctx);
}

void update(void) {
  while (!SDL_TICKS_PASSED(SDL_GetTicks(),
                           previous_frame_time + FRAME_TARGET_TIME))
    ;

  // delta time can be used to update game objects so they dont vary with the
  // fps we choose we get detla time and convert it to seconds
  delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;
  previous_frame_time = SDL_GetTicks();

  // initalize the array of triangles to render
  num_triangles_to_render = 0;

  // mesh.rotation.x += 0.006 *delta_time;
  // mesh.rotation.y += 0.000 *delta_time;
  // mesh.rotation.z += 0.000 *delta_time;
  mesh.translation.z = 5;
  // mesh.translation.x += 1.0 *delta_time;

  // create a view matrix looking at  target point

  Vec3_t up = {0, 1, 0}; // normalized y axis

  // for finding target , we have to take in account for yaw rotation etc
  Vec3_t target = {0, 0, 1};

  mat4_t camera_yaw_matrix = mat4_rotation_y(camera.yaw);
  camera.direction =
      vec3_from_vec4(mat4_mul_vec4(camera_yaw_matrix, vec4_from_vec3(target)));

  // offset camera position in the direction where the camera is pointing at

  target = vec3_add(camera.position, camera.direction);

  mat4_t view_matrix = mat4_look_at(camera.position, target, up);

  mat4_t scale_matrix = mat4_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

  mat4_t translation_matrix = mat4_translaton(
      mesh.translation.x, mesh.translation.y, mesh.translation.z);

  mat4_t rotation_matrix_x = mat4_rotation_x(mesh.rotation.x);
  mat4_t rotation_matrix_y = mat4_rotation_y(mesh.rotation.y);
  mat4_t rotation_matrix_z = mat4_rotation_z(mesh.rotation.z);

  int num_faces = array_length(mesh.faces);

  for (int i = 0; i < num_faces; i++) {

    face_t mesh_face = mesh.faces[i];

    Vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a];
    face_vertices[1] = mesh.vertices[mesh_face.b];
    face_vertices[2] = mesh.vertices[mesh_face.c];
    Vec4_t transformed_vertices[3];

    // world matrix: scale -> rotate -> translate (computed once per face)
    mat4_t world_matrix = mat4_identity();
    world_matrix = mat4_t_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_t_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_t_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_t_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_t_mul_mat4(translation_matrix, world_matrix);

    for (int j = 0; j < 3; j++) {
      Vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

      // world transform then view transform
      transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
      transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

      transformed_vertices[j] = transformed_vertex;
    }

    /////////////////////////////////
    ///// backface culling     //////
    /////////////////////////////////

    Vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
    Vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
    Vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

    Vec3_t vector_ab = vec3_sub(vector_b, vector_a);
    Vec3_t vector_ac = vec3_sub(vector_c, vector_a);
    vec3_normalize(&vector_ab);
    vec3_normalize(&vector_ac);

    Vec3_t normal = vec3_cross(vector_ab, vector_ac);

    // normalize the face of the normal vector

    vec3_normalize(&normal);

    // find a vector between point in triangle and camera origin
    //

    Vec3_t origin = {0, 0, 0};

    Vec3_t cam_ray = vec3_sub(origin, vector_a);

    // calculate hoiw aligned camera ray is with face normal using dot product

    float normal_camera = vec3_dot(normal, cam_ray);

    // bypass trianngles that are looking away from camera
    /* skip back-facing triangles only when culling is enabled */
    if (cull_method == CULL_BACKFACE && normal_camera < 0) {
      continue;
    }

    ////////////////////////////////////////
    /// clipping //////////////////////////
    //////////////////////////////////////

    polygon_t polygon = create_poly_from_triangle(
        vec3_from_vec4(transformed_vertices[0]),
        vec3_from_vec4(transformed_vertices[1]),
        vec3_from_vec4(transformed_vertices[2]), mesh_face.a_uv, mesh_face.b_uv,
        mesh_face.c_uv);

    clip_polygon(&polygon);

    // break polygon into individual triangles

    triangle_t triangles_after_clipping[MAX_NO_OF_POLYGON_TRIANGLES];

    int no_of_triangles_after_clipping = 0;

    triangles_from_polygon(&polygon, triangles_after_clipping,
                           &no_of_triangles_after_clipping);

    for (int t = 0; t < no_of_triangles_after_clipping; t++) {
      triangle_t triangle_after_clipping = triangles_after_clipping[t];
      Vec4_t projected_point[3];

      for (int j = 0; j < 3; j++) {
        // Project the current vertex using the perspective projection matrix
        projected_point[j] =
            mat4_mul_vec4(projection_matrix, triangle_after_clipping.points[j]);

        // Perform perspective divide with the original w value
        if (projected_point[j].w != 0.0) {
          projected_point[j].x /= projected_point[j].w;
          projected_point[j].y /= projected_point[j].w;
          projected_point[j].z /= projected_point[j].w;
        }

        // Invert y values to account for flipped screen y-coordinates
        projected_point[j].y *= -1;

        // Scale into the view
        projected_point[j].x *= (get_window_width() / 2.0);
        projected_point[j].y *= (get_window_height() / 2.0);

        // Translate the projected points to the middle of the screen
        projected_point[j].x += (get_window_width() / 2.0);
        projected_point[j].y += (get_window_height() / 2.0);
      }

      //////////////////////////////////////////////////
      /////////// light calculation ///////////////////
      ////////////////////////////////////////////////

      // calc the shade intensity based on how aligned the normal is to the
      // inverse of light ray
      float light_intensity_factor = -vec3_dot(normal, light.direction);

      // calculate triangle color based on the light

      uint32_t triangle_color =
          light_intensity_apply(mesh_face.color, light_intensity_factor);

      triangle_t triangle_to_render = {
          .points =
              {
                  {projected_point[0].x, projected_point[0].y,
                   projected_point[0].z, projected_point[0].w},
                  {projected_point[1].x, projected_point[1].y,
                   projected_point[1].z, projected_point[1].w},
                  {projected_point[2].x, projected_point[2].y,
                   projected_point[2].z, projected_point[2].w},
              },
          .tex_cordinates =
              {
                  {triangle_after_clipping.tex_cordinates[0].u,
                   triangle_after_clipping.tex_cordinates[0].v},
                  {triangle_after_clipping.tex_cordinates[1].u,
                   triangle_after_clipping.tex_cordinates[1].v},
                  {triangle_after_clipping.tex_cordinates[2].u,
                   triangle_after_clipping.tex_cordinates[2].v},
              },
          .color = triangle_color,

      };
      // save the projcted triangles in the array of triangles to render;
      if (num_triangles_to_render < MAX_TRIANGLE_PER_MESH) {
        triangles_to_render[num_triangles_to_render++] = triangle_to_render;
      }
    }
  }
}

/*///////////////////////////////////////////////
/////////////////////////////////////////////////
///////////// RENDER ////////////////////////////
/////////////////////////////////////////////////
//////////////////////////////////////////////*/

void render(void) {

    clear_color_buffer(0XFF000000);
    clear_z_buffer();

  /* [#7] grid toggle from debug panel */
  if (dbg_show_grid)
    draw_grid();

  for (int i = 0; i < num_triangles_to_render; i++) {

    triangle_t triangle = triangles_to_render[i];

    if (should_render_filled_triangles()) {
      draw_filled_triangle(
          triangle.points[0].x, triangle.points[0].y, triangle.points[0].z,
          triangle.points[0].w, triangle.points[1].x, triangle.points[1].y,
          triangle.points[1].z, triangle.points[1].w, triangle.points[2].x,
          triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
          triangle.color);
    }

    // draw textured triangle
    if (should_render_textured_triangles()) {
      draw_textured_triangle(
          triangle.points[0].x, triangle.points[0].y, triangle.points[0].z,
          triangle.points[0].w, triangle.tex_cordinates[0].u,
          triangle.tex_cordinates[0].v, // vertex a
          triangle.points[1].x, triangle.points[1].y, triangle.points[1].z,
          triangle.points[1].w, triangle.tex_cordinates[1].u,
          triangle.tex_cordinates[1].v, // vertex b
          triangle.points[2].x, triangle.points[2].y, triangle.points[2].z,
          triangle.points[2].w, triangle.tex_cordinates[2].u,
          triangle.tex_cordinates[2].v, // vertex c
          mesh_texture);
    }

    if (render_method == RENDER_WIRE_VERTEX || render_method == RENDER_WIRE ||
        render_method == RENDER_TEXTURE_WIRE ||
        render_method == RENDER_FILL_WIRE ||
        dbg_wireframe_overlay) { /* [#7] wireframe overlay toggle */

      // draw an unfilled trinalge it looks ugly without it
      draw_triangle(triangle.points[0].x, triangle.points[0].y,
                    triangle.points[1].x, triangle.points[1].y,
                    triangle.points[2].x, triangle.points[2].y, 0xFF00FF00);
    }

    if (render_method == RENDER_WIRE_VERTEX) {
      draw_rec(triangle.points[0].x - 2, triangle.points[0].y - 2, 5, 5,
               0xFFFF0000);
      draw_rec(triangle.points[1].x - 2, triangle.points[1].y - 2, 5, 5,
               0xFFFF0000);
      draw_rec(triangle.points[2].x - 2, triangle.points[2].y - 2, 5, 5,
               0xFFFF0000);
    }
  }

  render_color_buffer();


  // debug panel moved
  int total_faces = array_length(mesh.faces);
  bool swap =
      debug_ui_render(nk_ctx, delta_time, num_triangles_to_render, total_faces);

  /*  handle model hot-swap */
  if (swap && dbg_selected_model >= 0 && dbg_selected_model < dbg_model_count) {
    dbg_model_entry_t *m = &dbg_models[dbg_selected_model];

    /* free old mesh data */
    array_free(mesh.faces);
    mesh.faces = NULL;
    array_free(mesh.vertices);
    mesh.vertices = NULL;

    /* free old texture */
    if (png_texture) {
      upng_free(png_texture);
      png_texture = NULL;
    }
    mesh_texture = NULL;

    /* load new model */
    load_obj_file(m->obj_path);
    if (m->has_texture) {
      load_png_texture(m->tex_path);
    }

    /* reset mesh transform so it's visible */
    mesh.rotation = (Vec3_t){0, 0, 0};
    mesh.scale = (Vec3_t){1, 1, 1};
    mesh.translation = (Vec3_t){0, 0, 5};
  }

  /* flush Nuklear draw commands on top of the 3D scene */
  nk_sdl_render(NK_ANTI_ALIASING_ON);

  /* flip — everything is now on screen */
  SDL_RenderPresent(renderer);
}

// free memory

void free_resources(void) {
  if (png_texture) upng_free(png_texture);
  array_free(mesh.faces);
  array_free(mesh.vertices);
  nk_sdl_shutdown();
}

// game loop

int main(void) {
  // SDL window
  is_running = init_window();
  setup();

  while (is_running) {
    process_input();
    update();
    render();
  }
  destruct_window();
  free_resources();
  return 0;
}
