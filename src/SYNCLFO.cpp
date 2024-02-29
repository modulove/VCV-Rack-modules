#include "plugin.hpp"
#include <iostream>



struct SYNCLFO : Module {
	float phase = 0.0f;
	float blinkRate = 1.0f; // Blinking once per second

	int step = 0;
	int next_event = 0;
    // Timer and state for the LED
    float ledTimer = 0.0;
    bool ledState = false;


	float last_value = 0.5;
	int next_target = 5000;
	float target_value = 1.0;
	
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

	SYNCLFO() {
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
	void print(std::string s) {
		std::cout << s << std::endl;
	}

	void process(const ProcessArgs& args) override {
		// Compute the frequency from the pitch parameter and input
		float level = params[KNOB1_PARAM].getValue();
		float stretch = params[KNOB2_PARAM].getValue();
		float smooth = params[KNOB3_PARAM].getValue();
		float fluctuate = params[KNOB4_PARAM].getValue();
		float input = inputs[IN1_INPUT].getVoltage();

		// Intelligenz

		float delta = (target_value - last_value) / next_target;
		float next_value = last_value + delta;
		next_target -= 1;
		if (next_target <= 0) {
			print("new target!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			next_target = random::uniform() * 100000.0 * stretch + 1000.0;
			print("new target: " + std::to_string(next_target));
			target_value = random::uniform();
			print("new target value: " + std::to_string(target_value));
		}
		float new_voltage = level * next_value;
		


		static int counter = 0;
		static int second_counter = 0;
		if (++counter % 4410 == 0) { // Log once every second at 44.1kHz
			second_counter++;

			std::cout << "delta: " << delta << std::endl;
			std::cout << "next_value" << next_value << std::endl;
			std::cout << "next_target" << next_target << std::endl;
			std::cout << "target_value" << target_value << std::endl;
        	std::cout << "new_voltage" << new_voltage << std::endl;
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

		last_value = next_value;
	}
};


struct SYNCLFOWidget : ModuleWidget {
	SYNCLFOWidget(SYNCLFO* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SYNCLFO.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.239, 26.298)), module, SYNCLFO::KNOB1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.246, 44.665)), module, SYNCLFO::KNOB2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.223, 63.085)), module, SYNCLFO::KNOB3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.263, 81.506)), module, SYNCLFO::KNOB4_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.665, 98.447)), module, SYNCLFO::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.631, 98.54)), module, SYNCLFO::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.836, 110.024)), module, SYNCLFO::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.594, 110.022)), module, SYNCLFO::OUT3_OUTPUT));

		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(15.245, 104.27)), module, SYNCLFO::BLINK_LIGHT));
	}
};


Model* modelSYNCLFO = createModel<SYNCLFO, SYNCLFOWidget>("SYNCLFO");
