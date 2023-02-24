#include "plugin.hpp"
#include <math.h>  

struct SuperSaw : Module
{
	float left = 0;
	float right = 0;
	float accums[16];
	enum ParamId
	{
		SPREAD_PARAM,
		VOICES_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		VOCT_INPUT,
		VOICES_INPUT,
		SPREAD_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		OUTPUT_LEFT,
		OUTPUT_RIGHT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};

	SuperSaw()
	{
		for (int i = 0; i < 6; i++)
		{
			accums[i] = 0;
		}
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SPREAD_PARAM, 0.f, 1.f, 0.f, "spread");
		configParam(VOICES_PARAM, 0.1f, 15.9f, 0.f, "voices");
		configInput(SPREAD_INPUT, "spread");
		configInput(VOICES_INPUT, "voices");
		configInput(VOCT_INPUT, "v/oct");
		configOutput(OUTPUT_LEFT, "out");
		configOutput(OUTPUT_RIGHT, "out");
	}
	
	float convertRange(float oldValue, float oldMin, float oldMax, float newMin, float newMax)
	{
		return (((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin)) + newMin;
	}

	float accumulate(float inc, float max, int i)
	{
		accums[i] = accums[i] + inc;
		if (accums[i] >= max)
			accums[i] = 0;
		return accums[i];
	}

	// anti-aliasing sawtooth oscillator
	float sawUp(const ProcessArgs &args, float fc, int accumsIndex) // frequency
	{
		float srx3 = args.sampleRate * 3;				// samplerate * 3
		float sr3d3 = std::pow(args.sampleRate, 3) / 3; // samplerate^3 /3
		float rsr = args.sampleTime;					// 1/samplerate
		float inc = fc * rsr;							// fc * rsr
		float r1 = accumulate(inc, 1, accumsIndex);		// accum(inc, max = 1)
		float D;
		if (r1 < inc)
		{
			return pow(r1 / fc, 3) * sr3d3 + r1 * fc * rsr + 1;
		}
		else if (r1 < inc * 2)
		{
			D = r1 * args.sampleRate / fc;
			return D * D * D * .66666666666666666667 - D * D * 3 + r1 * srx3 / fc + r1 * 2;
		}
		else if (r1 < inc * 3)
		{
			D = r1 * args.sampleRate / fc;
			return D * D * D * -.33333333333333333333 + D * D * 3 + D * inc * 2 - D * 9 + 8;
		}
		return r1 * 2 - 1;
	}
	void process(const ProcessArgs &args) override
	{
		float voices = (int) ceil(params[VOICES_PARAM].getValue());
		if(inputs[1].isConnected()){
			voices = (int) ceil(convertRange(inputs[1].getVoltage(), -10, 10, 0.1, 15.9));
		}
		float spread = params[SPREAD_PARAM].getValue();
		spread += inputs[2].isConnected() ? inputs[2].getVoltage() : 0.f;
		// Compute the frequency from the pitch parameter and input
		float pitch = inputs[0].getVoltage();
		pitch = clamp(pitch, -4.f, 4.f);
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

		for (int i = 0; i <= 6; i++)
		{
			left += sawUp(args, (freq + spread * i), i);
			right += sawUp(args, (freq + spread * i), i);
		}
		outputs[0].setVoltage(left);
		outputs[1].setVoltage(right);
	}
};

struct SuperSawWidget : ModuleWidget
{
	SuperSawWidget(SuperSaw *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SuperSaw.svg")));
		addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(15.542, 24.112)), module, SuperSaw::SPREAD_PARAM));
		addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(15.542, 65.625)), module, SuperSaw::VOICES_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.828, 44.536)), module, SuperSaw::SPREAD_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.828, 85.882)), module, SuperSaw::VOICES_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(4.31, 121.349)), module, SuperSaw::VOCT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(26.422, 100.232)), module, SuperSaw::OUTPUT_LEFT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(26.422, 121.232)), module, SuperSaw::OUTPUT_RIGHT));
		
	}
};

Model *modelSuperSaw = createModel<SuperSaw, SuperSawWidget>("SuperSaw");