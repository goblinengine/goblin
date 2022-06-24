/* 
Copyright (c) 2021 Filip Anton (filipworks) 
Created for Goblin Engine github.com/goblinengine
Initial implementation based on https://github.com/RodZill4/godot-music

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "midi_player.h"

#include "core/bind/core_bind.h"
#define TSF_IMPLEMENTATION
#include "../libs/tsf.h"
#define TML_IMPLEMENTATION
#include "../libs/tml.h"

// MIDIFILE
Error MidiFile::load(const String fileName) {
	String ext = fileName.get_extension().to_lower();
	if(!(ext == "sf2" || ext == "mid" || ext == "midi")) {
		ERR_FAIL_COND_V("Incorrect file type", FAILED);
	}

	FileAccess *f = FileAccess::open(fileName, FileAccess::READ);
	ERR_FAIL_COND_V_MSG(!f, FAILED, "Couldn't open file " + fileName + ".");

	int size = f->get_len();

	PoolVector<uint8_t> theData;
	theData.resize(size);

	PoolVector<uint8_t>::Write w = theData.write();
	int theReadSize = f->get_buffer(&w[0], size);

	memdelete(f);

	if (theReadSize < size) { 
		theData.resize(size);
	}
	ERR_FAIL_COND_V_MSG(theReadSize == 0, FAILED, "Could't read file.");

	set_data(theData);

	if (fileName.get_extension().to_lower() == "sf2") {
		set_format(FORMAT_SF2);
	} else {
		set_format(FORMAT_MIDI);
	}

	return OK;
}

void MidiFile::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load", "file_path"), &MidiFile::load);

	ClassDB::bind_method(D_METHOD("set_data", "data"), &MidiFile::set_data);
	ClassDB::bind_method(D_METHOD("get_data"), &MidiFile::get_data);
	ADD_PROPERTY(PropertyInfo(Variant::POOL_BYTE_ARRAY, "data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_data", "get_data");

	ClassDB::bind_method(D_METHOD("set_format", "format"), &MidiFile::set_format);
	ClassDB::bind_method(D_METHOD("get_format"), &MidiFile::get_format);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "format"), "set_format", "get_format");

	BIND_ENUM_CONSTANT(FORMAT_MIDI);
	BIND_ENUM_CONSTANT(FORMAT_SF2);
}

PoolByteArray MidiFile::get_data() const {
	PoolVector<uint8_t> vdata;

	if (data_len && data) {
		vdata.resize(data_len);
		{
			PoolVector<uint8_t>::Write w = vdata.write();
			memcpy(w.ptr(), data, data_len);
		}
	}

	return vdata;
}

void MidiFile::set_data(const PoolVector<uint8_t> &dataIn) {
	data_len = dataIn.size();
	data = memalloc(data_len);
	memcpy(data, dataIn.read().ptr(), data_len);
}

// MIDIFILE IMPORTER
void ResourceImporterMidiFile::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("sf2");
	p_extensions->push_back("midi");
	p_extensions->push_back("mid");
}

Error ResourceImporterMidiFile::import(const String &p_source_file, const String &p_save_path, const Map<StringName, Variant> &p_options, List<String> *r_platform_variants, List<String> *r_gen_files, Variant *r_metadata) {
	Ref<MidiFile> mdf;
	mdf.instance();
	mdf->load(p_source_file);

	return ResourceSaver::save(p_save_path + ".mdf", mdf);

	return OK;
}

// MIDIPLAYER
MidiPlayer::MidiPlayer(): mTsf(NULL), midi_speed(1.0) {
	mTsf = NULL;
	//mSoundFontName = "";
	mTml = NULL;
	//mMidiName = "";
	midiCurrent = NULL;
	looping = true;
}

// convenience load + set
void MidiPlayer::load_soundfont(String inSoundFontName) {
	Ref<MidiFile> sf = ResourceLoader::load(inSoundFontName);
	ERR_FAIL_COND_MSG(sf == nullptr, "Couldn't open soundfont " + inSoundFontName + ".");

	set_soundfont(sf);
}

void MidiPlayer::set_soundfont(Ref<MidiFile> sf) {
	soundfont = sf;
	if (sf == nullptr) return;

	if (sf->format < 1) {
		print_error("Resource is not a SoundFont");
		return;
	}

	// Clear the old sound font
	if (mTsf != NULL) {
	    note_off_all();
		tsf_close(mTsf);
		mTsf = NULL;
	}
	
	if (soundfont != nullptr) {
		//load data in memory
		int data_len = soundfont->get_data().size();
		PoolVector<uint8_t> dataIn = soundfont->get_data();
		void * dataOut = memalloc(data_len);
		memcpy(dataOut, dataIn.read().ptr(), data_len);
		//load tiny sound font using memory data
		mTsf = tsf_load_memory(dataOut, data_len);
	}
}


Ref<MidiFile> MidiPlayer::get_soundfont() const {
	return soundfont;
}

// convenience load + set
void MidiPlayer::load_midi(String inMidiFileName) {
	// Open a new midi file
	//midiFile = FileAccess::open(mMidiName, FileAccess::READ);
	Ref<MidiFile> mid = ResourceLoader::load(inMidiFileName);
	ERR_FAIL_COND_MSG(mid == nullptr, "Couldn't open " + inMidiFileName + ".");

	set_midi(mid);
}

void MidiPlayer::set_midi(Ref<MidiFile> mid) {
	midi = mid;
	if (midi == nullptr) return;
	
	if (midi->format != 0) {
		print_error("Resource not a Midi file.");
		return;
	}

	// Clear the current midi
	if (mTml != NULL) {
	    note_off_all();
		tml_free(mTml);
		mTml = NULL;
		midiCurrent = NULL;
	}

	// Load the new midi and set parameters
	if (midi != nullptr) {
		//load data in memory
		int data_len = midi->get_data().size();
		PoolVector<uint8_t> dataIn = midi->get_data();
		void * dataOut = memalloc(data_len);
		memcpy(dataOut, dataIn.read().ptr(), data_len);
		//load tiny midy loader using memory data
		mTml = tml_load_memory(dataOut, data_len);

		//set midi to start at the beginning
		midiCurrent = mTml;
		midiTime = 0.0;
	}
}


Ref<MidiFile> MidiPlayer::get_midi() const {
	return midi;
}

PoolStringArray MidiPlayer::get_preset_names() const {
	PoolStringArray theReturnValue;

	if (mTsf != NULL) {
		unsigned theIndex, theEnd;

		for (theIndex = 0, theEnd = tsf_get_presetcount(mTsf); theIndex < theEnd; ++theIndex) {
			theReturnValue.append(tsf_get_presetname(mTsf, theIndex));
		}
	}
	return theReturnValue;
}

int MidiPlayer::get_preset_index(int inBank, int inPresetNumber) {
	if (mTsf == NULL) {
		return -1;
	}
	return tsf_get_presetindex(mTsf, inBank, inPresetNumber);
}

void MidiPlayer::note_on(int inPresetIndex, int inKey, float inVelocity) {
	if (mTsf == NULL) {
		return;
	}
	if (inVelocity > 0.0) {
		tsf_note_on(mTsf, inPresetIndex, inKey, inVelocity);
	} else {
		tsf_note_off(mTsf, inPresetIndex, inKey);
	}
}

void MidiPlayer::note_off(int inPresetIndex, int inKey) {
	if (mTsf == NULL) {
		return;
	}
	tsf_note_off(mTsf, inPresetIndex, inKey);
	//tsf_note_on(mTsf, inPresetIndex, inKey, 0.0);
}

void MidiPlayer::note_off_all() {
	if (mTsf == NULL) {
		return;
	}
	tsf_note_off_all(mTsf);
}

void MidiPlayer::channel_set_preset_index(int inChannel, int inPresetIndex) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_set_presetindex(mTsf, inChannel, inPresetIndex);
}


int MidiPlayer::channel_set_preset_number(int inChannel, int inPresetNumber, int inDrums) {
	if (mTsf == NULL) {
		return -1;
	}
	return tsf_channel_set_presetnumber(mTsf, inChannel, inPresetNumber, inDrums);
}


void MidiPlayer::channel_set_bank(int inChannel, int inBank) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_set_bank(mTsf, inChannel, inBank);
}


int MidiPlayer::channel_set_bank_preset(int inChannel, int inBank, int inPresetNumber) {
	if (mTsf == NULL) {
		return -1;
	}
	return tsf_channel_set_bank_preset(mTsf, inChannel, inBank, inPresetNumber);
}


void MidiPlayer::channel_set_pan(int inChannel, float inPan) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_set_pan(mTsf, inChannel, inPan);
}

void MidiPlayer::channel_set_volume(int inChannel, float inVolume) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_set_volume(mTsf, inChannel, inVolume);
}

void MidiPlayer::channel_set_pitchwheel(int inChannel, int inPitchWheel) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_set_pitchwheel(mTsf, inChannel, inPitchWheel);
}

void MidiPlayer::channel_set_pitchrange(int inChannel, float inPitchRange) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_set_pitchrange(mTsf, inChannel, inPitchRange);
}

void MidiPlayer::channel_set_tuning(int inChannel, float inTuning) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_set_tuning(mTsf, inChannel, inTuning);
}

void MidiPlayer::channel_note_on(int inChannel, int inKey, float inVelocity) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_note_on(mTsf, inChannel, inKey, inVelocity);
}

void MidiPlayer::channel_note_off(int inChannel, int inKey) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_note_off(mTsf, inChannel, inKey);
}

void MidiPlayer::channel_note_off_all(int inChannel) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_note_off_all(mTsf, inChannel);
}

// end with sustain and release
void MidiPlayer::channel_sounds_off_all(int inChannel) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_sounds_off_all(mTsf, inChannel);
}

// end immediatly
void MidiPlayer::channel_midi_control(int inChannel, int inController, int inValue) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_midi_control(mTsf, inChannel, inController, inValue);
}

int MidiPlayer::channel_get_preset_index(int inChannel) {
	if (mTsf == NULL) {
		return -1;
	}
	return tsf_channel_get_preset_index(mTsf, inChannel);
}

int MidiPlayer::channel_get_preset_bank(int inChannel) {
	if (mTsf == NULL) {
		return -1;
	}
	return tsf_channel_get_preset_bank(mTsf, inChannel);
}

int MidiPlayer::channel_get_preset_number(int inChannel) {
	if (mTsf == NULL) {
		return -1;
	}
	return tsf_channel_get_preset_number(mTsf, inChannel);
}


float MidiPlayer::channel_get_pan(int inChannel) {
	if (mTsf == NULL) {
		return 0.0;
	}
	return tsf_channel_get_pan(mTsf, inChannel);
}

float MidiPlayer::channel_get_volume(int inChannel) {
	if (mTsf == NULL) {
		return 0.0;
	}
	return tsf_channel_get_volume(mTsf, inChannel);
}


int MidiPlayer::channel_get_pitchwheel(int inChannel) {
	if (mTsf == NULL) {
		return 0;
	}
	return tsf_channel_get_pitchwheel(mTsf, inChannel);
}

float MidiPlayer::channel_get_pitchrange(int inChannel) {
	if (mTsf == NULL) {
		return 0.0;
	}
	return tsf_channel_get_pitchrange(mTsf, inChannel);
}

float MidiPlayer::channel_get_tuning(int inChannel) {
	if (mTsf == NULL) {
		return 0.0;
	}
	return tsf_channel_get_tuning(mTsf, inChannel);
}

PoolVector2Array MidiPlayer::get_buffer(int inSize) {
	PoolVector2Array theBuffer;
	
	if (inSize > 0 && mTsf != NULL) {
		theBuffer.resize(inSize);
		Vector2 *theData = theBuffer.write().ptr();
 
		// if there is a midi loaded play the midi
		if (midiCurrent != NULL) {
			int SampleBlock;
			int SampleCount = inSize;
			for (SampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK; SampleCount; SampleCount -= SampleBlock, theData += SampleBlock) {
				// We progress the MIDI playback and then process TSF_RENDER_EFFECTSAMPLEBLOCK samples at once
				if (SampleBlock > SampleCount) SampleBlock = SampleCount;

				// Loop through all MIDI messages which need to be played up until the current playback time
				for (midiTime += SampleBlock * midi_speed * (1000.0 / 44100.0); midiCurrent && midiTime >= midiCurrent->time; midiCurrent = midiCurrent->next) {
					switch (midiCurrent->type) {
						case TML_PROGRAM_CHANGE: //channel program (preset) change (special handling for 10th MIDI channel with drums)
							tsf_channel_set_presetnumber(mTsf, midiCurrent->channel, midiCurrent->program, (midiCurrent->channel == 9));
							break;
						case TML_NOTE_ON: //play a note
							tsf_channel_note_on(mTsf, midiCurrent->channel, midiCurrent->key, midiCurrent->velocity / 127.0f);
							break;
						case TML_NOTE_OFF: //stop a note
							tsf_channel_note_off(mTsf, midiCurrent->channel, midiCurrent->key);
							break;
						case TML_PITCH_BEND: //pitch wheel modification
							tsf_channel_set_pitchwheel(mTsf, midiCurrent->channel, midiCurrent->pitch_bend);
							break;
						case TML_CONTROL_CHANGE: //MIDI controller messages
							tsf_channel_midi_control(mTsf, midiCurrent->channel, midiCurrent->control, midiCurrent->control_value);
							break;
					}
				}
				// Render the block of audio samples in float format
				// play the soundfont samples from note_on fuctions and from midi file
				tsf_render_float(mTsf, (float*)theData, SampleBlock, 0);
			}
		} else {
			// otherwise if there is a midi loaded but finished then loop if looping or else stop
			if (mTml) {
				if (looping == true) {
					note_off_all();
					midiCurrent = mTml;
					midiTime = 0.0;	
					// If looping, emit loop_finished instead of finished
					emit_signal("loop_finished");
				} else {
					stop();
					// stop() automatically emits finished but this will likely change in the future
					// see https://github.com/godotengine/godot/pull/33602
					// commmenting this out for now until above change is merged
					// emit_signal("finished");
				}	
			}

			// play the soundfont samples even without midi loaded
			tsf_render_float(mTsf, (float*)theData, inSize, 0);
		}
	}
	return theBuffer;
}

// handle ready and interan process which feeds the buffer
void MidiPlayer::_notification(int p_what) {
 	switch (p_what) {
		case NOTIFICATION_READY: {
			set_process_internal(true);
		}
		case NOTIFICATION_INTERNAL_PROCESS: {
			// if soundfont and stream exist then start buferring
			if (mTsf) {
				playback_gen = get_stream_playback();
				if (playback_gen != nullptr) {
					playback_gen->push_buffer(get_buffer(playback_gen->get_frames_available()));
				} 
			} 
			break;
		}
	}
} 

void MidiPlayer::set_looping(bool p_looping) {
	looping = p_looping;
}

bool MidiPlayer::is_looping() {
	return looping;
}

void MidiPlayer::set_midi_speed(float p_speed) {
	midi_speed = p_speed;
}

float MidiPlayer::get_midi_speed() {
	return midi_speed;
}

void MidiPlayer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load_soundfont", "soundfont_resource_path"), &MidiPlayer::load_soundfont);
	ClassDB::bind_method(D_METHOD("set_soundfont", "soundfont_resource"), &MidiPlayer::set_soundfont);
	ClassDB::bind_method(D_METHOD("get_soundfont"), &MidiPlayer::get_soundfont);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "soundfont", PROPERTY_HINT_RESOURCE_TYPE, "MidiFile"), "set_soundfont", "get_soundfont");

	ClassDB::bind_method(D_METHOD("load_midi", "midi_resource_path"), &MidiPlayer::load_midi);
	ClassDB::bind_method(D_METHOD("set_midi", "midi_resource"), &MidiPlayer::set_midi);
	ClassDB::bind_method(D_METHOD("get_midi"), &MidiPlayer::get_midi);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "midi", PROPERTY_HINT_RESOURCE_TYPE, "MidiFile"), "set_midi", "get_midi");

	ClassDB::bind_method(D_METHOD("get_preset_names"), &MidiPlayer::get_preset_names);

	ClassDB::bind_method(D_METHOD("note_on", "preset", "note", "volume"), &MidiPlayer::note_on);
	ClassDB::bind_method(D_METHOD("note_off", "preset", "note"), &MidiPlayer::note_off);
	ClassDB::bind_method(D_METHOD("note_off_all"), &MidiPlayer::note_off_all);

	// change everything to D_METHOD(func, params)
	ClassDB::bind_method(D_METHOD("channel_set_preset_index", "channel", "preset_index"), &MidiPlayer::channel_set_preset_index);
	ClassDB::bind_method(D_METHOD("channel_set_preset_number", "channel", "preset_number", "drums"), &MidiPlayer::channel_set_preset_number);
	ClassDB::bind_method(D_METHOD("channel_set_bank", "channel", "bank"), &MidiPlayer::channel_set_bank);
	ClassDB::bind_method(D_METHOD("channel_set_bank_preset", "channel", "bank", "preset"), &MidiPlayer::channel_set_bank_preset);
	ClassDB::bind_method(D_METHOD("channel_set_pan", "channel", "pan"), &MidiPlayer::channel_set_pan);
	ClassDB::bind_method(D_METHOD("channel_set_volume", "channel", "volume"), &MidiPlayer::channel_set_volume);
	ClassDB::bind_method(D_METHOD("channel_set_pitchwheel", "channel", "pitchwheel"), &MidiPlayer::channel_set_pitchwheel);
	ClassDB::bind_method(D_METHOD("channel_set_pitchrange", "channel", "pitchrange"), &MidiPlayer::channel_set_pitchrange);
	ClassDB::bind_method(D_METHOD("channel_set_tuning", "channel", "tuning"), &MidiPlayer::channel_set_tuning);
	ClassDB::bind_method(D_METHOD("channel_note_on", "channel", "note", "volume"), &MidiPlayer::channel_note_on);
	ClassDB::bind_method(D_METHOD("channel_note_off", "channel", "note"), &MidiPlayer::channel_note_off);
	ClassDB::bind_method(D_METHOD("channel_note_off_all", "channel"), &MidiPlayer::channel_note_off_all);
	ClassDB::bind_method(D_METHOD("channel_midi_control", "channel", "control", "value"), &MidiPlayer::channel_midi_control);
	ClassDB::bind_method(D_METHOD("channel_get_preset_index", "channel"), &MidiPlayer::channel_get_preset_index);
	ClassDB::bind_method(D_METHOD("channel_get_preset_bank", "channel"), &MidiPlayer::channel_get_preset_bank);
	ClassDB::bind_method(D_METHOD("channel_get_preset_number", "channel"), &MidiPlayer::channel_get_preset_number);
	ClassDB::bind_method(D_METHOD("channel_get_pan", "channel"), &MidiPlayer::channel_get_pan);
	ClassDB::bind_method(D_METHOD("channel_get_volume", "channel"), &MidiPlayer::channel_get_volume);
	ClassDB::bind_method(D_METHOD("channel_get_pitchwheel", "channel"), &MidiPlayer::channel_get_pitchwheel);
	ClassDB::bind_method(D_METHOD("channel_get_pitchrange", "channel"), &MidiPlayer::channel_get_pitchrange);
	ClassDB::bind_method(D_METHOD("channel_get_tuning", "channel"), &MidiPlayer::channel_get_tuning);

	ClassDB::bind_method(D_METHOD("set_looping", "looping"), &MidiPlayer::set_looping);
	ClassDB::bind_method(D_METHOD("is_looping"), &MidiPlayer::is_looping);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "looping", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR), "set_looping", "is_looping");
	ClassDB::bind_method(D_METHOD("set_midi_speed", "speed"), &MidiPlayer::set_midi_speed);
	ClassDB::bind_method(D_METHOD("get_midi_speed"), &MidiPlayer::get_midi_speed);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "midi_speed", PROPERTY_HINT_RANGE, "0,8,0.1", PROPERTY_USAGE_EDITOR), "set_midi_speed", "get_midi_speed");

	ADD_SIGNAL(MethodInfo("loop_finished"));
}

MidiPlayer::~MidiPlayer() {
	// frees memory related to soundfont
	if (mTsf != NULL) {
		tsf_close(mTsf);
	}
}