#include "gui.h"
#include "graphics.h"
#include "gf2d_text.h"
#include "gf2d_shape.h"

typedef struct
{
	float framerate;
	float healthPct;
	float timeLeft;
	float alert;
}GUI;

static GUI gui = { 0 };

void GUISetupHUD()
{
	memset(&gui, 0, sizeof(GUI));
	gui.healthPct = 100;
	gui.timeLeft = 60.0f;
	gui.framerate = GetFrameRate();
	gui.alert = 0.0f;
}

void gui_draw_percent_bar_horizontal(Rect rect, float percent, Vector4D fgColor, Vector4D bgColor, int left)
{
	SDL_Rect r;
	r = gf2d_rect_to_sdl_rect(rect);
	gf2d_draw_solid_rect(r, bgColor);
	if (left)
	{
		r = gf2d_rect_to_sdl_rect(gf2d_rect(rect.x, rect.y, (float)rect.w*percent, rect.h));
	}
	else
	{
		r = gf2d_rect_to_sdl_rect(gf2d_rect(rect.x + (1 - percent)*rect.w, rect.y, (float)rect.w*percent, rect.h));
	}
	gf2d_draw_solid_rect(r, fgColor);
}

void GUIDrawHUD()
{
	TextLine fps = {0};

	Vector4D color = { 255, 255, 255, 255 };
	
	if (GetFPSCounterEnabled())
	{
		sprintf(fps, "FPS: %i", (int)GetFrameRate());
	}

	if (gui.healthPct < .2)
	{
		gui.alert = (gui.alert + 0.02);
		if (gui.alert >= M_PI)gui.alert = 0;
		color.y = color.z = sin(gui.alert) * 255;
	}
	gui_draw_percent_bar_horizontal(gf2d_rect(10, 10, 145, 20), gui.healthPct, vector4d((1 - gui.healthPct) * 255, gui.healthPct * 255, 0, 255), vector4d(145, 0, 0, 128), 0);
	gf2d_text_draw_line("Health", FT_Small , gf2d_color8(0, 0, 0, 255), vector2d(10, 10));
	
	if (GetFPSCounterEnabled())
	{
		gf2d_text_draw_line(fps, FT_Small, gf2d_color8(255, 255, 255, 255), vector2d(GetRenderDimensions().x * .80f, 10));
	}

}