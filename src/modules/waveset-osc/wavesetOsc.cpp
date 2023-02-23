#include "plugin.hpp"
#include "genExportWaveset.h"
#include <array>
#include <string>
using namespace wavesetOscGen;

struct WavesetOsc : Module
{
	enum ParamId
	{
		PARAM_SELECT_PARAM,
		PARAM_WIDTH_PARAM,
		PARAMS_LEN
	};
	enum InputId
	{
		IN_VOCT_INPUT,
		IN_SELECT_INPUT,
		IN_WIDTH_INPUT,
		INPUTS_LEN
	};
	enum OutputId
	{
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId
	{
		LIGHTS_LEN
	};

	CommonState *moduleState;
	t_sample **inputBuffers;
	t_sample **outputBuffers;

	std::array<int, 10> validBufferSizes = {1, 4, 16, 32, 64, 128, 256, 512, 1024, 2048};
	int currentBufferSize = 256;
	int currentSampleRate = 44100;

	int numParams;
	int numInputs;
	int numOutputs;
	int counter = 0;
	int count = 0;
	float pitch = 0.5;
	float frequency;

	WavesetOsc()
	{
		moduleState = (CommonState *)create(currentSampleRate, currentBufferSize);
		setWavePath(moduleState, (std::string)asset::plugin(pluginInstance, "res/wavesets.wav"));
		reset(moduleState);
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PARAM_SELECT_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM_WIDTH_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_SELECT_INPUT, "");
		configInput(IN_WIDTH_INPUT, "");
		configInput(IN_VOCT_INPUT, "");
		configOutput(OUT_OUTPUT, "");
		numParams = 2;
		numInputs = 3;
		numOutputs = 1;

		// Initialize sample buffers
		inputBuffers = new t_sample *[numInputs];
		for (int i = 0; i < numInputs; i++)
		{
			inputBuffers[i] = new t_sample[currentBufferSize];
		}

		outputBuffers = new t_sample *[numOutputs];
		for (int i = 0; i < numOutputs; i++)
		{
			outputBuffers[i] = new t_sample[currentBufferSize];
		}
	}
	~WavesetOsc()
	{
		destroy(moduleState);
		deleteBuffers();
	}

	float convertRange(float oldValue, float oldMin, float oldMax, float newMin, float newMax)
	{
		return (((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin)) + newMin;
	}

	void deleteBuffers()
	{
		for (int i = 0; i < numInputs; i++)
		{
			if (inputBuffers[i])
			{
				delete inputBuffers[i];
			}
		}

		for (int i = 0; i < numOutputs; i++)
		{
			if (outputBuffers[i])
			{
				delete outputBuffers[i];
			}
		}
	}
	void setBufferSize(long bufferSize)
	{
		if (bufferSize > currentBufferSize)
		{
			for (int i = 0; i < numInputs; i++)
			{
				if (inputBuffers[i])
				{
					delete inputBuffers[i];
				}
				inputBuffers[i] = new t_sample[bufferSize];
			}

			for (int i = 0; i < numOutputs; i++)
			{
				if (outputBuffers[i])
				{
					delete outputBuffers[i];
				}
				outputBuffers[i] = new t_sample[bufferSize];
			}
		}
		currentBufferSize = bufferSize;
		moduleState->vs = currentBufferSize;
		count = 0;
	}

	void process(const ProcessArgs &args) override
	{
		if (args.sampleRate != currentSampleRate)
		{
			moduleState->sr = args.sampleRate;
			currentSampleRate = args.sampleRate;
		}

		if (count >= currentBufferSize)
		{
			count = 0;
		}

		// Fill inputs
		for (int i = 0; i < numInputs; i++)
		{
			if (inputs[i].isConnected())
			{
				// frequency
				if (i == 0)
				{
					pitch = inputs[0].getVoltage();
					pitch = clamp(pitch, -4.f, 4.f);
					frequency = dsp::FREQ_C4 * std::pow(2.f, pitch);
					inputBuffers[0][count] = frequency;
				}
				if (i == 1)
				{
					inputBuffers[1][count] = convertRange(inputs[1].getVoltage(), 0.f, 10.f, 1.f, 47);
				}
				if (i == 2)
				{
					inputBuffers[2][count] = convertRange(inputs[2].getVoltage(), 0.f, 10.f, 0.f, 126);
				}
			}
			else
			{
				if (i == 0) // frequency
				{
					inputBuffers[0][count] = 440;
				}
				if (i == 1)
				{
					inputBuffers[1][count] = convertRange(params[PARAM_SELECT_PARAM].getValue(), 0, 1, 1, 47);
				}
				if (i == 2)
				{
					inputBuffers[2][count] = convertRange(params[PARAM_WIDTH_PARAM].getValue(), 0, 1, 1, 126);
				}
			}
		}

		// Set output
		for (int i = 0; i < numOutputs; i++)
		{
			outputs[i].setVoltage(outputBuffers[i][count] * 5.f);
		}

		// Step forward
		count++;

		// Perform when we've filled the buffer
		if (count == currentBufferSize)
		{

			// Update any parameters
			for (int i = 0; i < numParams; i++)
			{
				// Get VCV inputs
				float knobVal = params[i].getValue();																// Already scaled to range that genlib will understand
				float cvVal = inputs[i + numInputs].isConnected() ? inputs[i + numInputs].getVoltage() / 5.f : 0.f; // Normalize to -1..1

				// Scale to range of parameter
				t_param min = getparametermin(moduleState, i);
				t_param max = getparametermax(moduleState, i);
				t_param range = fabs(max - min);
				t_param val = clamp(knobVal + cvVal * range, min, max); // Offset the knobVal by the CV input
				setparameter(moduleState, i, val, NULL);
			}

			// Fill the buffers
			perform(moduleState, inputBuffers, numInputs, outputBuffers, numOutputs, currentBufferSize);
			counter++;
		}
	}
};

struct WavesetOscWidget : ModuleWidget
{
	WavesetOscWidget(WavesetOsc *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/wavesetOsc.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(22.925, 29.737)), module, WavesetOsc::PARAM_SELECT_PARAM));
		addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(22.925, 74.801)), module, WavesetOsc::PARAM_WIDTH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.056, 111.153)), module, WavesetOsc::IN_VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.859, 49.976)), module, WavesetOsc::IN_SELECT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.859, 95.04)), module, WavesetOsc::IN_WIDTH_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.793, 111.153)), module, WavesetOsc::OUT_OUTPUT));
	}
};

Model *modelWavesetOsc = createModel<WavesetOsc, WavesetOscWidget>("wavesetOsc");