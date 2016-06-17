#pragma once

namespace CYB {
	namespace Platform {
		namespace System {
			namespace Implementation {
				class Sys {
				public:
					enum CallNumber : long long {
						EXIT = 60,
						EXIT_PROC = -1,
						LOAD_LIBRARY = -2,
						LOAD_SYMBOL = -3,
						CLOSE_LIBRARY = -4,
					};
				};
			};
		};
	};
};