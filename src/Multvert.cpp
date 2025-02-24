#include "plugin.hpp"

using simd::float_4;

struct Multvert : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		IN1_INPUT,
		IN2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_A_OUTPUT,
		OUT1_B_OUTPUT,
		OUT1_C_OUTPUT,
		OUT1_D_OUTPUT,
		OUT2_A_OUTPUT,
		OUT2_B_OUTPUT,
		OUT2_INV_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(CROSS_LED, 3),
		ENUMS(PHI_LED, 3),
		LIGHTS_LEN
	};

	Multvert() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(IN1_INPUT, "First");
		configInput(IN2_INPUT, "Second");
		configOutput(OUT1_A_OUTPUT, "First A");
		configOutput(OUT1_B_OUTPUT, "First B");
		configOutput(OUT1_C_OUTPUT, "First C");
		configOutput(OUT1_D_OUTPUT, "First D");
		configOutput(OUT2_A_OUTPUT, "Second A");
		configOutput(OUT2_B_OUTPUT, "Second B");
		configOutput(OUT2_INV_OUTPUT, "Second Inv");
	}

	void process(const ProcessArgs& args) override {

		const int numPolyphonyEngines1 = std::max(inputs[IN1_INPUT].getChannels(), 1);

		for (int c = 0; c < numPolyphonyEngines1; c += 4) {
			const float_4 in1 = inputs[IN1_INPUT].getPolyVoltageSimd<float_4>(c);

			outputs[OUT1_A_OUTPUT].setVoltageSimd(in1, c);
			outputs[OUT1_B_OUTPUT].setVoltageSimd(in1, c);
			outputs[OUT1_C_OUTPUT].setVoltageSimd(in1, c);
			outputs[OUT1_D_OUTPUT].setVoltageSimd(in1, c);

		}
		outputs[OUT1_A_OUTPUT].setChannels(numPolyphonyEngines1);
		outputs[OUT1_B_OUTPUT].setChannels(numPolyphonyEngines1);
		outputs[OUT1_C_OUTPUT].setChannels(numPolyphonyEngines1);
		outputs[OUT1_D_OUTPUT].setChannels(numPolyphonyEngines1);

		const float sampleTime = args.sampleTime;
		if (numPolyphonyEngines1 > 1) {
			lights[CROSS_LED + 0].setBrightness(0.f);
			lights[CROSS_LED + 1].setBrightness(0.f);
			lights[CROSS_LED + 2].setBrightness(1.0f);
		}
		else {
			// green positive, red negative
			const float in1_0 = inputs[IN1_INPUT].getVoltage() / 5.f;
			lights[CROSS_LED + 0].setBrightnessSmooth(in1_0 < 0.f ? -in1_0 : 0.f, sampleTime);
			lights[CROSS_LED + 1].setBrightnessSmooth(in1_0 > 0.f ? +in1_0 : 0.f, sampleTime);
			lights[CROSS_LED + 2].setBrightness(0.f);			
		}

		const int numPolyphonyEngines2 = inputs[IN2_INPUT].isConnected() ? std::max(inputs[IN2_INPUT].getChannels(), 1) : numPolyphonyEngines1;	
		for (int c = 0; c < numPolyphonyEngines2; c += 4) {

			const float_4 in2 = inputs[IN2_INPUT].getNormalPolyVoltageSimd<float_4>(inputs[IN1_INPUT].getPolyVoltageSimd<float_4>(c), c);

			outputs[OUT2_A_OUTPUT].setVoltageSimd(in2, c);
			outputs[OUT2_B_OUTPUT].setVoltageSimd(in2, c);
			outputs[OUT2_INV_OUTPUT].setVoltageSimd(-in2, c);
		}
		outputs[OUT2_A_OUTPUT].setChannels(numPolyphonyEngines2);
		outputs[OUT2_B_OUTPUT].setChannels(numPolyphonyEngines2);
		outputs[OUT2_INV_OUTPUT].setChannels(numPolyphonyEngines2);

		if (numPolyphonyEngines2 > 1) {
			lights[PHI_LED + 0].setBrightness(0.f);
			lights[PHI_LED + 1].setBrightness(0.f);
			lights[PHI_LED + 2].setBrightness(1.0f);
		}
		else {
			// green positive, red negative
			const float out2_inv = outputs[OUT2_INV_OUTPUT].getVoltage() / 5.f;
			lights[PHI_LED + 0].setBrightnessSmooth(out2_inv < 0.f ? -out2_inv : 0.f, sampleTime);
			lights[PHI_LED + 1].setBrightnessSmooth(out2_inv > 0.f ? +out2_inv : 0.f, sampleTime);
			lights[PHI_LED + 2].setBrightness(0.f); 
		}
	}


};

struct CrossLed : ModuloveLed {
	CrossLed() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/cross_led.svg")));
	}
};
struct PhiLed : ModuloveLed {
	PhiLed() {
		this->setSvg(Svg::load(asset::plugin(pluginInstance, "res/components/phi_led.svg")));
	}
};


struct MultvertWidget : ModuleWidget {
	MultvertWidget(Multvert* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/panels/Multvert.svg")));

		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.14, 15.366)), module, Multvert::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.08, 75.74)), module, Multvert::IN2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 27.25)), module, Multvert::OUT1_A_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 39.5)), module, Multvert::OUT1_B_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 51.75)), module, Multvert::OUT1_C_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 64.0)), module, Multvert::OUT1_D_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 88.052)), module, Multvert::OUT2_A_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 100.078)), module, Multvert::OUT2_B_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5.08, 112.155)), module, Multvert::OUT2_INV_OUTPUT));

		addChild(createLightCentered<CrossLed>(mm2px(Vec(5.08, 21.04)), module, Multvert::CROSS_LED));
		addChild(createLight<PhiLed>(mm2px(Vec(3.579, 116.5)), module, Multvert::PHI_LED));

	}
};


Model* modelMultvert = createModel<Multvert, MultvertWidget>("Multvert");