#include "plugin.hpp"


struct ARYTHMATIK : Module {
	enum ParamId {
		KNOB1_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN1_INPUT,
		IN2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT4_OUTPUT,
		OUT2_OUTPUT,
		OUT5_OUTPUT,
		OUT3_OUTPUT,
		OUT6_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK1_LIGHT,
		BLINK4_LIGHT,
		BLINK2_LIGHT,
		BLINK5_LIGHT,
		BLINK3_LIGHT,
		BLINK6_LIGHT,
		LIGHTS_LEN
	};

	ARYTHMATIK() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOB1_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN1_INPUT, "");
		configInput(IN2_INPUT, "");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT4_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT5_OUTPUT, "");
		configOutput(OUT3_OUTPUT, "");
		configOutput(OUT6_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct ARYTHMATIKWidget : ModuleWidget {
	ARYTHMATIKWidget(ARYTHMATIK* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ARYTHMATIK.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(-16.711, 65.281)), module, ARYTHMATIK::KNOB1_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(-35.006, 80.18)), module, ARYTHMATIK::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(-17.582, 80.215)), module, ARYTHMATIK::IN2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-32.541, 96.139)), module, ARYTHMATIK::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-20.133, 96.144)), module, ARYTHMATIK::OUT4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-32.578, 109.077)), module, ARYTHMATIK::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-20.133, 109.142)), module, ARYTHMATIK::OUT5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-32.515, 122.141)), module, ARYTHMATIK::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-20.12, 122.141)), module, ARYTHMATIK::OUT6_OUTPUT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(-39.292, 100.411)), module, ARYTHMATIK::BLINK1_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(-13.476, 100.374)), module, ARYTHMATIK::BLINK4_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(-39.216, 113.34)), module, ARYTHMATIK::BLINK2_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<RedLight>>(mm2px(Vec(-13.445, 113.295)), module, ARYTHMATIK::BLINK5_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<RedLight>>(mm2px(Vec(-39.083, 126.304)), module, ARYTHMATIK::BLINK3_LIGHT));
		addChild(createLightCentered<SmallSimpleLight<RedLight>>(mm2px(Vec(-13.344, 126.391)), module, ARYTHMATIK::BLINK6_LIGHT));
	}
};


Model* modelARYTHMATIK = createModel<ARYTHMATIK, ARYTHMATIKWidget>("ARYTHMATIK");