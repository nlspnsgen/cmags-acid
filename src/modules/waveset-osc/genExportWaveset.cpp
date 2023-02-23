#include "genExportWaveset.h"
#include "AudioFile.h"

namespace wavesetOscGen
{

	/*******************************************************************************************************************
	Cycling '74 License for Max-Generated Code for Export
	Copyright (c) 2016 Cycling '74
	The code that Max generates automatically and that end users are capable of exporting and using, and any
	  associated documentation files (the “Software”) is a work of authorship for which Cycling '74 is the author
	  and owner for copyright purposes.  A license is hereby granted, free of charge, to any person obtaining a
	  copy of the Software (“Licensee”) to use, copy, modify, merge, publish, and distribute copies of the Software,
	  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
	The Software is licensed to Licensee only for non-commercial use. Users who wish to make commercial use of the
	  Software must contact the copyright owner to determine if a license for commercial use is available, and the
	  terms and conditions for same, which may include fees or royalties. For commercial use, please send inquiries
	  to licensing (at) cycling74.com.  The determination of whether a use is commercial use or non-commercial use is based
	  upon the use, not the user. The Software may be used by individuals, institutions, governments, corporations, or
	  other business whether for-profit or non-profit so long as the use itself is not a commercialization of the
	  materials or a use that generates or is intended to generate income, revenue, sales or profit.
	The above copyright notice and this license shall be included in all copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
	  THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
	  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
	  DEALINGS IN THE SOFTWARE.
	*******************************************************************************************************************/

	// global noise generator
	Noise noise;
	static const int GENLIB_LOOPCOUNT_BAIL = 100000;

	// The State struct contains all the state and procedures for the gendsp kernel
	typedef struct State
	{
		CommonState __commonstate;
		Data m_wavesets_1;
		Phasor __m_phasor_6;
		int __exception;
		int __loopcount;
		int vectorsize;
		t_sample m_init_2;
		t_sample samples_to_seconds;
		t_sample samplerate;
		AudioFile<double> audioFile;
		int numChannels;
		int audioFileSampleRate;
		int numSamples;
		std::string wavePath;
		// re-initialize all member variables;
		inline void reset(t_param __sr, int __vs)
		{
			__exception = 0;
			vectorsize = __vs;
			samplerate = __sr;
			loadSample();
			m_wavesets_1.reset("wavesets", ((int)44100), ((int)50));
			m_init_2 = ((int)1);
			samples_to_seconds = (1 / samplerate);
			__m_phasor_6.reset(0);
			genlib_reset_complete(this);
		};
		void loadSample()
		{
			audioFile.load(wavePath);
			audioFileSampleRate = audioFile.getSampleRate();
			numSamples = audioFile.getNumSamplesPerChannel();
			numChannels = audioFile.getNumChannels();
		};
		
		inline void setWavePath(std::string path){
			wavePath = path;
		}
		inline int getNumSamples() {
			return (int) numSamples;
		};
		// the signal processing routine;
		inline int perform(t_sample **__ins, t_sample **__outs, int __n)
		{
			vectorsize = __n;
			const t_sample *__in1 = __ins[0];
			const t_sample *__in2 = __ins[1];
			const t_sample *__in3 = __ins[2];
			t_sample *__out1 = __outs[0];
			if (__exception)
			{
				return __exception;
			}
			else if (((__in1 == 0) || (__in2 == 0) || (__in3 == 0) || (__out1 == 0)))
			{
				__exception = GENLIB_ERR_NULL_BUFFER;
				return __exception;
			};
			__loopcount = (__n * GENLIB_LOOPCOUNT_BAIL);
			samples_to_seconds = (1 / samplerate);
			int wavesets_dim = m_wavesets_1.dim;
			int wavesets_channels = m_wavesets_1.channels;
			// the main sample loop;
			while ((__n--))
			{
				const t_sample in1 = (*(__in1++));
				const t_sample in2 = (*(__in2++));
				const t_sample in3 = (*(__in3++));
				if (m_init_2)
				{
					m_init_2 = ((int)0);
					// for loop initializer;
					int i = ((int)0);
					// for loop condition;
					int wavesets_dim = m_wavesets_1.dim;
					int wavesets_channels = m_wavesets_1.channels;
					while ((i < wavesets_dim))
					{ // change this to actual sample size
						// abort processing if an infinite loop is suspected;
						if (((__loopcount--) <= 0))
						{
							__exception = GENLIB_ERR_LOOP_OVERFLOW;
							break;
						};

						for(int j = 0; j<numChannels; j++){
							t_sample value = audioFile.samples[j][i]; // Get value with index i at channel j.
							m_wavesets_1.write(value, i, j);
						}
						// for loop increment;
						i = (i + ((int)1));
					};
				};
				// abort processing if an infinite loop is suspected;
				if (((__loopcount--) <= 0))
				{
					__exception = GENLIB_ERR_LOOP_OVERFLOW;
					break;
				};
				t_sample phasor_45 = __m_phasor_6(in1, samples_to_seconds);
				t_sample floor_52 = floor(in3);
				t_sample mul_51 = (floor_52 * ((int)256));
				t_sample add_49 = (mul_51 + ((int)255));
				int wave_end_7 = ((wavesets_dim > int(add_49)) ? int(add_49) : wavesets_dim);
				int wave_begin_8 = ((0 > int(mul_51)) ? 0 : int(mul_51));
				int wave_offset_9;
				int wave_len_10;
				if ((wave_end_7 > wave_begin_8))
				{
					wave_offset_9 = wave_begin_8;
					wave_len_10 = (wave_end_7 - wave_begin_8);
				}
				else
				{
					wave_offset_9 = 0;
					wave_len_10 = wavesets_dim;
				};
				double sample_index_11 = (phasor_45 * (wave_len_10 - 1));
				int index_trunc_12 = fixnan(floor(sample_index_11));
				double index_fract_13 = (sample_index_11 - index_trunc_12);
				int index_trunc_14 = (index_trunc_12 - 1);
				int index_trunc_15 = (index_trunc_12 + 1);
				int index_trunc_16 = (index_trunc_12 + 2);
				bool index_ignore_17 = ((index_trunc_14 >= wave_len_10) || (index_trunc_14 < 0));
				int index_offset_18 = (index_trunc_14 + wave_offset_9);
				bool index_ignore_19 = ((index_trunc_12 >= wave_len_10) || (index_trunc_12 < 0));
				int index_offset_20 = (index_trunc_12 + wave_offset_9);
				bool index_ignore_21 = ((index_trunc_15 >= wave_len_10) || (index_trunc_15 < 0));
				int index_offset_22 = (index_trunc_15 + wave_offset_9);
				bool index_ignore_23 = ((index_trunc_16 >= wave_len_10) || (index_trunc_16 < 0));
				int index_offset_24 = (index_trunc_16 + wave_offset_9);
				// wave wavesets channel 1;
				int chan_25 = in2;
				bool chan_ignore_26 = ((chan_25 < 0) || (chan_25 >= wavesets_channels));
				double read_wavesets_27 = ((chan_ignore_26 || index_ignore_17) ? 0 : m_wavesets_1.read(index_offset_18, chan_25));
				double read_wavesets_28 = ((chan_ignore_26 || index_ignore_19) ? 0 : m_wavesets_1.read(index_offset_20, chan_25));
				double read_wavesets_29 = ((chan_ignore_26 || index_ignore_21) ? 0 : m_wavesets_1.read(index_offset_22, chan_25));
				double read_wavesets_30 = ((chan_ignore_26 || index_ignore_23) ? 0 : m_wavesets_1.read(index_offset_24, chan_25));
				double readinterp_31 = spline_interp(index_fract_13, read_wavesets_27, read_wavesets_28, read_wavesets_29, read_wavesets_30);
				t_sample sample_wavesets_55 = readinterp_31;
				t_sample index_wavesets_56 = sample_index_11;
				t_sample add_50 = (mul_51 + ((int)256));
				t_sample add_48 = (mul_51 + ((int)511));
				int wave_end_32 = ((wavesets_dim > int(add_48)) ? int(add_48) : wavesets_dim);
				int wave_begin_33 = ((0 > int(add_50)) ? 0 : int(add_50));
				int wave_offset_34;
				int wave_len_35;
				if ((wave_end_32 > wave_begin_33))
				{
					wave_offset_34 = wave_begin_33;
					wave_len_35 = (wave_end_32 - wave_begin_33);
				}
				else
				{
					wave_offset_34 = 0;
					wave_len_35 = wavesets_dim;
				};
				double sample_index_36 = (phasor_45 * (wave_len_35 - 1));
				int index_trunc_37 = fixnan(floor(sample_index_36));
				double index_fract_38 = (sample_index_36 - index_trunc_37);
				int index_trunc_39 = (index_trunc_37 - 1);
				int index_trunc_40 = (index_trunc_37 + 1);
				int index_trunc_41 = (index_trunc_37 + 2);
				bool index_ignore_42 = ((index_trunc_39 >= wave_len_35) || (index_trunc_39 < 0));
				int index_offset_43 = (index_trunc_39 + wave_offset_34);
				bool index_ignore_44 = ((index_trunc_37 >= wave_len_35) || (index_trunc_37 < 0));
				int index_offset_45 = (index_trunc_37 + wave_offset_34);
				bool index_ignore_46 = ((index_trunc_40 >= wave_len_35) || (index_trunc_40 < 0));
				int index_offset_47 = (index_trunc_40 + wave_offset_34);
				bool index_ignore_48 = ((index_trunc_41 >= wave_len_35) || (index_trunc_41 < 0));
				int index_offset_49 = (index_trunc_41 + wave_offset_34);
				// wave wavesets channel 1;
				int chan_50 = in2;
				bool chan_ignore_51 = ((chan_50 < 0) || (chan_50 >= wavesets_channels));
				double read_wavesets_52 = ((chan_ignore_51 || index_ignore_42) ? 0 : m_wavesets_1.read(index_offset_43, chan_50));
				double read_wavesets_53 = ((chan_ignore_51 || index_ignore_44) ? 0 : m_wavesets_1.read(index_offset_45, chan_50));
				double read_wavesets_54 = ((chan_ignore_51 || index_ignore_46) ? 0 : m_wavesets_1.read(index_offset_47, chan_50));
				double read_wavesets_55 = ((chan_ignore_51 || index_ignore_48) ? 0 : m_wavesets_1.read(index_offset_49, chan_50));
				double readinterp_56 = spline_interp(index_fract_38, read_wavesets_52, read_wavesets_53, read_wavesets_54, read_wavesets_55);
				t_sample sample_wavesets_53 = readinterp_56;
				t_sample index_wavesets_54 = sample_index_36;
				t_sample mix_79 = (sample_wavesets_55 + (((t_sample)0.5) * (sample_wavesets_53 - sample_wavesets_55)));
				t_sample out1 = mix_79;
				// assign results to output buffer;
				(*(__out1++)) = out1;
			};
			return __exception;
		};
		inline void set_wavesets(void *_value)
		{
			m_wavesets_1.setbuffer(_value);
		};

	} State;

	///
	///	Configuration for the genlib API
	///

	/// Number of signal inputs and outputs

	int gen_kernel_numins = 3;
	int gen_kernel_numouts = 1;

	int num_inputs() { return gen_kernel_numins; }
	int num_outputs() { return gen_kernel_numouts; }
	int num_params() { return 1; }

	/// Assistive lables for the signal inputs and outputs

	const char *gen_kernel_innames[] = {"frequency", "sel", "width"};
	const char *gen_kernel_outnames[] = {"sound"};

	/// Invoke the signal process of a State object

	int perform(CommonState *cself, t_sample **ins, long numins, t_sample **outs, long numouts, long n)
	{
		State *self = (State *)cself;
		return self->perform(ins, outs, n);
	}

	int getNumSamples(CommonState *cself){
		State *self = (State *)cself;
		return self->getNumSamples();
	}

	void setWavePath(CommonState *cself, std::string path){
		State *self = (State *)cself;
		return self->setWavePath(path);
	}
	/// Reset all parameters and stateful operators of a State object

	void reset(CommonState *cself)
	{
		State *self = (State *)cself;
		self->reset(cself->sr, cself->vs);
	}

	/// Set a parameter of a State object

	void setparameter(CommonState *cself, long index, t_param value, void *ref)
	{
		State *self = (State *)cself;
		switch (index)
		{
		case 0:
			self->set_wavesets(ref);
			break;

		default:
			break;
		}
	}

	/// Get the value of a parameter of a State object

	void getparameter(CommonState *cself, long index, t_param *value)
	{
		State *self = (State *)cself;
		switch (index)
		{

		default:
			break;
		}
	}

	/// Get the name of a parameter of a State object

	const char *getparametername(CommonState *cself, long index)
	{
		if (index >= 0 && index < cself->numparams)
		{
			return cself->params[index].name;
		}
		return 0;
	}

	/// Get the minimum value of a parameter of a State object

	t_param getparametermin(CommonState *cself, long index)
	{
		if (index >= 0 && index < cself->numparams)
		{
			return cself->params[index].outputmin;
		}
		return 0;
	}

	/// Get the maximum value of a parameter of a State object

	t_param getparametermax(CommonState *cself, long index)
	{
		if (index >= 0 && index < cself->numparams)
		{
			return cself->params[index].outputmax;
		}
		return 0;
	}

	/// Get parameter of a State object has a minimum and maximum value

	char getparameterhasminmax(CommonState *cself, long index)
	{
		if (index >= 0 && index < cself->numparams)
		{
			return cself->params[index].hasminmax;
		}
		return 0;
	}

	/// Get the units of a parameter of a State object

	const char *getparameterunits(CommonState *cself, long index)
	{
		if (index >= 0 && index < cself->numparams)
		{
			return cself->params[index].units;
		}
		return 0;
	}

	/// Get the size of the state of all parameters of a State object

	size_t getstatesize(CommonState *cself)
	{
		return genlib_getstatesize(cself, &getparameter);
	}

	/// Get the state of all parameters of a State object

	short getstate(CommonState *cself, char *state)
	{
		return genlib_getstate(cself, state, &getparameter);
	}

	/// set the state of all parameters of a State object

	short setstate(CommonState *cself, const char *state)
	{
		return genlib_setstate(cself, state, &setparameter);
	}

	/// Allocate and configure a new State object and it's internal CommonState:

	void *create(t_param sr, long vs)
	{
		State *self = new State;
		self->reset(sr, vs);
		ParamInfo *pi;
		self->__commonstate.inputnames = gen_kernel_innames;
		self->__commonstate.outputnames = gen_kernel_outnames;
		self->__commonstate.numins = gen_kernel_numins;
		self->__commonstate.numouts = gen_kernel_numouts;
		self->__commonstate.sr = sr;
		self->__commonstate.vs = vs;
		self->__commonstate.params = (ParamInfo *)genlib_sysmem_newptr(1 * sizeof(ParamInfo));
		self->__commonstate.numparams = 1;
		// initialize parameter 0 ("m_wavesets_1")
		pi = self->__commonstate.params + 0;
		pi->name = "wavesets";
		pi->paramtype = GENLIB_PARAMTYPE_SYM;
		pi->defaultvalue = 0.;
		pi->defaultref = 0;
		pi->hasinputminmax = false;
		pi->inputmin = 0;
		pi->inputmax = 1;
		pi->hasminmax = false;
		pi->outputmin = 0;
		pi->outputmax = 1;
		pi->exp = 0;
		pi->units = ""; // no units defined

		return self;
	}

	/// Release all resources and memory used by a State object:

	void destroy(CommonState *cself)
	{
		State *self = (State *)cself;
		genlib_sysmem_freeptr(cself->params);

		delete self;
	}

} // waveset2::
