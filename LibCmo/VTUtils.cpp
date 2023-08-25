#include "VTUtils.hpp"

namespace LibCmo {

	void LibPanic(int line, const char* file, const char* errmsg) {
		fprintf(stderr, "LIBCMO PANIC:%s (%s:L%d)\n",
			errmsg ? errmsg : "", file, line);
		std::abort();
	}

	namespace StreamHelper {

		static constexpr const size_t CHUNK_SIZE = 10240;
		void CopyStream(const void* src, FILE* dest, size_t len) {
			fwrite(src, sizeof(char), len, dest);
		}
		void CopyStream(FILE* src, void* dest, size_t len) {
			size_t expected_size = 0u;
			char* p = reinterpret_cast<char*>(dest);

			while (len != 0) {
				expected_size = len < CHUNK_SIZE ? len : CHUNK_SIZE;
				fread(p, sizeof(char), expected_size, src);
				p += expected_size;
				len -= expected_size;
			}
		}

	}
}
