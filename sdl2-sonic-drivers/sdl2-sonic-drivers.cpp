// sdl2-sonic-drivers.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <sound_adlib.h>
#include <SDL_mixer.h>

using namespace std;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO);
	Mix_Init(MIX_INIT_MID);
	if (Mix_OpenAudio(44100, AUDIO_S16, 2, 1024) < 0) {
		cerr << Mix_GetError();
		return -1;
	}

	Mix_AllocateChannels(16);

	int freq;
	uint16_t fmt;
	int channels;
	Mix_QuerySpec(&freq, &fmt, &channels);
	cout << "freq: " << freq << endl
		<< "format: " << fmt << endl
		<< "channels: " << channels << endl;

	SDL_RWops* adlFile = SDL_RWFromFile("DUNE0.ADL", "rb");
	if (nullptr == adlFile) {
		cerr << "file not found" << endl;
		throw std::runtime_error("file not found");
	}

	SoundAdlibPC adlib = SoundAdlibPC(adlFile);
	

	auto songs = adlib.getSubsongs();
	//adlib.setVolume(0xFF);
	
	//Mix_Volume(-1, MIX_MAX_VOLUME);
	
	cout << "Volume: " << adlib.getVolume() << endl;
	cout << "num Tracks: " << songs.size() << endl;
	
	adlib.playTrack(2);
	Mix_HookMusic(adlib.callback, &adlib);
	do{
		cout << "playin music, waiting 1s..." << endl;
		SDL_Delay(1000);
	} while (adlib.isPlaying());
	cout << "end";
	//for (auto& s : songs) {
	//	Mix_Chunk* chunk = adlib.getSubsong(s);
	//	cout << "song: " << s << endl;
	//	
	//	adlib.playTrack(s);
	//	Mix_HookMusic(adlib.callback, &adlib);
	//	cout << "channels playing: " << Mix_Playing(-1) << endl;

	//	/*while (Mix_Playing(-1)) {
	//		cout << "waiting 1s ..." << endl;
	//		SDL_Delay(1000);
	//	}*/
	//	SDL_Delay(1000);
	//	Mix_FreeChunk(chunk);
	//}

	//for (auto& s : songs) {
	//	Mix_Chunk* chunk = adlib.getSubsong(s);
	//	cout << "song: " << s << endl;
	//	int channel = Mix_PlayChannel(-1, chunk, 0);
	//	//Mix_HookMusic(adlib.callback, &adlib);
	//	cout << "channels playing: " << Mix_Playing(-1) << endl;
	//	
	//	while (Mix_Playing(-1)) { 
	//		cout << "waiting 1s ..." << endl;
	//		SDL_Delay(1000); 
	//	}
	//	SDL_Delay(1000);
	//	Mix_FreeChunk(chunk);
	//}

	SDL_RWclose(adlFile);
	Mix_HaltChannel(-1);
	Mix_HaltMusic();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();
	return 0;
}
