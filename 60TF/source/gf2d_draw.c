#include "gf2d_draw.h"
#include "graphics.h"
#include "simple_logger.h"

void gf2d_draw_lines(Vector2D *p1,Vector2D *p2, Uint32 lines,Vector4D color)
{
    int i;
    SDL_SetRenderDrawColor(GetRenderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    for (i = 0; i < lines;i++)
    {
        SDL_RenderDrawLine(GetRenderer(),
                           p1[i].x,
                           p1[i].y,
                           p2[i].x,
                           p2[i].y);
    }
    SDL_SetRenderDrawColor(GetRenderer(),
                           255,
                           255,
                           255,
                           255);
}

void gf2d_draw_line(Vector2D p1,Vector2D p2, Vector4D color)
{
    SDL_SetRenderDrawColor(GetRenderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawLine(GetRenderer(),
                       p1.x,
                       p1.y,
                       p2.x,
                       p2.y);
    SDL_SetRenderDrawColor(GetRenderer(),
                           255,
                           255,
                           255,
                           255);
}

void gf2d_draw_rect(SDL_Rect rect,Vector4D color)
{
    SDL_SetRenderDrawColor(GetRenderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawRect(GetRenderer(),(const struct SDL_Rect *)&rect);
    SDL_SetRenderDrawColor(GetRenderer(),
                           255,
                           255,
                           255,
                           255);
}

void gf2d_draw_solid_rect(SDL_Rect rect, Vector4D color)
{
    SDL_SetRenderDrawColor(GetRenderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderFillRect(GetRenderer(),(const struct SDL_Rect *)&rect);

    SDL_SetRenderDrawColor(GetRenderer(),
                           255,
                           255,
                           255,
                           255);
}


void gf2d_draw_rects(SDL_Rect *rects,Uint32 count,Vector4D color)
{
    SDL_SetRenderDrawColor(GetRenderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawRects(GetRenderer(),rects,count);
}

void gf2d_draw_pixel(Vector2D pixel,Vector4D color)
{
    SDL_SetRenderDrawColor(GetRenderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawPoint(GetRenderer(),
                        pixel.x,
                        pixel.y);
}

void gf2d_draw_pixel_list(SDL_Point * pixels,Uint32 count,Vector4D color)
{
    SDL_SetRenderDrawColor(GetRenderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawPoints(GetRenderer(),
                        pixels,
                        count);
}

/*
 * credit circle drawing algorith:
 * http://groups.csail.mit.edu/graphics/classes/6.837/F98/Lecture6/circle.html
 */

static int gf2d_draw_circle_points(SDL_Point *p,Vector2D center, Vector2D point)
{  
  if (point.x == 0)
  {
    vector2d_set(p[0],center.x, center.y + point.y);
    vector2d_set(p[1],center.x, center.y - point.y);
    vector2d_set(p[2],center.x + point.y, center.y);
    vector2d_set(p[3],center.x - point.y, center.y);
    return 4;
  }
  if (point.x == point.y)
  {
    vector2d_set(p[0],center.x + point.x, center.y + point.y);
    vector2d_set(p[1],center.x - point.x, center.y + point.y);
    vector2d_set(p[2],center.x + point.x, center.y - point.y);
    vector2d_set(p[3],center.x - point.x, center.y - point.y);
    return 4;
  }
  if (point.x < point.y)
  {
    vector2d_set(p[0],center.x + point.x, center.y + point.y);
    vector2d_set(p[1],center.x - point.x, center.y + point.y);
    vector2d_set(p[2],center.x + point.x, center.y - point.y);
    vector2d_set(p[3],center.x - point.x, center.y - point.y);
    vector2d_set(p[4],center.x + point.y, center.y + point.x);
    vector2d_set(p[5],center.x - point.y, center.y + point.x);
    vector2d_set(p[6],center.x + point.y, center.y - point.x);
    vector2d_set(p[7],center.x - point.y, center.y - point.x);
    return 8;
  }
  return 0;
}

void gf2d_draw_circle(Vector2D center, int radius, Vector4D color)
{
    SDL_Point *pointArray;
    int i = 0;
    Vector2D point = {0,0};
    int p = (5 - radius*4)/4;
    point.y = radius;
    pointArray = (SDL_Point*)malloc(sizeof(SDL_Point)*radius*8);
    if (!pointArray)
    {
        slog("gf2d_draw_circle: failed to allocate points for circle drawing");
        return;
    }
    i = gf2d_draw_circle_points(&pointArray[i],center, point);
    while (point.x < point.y)
    {
        point.x++;
        if (p < 0)
        {
            p += 2*point.x+1;
        }
        else
        {
            point.y--;
            p += 2*(point.x-point.y)+1;
        }
        i += gf2d_draw_circle_points(&pointArray[i],center, point);
        if (i + 8 >= radius*8)
        {
            slog("too many points for circle");
            break;
        }
    }
    SDL_SetRenderDrawColor(GetRenderer(),
                           color.x,
                           color.y,
                           color.z,
                           color.w);
    SDL_RenderDrawPoints(GetRenderer(),pointArray,i);
    SDL_SetRenderDrawColor(GetRenderer(),
                            255,
                            255,
                            255,
                            255);
    free(pointArray);
}


/*eol@eof*/
