#include "plugin.hpp"
#include "cmagslib.h"
#include <string>

struct Clock : Module
{
	enum ParamId
	{
		BPM_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		RESET_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		RAMP_OUTPUT,
		TRIG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};
	double bpm = 127;
	double currentPhase;
	char clockText[24] = "127";
	cmags::Phasor mainPhasor;
	cmags::RampToTrig rampToTrig;
	std::string clockString = "";

	Clock()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BPM_PARAM, 1.f, 200.f, 127.f, "bpm", "bpm");
		configInput(RESET_INPUT, "reset");
		configOutput(RAMP_OUTPUT, "ramp out");
		configOutput(TRIG_OUTPUT, "trig out");
	}

	std::string double_to_string(double x)
	{
		std::ostringstream ss;
		ss << x;
		return ss.str();
	}

	void process(const ProcessArgs &args) override
	{
		if (bpm != params[BPM_PARAM].getValue())
		{
			bpm = params[BPM_PARAM].getValue();
			clockString = double_to_string(params[BPM_PARAM].getValue());
			std::strcpy(clockText, clockString.c_str());
		}
		if(inputs[0].isConnected() && inputs[0].getVoltage() > 0.1){
			mainPhasor.reset(0);
			rampToTrig.reset(0);
		}
		// hertz = beats per minute × 0.02
		currentPhase = mainPhasor(bpm * 0.02, args.sampleTime);
		outputs[0].setVoltage(currentPhase);
		outputs[1].setVoltage(rampToTrig(currentPhase));
	}
};

struct BPMDisplay : TransparentWidget
{
	Clock *module;
	BPMDisplay()
	{
	}
	void drawLayer(const DrawArgs &args, int layer)
	{
		nvgFillColor(args.vg, nvgRGBA(0xc00, 0x00, 0x00, 0xff));
		if (module)
		{
			nvgText(args.vg, 18, 100, module->clockText, NULL);
		}
	}
};

struct ClockWidget : ModuleWidget
{
	ClockWidget(Clock *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Clock.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		BPMDisplay *display = new BPMDisplay();
		display->box.pos = Vec(0, 0);
		display->box.size = Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
		display->module = module;
		addChild(display);

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.916, 45.578)), module, Clock::BPM_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.854, 119.464)), module, Clock::RESET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 106.235)), module, Clock::RAMP_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 119.465)), module, Clock::TRIG_OUTPUT));
	}
};

Model *modelClock = createModel<Clock, ClockWidget>("Clock");