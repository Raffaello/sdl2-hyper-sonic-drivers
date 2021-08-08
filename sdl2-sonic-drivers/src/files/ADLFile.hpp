#pragma once

#include <files/File.hpp>
#include <string>

namespace files
{
	class ADLFile final : public File
	{
	public:
		ADLFile(const std::string& filename);
		uint8_t getVersion() const noexcept;
	private:
		uint8_t _version = 0;
		void _detectVersion();
		void _validateVersion();
	};
}
