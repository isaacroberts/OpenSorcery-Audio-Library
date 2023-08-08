//
//  looper.cpp
//
//
//  Created by Isaac Roberts on 2/1/21.
//

#pragma once

namespace looper_rt
{
	template <typename Fn, typename Tuple, size_t... Ix>
	constexpr void forEachInTuple (Fn&& fn, Tuple&& tuple, std::index_sequence<Ix...>)
		noexcept (noexcept (std::initializer_list<int> { (fn (std::get<Ix> (tuple), Ix), 0)... }))
	{
		(void) std::initializer_list<int> { ((void) fn (std::get<Ix> (tuple), Ix), 0)... };
	}

	template <typename T>
	using TupleIndexSequence = std::make_index_sequence<std::tuple_size<std::remove_cv_t<std::remove_reference_t<T>>>::value>;

	template <typename Fn, typename Tuple>
	constexpr void forEachInTuple (Fn&& fn, Tuple&& tuple)
		noexcept (noexcept (forEachInTuple (std::forward<Fn> (fn), std::forward<Tuple> (tuple), TupleIndexSequence<Tuple>{})))
	{
		forEachInTuple (std::forward<Fn> (fn), std::forward<Tuple> (tuple), TupleIndexSequence<Tuple>{});
	}
	
	
	template <typename Fn, typename TupleA, typename TupleB, size_t... Ix>
	constexpr void zip (Fn&& fn, TupleA&& tupleA, TupleB&& tupleB, std::index_sequence<Ix...>)
		noexcept (noexcept (std::initializer_list<int> { (fn (std::get<Ix> (tupleA), std::get<Ix>(tupleB), Ix), 0) ... }))
	{
		(void) std::initializer_list<int> { ((void) fn (std::get<Ix> (tupleA), std::get<Ix>(tupleB), Ix), 0)... };
	}
	
	template <typename Fn, typename TupleA, typename TupleB>
	constexpr void zip (Fn&& fn, TupleA&& tupleA, TupleB&& tupleB)
		noexcept (noexcept (zip (std::forward<Fn> (fn), std::forward<TupleA> (tupleA), std::forward<TupleB>(tupleB), TupleIndexSequence<TupleA>{})))
	{
		zip (std::forward<Fn> (fn), std::forward<TupleA> (tupleA), std::forward<TupleB> (tupleB), TupleIndexSequence<TupleA>{});
	}
	
	
	template <typename Fn, typename TupleA, typename TupleB, typename TupleC, size_t... Ix>
	constexpr void tripleZip (Fn&& fn, TupleA&& tupleA, TupleB&& tupleB, TupleC&& tupleC, std::index_sequence<Ix...>)
		noexcept (noexcept (std::initializer_list<int> { (fn (std::get<Ix> (tupleA), std::get<Ix>(tupleB), std::get<Ix>(tupleC), Ix), 0) ... }))
	{
		(void) std::initializer_list<int> { ((void) fn (std::get<Ix> (tupleA), std::get<Ix>(tupleB), std::get<Ix>(tupleC), Ix), 0)... };
	}
	
	template <typename Fn, typename TupleA, typename TupleB, typename TupleC>
	constexpr void tripleZip (Fn&& fn, TupleA&& tupleA, TupleB&& tupleB, TupleC&& tupleC)
		noexcept (noexcept (zip (std::forward<Fn> (fn), std::forward<TupleA> (tupleA), std::forward<TupleB>(tupleB), std::forward<TupleC>(tupleC), TupleIndexSequence<TupleA>{})))
	{
		zip (std::forward<Fn> (fn), std::forward<TupleA> (tupleA), std::forward<TupleB> (tupleB), std::forward<TupleC> (tupleC), TupleIndexSequence<TupleA>{});
	}
}
