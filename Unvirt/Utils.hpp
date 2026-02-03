#pragma once
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <cstddef>

namespace Unvirt::Utils {

	class PageBreaker {
	public:
		/**
		 * @brief 
		 * @param[in] count 
		 * @param[in] count_per_page 
		 * @exception std::logic_error if the count per page is zero.
		 */
		PageBreaker(size_t count, size_t count_per_page);
		~PageBreaker();
		YYCC_DEFAULT_COPY_MOVE(PageBreaker)

	public:
		/**
		 * @brief 
		 * @param[in] page The page index (zero-based)
		 * @return 
		 * @remarks Page zero is always valid because we at least have one page.
		 */
		bool IsValidPage(size_t page) const;
		/**
		 * @brief 
		 * @return The max page index (zero-based, inclusive)
		 * @remarks Zero count will produce at least one page.
		 */
		size_t GetMaxPage() const;
		/**
		 * @brief 
		 * @param[in] page The page index (zero-based)
		 * @return The start index of this page (inclusive). 
		 * Is is usually used as the init statement `size_t i = pb.GetPageBeginIndex()` in "for" statement.
		 * @exception std::runtime_error if page index is out of range.
		 */
		size_t GetPageBeginIndex(size_t page) const;
		/**
		 * @brief 
		 * @param[in] page The page index (zero-based)
		 * @return The end index of this page (exclusive). 
		 * Is is usually used as the stop statement `i < pb.GetPageEndIndex()` in "for" statement.
		 * @exception std::runtime_error if page index is out of range.
		 */
		size_t GetPageEndIndex(size_t page) const;

	private:
		size_t count;
		size_t count_per_page;
	};

}
