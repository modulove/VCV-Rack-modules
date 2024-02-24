#include "plugin.hpp"
#include <iostream>


struct MVMNT2 : Module {
	float phase = 0.0f;
	float blinkRate = 1.0f; // Blinking once per second

	int step = 0;
	int next_event = 0;
    // Timer and state for the LED
    float ledTimer = 0.0;
    bool ledState = false;


 	float last_value = 0.5; // Example starting voltage
    float target_value = 1.0; // Example initial target voltage, will be updated
    int next_target = 5000; // Steps to reach the target voltage, example initial value
    int initial_next_target = next_target; // Store the initial steps value to calculate t
    float level = 1.0; // Level for adjusting the final voltage output, example value
    float stretch = 1.0; // Just an example, replace with actual stretch parameter usage
    
    // Additional variables for the cubic Bezier calculation
    float P1_y = last_value; // y-value for P1 to ensure starting with a 0 slope
    float P2_y = target_value; // y-value for P2 to ensure ending with a 0 slope
    
	int displayNumber = 0; // The number to be displayed
    dsp::SchmittTrigger buttonTrigger; // For detecting button presses


	enum ParamId {
		KNOB1_PARAM,
		KNOB2_PARAM,
		KNOB3_PARAM,
		KNOB4_PARAM,
		BUTTON_PARAM,
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
	enum DisplayId {
        DISPLAY_NUMBER,
        DISPLAYS_LEN
    };

	MVMNT2() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOB1_PARAM, 0.f, 10.f, 5.f, "ELEVATE");
		configParam(KNOB2_PARAM, 0.1f, 10.f, 1.f, "STRETCH");
		configParam(KNOB3_PARAM, 0.f, 1.f, 0.f, "SMOOTH");
		configParam(KNOB4_PARAM, 0.f, 1.f, 0.f, "FLUCTUATE");
		configInput(IN1_INPUT, "FREEZE");
		configOutput(OUT1_OUTPUT, "INV");
		configOutput(OUT2_OUTPUT, "OUT");
		configOutput(OUT3_OUTPUT, "BI");

		void not_smooth_random(const ProcessArgs& args);
    	void smooth_random(const ProcessArgs& args);
	}
	void print(std::string s) {
		std::cout << s << std::endl;
	}

	void smooth_random(const ProcessArgs& args) {

		// INPUTS
		float level = params[KNOB1_PARAM].getValue();
		float stretch = params[KNOB2_PARAM].getValue();
		float smooth = params[KNOB3_PARAM].getValue();
		float fluctuate = params[KNOB4_PARAM].getValue();
		float input = inputs[IN1_INPUT].getVoltage();
		
		// LOGIC FOR NOT SMOOTH RANDOM
		if (next_target <= 0) {
			// Logic for setting a new target...
			next_target = random::uniform() * 100000.0 * stretch + 1000.0 + fluctuate * 10000.0;
			initial_next_target = next_target; // Reset initial steps to target with the new target
			target_value = random::uniform();
			
		
			float distance = target_value - last_value; 
			P1_y = last_value + (distance * smooth * 0.25); 
			P2_y = target_value - (distance * smooth * 0.25);
		}

		float t = 1.0f - (float)next_target / (float)initial_next_target;
    
		// Cubic Bezier calculation remains the same
		float B_t = pow(1 - t, 3) * last_value +
					3 * pow(1 - t, 2) * t * P1_y +
					3 * (1 - t) * pow(t, 2) * P2_y +
					pow(t, 3) * target_value;
		
		float next_value = B_t; // Calculate the next value using the Bezier curve
		next_target -= 1; // Decrement the target counter
		
		float new_voltage = level * next_value; // Apply the level to the calculated next value
		
		last_value = next_value; // Update the last_value for the next iteration

		
		// Debugging
		static int counter = 0;
		static int second_counter = 0;
		bool debug = false;
		if (debug && ++counter % 4410 == 0) { // Log once every second at 44.1kHz
			second_counter++;
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
		
		// LIGHT
        ledTimer -= args.sampleTime;
        if (ledTimer <= 0.0) {
            ledState = !ledState;
            ledTimer = (random::uniform() * 0.9f + 0.1f) * stretch; // Adjust range as needed
        }
        lights[BLINK_LIGHT].setBrightness(ledState ? 1.0f : 0.0f);
	
		// OUTPUTS
		outputs[OUT1_OUTPUT].setVoltage(10-new_voltage); // Inverse of output 2
		outputs[OUT2_OUTPUT].setVoltage(new_voltage); // Output between 0 and 10V
		outputs[OUT3_OUTPUT].setVoltage(2 * new_voltage - 10); // Output beween -10 and 10V
	}

	void not_smooth_random(const ProcessArgs& args) {
		// INPUTS
		float level = params[KNOB1_PARAM].getValue();
		float stretch = params[KNOB2_PARAM].getValue();
		float smooth = params[KNOB3_PARAM].getValue();
		float fluctuate = params[KNOB4_PARAM].getValue();
		float input = inputs[IN1_INPUT].getVoltage();

		// LOGIC NOT SMOOTH RANDOM
		float delta = (target_value - last_value) / next_target;
		float next_value = last_value + delta;
		next_target -= 1;
		if (next_target <= 0) {
			next_target = random::uniform() * 100000.0 * stretch + 1000.0;
			target_value = random::uniform();
		}
		float new_voltage = level * next_value;
		last_value = next_value;
		
		// Debugging
		bool debug = false;
		static int counter = 0;
		static int second_counter = 0;
		if (debug && ++counter % 4410 == 0) { // Log once every second at 44.1kHz
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
		
		// LIGHT
        ledTimer -= args.sampleTime;
        if (ledTimer <= 0.0) {
            ledState = !ledState;
            ledTimer = (random::uniform() * 0.9f + 0.1f) * stretch;
        }
        lights[BLINK_LIGHT].setBrightness(ledState ? 1.0f : 0.0f);

		// OUTPUTS
		outputs[OUT1_OUTPUT].setVoltage(10-new_voltage); // Inverse of output 2
		outputs[OUT2_OUTPUT].setVoltage(new_voltage); // Output between 0 and 10V
		outputs[OUT3_OUTPUT].setVoltage(2 * new_voltage - 10); // Output beween -10 and 10V
	}

	void process(const ProcessArgs& args) override {
		
		if(buttonTrigger.process(params[BUTTON_PARAM].getValue())) {
            displayNumber = (displayNumber + 1) % 2; // Increment and wrap around at 4
        }
		
		if (displayNumber == 0) {
			not_smooth_random(args);
		}
		else if (displayNumber == 1) {
			smooth_random(args);
		}
	}
};

struct NumberDisplayWidget : TransparentWidget {
    MVMNT2* module = nullptr; // Ensure this is defined
    std::shared_ptr<Font> font;

    NumberDisplayWidget() {
        box.size = Vec(50, 20); // Set the display size
    }

    void draw(const DrawArgs& args) override {
        if (!module) return;

        // Background (optional)
        nvgFillColor(args.vg, nvgRGB(0x30, 0x00, 0x00));
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFill(args.vg);

		nvgFontSize(args.vg, 18); // Set font size
		nvgFontFaceId(args.vg, 0);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGB(0xff, 0xff, 0xff)); // Set text color

		std::string toDisplay = std::to_string(module->displayNumber);
		nvgText(args.vg, box.size.x / 2, box.size.y / 2, toDisplay.c_str(), nullptr);
	}
};

struct MVMNT2Widget : ModuleWidget {
	MVMNT2Widget(MVMNT2* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/MVMNT.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.239, 26.298)), module, MVMNT2::KNOB1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.246, 44.665)), module, MVMNT2::KNOB2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.223, 63.085)), module, MVMNT2::KNOB3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.263, 81.506)), module, MVMNT2::KNOB4_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.665, 98.447)), module, MVMNT2::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.631, 98.54)), module, MVMNT2::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.836, 110.024)), module, MVMNT2::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.594, 110.022)), module, MVMNT2::OUT3_OUTPUT));

		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(15.245, 104.27)), module, MVMNT2::BLINK_LIGHT));
		addParam(createParamCentered<LEDButton>(mm2px(Vec(5, 125)), module, MVMNT2::BUTTON_PARAM));
	 	// Add custom display widget
        NumberDisplayWidget* displayWidget = createWidget<NumberDisplayWidget>(mm2px(Vec(15, 120)));
        displayWidget->module = module;
        addChild(displayWidget);
	}
};


Model* modelMVMNT2 = createModel<MVMNT2, MVMNT2Widget>("MVMNT2");