#pragma once

#include <files/File.hpp>
#include <string>

namespace files
{
	class ADLFile final : public File
	{
	public:
		ADLFile(const std::string& filename);
	};
}
