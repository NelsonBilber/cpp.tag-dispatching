//https://www.fluentcpp.com/2018/04/27/tag-dispatching/
//https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Tag_Dispatching
//https://www.boost.org/community/generic_programming.html#tag_dispatching
//http://barendgehrels.blogspot.com/2010/10/tag-dispatching-by-type-tag-dispatching.html?m=1
//http://efesx.com/2010/01/03/tag-dispatching/

//" ... Tag dispatching is a way of using function overloading to dispatch based on properties of a type, and is often used hand in hand with my_traits classes.  ..."

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>


/*---------------- Example 01 ---------------------*/

namespace detail
{
	struct pick3 {};
	struct pick2 : pick3 {};
	struct pick1 : pick2 {};
	static pick1 selector;

	//first choise
	template<typename Cont, typename Op>
	auto remove_if(pick1, Cont& cont, Op&& op)-> decltype(cont.remove_if(op), void())
	{
		cont.remove_if(std::forward<Op>(op));
	}

	// second choice - erase remove idiom
	template<typename Cont, typename Op>
	auto remove_if(pick2, Cont& cont, Op&& op)-> decltype(cont.erase(std::remove_if(std::begin(cont), std::end(cont), std::forward<Op>(op)), std::end(cont)), void())
	{
		cont.erase(std::remove_if(std::begin(cont), std::end(cont), std::forward<Op>(op)), std::end(cont));
	}

	// last choice - manual looping
	template<typename Cont, typename Op>
	auto remove_if(pick3, Cont& cont, Op&& op) -> void
	{
		auto it = std::begin(cont);
		while (it != std::end(cont))
		{
			if (op(*it))
				it = cont.erase(it);
			else
				++it;
		}
	}
} // end namespace detail


template<typename Cont, typename Op>
auto remove_if(Cont& cont, Op&& op)
{
	detail::remove_if(detail::selector, cont, std::forward<Op>(op));
}


/*---------------- Example 02 ---------------------*/

struct fast_speed_tag{};
struct slow_speed_tag {};

template<typename T>
struct my_traits
{
	typedef slow_speed_tag speed;
};

template<>
struct my_traits<int>
{
	typedef fast_speed_tag speed;
};

template< typename T>
void work_dispatch(const T &val, const slow_speed_tag&) { std::cout << "slow<T>" << std::endl; }

template< typename T>
void work_dispatch(const T &val, const fast_speed_tag&) { std::cout << "fast<int>" << std::endl; }

template<typename T>
void work(const T& val)
{
	work_dispatch(val, typename my_traits<T>::speed());
}


int main(int argc, const char * argv[])
{
	std::vector<int> v{1,2,3,4};
	
	remove_if(v, [](int i) {return i == 2; });
	
	for (auto&& a : v)
		std::cout << a << std::endl;
	std::cout << "\n";

	int x;
	float y;

	work(x);
	work(y);

	return 0;
}
