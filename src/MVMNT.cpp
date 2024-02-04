#include "plugin.hpp"
#include <iostream>



struct MVMNT : Module {
	float phase = 0.0f;
	float blinkRate = 1.0f; // Blinking once per second

	int step = 0;
	int next_event = 0;
    // Timer and state for the LED
    float ledTimer = 0.0;
    bool ledState = false;
	
	enum ParamId {
		KNOB1_PARAM,
		KNOB2_PARAM,
		KNOB3_PARAM,
		KNOB4_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN1_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK_LIGHT,
		LIGHTS_LEN
	};

	MVMNT() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOB1_PARAM, 0.f, 10.f, 5.f, "ELEVATE");
		configParam(KNOB2_PARAM, 0.1f, 10.f, 1.f, "STRETCH");
		configParam(KNOB3_PARAM, 0.f, 1.f, 0.f, "SMOOTH");
		configParam(KNOB4_PARAM, 0.f, 1.f, 0.f, "FLUCTUATE");
		configInput(IN1_INPUT, "FREEZE");
		configOutput(OUT1_OUTPUT, "INV");
		configOutput(OUT2_OUTPUT, "OUT");
		configOutput(OUT3_OUTPUT, "BI");
	}

	void process(const ProcessArgs& args) override {
		// Compute the frequency from the pitch parameter and input
		float level = params[KNOB1_PARAM].getValue();
		float stretch = params[KNOB2_PARAM].getValue();
		float smooth = params[KNOB3_PARAM].getValue();
		float fluctuate = params[KNOB4_PARAM].getValue();
		float input = inputs[IN1_INPUT].getVoltage();

		// Compute the frequency from the pitch and input
		float new_value = 1;
		float new_voltage = level * new_value;

		static int counter = 0;
		static int second_counter = 0;
		if (++counter % 44100 == 0) { // Log once every second at 44.1kHz
			second_counter++;

        	std::cout << second_counter << " seconds passed" << std::endl;
			std::cout << "level: " << level << std::endl;
			std::cout << "stretch: " << stretch << std::endl;
			std::cout << "smooth: " << smooth << std::endl;
			std::cout << "fluctuate: " << fluctuate << std::endl;
			std::cout << "input: " << input << std::endl;
			std::cout << "sampletime: " << args.sampleTime << std::endl;
			
		}
		
		// Decrement the timer
        ledTimer -= args.sampleTime;

        // Check if it's time to toggle the LED state
        if (ledTimer <= 0.0) {
            // Toggle the LED state
            ledState = !ledState;

            // Reset the timer with a new random duration between 0.1 and 1 seconds
            ledTimer = (random::uniform() * 0.9f + 0.1f) * stretch; // Adjust range as needed
        }

        // Update the LED brightness based on the ledState
        lights[BLINK_LIGHT].setBrightness(ledState ? 1.0f : 0.0f);


		
		// OUTPUTS
		outputs[OUT1_OUTPUT].setVoltage(10-new_voltage); // Inverse of output 2
		outputs[OUT2_OUTPUT].setVoltage(new_voltage); // Output between 0 and 10V
		outputs[OUT3_OUTPUT].setVoltage(2 * new_voltage - 10); // Output beween -10 and 10V
	}
};


struct MVMNTWidget : ModuleWidget {
	MVMNTWidget(MVMNT* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/MVMNT.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.239, 26.298)), module, MVMNT::KNOB1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.246, 44.665)), module, MVMNT::KNOB2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.223, 63.085)), module, MVMNT::KNOB3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.263, 81.506)), module, MVMNT::KNOB4_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.665, 98.447)), module, MVMNT::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.631, 98.54)), module, MVMNT::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.836, 110.024)), module, MVMNT::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.594, 110.022)), module, MVMNT::OUT3_OUTPUT));

		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(15.245, 104.27)), module, MVMNT::BLINK_LIGHT));
	}
};


Model* modelMVMNT = createModel<MVMNT, MVMNTWidget>("MVMNT");