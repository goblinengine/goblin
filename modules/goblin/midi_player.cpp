#include "midi_player.h"

#include "core/bind/core_bind.h"
#define TSF_IMPLEMENTATION
#include "libs/tsf.h"
#define TML_IMPLEMENTATION
#include "libs/tml.h"

static int readTsfFile(void* inData, void* inPtr, unsigned int inSize) {
	FileAccess *theFile = (FileAccess*)inData;
	ERR_FAIL_COND_V_MSG(!theFile, 0, "File pointer was lost.");

	if (theFile) {
		PoolVector<uint8_t> theData;
		Error err = theData.resize(inSize);
		if (err) ERR_FAIL_V_MSG(0, "Could not resize data");
		PoolVector<uint8_t>::Write w = theData.write();
		int theReadSize = theFile->get_buffer(&w[0], inSize);
		w.release();
		if (theReadSize < inSize) theData.resize(inSize);
		ERR_FAIL_COND_V_MSG(theReadSize == 0, 0, "Could't read file.");

		memcpy(inPtr, theData.read().ptr(), theReadSize);

		return theReadSize;
	}
}

static int skipTsfFile(void* inData, unsigned int inSize) {
	FileAccess *theFile = (FileAccess*)inData;
	int64_t theNewPosition = theFile->get_position()+inSize;
	theFile->seek(theNewPosition);
	return (theFile->get_position() == theNewPosition) ? 1 : 0;
}

MidiPlayer::MidiPlayer(): mTsf(NULL), mTsfFile(NULL), mMidiSpeed(1.0) {
	FileAccess *mTsfFile = nullptr;
	FileAccess *mTmlFile = nullptr;
	mTsf = NULL;
	mSoundFontName = "";
	mTsfStream.read = readTsfFile;
	mTsfStream.skip = skipTsfFile;
	mTml = NULL;
	mTmlCurrent = NULL;
	loop_mode = false;
	mTmlStream.read = readTsfFile;
}

void MidiPlayer::set_sound_font(String inSoundFontName) {
	mSoundFontName = inSoundFontName;
	if (mTsf != NULL) {
		tsf_close(mTsf);
		mTsf = NULL;
	}

	if (mTsfFile) {
		mTsfFile->close();
	}

	mTsfFile = FileAccess::open(mSoundFontName, FileAccess::READ);
	ERR_FAIL_COND_MSG(!mTsfFile, "Couldn't open " + mSoundFontName + ".");
	mTsfStream.data = mTsfFile;

	if (mTsfFile) {
		mTsf = tsf_load(&mTsfStream);
	}
}

String MidiPlayer::get_sound_font() const {
	return mSoundFontName;
}

void MidiPlayer::play_midi(String inMidiFileName, bool inloop) {
	if (mTml != NULL) {
		tml_free(mTml);
		mTml = NULL;
		mTmlCurrent = NULL;
		loop_mode = false;
		mTmlFile->close();
	}

	if (mTsfFile) {
		mTsfFile->close();
	}

	mTmlFile = FileAccess::open(inMidiFileName, FileAccess::READ);
	ERR_FAIL_COND_MSG(!mTsfFile, "Couldn't open " + inMidiFileName + ".");
	mTmlStream.data = mTmlFile;

	mTml = tml_load(&mTmlStream);
	mTmlCurrent = mTml;
	mTmlTime = 0.0;
	loop_mode = inloop;
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

int MidiPlayer::get_presetindex(int inBank, int inPresetNumber) {
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
	tsf_note_on(mTsf, inPresetIndex, inKey, 0.0);
}

void MidiPlayer::note_off_all() {
	if (mTsf == NULL) {
		return;
	}
	tsf_note_off_all(mTsf);
}

void MidiPlayer::channel_set_presetindex(int inChannel, int inPresetIndex) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_set_presetindex(mTsf, inChannel, inPresetIndex);
}


int MidiPlayer::channel_set_presetnumber(int inChannel, int inPresetNumber, int inDrums) {
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

 //end with sustain and release
void MidiPlayer::channel_sounds_off_all(int inChannel) {
	if (mTsf == NULL) {
		return;
	}
	tsf_channel_sounds_off_all(mTsf, inChannel);
}

 //end immediatly
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

bool MidiPlayer::finished() {
	if (mTmlCurrent == NULL || (mTmlCurrent != NULL && mTmlCurrent->next == NULL)) {
		return true;
	}
	return false;
}

/* void MidiPlayer::loop(bool l) {
	loop_mode = l;
} */

PoolVector2Array MidiPlayer::get_buffer(int inSize) {
	PoolVector2Array theBuffer;
	
	if (inSize > 0 && mTsf != NULL) {
		if (mTmlCurrent == NULL && loop_mode == true) {
			note_off_all();
			mTmlCurrent = mTml;
			mTmlTime = 0.0;
			emit_signal("midi_finished");
		}
		theBuffer.resize(inSize);
		Vector2 *theData = theBuffer.write().ptr();
		if (mTmlCurrent != NULL) {
			int SampleBlock;
			int SampleCount = inSize;
			for (SampleBlock = TSF_RENDER_EFFECTSAMPLEBLOCK; SampleCount; SampleCount -= SampleBlock, theData += SampleBlock) {
				//We progress the MIDI playback and then process TSF_RENDER_EFFECTSAMPLEBLOCK samples at once
				if (SampleBlock > SampleCount) SampleBlock = SampleCount;

				//Loop through all MIDI messages which need to be played up until the current playback time
				for (mTmlTime += SampleBlock * mMidiSpeed * (1000.0 / 44100.0); mTmlCurrent && mTmlTime >= mTmlCurrent->time; mTmlCurrent = mTmlCurrent->next) {
					switch (mTmlCurrent->type) {
						case TML_PROGRAM_CHANGE: //channel program (preset) change (special handling for 10th MIDI channel with drums)
							tsf_channel_set_presetnumber(mTsf, mTmlCurrent->channel, mTmlCurrent->program, (mTmlCurrent->channel == 9));
							break;
						case TML_NOTE_ON: //play a note
							tsf_channel_note_on(mTsf, mTmlCurrent->channel, mTmlCurrent->key, mTmlCurrent->velocity / 127.0f);
							break;
						case TML_NOTE_OFF: //stop a note
							tsf_channel_note_off(mTsf, mTmlCurrent->channel, mTmlCurrent->key);
							break;
						case TML_PITCH_BEND: //pitch wheel modification
							tsf_channel_set_pitchwheel(mTsf, mTmlCurrent->channel, mTmlCurrent->pitch_bend);
							break;
						case TML_CONTROL_CHANGE: //MIDI controller messages
							tsf_channel_midi_control(mTsf, mTmlCurrent->channel, mTmlCurrent->control, mTmlCurrent->control_value);
							break;
					}
				}
				// Render the block of audio samples in float format
				tsf_render_float(mTsf, (float*)theData, SampleBlock, 0);
			}
		} else {
			tsf_render_float(mTsf, (float*)theData, inSize, 0);
		}
	}
	return theBuffer;
}

void set_playback(AudioStreamPlayback *inAudioStreamPlayback) {
}

void MidiPlayer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sound_font", "sound_font"), &MidiPlayer::set_sound_font);
	ClassDB::bind_method(D_METHOD("get_sound_font"), &MidiPlayer::get_sound_font);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "sound_font", PROPERTY_HINT_NONE, "", 0), "set_sound_font", "get_sound_font");

	//ClassDB::bind_method("loop", &MidiPlayer::loop);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "loop", PROPERTY_HINT_NONE, "", true), "", "");

	//ClassDB::bind_method(D_METHOD("get_midi_speed"), &MidiPlayer::get_midi_speed);
	//ADD_PROPERTY(PropertyInfo(Variant::STRING, "midi_speed", PROPERTY_HINT_NONE, "", 0), "", "get_midi_speed");

	ClassDB::bind_method("get_preset_names", &MidiPlayer::get_preset_names);

	ClassDB::bind_method("note_on", &MidiPlayer::note_on);
	ClassDB::bind_method("note_off", &MidiPlayer::note_off);
	ClassDB::bind_method("note_off_all", &MidiPlayer::note_off_all);

	ClassDB::bind_method("channel_set_presetindex", &MidiPlayer::channel_set_presetindex);
	ClassDB::bind_method("channel_set_presetnumber", &MidiPlayer::channel_set_presetnumber);
	ClassDB::bind_method("channel_set_bank", &MidiPlayer::channel_set_bank);
	ClassDB::bind_method("channel_set_bank_preset", &MidiPlayer::channel_set_bank_preset);
	ClassDB::bind_method("channel_set_pan", &MidiPlayer::channel_set_pan);
	ClassDB::bind_method("channel_set_volume", &MidiPlayer::channel_set_volume);
	ClassDB::bind_method("channel_set_pitchwheel", &MidiPlayer::channel_set_pitchwheel);
	ClassDB::bind_method("channel_set_pitchrange", &MidiPlayer::channel_set_pitchrange);
	ClassDB::bind_method("channel_set_tuning", &MidiPlayer::channel_set_tuning);
	ClassDB::bind_method("channel_note_on", &MidiPlayer::channel_note_on);
	ClassDB::bind_method("channel_note_off", &MidiPlayer::channel_note_off);
	ClassDB::bind_method("channel_note_off_all", &MidiPlayer::channel_note_off_all);
	ClassDB::bind_method("channel_midi_control", &MidiPlayer::channel_midi_control);
	ClassDB::bind_method("channel_get_preset_index", &MidiPlayer::channel_get_preset_index);
	ClassDB::bind_method("channel_get_preset_bank", &MidiPlayer::channel_get_preset_bank);
	ClassDB::bind_method("channel_get_preset_number", &MidiPlayer::channel_get_preset_number);
	ClassDB::bind_method("channel_get_pan", &MidiPlayer::channel_get_pan);
	ClassDB::bind_method("channel_get_volume", &MidiPlayer::channel_get_volume);
	ClassDB::bind_method("channel_get_pitchwheel", &MidiPlayer::channel_get_pitchwheel);
	ClassDB::bind_method("channel_get_pitchrange", &MidiPlayer::channel_get_pitchrange);
	ClassDB::bind_method("channel_get_tuning", &MidiPlayer::channel_get_tuning);

	ClassDB::bind_method("play_midi", &MidiPlayer::play_midi);
	ClassDB::bind_method("finished", &MidiPlayer::finished);
	ClassDB::bind_method("get_buffer", &MidiPlayer::get_buffer);

	ADD_SIGNAL(MethodInfo("midi_finished"));
	//register_signal<SoundFont>((char *)"position_changed", "node", GODOT_VARIANT_TYPE_OBJECT, "new_pos", GODOT_VARIANT_TYPE_VECTOR2);
} 

MidiPlayer::~MidiPlayer() {
	if (mTsf != NULL) {
		tsf_close(mTsf);
	}
	mTsfFile = nullptr;
	delete mTsfFile;
	Error err;
	mTsfFile = FileAccess::open(mSoundFontName, FileAccess::READ, &err);
	if (err == OK && mTsfFile) {
		mTsf = tsf_load(&mTsfStream);
	}
	mTmlFile = nullptr;
	delete mTmlFile;
}