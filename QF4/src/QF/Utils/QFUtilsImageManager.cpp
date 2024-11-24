#include "QFUtilsImage.h"

namespace utils = QF::Utils;
using self = utils::ImageManager;

/* Constructor & Destructor */
	self::ImageManager() = default;
	self::~ImageManager() = default;

/* Put & Get */
	void self::putImage(std::unique_ptr<utils::Image> _Image, const std::string& _Name) {
		if (_Image == nullptr) {
#ifndef NDEBUG
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, std::format("Failed to push image[{}], nullptr given as arg", _Name));
#endif 
			throw std::invalid_argument("ImageMenager: _Image[arg: 0] cannot be nullptr.");
		}

		m_ImageCollection[_Name] = std::move(_Image);
	}

	utils::Image* self::g_Image(const std::string& _Name) const {
		auto element = m_ImageCollection.find(_Name);
		
		if (element == m_ImageCollection.end()) {
#ifndef NDEBUG
			__QF_DEBUG_LOG(__QF_ERROR, __FUNCTION__, std::format("Failed to retrive image[{}] from collection.", _Name));
#endif 
			throw std::runtime_error("ImageManager: Failed to retrive image from collection.");
		}
		return element->second.get();
	}

	const bool self::is_There(const std::string& _Name) const {
		return (m_ImageCollection.find(_Name) != m_ImageCollection.end());
	}

	void self::eraseImage(const std::string& _Name) {
		auto element = m_ImageCollection.find(_Name);

		if (element == m_ImageCollection.end()) {
#ifndef NDEBUG
			__QF_DEBUG_LOG(__QF_WARNING, __FUNCTION__, std::format("Failed to retrive image[{}] from collection.", _Name));
#endif 
			return; 
		}
		m_ImageCollection.erase(element);
	}