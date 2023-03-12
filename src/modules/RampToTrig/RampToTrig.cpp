#include "plugin.hpp"
#include "cmagslib.h"

struct RampToTrig : Module
{
	enum ParamId
	{
		PARAMS_LEN
	};
	enum InputId
	{
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		IN7_INPUT,
		IN8_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OU3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUT7_OUTPUT,
		TRIG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};

	cmags::RampToTrig rampToTrig1;
	cmags::RampToTrig rampToTrig2;
	cmags::RampToTrig rampToTrig3;
	cmags::RampToTrig rampToTrig4;
	cmags::RampToTrig rampToTrig5;
	cmags::RampToTrig rampToTrig6;
	cmags::RampToTrig rampToTrig7;
	cmags::RampToTrig rampToTrig8;
	
	RampToTrig()
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(IN1_INPUT, "");
		configInput(IN2_INPUT, "");
		configInput(IN3_INPUT, "");
		configInput(IN4_INPUT, "");
		configInput(IN5_INPUT, "");
		configInput(IN6_INPUT, "");
		configInput(IN7_INPUT, "");
		configInput(IN8_INPUT, "");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OU3_OUTPUT, "");
		configOutput(OUT4_OUTPUT, "");
		configOutput(OUT5_OUTPUT, "");
		configOutput(OUT6_OUTPUT, "");
		configOutput(OUT7_OUTPUT, "");
		configOutput(TRIG_OUTPUT, "");
	}

	void process(const ProcessArgs &args) override
	{
		outputs[0].setVoltage(rampToTrig1(inputs[0].getVoltage()));
		outputs[1].setVoltage(rampToTrig2(inputs[1].getVoltage()));
		outputs[2].setVoltage(rampToTrig3(inputs[2].getVoltage()));
		outputs[3].setVoltage(rampToTrig4(inputs[3].getVoltage()));
		outputs[4].setVoltage(rampToTrig5(inputs[4].getVoltage()));
		outputs[5].setVoltage(rampToTrig6(inputs[5].getVoltage()));
		outputs[6].setVoltage(rampToTrig7(inputs[6].getVoltage()));
		outputs[7].setVoltage(rampToTrig8(inputs[7].getVoltage()));
	}
};

struct RampToTrigWidget : ModuleWidget
{
	RampToTrigWidget(RampToTrig *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RampToTrig.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.854, 39.031)), module, RampToTrig::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.854, 50.672)), module, RampToTrig::IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.854, 61.785)), module, RampToTrig::IN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.854, 73.427)), module, RampToTrig::IN4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.854, 85.068)), module, RampToTrig::IN5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.854, 96.71)), module, RampToTrig::IN6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.854, 107.822)), module, RampToTrig::IN7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.854, 119.464)), module, RampToTrig::IN8_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 39.031)), module, RampToTrig::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 50.673)), module, RampToTrig::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 61.785)), module, RampToTrig::OU3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 73.427)), module, RampToTrig::OUT4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 85.069)), module, RampToTrig::OUT5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 96.71)), module, RampToTrig::OUT6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 107.823)), module, RampToTrig::OUT7_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.61, 119.465)), module, RampToTrig::TRIG_OUTPUT));
	}
};

Model *modelRampToTrig = createModel<RampToTrig, RampToTrigWidget>("RampToTrig");