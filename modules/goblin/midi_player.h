#ifndef SoundFont_H
#define SoundFont_H

#include "scene/main/node.h"
#include "core/os/file_access.h"
#include "scene/audio/audio_stream_player.h"
#include "libs/tsf.h"
#include "libs/tml.h"

class MidiPlayer : public AudioStreamPlayer {
	GDCLASS(MidiPlayer, AudioStreamPlayer);

private:
	String       mSoundFontName;
	tsf         *mTsf;
	FileAccess  *mTsfFile;
	tsf_stream   mTsfStream;
	tml_message *mTml;
	tml_message *mTmlCurrent;
	double       mTmlTime;
	float        mMidiSpeed;
	FileAccess   *mTmlFile;
	tml_stream   mTmlStream;
	bool         loop_mode;

protected:
	static void _bind_methods();

public:

	MidiPlayer();
	~MidiPlayer();

	void set_sound_font(String inSoundFontName);
	String get_sound_font() const;

	PoolStringArray get_preset_names() const;
	int get_presetindex(int inBank, int inPresetNumber);

	void set_output(int inSampleRate, float inGainDb);
	
	void note_on(int inPresetIndex, int inKey, float inVelocity);
	void note_off(int inPresetIndex, int inKey);
	void note_off_all();

    void channel_set_presetindex(int inChannel, int inPresetIndex);
    int channel_set_presetnumber(int inChannel, int inPresetNumber, int inDrums);
    void channel_set_bank(int inChannel, int inBank);
    int channel_set_bank_preset(int inChannel, int inBank, int inPresetNumber);
    void channel_set_pan(int inChannel, float inPan);
    void channel_set_volume(int inChannel, float inVolume);
    void channel_set_pitchwheel(int inChannel, int inPitchWheel);
    void channel_set_pitchrange(int inChannel, float inPitchRange);
    void channel_set_tuning(int inChannel, float inTuning);
    void channel_note_on(int inChannel, int inKey, float inVelocity);
    void channel_note_off(int inChannel, int inKey);
    void channel_note_off_all(int inChannel); //end with sustain and release
    void channel_sounds_off_all(int inChannel); //end immediatly
    void channel_midi_control(int inChannel, int inController, int inValue);
    int channel_get_preset_index(int inChannel);
    int channel_get_preset_bank(int inChannel);
    int channel_get_preset_number(int inChannel);
    float channel_get_pan(int inChannel);
    float channel_get_volume(int inChannel);
    int channel_get_pitchwheel(int inChannel);
    float channel_get_pitchrange(int inChannel);
    float channel_get_tuning(int inChannel);

	void play_midi(String inMidiFileName, bool loop);
	bool finished();
	void loop(bool l);

	PoolVector2Array get_buffer(int inSize);
};

#endif
