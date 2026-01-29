#include "Utils.hpp"
#include <yycc/num/op.hpp>
#include <stdexcept>

namespace Unvirt::Utils {

	PageBreaker::PageBreaker(size_t count, size_t count_per_page) : count(count), count_per_page(count_per_page) {
		if (count_per_page == 0) throw std::logic_error("item count per page should not be zero");
	}

	PageBreaker::~PageBreaker() {}

	bool PageBreaker::IsValidPage(size_t page) const {
		if (page == 0) return true;
		else return page < this->GetMaxPage();
	}

	size_t PageBreaker::GetMaxPage() const {
		if (this->count == 0) return 0;
		else return yycc::num::op::div_ceil(this->count, this->count_per_page) - 1;
	}

	size_t PageBreaker::GetPageBeginIndex(size_t page) const {
		if (this->IsValidPage(page)) return page * this->count_per_page;
		else throw std::runtime_error("invalid page index");
	}

	size_t PageBreaker::GetPageEndIndex(size_t page) const {
		if (this->IsValidPage(page)) return std::min(this->count, (page + 1) * this->count_per_page);
		else throw std::runtime_error("invalid page index");
	}

} // namespace Unvirt::Utils
