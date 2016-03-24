#ifndef RENDERING_PLUGINS_H
#define RENDERING_PLUGINS_H

#include "convection.h"

/******************************************
   Interface for rendering plugins.
   There is space in the center of the
   domain which could be used to display
   various things of interest/supporting
   data. Define a basic plugin architecture
   for accomplishing this.
******************************************/

class RenderingPlugin
{
  public:
    RenderingPlugin( const ConvectionSimulator &sim ) : sim(sim) {}
    void setup();
    void draw();
    void cleanup();

  protected:
    const ConvectionSimulator &sim;
};


class Core : RenderingPlugin
{
  public:
    Core( const ConvectionSimulator &sim ) : RenderingPlugin(sim) {}
    void setup();
    void draw();
    void cleanup();
  private:
    //Data for rendering with OpenGL
    GLfloat* vertices;
    GLfloat* vertex_colors;
    GLuint* triangle_vertex_indices;

    GLuint plugin_program;
    GLuint plugin_vertices;
    GLuint plugin_vertex_colors;
    GLuint plugin_triangle_vertex_indices;
    GLint plugin_attribute_coord2d;
    GLint plugin_attribute_v_color;
};

class Axis : RenderingPlugin
{
  public:
    Axis( const ConvectionSimulator &sim ) : RenderingPlugin(sim) {}
    void setup();
    void draw();
    void cleanup();
  private:
    //Data for rendering with OpenGL
    GLfloat* vertices;
    GLfloat* vertex_colors;
    GLuint* triangle_vertex_indices;

    GLuint plugin_program;
    GLuint plugin_vertices;
    GLuint plugin_vertex_colors;
    GLuint plugin_triangle_vertex_indices;
    GLint plugin_attribute_coord2d;
    GLint plugin_attribute_v_color;
};

#endif
