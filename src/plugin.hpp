#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelMVMNT;
extern Model* modelMVMNT2;
extern Model* modelARYTHMATIK;
extern Model* modelBlank;
extern Model* modelMultvert;


struct ModuloveLed : TSvgLight<RedGreenBlueLight> {


	ModuloveLed() {
	}

	void draw(const DrawArgs& args) override {}
	void drawLayer(const DrawArgs& args, int layer) override {
		if (layer == 1) {

			if (!sw->svg)
				return;

			if (module && !module->isBypassed()) {

				for (auto s = sw->svg->handle->shapes; s; s = s->next) {
					s->fill.color = ((int)(color.a * 255) << 24) + (((int)(color.b * 255)) << 16) + (((int)(color.g * 255)) << 8) + (int)(color.r * 255);
					s->fill.type = NSVG_PAINT_COLOR;
				}

				nvgGlobalCompositeBlendFunc(args.vg, NVG_ONE_MINUS_DST_COLOR, NVG_ONE);
				svgDraw(args.vg, sw->svg->handle);
				drawHalo(args);
			}
		}
		Widget::drawLayer(args, layer);
	}
};