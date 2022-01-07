

#include <string>
#include <vector>
#include "TwentySeconds.h"
#include "PenalCode.h"
#include "GLAWK.h"
#include "PS1.h"
//#include "MINE.h"
//#include "REED.h"
#include "DUDE.h"
#include "BTG.h"
namespace SoundData {

	struct Sound {
		int Index;
		std::string Name;
		std::vector<std::string> FileData;
	};
	inline std::vector<Sound> Sounds = {
		Sound{0, "20Seconds", TwentySeconds::Sound},
		Sound{1, "Penal", PenalCode::Sound},
		Sound{2, "Glawk", GLAWK::Sound},
		Sound{3, "PS1", PS1::Sound},
		//Sound{4, "MINE", MINE::Sound},
		//Sound{5, "REED", REED::Sound}
		//Sound{4, "DUDE", DUDE::Sound},
		Sound{4, "BTG", BTG::Sound}
	};
}